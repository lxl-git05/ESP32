# 学习在 XIAO ESP32C3 上使用 WiFiClient 和 HTTPClient——XIAO ESP32C3 与 ChatGPT 实战

<div align=center><img width = 1000 src="https://files.seeedstudio.com/wiki/xiaoesp32c3-chatgpt/18.png"/></div>

ChatGPT 是由人工智能研究实验室 OpenAI 于 2022 年 11 月 30 日发布的一款新型聊天机器人模型，是一种由人工智能技术驱动的自然语言处理工具。

它能够通过学习和理解人类语言来进行对话，还可以结合上下文进行交互，像人一样真正地聊天和沟通，甚至能够完成撰写邮件、视频脚本、文案、翻译和编程等任务。

<div align=center><img width = 800 src="https://files.seeedstudio.com/wiki/xiaoesp32c3-chatgpt/2.png"/></div>

在嵌入式系统中，ChatGPT 可以成为一名很好的助手，帮助我们编写简单程序，甚至检查和修复程序中出现的错误。

令人兴奋的是，OpenAI 官方提供了调用 GPT-3 模型的接口，这使我们能够调用这些接口，并通过多种方式将这一强大模型应用到自己的嵌入式系统中。

Seeed Studio XIAO ESP32C3 是一款基于乐鑫 ESP32-C3 Wi-Fi/蓝牙双模芯片的微型物联网开发板。它具有优异的射频性能，支持 IEEE 802.11 b/g/n Wi-Fi 和蓝牙 5（LE）协议，可以完整支持 ESP32 官方提供的 WiFi Client 和 WiFi Server 服务。当然，它也能够完美支持 Arduino。

<div align=center><img width = 200 src="https://files.seeedstudio.com/wiki/XIAO_WiFi/board-pic.png"/></div>

<p style=":center"><a href="https://www.seeedstudio.com/seeed-xiao-esp32c3-p-5431.html" target="_blank"><img src="https://files.seeedstudio.com/wiki/Seeed-WiKi/docs/images/get_one_now.png" /></a></p>

因此，本教程将引导用户学习和使用 XIAO ESP32C3 的 WiFiClient 和 HTTPClient 库，了解如何连接网络、发布网页，以及 HTTP GET 和 POST 的基础知识。我们的目标是调用 OpenAI ChatGPT，并创建属于你自己的问答网站。

## 开始使用

在本教程中，我们将使用 XIAO ESP32C3 配置一个自己的 ChatGPT 问答页面。你可以在该页面中输入问题，XIAO ESP32C3 会记录该问题，然后使用 OpenAI 提供的 API 调用方式，通过 HTTP Client 发送请求命令，获取 ChatGPT 的回答并将其打印到串口。

<div align=center><img width = 800 src="https://files.seeedstudio.com/wiki/xiaoesp32c3-chatgpt/19.png"/></div>

本教程的任务可以分为以下四个主要步骤。

1. [配置 XIAO ESP32C3 连接网络](#配置-xiao-esp32c3-连接网络)：在这一步中，我们将学习使用 XIAO ESP32C3 进行 Wi-Fi 配置的基本流程，以及网络配置、连接网络服务和获取 IP 地址等基本操作。

2. [构建嵌入式网页](#构建嵌入式网页)：在这一步中，我们主要使用 WiFi Client 库。通过该库的 GET 和 POST 功能，我们可以编写自己的问答网页，并将其部署在 XIAO ESP32C3 上。

3. [通过内置网页提交问题](#通过内置网页提交问题)：在这一步中，我们将主要学习使用 HTTP Client 的 POST 方法，按照 OpenAI API 的标准将我们提出的问题以 POST 方式发送出去。我们将重点关注如何从网页中收集和存储问题。

4. [获取 ChatGPT 的回答](#获取-chatgpt-的回答)：在这一步中，我们将学习使用 HTTP Client 的 POST 方法，并从返回消息中提取所需的问题答案。最后，我们会整理代码结构并完成最终集成。

### 所需材料

<table align="center">
	<tr>
	    <th>材料</th>
	</tr>
    <tr>
	    <td align="center"><div align=center><img width = 130 src="https://files.seeedstudio.com/wiki/XIAO_WiFi/board-pic.png"/></div></td>
	</tr>
	<tr>
	    <td align="center"><a href="https://www.seeedstudio.com/seeed-xiao-esp32c3-p-5431.html"><strong>立即购买</strong></a></td>
	</tr>
</table>

### 前期准备

本教程中的所有程序和步骤均基于 XIAO ESP32C3 完成。在准备阶段，我们首先需要完成 XIAO ESP32C3 使用环境的配置。

**步骤 1.** 使用 USB Type-C 数据线将 XIAO ESP32C3 连接到电脑。

<div align=center><img src="https://files.seeedstudio.com/wiki/XIAO_WiFi/cable-connect.png" alt="pir" width="120" height="auto"></div>

**步骤 2.** 根据你使用的操作系统，下载并安装最新版 Arduino IDE。

<p style="text-align:center;"><a href="https://www.arduino.cc/en/software"><img src="https://files.seeedstudio.com/wiki/Seeeduino_Stalker_V3_1/images/Download_IDE.png" alt="pir" width="600" height="auto"></a></p>

**步骤 3.** 启动 Arduino 应用程序。

<div align=center><img width = 600 src="https://files.seeedstudio.com/wiki/seeed_logo/arduino.jpg"/></div>

- **步骤 4.** 向 Arduino IDE 添加 ESP32 开发板软件包。

依次打开 **文件 > 首选项**，然后在“附加开发板管理器网址”中填入下面的地址：

*https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_dev_index.json*

<div align=center><img width = 600 src="https://files.seeedstudio.com/wiki/xiaoesp32c3-chatgpt/4.png"/></div>

依次打开 **工具 > 开发板 > 开发板管理器...**，在搜索框中输入关键字 **esp32**，选择最新版本的 **esp32** 并进行安装。

<div align=center><img src="https://files.seeedstudio.com/wiki/XIAO_WiFi/add_esp32c3.png" alt="pir" width="650" height="auto"></div>

- **步骤 5.** 选择开发板和端口。

依次打开 **工具 > 开发板 > ESP32 Arduino**，然后选择 **XIAO_ESP32C3**。开发板列表较长，需要向下滚动才能找到它。

<div align=center><img width = 800 src="https://files.seeedstudio.com/wiki/Seeed-Studio-XIAO-ESP32/XIAO_ESP32_board.png"/></div>

打开 **工具 > 端口**，选择已连接的 XIAO ESP32C3 所对应的串口名称。它很可能是 COM3 或更高编号的端口（**COM1** 和 **COM2** 通常为硬件串口保留）。

## 配置 XIAO ESP32C3 连接网络

[XIAO ESP32C3 Wi-Fi 使用教程](https://wiki.seeedstudio.com/XIAO_ESP32C3_WiFi_Usage/#connect-to-a-wifi-network)已经对 Wi-Fi 的使用方法进行了详细介绍。

当 ESP32 被设置为 Wi-Fi 站点模式时，它可以连接其他网络（例如你的路由器）。在这种情况下，路由器会为 ESP 开发板分配一个唯一的 IP 地址。

要使用 ESP32 的 Wi-Fi 功能，首先需要在代码中引入 `WiFi.h` 库，如下所示：

```c
#include <WiFi.h>
```

要让 ESP32 连接指定的 Wi-Fi 网络，你必须知道该网络的 SSID 和密码。此外，该网络必须位于 ESP32 的 Wi-Fi 覆盖范围内。

首先设置 Wi-Fi 模式。如果 ESP32 要连接另一个网络（接入点或热点），就必须将它设置为站点模式。

```c
WiFi.mode(WIFI_STA);
```

然后，使用 `WiFi.begin()` 连接网络。需要向它传入网络 SSID 和密码作为参数。

连接 Wi-Fi 网络可能需要一段时间，因此我们通常会添加一个 `while` 循环，通过 `WiFi.status()` 持续检查连接是否已经建立。连接成功建立时，该函数会返回 `WL_CONNECTED`。

当 ESP32 以 Wi-Fi 站点模式连接路由器时，路由器会为它分配一个唯一的 IP 地址。要获取开发板的 IP 地址，需要在成功连接网络后调用 `WiFi.localIP()`。

```c
void WiFiConnect(void){
    WiFi.begin(ssid, password);
    Serial.print("Connecting to ");
    Serial.println(ssid);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected!");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
}
```

`ssid` 和 `password` 变量用来保存你希望连接的网络 SSID 和密码。

```c
// Replace with your network credentials
const char* ssid = "REPLACE_WITH_YOUR_SSID";
const char* password = "REPLACE_WITH_YOUR_PASSWORD";
```

这是一个非常简单的 Wi-Fi 连接程序。将程序上传到 XIAO ESP32C3，然后打开串口助手，将波特率设置为 115200。如果连接一切正常，你将看到打印出的 XIAO IP 地址。

<div align=center><img width = 600 src="https://files.seeedstudio.com/wiki/xiaoesp32c3-chatgpt/5.png"/></div>

如果你希望了解 ESP32C3 中更多与 Wi-Fi 相关的应用和功能，我们建议阅读 [ESP32 实用 Wi-Fi 库函数](https://randomnerdtutorials.com/esp32-useful-wi-fi-functions-arduino/)。

## 构建嵌入式网页

ESP32 在 WiFi 库中集成了许多非常实用的 WiFiClient 函数，使我们无需添加其他库，就能设计和开发嵌入式网页。

创建一个新的 `WiFiServer` 对象，以便使用该对象控制由 XIAO ESP32C3 建立的物联网服务器。

```c
WiFiServer server(80);
WiFiClient client1;
```

在上一步中，我们已经让 XIAO ESP32C3 连接了 Wi-Fi。Wi-Fi 连接成功后，你可以从串口监视器中获取 XIAO 当前的 IP 地址。此时，XIAO 已经成功建立了 Web 服务器，你可以通过 XIAO 的 IP 地址访问该服务器。

假设你的 XIAO ESP32C3 IP 地址为 `192.168.7.152`，接下来就可以在浏览器中输入该地址。

输入该 IP 地址后，我们可能只会看到一个空白页面。这是因为我们尚未向该页面发布任何内容。

<div align=center><img width = 500 src="https://files.seeedstudio.com/wiki/xiaoesp32c3-chatgpt/6.png"/></div>

现在，我们创建一个数组，用来存储希望布置的页面内容，也就是使用 C 语言字符串表示的 HTML 代码。

```c
const char html_page[] PROGMEM = {
    "HTTP/1.1 200 OK\r\n"
    "Content-Type: text/html\r\n"
    "Connection: close\r\n"  // the connection will be closed after completion of the response
    //"Refresh: 1\r\n"         // refresh the page automatically every n sec
    "\r\n"
    "<!DOCTYPE HTML>\r\n"
    "<html>\r\n"
    "<head>\r\n"
      "<meta charset=\"UTF-8\">\r\n"
      "<title>Cloud Printer: ChatGPT</title>\r\n"
      "<link rel=\"icon\" href=\"https://files.seeedstudio.com/wiki/xiaoesp32c3-chatgpt/chatgpt-logo.png\" type=\"image/x-icon\">\r\n"
    "</head>\r\n"
    "<body>\r\n"
    "<img alt=\"SEEED\" src=\"https://files.seeedstudio.com/wiki/xiaoesp32c3-chatgpt/logo.png\" height=\"100\" width=\"410\">\r\n"
    "<p style=\"text-align:center;\">\r\n"
    "<img alt=\"ChatGPT\" src=\"https://files.seeedstudio.com/wiki/xiaoesp32c3-chatgpt/chatgpt-logo.png\" height=\"200\" width=\"200\">\r\n"
    "<h1 align=\"center\">Cloud Printer</h1>\r\n" 
    "<h1 align=\"center\">OpenAI ChatGPT</h1>\r\n" 
    "<div style=\"text-align:center;vertical-align:middle;\">"
    "<form action=\"/\" method=\"post\">"
    "<input type=\"text\" placeholder=\"Please enter your question\" size=\"35\" name=\"chatgpttext\" required=\"required\"/>\r\n"
    "<input type=\"submit\" value=\"Submit\" style=\"height:30px; width:80px;\"/>"
    "</form>"
    "</div>"
    "</p>\r\n"
    "</body>\r\n"
    "<html>\r\n"
};
```

上述代码将呈现出下图所示的页面效果。

<div align=center><img width = 800 src="https://files.seeedstudio.com/wiki/xiaoesp32c3-chatgpt/7.png"/></div>

> **提示**
>
> 网页的 HTML 语法不在本教程的讨论范围内。你可以自行学习 HTML，或者使用现有的生成工具完成代码生成。我们推荐使用 [HTML Generator](https://webcode.tools/generators/html)。
>
> 需要注意的是，在 C 程序中，反斜杠 `\` 和双引号 `"` 是特殊字符。如果希望在字符串中保留这些特殊字符的作用，需要在它们前面添加反斜杠进行转义。

`client1` 表示 Web 服务器建立后的 Socket 客户端连接。下面的代码展示了 Web 服务器的处理流程。

```c
client1 = server.available();
if (client1){
    Serial.println("New Client.");           // print a message out the serial port
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;    
    while (client1.connected()){
        if (client1.available()){  // Check if the client is connected
            char c = client1.read();
            json_String += c;
            if (c == '\n' && currentLineIsBlank) 
            {                                 
                dataStr = json_String.substring(0, 4);
                Serial.println(dataStr);
                if(dataStr == "GET "){
                    client1.print(html_page);  //Send the response body to the client
                }         
                else if(dataStr == "POST")
                {
                    json_String = "";
                    while(client1.available())
                    {
                        json_String += (char)client1.read();
                    }
                    Serial.println(json_String); 
                    dataStart = json_String.indexOf("chatgpttext=") + strlen("chatgpttext=");
                    chatgpt_Q = json_String.substring(dataStart, json_String.length());                    
                    client1.print(html_page);        
                    // close the connection:
                    delay(10);
                    client1.stop();       
                }
                json_String = "";
                break;
            }
            if (c == '\n') {
                // you're starting a new line
                currentLineIsBlank = true;
            }
            else if (c != '\r') {
                // you've gotten a character on the current line
                currentLineIsBlank = false;
            }
        }
    }
}
```

在上面的示例程序中，我们需要使用 `server.begin()` 启动物联网服务器。该语句需要放在 `setup` 函数中。

```c
void setup()
{
    Serial.begin(115200);
 
    // Set WiFi to station mode and disconnect from an AP if it was previously connected
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    while(!Serial);

    Serial.println("WiFi Setup done!");
    WiFiConnect();

    // Start the TCP server server
    server.begin();
}
```

运行上述程序后，在浏览器中输入 XIAO ESP32C3 的 IP 地址（前提是你的主机也需要与 XIAO ESP32C3 处于同一局域网中），程序就会开始执行 WiFiClient 的 GET 步骤。此时，我们通过客户端的 `print` 方法发送页面所需的 HTML 代码。

```c
if(dataStr == "GET "){
    client1.print(html_page);
}
```

我们还在页面中设计了一个用于输入问题的输入框。用户输入内容并单击 **Submit** 按钮后，网页会获取按钮状态，并将输入的问题存储到字符串变量 `chatgpt_Q` 中。

```c
json_String = "";
while(client1.available()){
    json_String += (char)client1.read();
}
Serial.println(json_String); 
dataStart = json_String.indexOf("chatgpttext=") + strlen("chatgpttext=");
chatgpt_Q = json_String.substring(dataStart, json_String.length());                    
client1.print(html_page);        
// close the connection:
delay(10);
client1.stop();      
```

程序运行效果如下图所示。

<div align=center><img width = 800 src="https://files.seeedstudio.com/wiki/xiaoesp32c3-chatgpt/15.png"/></div>

## 通过内置网页提交问题

在上一步创建的页面中，有一个输入框。该输入框用来输入用户希望询问的问题。我们需要做的，就是获取这个问题，并通过 OpenAI 提供的 API 请求将其发送出去。

**步骤 1.** 注册 OpenAI 账户。

你可以点击[这里](https://beta.openai.com/signup)前往 OpenAI 注册页面。如果你以前已经注册过账户，可以跳过这一步。

<div align=center><img width = 400 src="https://files.seeedstudio.com/wiki/xiaoesp32c3-chatgpt/8.png"/></div>

**步骤 2.** 获取 OpenAI API 密钥。

登录 [OpenAI 网站](https://platform.openai.com/overview)，单击右上角的账户头像，然后选择 **View API keys**。

<div align=center><img width = 800 src="https://files.seeedstudio.com/wiki/xiaoesp32c3-chatgpt/9.png"/></div>

在新弹出的页面中选择 **+Create new secret key**，然后复制并妥善保存该密钥。

<div align=center><img width = 800 src="https://files.seeedstudio.com/wiki/xiaoesp32c3-chatgpt/10.png"/></div>

同时，我们可以在程序中创建一个字符串变量，并将该密钥复制到这里。

```c
char chatgpt_token[] = "sk**********Rj9DYiXLJJH";
```

> **提示**
>
> 根据原文在 2023 年 2 月 15 日时的说明，OpenAI 当时会向每位新用户赠送价值 **18 美元**的免费额度。详细费率可查看 [OpenAI 官方文档](https://openai.com/api/pricing/)。该信息可能已随时间变化，请以官方当前规则为准。
>
> <div align=center><img width = 800 src="https://files.seeedstudio.com/wiki/xiaoesp32c3-chatgpt/11.png"/></div>

**步骤 3.** 根据 OpenAI 的 HTTP 请求规范编写程序。

OpenAI 提供了非常详细的 [API 使用说明](https://platform.openai.com/docs/api-reference/making-requests)，以帮助用户使用自己的 API 密钥调用 ChatGPT。

根据 ChatGPT 文档，我们需要按照以下格式发送请求：

```shell
curl https://api.openai.com/v1/completions \
-H "Content-Type: application/json" \
-H "Authorization: Bearer YOUR_API_KEY" \
-d '{"model": "gpt-3.5-turbo-instruct", "prompt": "Say this is a test", "temperature": 0, "max_tokens": 7}'
```

超文本传输协议（HTTP）以请求—响应模式在客户端和服务器之间工作。

**GET** 用于从指定资源请求数据，常用于从 API 中获取值。

**POST** 用于向服务器发送数据，以创建或更新资源。

ESP32 可以使用三种常见的请求体格式发送 HTTP POST 请求：URL 编码数据、JSON 对象或纯文本。这些是最常用的方式，可以与大多数 API 或 Web 服务集成。

上述信息非常重要，它为编写 HTTP POST 程序提供了理论基础。首先，我们引入 HTTPClient 库。

```c
#include <HTTPClient.h>
```

还需要填写 OpenAI 的域名，以便 ESP 能够将问题发布给 ChatGPT。同时不要忘记填写 OpenAI API 密钥。

```c
HTTPClient https;

const char* chatgpt_token = "YOUR_API_KEY";
char chatgpt_server[] = "https://api.openai.com/v1/completions";
```

我们需要使用 JSON 对象发送 HTTP POST 请求。

```c
if (https.begin(chatgpt_server)) {  // HTTPS
    https.addHeader("Content-Type", "application/json"); 
    String token_key = String("Bearer ") + chatgpt_token;
    https.addHeader("Authorization", token_key);
    String payload = String("{\"model\": \"gpt-3.5-turbo-instruct\", \"prompt\": \"") + chatgpt_Q + String("\", \"temperature\": 0, \"max_tokens\": 100}"); //Instead of TEXT as Payload, can be JSON as Paylaod
    httpCode = https.POST(payload);   // start connection and send HTTP header
    payload = "";
}
else {
    Serial.println("[HTTPS] Unable to connect");
    delay(1000);
}
```

在该程序中，我们通过 `POST()` 方法将 `payload` 发送给服务器。`chatgpt_Q` 是我们希望发送给 ChatGPT 的问题内容，它来自前面获取问题的网页。

如果你希望了解 ESP32C3 HTTPClient 的更多功能，我们建议阅读 [使用 Arduino IDE 进行 ESP32 HTTP GET 和 HTTP POST](https://randomnerdtutorials.com/esp32-http-get-post-arduino/)。

## 获取 ChatGPT 的回答

接下来是整个教程的最后一步：如何获取并记录 ChatGPT 的回答。

我们继续阅读 OpenAI 提供的 [API 文档](https://platform.openai.com/docs/api-reference/making-requests)，了解 ChatGPT 返回消息的内容结构。这将帮助我们编写程序，从中解析出所需的内容。

```shell
{
  "id": "cmpl-uqkvlQyYK7bGYrRHQ0eXlWi7",
  "object": "text_completion",
  "created": 1589478378,
  "model": "gpt-3.5-turbo-instruct",
  "system_fingerprint": "fp_44709d6fcb",
  "choices": [
    {
      "text": "\n\nThis is indeed a test",
      "index": 0,
      "logprobs": null,
      "finish_reason": "length"
    }
  ],
  "usage": {
    "prompt_tokens": 5,
    "completion_tokens": 7,
    "total_tokens": 12
  }
}
```

通过 OpenAI 提供的参考文档，我们可以知道，接口返回消息中问题答案的位置是 `{"choices": [{"text": "\n\nxxxxxxx",}]}`。

因此，我们可以确定，所需的“答案”应当从 **\n\n** 开始，并在逗号前结束。然后，程序可以使用 `indexOf()` 方法查找文本的起始与结束位置，并存储返回的答案内容。

```c
dataStart = payload.indexOf("\\n\\n") + strlen("\\n\\n");
dataEnd = payload.indexOf("\",", dataStart); 
chatgpt_A = payload.substring(dataStart, dataEnd);
```

总结来说，我们可以结合程序的当前状态，使用 `switch` 语句判断接下来应执行哪一个步骤。

```c
typedef enum 
{
  do_webserver_index,
  send_chatgpt_request,
  get_chatgpt_list,
}STATE_;

STATE_ currentState;

switch(currentState){
    case do_webserver_index:
        ...
    case send_chatgpt_request:
        ...
    case get_chatgpt_list:
        ...
}
```

至此，整个程序的逻辑结构就已经完成了。可以单击下方图片获取完整的程序代码。请先不要急于上传程序，你需要将程序中的 **ssid、password 和 chatgpt_token** 替换为自己的内容。

<p style=":center"><a href="https://github.com/limengdu/xiaoesp32c3-chatgpt" target="_blank"><div align=center><img width = 300 src="https://files.seeedstudio.com/wiki/seeed_logo/github.png" /></div></a></p>

现在，尽情使用它吧！

<div align=center><img width = 800 src="https://files.seeedstudio.com/wiki/xiaoesp32c3-chatgpt/16.gif"/></div>

## 后续可以做什么？

在本教程中，我们介绍了如何使用 Arduino、XIAO ESP32C3 之类的嵌入式开发板调用 ChatGPT 接口的基本方法。接下来，就请尽情发挥你的创意吧！

例如，你是否可以考虑添加屏幕或键盘，把它制作成一台只属于你的独立显示设备？来看看 Gavin 的创意，他制作了一台非常有趣的监控设备！我们也特别感谢他为本教程提供了必要的步骤和思路。

- [Gavin - ChatGPT 记录器与监视器](https://www.hackster.io/gavinchiong/chatgpt-recorder-monitor-601ef6)

<div align=center><img width = 600 src="https://files.seeedstudio.com/wiki/xiaoesp32c3-chatgpt/14.jpg"/></div>

或者更进一步，添加语音识别模块，从此摆脱键盘和鼠标，制作一个属于你自己的语音助手等。总之，我们非常期待你能够与大家分享如何将这样一款优秀的产品用于自己的创意项目！

## 故障排查

### 问题 1：使用 XIAO ESP32C3 调用 OpenAI API 获取答案时，是否存在地区或网络方面的限制？

> 回答：根据原文作者在 2023 年 2 月 17 日进行的测试，当时在中国大陆和中国网络环境中也能非常顺畅地获得 ChatGPT 响应，当时尚无限制。只要能获取 ChatGPT API 密钥，就能顺利完成调用。该描述仅反映原文测试时的情况，当前可用性请以 OpenAI 的最新政策和实际网络测试为准。

## 技术支持

如果你需要技术支持，欢迎在我们的[论坛](https://forum.seeedstudio.com/)中发帖。

<br /><p style="text-align:center"><a href="https://www.seeedstudio.com/act-4.html?utm_source=wiki&utm_medium=wikibanner&utm_campaign=newproducts" target="_blank"><img src="https://files.seeedstudio.com/wiki/Wiki_Banner/new_product.jpg" /></a></p>
