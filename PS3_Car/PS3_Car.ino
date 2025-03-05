#include <Ps3Controller.h>

//bts kanan
int lpwm_1 = 13;    //13
int rpwm_1 = 12;    //12

//bts kiri
int lpwm_2 = 14;    //14
int rpwm_2 = 27;    //27

void notify()
{
  int yAxisValue =(Ps3.data.analog.stick.ly);  //Left stick  - y axis - forward/backward car movement
  int xAxisValue =(Ps3.data.analog.stick.rx);  //Right stick - x axis - left/right car movement

  if (yAxisValue <= -50)       //MAJU
  {
    maju();
  }
  else if (yAxisValue >= 50)   //MUNDUR
  { 
    mundur();
  }
  else if (xAxisValue >= 50)  //KANAN
  {
    kanan();
  }
  else if (xAxisValue <= -50)   //KIRI
  {
    kiri();
  }
  else                         //BERHENTI
  {
    berhenti();
  } 
}

void onConnect()
{
  Serial.println("Connected!.");
}

void onDisConnect()
{
  berhenti();
}

void maju(){
  //KANAN

  digitalWrite(rpwm_1, 100);
  digitalWrite(lpwm_1, 0);
  //KIRI

  digitalWrite(rpwm_2, 100);
  digitalWrite(lpwm_2, 0);  
}

void mundur(){
  //KANAN

  digitalWrite(rpwm_1, 0);
  digitalWrite(lpwm_1, 100);
  //KIRI

  digitalWrite(rpwm_2, 0);
  digitalWrite(lpwm_2, 100);  
}

void kanan(){
  //KANAN

  digitalWrite(rpwm_1, 100);
  digitalWrite(lpwm_1, 0);
  //KIRI

  digitalWrite(rpwm_2, 0);
  digitalWrite(lpwm_2, 200);  
}

void kiri(){
  //KANAN

  digitalWrite(rpwm_1, 0);
  digitalWrite(lpwm_1, 200);
  //KIRI

  digitalWrite(rpwm_2, 100);
  digitalWrite(lpwm_2, 0); 
}

void berhenti(){
  //KANAN

  digitalWrite(rpwm_1, 0);
  digitalWrite(lpwm_1, 0);
  //KIRI

  digitalWrite(rpwm_2, 0);
  digitalWrite(lpwm_2, 0); 
}

void setup()
{
  //bts kanan
  pinMode(lpwm_1, OUTPUT);
  pinMode(rpwm_1, OUTPUT);

  //bts kiri
  pinMode(lpwm_2, OUTPUT);
  pinMode(rpwm_2, OUTPUT);

  Serial.begin(115200);
  Ps3.attach(notify);
  Ps3.attachOnConnect(onConnect);
  Ps3.attachOnDisconnect(onDisConnect);
  Ps3.begin("d4:d4:da:5c:2b:6a");
  Serial.println("Ready.");
}

void loop()
{
}
