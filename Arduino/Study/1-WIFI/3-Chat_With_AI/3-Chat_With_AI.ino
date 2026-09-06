// 项目: Chat With AI

/*
    这是一个简单的ESP32项目,实现了通过嵌入式网页与AI进行对话的功能。
    需要注意的是，如果真的需要应用的话，最好是进行状态机的配置，联网->网页收发->发送问题->获取回答
*/

// 导入库
#include <Arduino.h>
#include <WiFi.h>

// 全局变量
// 1. WIFI网络名称和密码,需要换成自己手机的热点信息
const char* ssid = "lxl_WIFI";
const char* password = "88888888";
// 2. 构建嵌入式网页
#include "MyHTML.h"
WiFiServer server(80);
WiFiClient client1;
String chatgpt_Q = "" ;  // 存储手机端发送的问题
bool isAsked = false;    // 标记是否已经发送过问题给 DeepSeek

// 3. 通过内置网页提交问题:Chatgpt改为deepseek
#include <HTTPClient.h> // 用于发送HTTP请求
HTTPClient https;
const char* chatgpt_token = "sk-d080ab317bee42aaaacef4102603dffd"; // 替换为自己的API Key,需要妥善保管
char chatgpt_server[] = "https://api.deepseek.com/chat/completions";

// 4. 获取AI的回答
#include <ArduinoJson.h>
String AI_Response = ""; // DeepSeek返回的完整JSON
String chatgpt_A = "";   // 提取出来的最终回答

// 1. 使用STA模式连接到网络
void WiFi_STAConnect(void)
{
    // 配置模式
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    while(!Serial);

    // 进行网络连接
    Serial.print("Starting WiFi connection to SSID: ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    Serial.print("Connecting to ");
    Serial.println(ssid);

    // 等待连接成功
    while (WiFi.status() != WL_CONNECTED) 
    {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected!");
    Serial.print("ESP32 IP address: ");
    Serial.println(WiFi.localIP());
}

// 使用LED亮灭来表示WiFi连接状态
void WiFi_Connect_LED(void)
{
    if (WiFi.status() == WL_CONNECTED) 
    {
        digitalWrite(LED_BUILTIN, LOW);   // 亮灯表示连接成功
    } 
    else 
    {
        digitalWrite(LED_BUILTIN, HIGH);    // 灭灯表示未连接
    }
}

// 2. 嵌入式网页收发

// 将网页收到的问题清空
void Clear_Question(void)
{
    chatgpt_Q = "";
}

void WebServer(void)
{
    // 检查是否有客户端连接
    client1 = server.available();
    if (client1) 
    {
        Serial.println("New Client.");
        boolean currentLineIsBlank = true;
        String json_String = "";
        while (client1.connected()) 
        {
            if (client1.available())
            {
                char c = client1.read() ;
                json_String += c;
                if (c == '\n' && currentLineIsBlank) 
                {
                    String dataStr = json_String.substring(0,4);    // 读取前四个字符,判断是什么请求
                    Serial.println(dataStr);    // 打印
                    // 开始进行数据处理
                    // 1. 如果是GET请求,则发送html网页
                    if(dataStr == "GET ")    
                    {
                        client1.print(html_page);  //Send the response body to the client
                    }
                    // 2. 如果是POST请求,则进行数据处理
                    else if(dataStr == "POST")    
                    {
                        // 读取json数据
                        json_String = "" ;
                        while (client1.available()) 
                        {
                            json_String += (char)(client1.read());
                        }
                        Serial.println(json_String);   // 打印json数据
                        // 读取手机端发送的问题,存储在chatgpt_Q中
                        int dataStart = json_String.indexOf("chatgpttext=") + strlen("chatgpttext=");   // POST来的信息,提取出问题
                        chatgpt_Q = json_String.substring(dataStart, json_String.length());  
                        isAsked = true;   // 标记已经发送过问题给 DeepSeek
                        // 页面清空,用户端可以发送新的问题了
                        client1.print(html_page);        
                        // close the connection:
                        delay(10);
                        client1.stop();
                    }
                    json_String = "";
                    break;
                }
                if (c == '\n') 
                {
                    // you're starting a new line
                    currentLineIsBlank = true;
                } 
                else if (c != '\r') 
                {
                    // you've gotten a character on the current line
                    currentLineIsBlank = false;
                }
            }
        }
    }
}

// 3. 把网页收到的问题发送给 DeepSeek
bool Chatgpt_Ask(void)
{
    // 3-1 检查是否能够连接到 DeepSeek
    if (!https.begin(chatgpt_server))
    {
        Serial.println("[HTTPS] Unable to connect");
        delay(1000);
        return false;
    }

    // 3-2 设置请求头
    https.addHeader("Content-Type", "application/json");
    https.addHeader(
        "Authorization",
        String("Bearer ") + chatgpt_token
    );

    // 3-3 构建请求体,发送问题给 DeepSeek
    // 防止问题中的引号、反斜杠和换行破坏 JSON
    String question = chatgpt_Q;
    Clear_Question(); // 清空问题,避免重复发送
    isAsked = false ;
    question.replace("\\", "\\\\");
    question.replace("\"", "\\\"");
    question.replace("\r", "\\r");
    question.replace("\n", "\\n");

    String payload =
        String("{\"model\":\"deepseek-v4-pro\",")
        + "\"messages\":["
        + "{\"role\":\"system\",\"content\":\"You are a helpful assistant.\"},"
        + "{\"role\":\"user\",\"content\":\""
        + question
        + "\"}],"
        + "\"thinking\":{\"type\":\"enabled\"},"
        + "\"reasoning_effort\":\"high\","
        + "\"stream\":false}";

    int httpCode = https.POST(payload);

    if (httpCode == HTTP_CODE_OK)
    {
        Serial.println("DeepSeek原始响应:");
        AI_Response = https.getString();
        Serial.println(AI_Response);
        https.end();
        return true;
    }
    else
    {
        Serial.print("HTTP请求失败，状态码: ");
        Serial.println(httpCode);
        https.end();
        return false;
    }
}

// 4. 获取AI的回答: 解析 DeepSeek 的 JSON 响应
bool Chatgpt_Parse(void)
{
    JsonDocument doc;

    DeserializationError error =
        deserializeJson(doc, AI_Response);

    if (error)
    {
        Serial.print("JSON解析失败: ");
        Serial.println(error.c_str());
        return false;
    }

    const char* answer =
        doc["choices"][0]["message"]["content"];

    if (answer == nullptr)
    {
        Serial.println("JSON中没有找到AI回答");
        return false;
    }

    // 复制到String中，避免doc销毁后指针失效
    chatgpt_A = answer;

    const char* finishReason =
        doc["choices"][0]["finish_reason"] | "";

    int totalTokens =
        doc["usage"]["total_tokens"] | 0;

    Serial.println("AI最终回答:");
    Serial.println(chatgpt_A);

    Serial.print("结束原因: ");
    Serial.println(finishReason);

    Serial.print("消耗Token: ");
    Serial.println(totalTokens);

    AI_Response = "";
    return true;
}


void setup() 
{
    // 串口和LED初始化
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);   // 灭灯表示未连接
    Serial.begin(115200);
    // 1. 连接到网络
    WiFi_STAConnect();
    // 2. 嵌入式网页收发
    server.begin();
}

void loop() 
{
    // 1. 监测是否联网
    WiFi_Connect_LED() ;
    // 2. 嵌入式网页收发
    WebServer() ;
    // 3. 网页传输问题给ESP32 
    // 4. 然后 ESP32 发送给DeepSeek,获取回答
    if (isAsked == true && Chatgpt_Ask() == true)
    {
        if (Chatgpt_Parse() == true)
        {
            Serial.println("DeepSeek回答解析成功");
        }
        else
        {
            Serial.println("DeepSeek回答解析失败");
        }
    }
}
