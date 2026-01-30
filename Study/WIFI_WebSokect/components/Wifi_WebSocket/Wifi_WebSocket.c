#include "Wifi_WebSocket.h"

// ======================= 外部参数 =======================
extern int led_state;
// ======================= WebSocket参数 =======================
// HTTP服务器句柄
httpd_handle_t server = NULL;   
// 用于异步发送 WebSocket 数据的结构体
struct async_resp_arg 
{
    httpd_handle_t hd;
    int fd;
};
// TAG
static const char *TAG = "WebSocket Server"; // TAG for debug
// ======================= HTML参数 =======================
//  SPIFFS 中 HTML 文件路径
#define INDEX_HTML_PATH "/spiffs/index.html"
// 缓存 HTML 内容和发送给 客户端 的字符串
#define HTML_STRING_SIZE 4096
char index_html[HTML_STRING_SIZE];
char response_data[HTML_STRING_SIZE];
// ======================= 宏定义 =======================
// 自定义宏:替代原先宏,增强性能
#define CONFIG_LWIP_MAX_LISTENING_TCP_lxl 4
#define HTML_STACK_SIZE 4096    // 视HTML大小定
// ======================= 程序移植 =======================
/*
    1. HTML文件的位置:INDEX_HTML_PATH:建议不变,懒得折腾
    2. HTML文件的大小和后续发送存储栈的大小 1. HTML_STRING_SIZE 2. HTML_STACK_SIZE
    3. HTML客户端最大个数: CONFIG_LWIP_MAX_LISTENING_TCP_lxl
    4. 处理HTML的GET请求:get_req_handler,其实就是数据更新
    5. 
*/

// ======================= 函数定义 =======================
// SPIFFS 初始化和读取 HTML 文件
static void initi_web_page_buffer(void)
{
    static bool spiffs_registered = false;
    if (spiffs_registered) return;  // 已经注册过就直接返回

    esp_vfs_spiffs_conf_t conf = {
        .base_path = "/spiffs",
        .partition_label = NULL,    // 使用的分区,NULL表示自己检测到的第一个叫spiff的分区
        .max_files = 5,             // 同一时间最多能打开的文件数
        .format_if_mount_failed = true
    };
    // esp_vfs_spiffs_register只能调用一次，如果已经注册，再调用就会重启程序,所以需要进行注册判断
    ESP_ERROR_CHECK(esp_vfs_spiffs_register(&conf));

    memset((void *)index_html, 0, sizeof(index_html));
    struct stat st;
    if (stat(INDEX_HTML_PATH, &st)) // 返回0:文件存在
    {
        ESP_LOGE(TAG, "index.html not found");
        return;
    }

    FILE *fp = fopen(INDEX_HTML_PATH, "r");
    if (fread(index_html, st.st_size, 1, fp) == 0)  // 读取路径下的文件,并且导入到index_html缓冲区
    {
        ESP_LOGE(TAG, "fread failed");
    }
    fclose(fp);
    spiffs_registered = true;  // 标记已注册
}
// 处理HTTP GET请求,发送html
esp_err_t get_req_handler(httpd_req_t *req)
{
    int response;
    // 自定义功能:字符串格式化,记得按顺序将%d , %s等替换为实际值
    if(led_state)
    {
        // 把 HTML 模板里的 %s 替换成 LED 状态
        sprintf(response_data, index_html, "ON");
    }
    else
    {
        sprintf(response_data, index_html, "OFF");
    }
    // 把 HTML 内容发送给浏览器
    response = httpd_resp_send(req, response_data, HTTPD_RESP_USE_STRLEN);
    return response;
}
// WebSocket 异步发送函数,把新的 LED 状态通过 WebSocket 广播给所有已连接的客户端
static void ws_async_send(void *arg)
{
    httpd_ws_frame_t ws_pkt;
    struct async_resp_arg *resp_arg = arg;
    httpd_handle_t hd = resp_arg->hd;
    int fd = resp_arg->fd;

    // 自定义参数处理模式
    led_state = !led_state;

    // 准备发送数据:也需要根据需求自定义
    char buff[4];
    memset(buff, 0, sizeof(buff));
    sprintf(buff, "%d",led_state);

    // 初始化 WebSocket 帧
    memset(&ws_pkt, 0, sizeof(httpd_ws_frame_t));
    ws_pkt.payload = (uint8_t *)buff;
    ws_pkt.len = strlen(buff);
    ws_pkt.type = HTTPD_WS_TYPE_TEXT;
    // 获取客户端列表
    static size_t max_clients = CONFIG_LWIP_MAX_LISTENING_TCP_lxl;  // 客户端最大数,我自己定义的
    size_t fds = max_clients;
    int client_fds[max_clients];

    esp_err_t ret = httpd_get_client_list(server, &fds, client_fds);

    if (ret != ESP_OK) {
        return;
    }
    // 遍历客户端并发送
    for (int i = 0; i < fds; i++) {
        int client_info = httpd_ws_get_fd_info(server, client_fds[i]);
        if (client_info == HTTPD_WS_CLIENT_WEBSOCKET) {
            httpd_ws_send_frame_async(hd, client_fds[i], &ws_pkt);
        }
    }
    free(resp_arg);
}
// 触发一次 LED 状态广播
static esp_err_t trigger_async_send(httpd_handle_t handle, httpd_req_t *req)
{
    struct async_resp_arg *resp_arg = malloc(sizeof(struct async_resp_arg));
    // resp_arg->hd = req->handle;
    resp_arg->fd = httpd_req_to_sockfd(req);
    return httpd_queue_work(handle, ws_async_send, resp_arg);
}
// 处理 WebSocket 客户端的请求
static esp_err_t handle_ws_req(httpd_req_t *req)
{
    // 第一次连接:检查握手请求
    if (req->method == HTTP_GET)
    {
        ESP_LOGI(TAG, "Handshake done, the new connection was opened");
        return ESP_OK;
    }
    // 初始化 WebSocket 帧结构
    httpd_ws_frame_t ws_pkt;
    uint8_t *buf = NULL;
    memset(&ws_pkt, 0, sizeof(httpd_ws_frame_t));
    ws_pkt.type = HTTPD_WS_TYPE_TEXT;
    // 获取消息长度
    esp_err_t ret = httpd_ws_recv_frame(req, &ws_pkt, 0);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "httpd_ws_recv_frame failed to get frame len with %d", ret);
        return ret;
    }
    // 分配内存并接收数据
    if (ws_pkt.len)
    {
        buf = calloc(1, ws_pkt.len + 1);
        if (buf == NULL)
        {
            ESP_LOGE(TAG, "Failed to calloc memory for buf");
            return ESP_ERR_NO_MEM;
        }
        ws_pkt.payload = buf;
        ret = httpd_ws_recv_frame(req, &ws_pkt, ws_pkt.len);
        if (ret != ESP_OK)
        {
            ESP_LOGE(TAG, "httpd_ws_recv_frame failed with %d", ret);
            free(buf);
            return ret;
        }
        ESP_LOGI(TAG, "Got packet with message: %s", ws_pkt.payload);
    }
    // 打印帧长度
    ESP_LOGI(TAG, "frame len is %d", ws_pkt.len);
    // 判断消息并触发异步发送
    if (ws_pkt.type == HTTPD_WS_TYPE_TEXT &&
        strcmp((char *)ws_pkt.payload, "toggle") == 0)
    {
        free(buf);
        return trigger_async_send(req->handle, req);
    }
    return ESP_OK;
}
// 启动一个能同时处理网页和 WebSocket 消息的服务器
httpd_handle_t setup_websocket_server(void)
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    // 性能配置
    config.stack_size = HTML_STACK_SIZE;     // 大页面或复杂逻辑需要更大栈,自己调整
    
    // 配置 HTTP GET 处理器（网页）
    httpd_uri_t uri_get = {
        .uri = "/",
        .method = HTTP_GET,
        .handler = get_req_handler,
        .user_ctx = NULL};
    // 配置 WebSocket 处理器
    httpd_uri_t ws = {
        .uri = "/ws",
        .method = HTTP_GET,
        .handler = handle_ws_req,
        .user_ctx = NULL,
        .is_websocket = true};
    // 启动 HTTP 服务器并注册 URI
    if (httpd_start(&server, &config) == ESP_OK)
    {
        httpd_register_uri_handler(server, &uri_get);
        httpd_register_uri_handler(server, &ws);
    }

    return server;
}

void Wifi_HTTP_Init(void)
{
    led_state = 0;
    ESP_LOGI(TAG, "ESP32 ESP-IDF WebSocket Web Server is running ... ...\n");
    initi_web_page_buffer();
    setup_websocket_server();
}
