// 串口调试
// 按键0:上拉
static int GPIO_KEY_0 = 0 ;

void setup() 
{
  // 初始化按键
  pinMode(GPIO_KEY_0, INPUT_PULLUP);
  // 初始化串口
  Serial.begin(115200) ;
}

void loop() 
{
  int key0 = digitalRead(GPIO_KEY_0) ;
  Serial.println(key0); // 按下输出0,松手输出1
}
