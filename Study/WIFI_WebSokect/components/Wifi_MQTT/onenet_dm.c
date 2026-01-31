#include "onenet_dm.h"
#include <string.h>

#define TAG "Onenet"

extern char Command[64] ;
extern int LED_PWM;
extern bool LED_Status ;

// =========================== 物模型数据处理函数 ===========================

// 下行数据接收:处理onenet下行的数据
void onenet_property_handle(cJSON* property_js)
{
    cJSON *params_js = cJSON_GetObjectItem(property_js,"params");
    if(params_js)
    {
        cJSON *name_js = params_js->child;	// 第1个孩子
        while(name_js)
        {
            // 分类1:bool值:LED_Status
            if(strcmp(name_js->string,"LED_Status") == 0)
            {
                if(cJSON_IsTrue(name_js))
                {
                    // bool值为true
                    LED_Status = true ;
                }
                else
                {
                    // bool值为false
                    LED_Status = false ;
                }
            }
            // 分类2:int类型数据:LED_PWM
            else if(strcmp(name_js->string,"LED_PWM") == 0)
            {
                // 得到数据
                LED_PWM = cJSON_GetNumberValue(name_js);
                // 执行逻辑
            }
            // 分类3:String类型数据
            else if (strcmp(name_js->string,"Command") == 0 && cJSON_IsString(name_js))
            {
                // 得到String值,赋值给Command
                const char *cmd = cJSON_GetStringValue(name_js);
                if (cmd)
                {
                    strncpy(Command, cmd, sizeof(Command) - 1);
                    Command[sizeof(Command) - 1] = '\0';
                }
            }
            name_js = name_js->next;         // 移动到下一个兄弟节点
        }
    }
}

// 生成上报给onene的所有数据的cJSON对象
cJSON* onenet_generate_property_data(void)
{
    // 顶级
    cJSON* root = cJSON_CreateObject();
    // 第1级
    cJSON_AddStringToObject(root,"id","123");
    cJSON_AddStringToObject(root,"version","1.0");
    cJSON* params_js = cJSON_AddObjectToObject(root,"params");
    // 第2级
    // 灯开关
    cJSON* LED_Status_js = cJSON_AddObjectToObject(params_js,"LED_Status");
    cJSON_AddBoolToObject(LED_Status_js,"value",LED_Status);
    // PWM
    cJSON* LED_PWM_js = cJSON_AddObjectToObject(params_js,"LED_PWM");
    cJSON_AddNumberToObject(LED_PWM_js,"value",LED_PWM);
    // Command:String
    cJSON* Command_js = cJSON_AddObjectToObject(params_js,"Command");
    cJSON_AddStringToObject(Command_js,"value",Command);
    return root;
}
// 上行数据发送:data是发送的内容,在这里和topic一起发送
esp_err_t onenet_post_property_data(esp_mqtt_client_handle_t onenet_client , const char* data , bool Onenet_connected_flg , const char* product_id , const char* device_name)
{
    if (!Onenet_connected_flg)
        return ESP_FAIL;
    // 上行订阅topic订阅
    char topic[128];
    snprintf(topic,sizeof(topic),"$sys/%s/%s/thing/property/post",product_id,device_name);
    ESP_LOGI(TAG,"Upload topic:%s,payload:%s",topic,data);
    // 上行数据发送:topic + data
    return esp_mqtt_client_publish(onenet_client,topic,data,strlen(data),1,0);
}

// =========================== 物模型订阅处理函数 ===========================
// 订阅来自上位机的topic: 1. 上位机收到信息后发来的回显 2. 上位机发来的数据处理信息
esp_err_t onenet_subscribe(esp_mqtt_client_handle_t onenet_client , bool Onenet_connected_flg , const char* product_id , const char* device_name)
{
    if (!Onenet_connected_flg)
        return ESP_FAIL;
    char topic[128];
    //订阅上报属性回复主题
    snprintf(topic,sizeof(topic),"$sys/%s/%s/thing/property/post/reply",product_id,device_name);
    esp_mqtt_client_subscribe_single(onenet_client,topic,1);
    //订阅下行设置属性主题
    snprintf(topic,sizeof(topic),"$sys/%s/%s/thing/property/set",product_id,device_name);
    return esp_mqtt_client_subscribe_single(onenet_client,topic,1);
    // //订阅OTA主题,暂时未开发
    // snprintf(topic,sizeof(topic),"$sys/%s/%s/ota/inform",
    //     ONENET_PRODUCT_ID,ONENET_DEVICE_NAME);
    // return esp_mqtt_client_subscribe_single(s_onenet_client,topic,1);
}

// 下位机回显上位机发来的数据处理信息
void onenet_property_ack(esp_mqtt_client_handle_t onenet_client , const char* id,int code,const char* message , const char* product_id , const char* device_name)
{
    // topic
    char topic[128];
    snprintf(topic,sizeof(topic),"$sys/%s/%s/thing/property/set_reply",product_id,device_name);
    // data
    cJSON *reply_js = cJSON_CreateObject();
    cJSON_AddStringToObject(reply_js,"id",id);
    cJSON_AddNumberToObject(reply_js,"code",code);
    cJSON_AddStringToObject(reply_js,"message",message);
    char* data = cJSON_PrintUnformatted(reply_js);
    esp_mqtt_client_publish(onenet_client,topic,data,strlen(data),1,0); 
    cJSON_free(data);
    cJSON_Delete(reply_js);
}
