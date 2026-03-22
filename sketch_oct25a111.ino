// 引脚定义
int lightSensor = 3;           // 光敏传感器DO接数字引脚3
int led = 9;                   // LED接数字引脚9  
int button = 2;                // 按钮接数字引脚2

// 模式定义
int mode = 0; // 0：自动, 1：常亮, 2:呼吸灯

//按钮防抖变量
int lastButtonState = HIGH;    //上次按钮状态，初始为HIGH
int currentButtonState = HIGH; //当前按钮状态，初始为HIGH
long markTime1 = 0;            //上次按下按钮的时间
long waitTime1 = 50;           //防抖延迟为50毫秒
bool buttonPressed = false;    //按钮按下的标志

//自动模式变量，传感器防抖
int lastLightState = HIGH;     //定义上次光线状态，初始为HIGH
long markTime3=0;              //上次光线状态变化的时间
long stableTime=300;           //设置300毫秒的稳定时间

//呼吸灯变量
int brightness = 0;             //亮度值
int breathSpeed = 10;           //呼吸灯变化速度为10毫秒
long markTime2 = 0;             //上次呼吸灯状态变化的时间 
bool increase = true;           //亮度变化方向


void setup() {
  // put your setup code here, to run once:
  
// 设置引脚模式
  pinMode(led, OUTPUT);
  pinMode(button, INPUT_PULLUP);
  pinMode(lightSensor, INPUT); 
  
  // 启动串口调试
  Serial.begin(9600);
  Serial.println("三模式智能灯");
  Serial.println("模式0:自动开关  模式1:常亮  模式2:呼吸灯");
  Serial.println("初始为自动模式");
}

void loop() {
  // put your main code here, to run repeatedly:
  
  //处理按钮
  handleButton();
  
  // 根据当前模式执行相应功能
  switch(mode) {
    case 0: 
    autoMode();
    break;  
    case 1: 
    alwaysOnMode();
    break;
    case 2:
    breathMode();
    break;
  }
}
void  handleButton(){
  currentButtonState=digitalRead(button);
  
  if(currentButtonState==LOW&&lastButtonState==HIGH){
    markTime1=millis();//记录按下的时间，用于防抖
    buttonPressed=true;
    }
    lastButtonState=currentButtonState;//更新按钮状态
  //防抖处理
  if((millis()-markTime1)>waitTime1&&buttonPressed==true){
    mode=(mode+1)%3;//切换模式：0-1-2-0循环

    //打印切换的模式
    Serial.print("按钮按下，当前模式：");
    switch(mode){
      case 0:
      Serial.println("自动模式");
      break;
      case 1:
      Serial.println("常亮模式");
      break;
      case 2:
      Serial.println("呼吸灯模式");
      break;
      }
      
    //重置按钮按下标志
    buttonPressed=false;

    //模式切换时LED会闪烁一下
    digitalWrite(led,LOW);
    delay(100);
    digitalWrite(led,HIGH);
    delay(100);
    digitalWrite(led,LOW);
    delay(100);
    }
}

//自动模式函数
void autoMode() {
  // 读取传感器状态
  // HIGH = 光线充足, LOW = 光线不足
  int lightState = digitalRead(lightSensor);
  
  if(lightState!=lastLightState){
      if(markTime3==0){//避免重复记录时间
        markTime3=millis();//记录时间，用于防抖
      }
     
     
    //只有光线状态变化持续时间超过300毫秒才认为是真正的变化,防止光线微小波动导致LED状态频繁变化
    if((millis()-markTime3)>stableTime){
      // 只有状态变化时才打印，避免串口输出过多      
        if (lightState == LOW) {
          Serial.println("光线不足，LED开启");
          digitalWrite(led,HIGH);//点亮LED
        } else {
          Serial.println("光线充足，LED关闭");  
          digitalWrite(led,LOW);//关闭LED
        }  
    
         lastLightState = lightState;//更新光线状态  
         markTime3=0;//重置计时器时间，表示当前状态稳定              
    } 
  }else {
      // 如果光线状态和上次确认的状态一致，则重置计时器，表示当前状态稳定
      //清除临时变化记录，确保下次计时准确
      markTime3 = 0;
  }
}
// 常亮模式函数
void alwaysOnMode(){
  digitalWrite(led,HIGH);//LED持续点亮
}

// 呼吸灯模式函数
void breathMode(){
  //非阻塞式呼吸灯，只有在经过了指定时间后才会变化亮度，且能随时响应按钮变化
  if(millis()-markTime2>breathSpeed){
    markTime2=millis();
    //根据方向调整亮度
    if(increase){
      brightness++;        //增加亮度
      if(brightness>=255){
        increase=false;     //达到最大亮度改为减小
        }
      }else{
        brightness--;       //减小亮度
        if(brightness<=0){
          increase=true;   //达到最大亮度改为增加
          }
        }
    
      analogWrite(led,brightness);//设置LED亮度
    }
  }
 
