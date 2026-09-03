// 串口调试
// 按键0:上拉
static int GPIO_KEY_0 = 0 ;

void setup() 
{
  // 初始化串口
  Serial.begin(115200) ;
  // 初始化ADC:这里没有显示是初始化哪个ADC,事实上读取的时候才会再ADC_Start()
  analogReadResolution(12); // 初始化ADC与位数(12位)
}

void loop() 
{
  // 读取ADC
  int analog_Data = analogRead(1) ; // GPIO1->A1
  int analog_Volt = analogReadMilliVolts(1) ; // 毫伏
  // 打印ADC的值
  Serial.printf("analog_Data = %d\n",analog_Data) ;
  Serial.printf("analog_Volt = %d\n",analog_Volt) ;
  // 延时
  delay(100) ;
}
