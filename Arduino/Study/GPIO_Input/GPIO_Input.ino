// GPIO输入
// XIAO ESP32 S3 板载 Key: GPIO_0
// 按键0:配置上拉
static int GPIO_KEY_0 = 0 ;

void setup() 
{
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(GPIO_KEY_0 , INPUT_PULLUP);
}

void loop() 
{
  int key = digitalRead(GPIO_KEY_0) ;
  if (key == 1)  // key == 1
  {
    digitalWrite(LED_BUILTIN, LOW);  // LED是输出HIGH的时候熄灭,输出LOW的时候点亮
  }
  else      // key == 0
  {
    digitalWrite(LED_BUILTIN, HIGH);   // turn the LED off by making the voltage LOW
  }
}
