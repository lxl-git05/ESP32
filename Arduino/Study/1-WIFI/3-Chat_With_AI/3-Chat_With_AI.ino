// 项目: Chat With AI

// 导入库
#include <Arduino.h>
#include <WiFi.h>

// 全局变量
// 1. WIFI网络名称和密码,需要换成自己手机的热点信息
const char* ssid = "lxl_WIFI";
const char* password = "88888888";
// 2. 构建嵌入式网页
#include "HTML.h"
WiFiServer server(80);
WiFiClient client1;
String chatgpt_Q = "" ;  // 存储手机端发送的问题

// 1. 连接到网络
void WiFiConnect(void)
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
void WiFiIsconnect(void)
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
                        //                
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

void setup() 
{
    // 串口和LED初始化
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);   // 灭灯表示未连接
    Serial.begin(115200);
    // 1. 连接到网络
    WiFiConnect();
    // 2. 嵌入式网页收发
    server.begin();
}

void loop() 
{
    // 监测是否联网
    WiFiIsconnect() ;
    // 嵌入式网页收发
    WebServer() ;
}
