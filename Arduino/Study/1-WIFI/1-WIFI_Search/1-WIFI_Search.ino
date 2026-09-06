// Template工程:模板工程

#include "Initial.h"
#include "WiFi.h"

void setup() 
{
    Initial_All();
    // 串口配置
    Serial.begin(115200);
    // WIFI配置
    WiFi.mode(WIFI_STA);        // 打开STA模式
    WiFi.disconnect();          // 断开WIFI连接
    delay(100);

    Serial.println("Setup done");
}


void loop() 
{
    Serial.println("scan start");

    // WiFi.scanNetworks will return the number of networks found
    int n = WiFi.scanNetworks();
    Serial.println("scan done");
    if (n == 0) 
    {
        Serial.println("no networks found");
    } 
    else 
    {
        Serial.print(n);
        Serial.println(" networks found");
        for (int i = 0; i < n; ++i) 
        {
            // Print SSID and RSSI for each network found
            Serial.print(i + 1);
            Serial.print(": ");
            Serial.print(WiFi.SSID(i));
            Serial.print(" (");
            Serial.print(WiFi.RSSI(i));
            Serial.print(")");
            Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN)?" ":"*");
            delay(10);
        }
    }
    Serial.println("");

    // Wait a bit before scanning again
    delay(5000);
    
}
