#include <EEPROM.h> 
#include "Button.h"
Button knopki;

int A2_LedPin = 8;             // Подключаем А2 к порту d8
int A1_LedPin = 7;             // Подключаем А1 к порту d7
int A0_LedPin = 11;            // Подключаем А0 к порту d10
int EN_LedPin = 10;             // Подключаем EN к порту d9
int RegGPin = A7;               //  кнопка смена режима A7  подтянуть его к VCC 10ком резюком
int CVControl = A6;               //  вход для напряжения CVVV A6  подтянуть его к VCC 10ком резюком
int TrigPin = 12;               //  кнопку к выходу D12  подтянуть его к VCC 10ком резюком
int CVVV;
int REGGG;
int Rflag = 1;
int Tflag = 0;
int regim = 1;
int inputt = 1;
int regim_01 = 1;

bool temp_signal; // Переменная для импульсов
boolean signalStatus1 = false; //логический флаг предыдущего состояния сигнала включено/выключено. По умолчанию при старте программы выключено
const int ledPin =  13;
int ledState = LOW;
unsigned long previousMillis = 0; 
unsigned long previousMillis2 = 0; 
unsigned long currentMillis2;
int flag = 0;
int flag2 = 0;
const long interval = 200; //скорость мигания режимного светодиода в мс
int blynk = 0;
int x = 0; 
int y = 0;
int z = 0;
int high_kanal = 6;
int otrav = 10; // таймер антиотрава в секундах

// выводы для дешифратора К155ИД1
int out1 = 2;
int out2 = 5;
int out4 = 6;
int out8 = 4;

void setup() {
  
// задаем частоту ШИМ на 9 выводе 30кГц
TCCR1B=TCCR1B&0b11111000|0x01;
analogWrite(9,110);

digitalWrite(3,HIGH);
pinMode(ledPin, OUTPUT);

randomSeed(analogRead(6)); // Семя рандома берётся из шума на порту A0

Serial.begin(9600);

pinMode(3, OUTPUT); //OPTO
pinMode(2, OUTPUT); //155 1
pinMode(4, OUTPUT); //155 8
pinMode(5, OUTPUT); //155 2
pinMode(6, OUTPUT); //155 4
pinMode(9, OUTPUT); //TUBE
pinMode(12, INPUT); //TRIG
pinMode(13, OUTPUT); //REGLED
pinMode(A7, INPUT_PULLUP); //REG
pinMode(A6, INPUT); //CV
//=========================================================================================
knopki.NO(); // Нормально разомкнутые
knopki.pullUp(); // Подтяжка
knopki.duration_bounce       (  50 ); // Таймер дребезга ms
knopki.duration_press        ( 1000 ); // Таймер длительного нажатия ms
knopki.duration_inactivity_Up ( otrav*1000 ); //состояние таймера неактивности ненажатой кнопки ms, 1000ms = 1c
knopki.button( 14, 15, 16, 17, 18, 19 ); // Пины кнопок
//=========================================================================================
pinMode(A2_LedPin, OUTPUT);   // выводы на мультиплексор
pinMode(A1_LedPin, OUTPUT);   
pinMode(A0_LedPin, OUTPUT);
pinMode(EN_LedPin, OUTPUT);   

regim = EEPROM.read(0);
regim_01 = EEPROM.read(2);
high_kanal = EEPROM.read(1);
//Serial.print("setup_regim == "); Serial.println(regim);
//Serial.print("setup_regim_01 == "); Serial.println(regim_01);
}

void loop() {
  
knopki.read();  // Читаем кнопки

//====================== уточняем работу аналоговой кнопки для режимов======================
  REGGG = analogRead(RegGPin); // BTN
  REGGG = map(REGGG, 0, 512, 1, 0);
  //REGGG = constrain (REGGG, 0, 1);

  if(REGGG == 0 && Rflag == 0) { regim ++; EEPROM.write(2, 1); regim_01 = 1; Rflag = 1; if( regim > 6 ) { regim = 1; } EEPROM.write(0, regim); }
  if(REGGG == 1 && Rflag == 1) { Rflag = 0; }  
//==========================================================================================

//====================================Режимы работы=========================================
  if ( regim_01 == 0 )
    {
        regim = 0;
      if(digitalRead(TrigPin) == HIGH && !temp_signal ) 
      { 

        temp_signal = 1; 
        y++;
        if ( y == 1 ) { inputt = 1; digitalWrite(ledPin, LOW); }
        if ( y == 2 ) { inputt = 0; digitalWrite(ledPin, HIGH); }       
        if ( y > 1 ) { y = 0; }       
      } 
      else if (digitalRead(TrigPin) == 0 && temp_signal){temp_signal = 0; }  
  }
  
  if (regim == 1)
    {
      if(digitalRead(TrigPin) == HIGH && !temp_signal ) 
      { 
        if( inputt >= high_kanal) { inputt = 0; } 
        temp_signal = 1; 
        signalStatus1 = !signalStatus1;
        inputt ++;
      } 
      else if (digitalRead(TrigPin) == 0 && temp_signal){temp_signal = 0;}  
    }

  if (regim == 2)
    {
      if(digitalRead(TrigPin) == HIGH && !temp_signal ) 
      { 
        if( inputt <= 1 ) { inputt = high_kanal + 1; } 
        temp_signal = 1; 
        signalStatus1 = !signalStatus1;
        inputt --;
      } 
      else if (digitalRead(TrigPin) == 0 && temp_signal){temp_signal = 0;}  
    }

  if (regim == 3)
    {
      if(digitalRead(TrigPin) == HIGH && !temp_signal ) 
      { 
        if( inputt >= high_kanal) { inputt = 0; } 
        temp_signal = 1; 
        signalStatus1 = !signalStatus1;
        inputt ++;
        inputt ++;
      } 
      else if (digitalRead(TrigPin) == 0 && temp_signal){temp_signal = 0;}  
    }

  if (regim == 4)
    {
      if(digitalRead(TrigPin) == HIGH && !temp_signal ) 
      { 
        if( inputt <= 1) { inputt = high_kanal + 1; } 
        temp_signal = 1; 
        signalStatus1 = !signalStatus1;
        inputt --;
        inputt --;
      } 
      else if (digitalRead(TrigPin) == 0 && temp_signal){temp_signal = 0;}  
    }

  if (regim == 5)
         {
      if(digitalRead(TrigPin) == HIGH && !temp_signal ) 
      { 
        temp_signal = 1; 
        signalStatus1 = !signalStatus1;
        if ( x == 0 ) { inputt++; }
        if (inputt == high_kanal + 1 ) { x = 1; inputt = high_kanal; }
        if (x == 1 ) { inputt--; }
        if (inputt == 1 ) { x = 0; }
      } 
      else if (digitalRead(TrigPin) == 0 && temp_signal){temp_signal = 0;}  
    }
    
  if (regim == 6)
        {
      if(digitalRead(TrigPin) == HIGH && !temp_signal ) 
      { 
        temp_signal = 1; 
        signalStatus1 = !signalStatus1;
        if ( high_kanal >= 2 ) { inputt = random(1, high_kanal + 1); }
        if ( high_kanal < 2 ) { inputt = random(1, 3); }
      } 
      else if (digitalRead(TrigPin) == 0 && temp_signal){temp_signal = 0;}  
    }

switch (inputt) {
  case 0: signal0(); break;
  case 1: signal1(); break;
  case 2: signal2(); break;
  case 3: signal3(); break;
  case 4: signal4(); break;
  case 5: signal5(); break;
  case 6: signal6(); break;
  default: inputt = 1;
  }
//=======================================================================================

//============================ блок CV управления =======================================
CVVV = analogRead(CVControl); // CV
CVVV = map(CVVV, 0, 900, 0, high_kanal );

switch (CVVV) {
  case 1: regim_01 = 1; inputt = 1;  break;
  case 2: regim_01 = 1; inputt = 2;  break;
  case 3: regim_01 = 1; inputt = 3;  break;
  case 4: regim_01 = 1; inputt = 4;  break;
  case 5: regim_01 = 1; inputt = 5;  break;
  case 6: regim_01 = 1; inputt = 6;  break;
  }
//========================================================================================

//=============================== Короткое нажатие кнопок ================================
if (knopki.event_press_short  (0)  == 1) { inputt = 1; digitalWrite(ledPin, LOW); }
if (knopki.event_press_short  (1)  == 1) { inputt = 2; digitalWrite(ledPin, LOW); }
if (knopki.event_press_short  (2)  == 1) { inputt = 3; digitalWrite(ledPin, LOW); }
if (knopki.event_press_short  (3)  == 1) { inputt = 4; digitalWrite(ledPin, LOW); }
if (knopki.event_press_short  (4)  == 1) { inputt = 5; digitalWrite(ledPin, LOW); }
if (knopki.event_press_short  (5)  == 1) { inputt = 6; digitalWrite(ledPin, LOW); }
//========================================================================================

//================================= Мигание светодиодом ==================================
if ( blynk > 0 )
  {  
    unsigned long currentMillis = millis();

    if (currentMillis - previousMillis >= interval) 
    {
      previousMillis = currentMillis;
      if (ledState == LOW) { ledState = HIGH; } else { ledState = LOW; blynk++; }
      digitalWrite(ledPin, ledState);
    }
    if ( blynk > high_kanal + high_kanal - 1 ) { blynk = 0; }
  }
//========================================================================================

//=============================== Длинное нажатие кнопок =================================
if (knopki.event_press_long  (0)  == 1) { regim_01 = 0; inputt = 1; EEPROM.write(2, 0); }
if (knopki.event_press_long  (1)  == 1) { regim_01 = 1; regim = EEPROM.read(0); high_kanal = 2; inputt = 2; blynk = 2; digitalWrite(ledPin, LOW); EEPROM.write(1, high_kanal); }
if (knopki.event_press_long  (2)  == 1) { regim_01 = 1; regim = EEPROM.read(0); high_kanal = 3; inputt = 3; blynk = 3; digitalWrite(ledPin, LOW); EEPROM.write(1, high_kanal); }
if (knopki.event_press_long  (3)  == 1) { regim_01 = 1; regim = EEPROM.read(0); high_kanal = 4; inputt = 4; blynk = 4; digitalWrite(ledPin, LOW); EEPROM.write(1, high_kanal); }
if (knopki.event_press_long  (4)  == 1) { regim_01 = 1; regim = EEPROM.read(0); high_kanal = 5; inputt = 5; blynk = 5; digitalWrite(ledPin, LOW); EEPROM.write(1, high_kanal); }
if (knopki.event_press_long  (5)  == 1) { regim_01 = 1; regim = EEPROM.read(0); high_kanal = 6; inputt = 6; blynk = 6; digitalWrite(ledPin, LOW); EEPROM.write(1, high_kanal); }
//========================================================================================

//==================================== Антиотрав =========================================
if ( ( CVVV > 0 || digitalRead(TrigPin) == HIGH ) && flag == 0 ) { flag = 1; }
if ( CVVV == 0 && digitalRead(TrigPin) == LOW && flag == 1 ) { currentMillis2 = millis(); flag = 0; }
if ( CVVV == 0 && digitalRead(TrigPin) == LOW && millis() - currentMillis2 >= otrav * 1000 && flag2 == 0) 
{
  flag2 = 1;
  for ( int number = 0; number <= 9; number++ ) { antiotrav(number); }
  //for ( int number = 8; number >= 0; number-- ) { antiotrav(number); }
}
z = (millis()/1000) % (otrav+1);
if ( flag == 0 && z == 10 ) { flag2 = 0; }

//========================================================================================

} // Конец основного цикла

//========================== Вывод на лампы и переключатель ==============================
void signal0 () 
{
      digitalWrite(A2_LedPin, LOW);
      digitalWrite(A1_LedPin, LOW);
      digitalWrite(A0_LedPin, LOW); 
      digitalWrite(EN_LedPin, LOW);      
      digitalWrite (out1,LOW);
      digitalWrite (out2,LOW);
      digitalWrite (out4,LOW);
      digitalWrite (out8,LOW);
}
void signal1 () 
 {
      digitalWrite(A2_LedPin, HIGH);
      digitalWrite(A1_LedPin, LOW);
      digitalWrite(A0_LedPin, HIGH); 
      digitalWrite(EN_LedPin, HIGH);         
      digitalWrite (out1,HIGH);
      digitalWrite (out2,LOW);
      digitalWrite (out4,LOW);
      digitalWrite (out8,LOW);
}
void signal2 () 
{
      digitalWrite(A2_LedPin, HIGH);
      digitalWrite(A1_LedPin, LOW);
      digitalWrite(A0_LedPin, LOW); 
      digitalWrite(EN_LedPin, HIGH);       
      digitalWrite (out1,LOW);
      digitalWrite (out2,HIGH);
      digitalWrite (out4,LOW);
      digitalWrite (out8,LOW);
}
void signal3 () 
{
      digitalWrite(A2_LedPin, LOW);
      digitalWrite(A1_LedPin, HIGH);
      digitalWrite(A0_LedPin, HIGH); 
      digitalWrite(EN_LedPin, HIGH);     
      digitalWrite (out1,HIGH);
      digitalWrite (out2,HIGH);
      digitalWrite (out4,LOW);
      digitalWrite (out8,LOW);
}
void signal4 () 
{
      digitalWrite(A2_LedPin, LOW);
      digitalWrite(A1_LedPin, HIGH);
      digitalWrite(A0_LedPin, LOW); 
      digitalWrite(EN_LedPin, HIGH);    
      digitalWrite (out1,LOW);
      digitalWrite (out2,LOW);
      digitalWrite (out4,HIGH);
      digitalWrite (out8,LOW);       
}
void signal5 () 
{
      digitalWrite(A2_LedPin, LOW);
      digitalWrite(A1_LedPin, LOW);
      digitalWrite(A0_LedPin, HIGH); 
      digitalWrite(EN_LedPin, HIGH);        
      digitalWrite (out1,HIGH);
      digitalWrite (out2,LOW);
      digitalWrite (out4,HIGH);
      digitalWrite (out8,LOW);
}
void signal6 () 
{
      digitalWrite(A2_LedPin, LOW);
      digitalWrite(A1_LedPin, LOW);
      digitalWrite(A0_LedPin, LOW); 
      digitalWrite(EN_LedPin, HIGH);       
      digitalWrite (out1,LOW);
      digitalWrite (out2,HIGH);
      digitalWrite (out4,HIGH);
      digitalWrite (out8,LOW);
}
//=======================================Антиотрав========================================
void antiotrav(int num) 
{
  switch (num)
  {
    case 0:
    digitalWrite (out1,LOW);
    digitalWrite (out2,LOW);
    digitalWrite (out4,LOW);
    digitalWrite (out8,LOW);
    break;
    case 1:
    digitalWrite (out1,HIGH);
    digitalWrite (out2,LOW);
    digitalWrite (out4,LOW);
    digitalWrite (out8,LOW);
    break;
    case 2:
    digitalWrite (out1,LOW);
    digitalWrite (out2,HIGH);
    digitalWrite (out4,LOW);
    digitalWrite (out8,LOW);
    break;
    case 3:
    digitalWrite (out1,HIGH);
    digitalWrite (out2,HIGH);
    digitalWrite (out4,LOW);
    digitalWrite (out8,LOW);
    break;
    case 4:
    digitalWrite (out1,LOW);
    digitalWrite (out2,LOW);
    digitalWrite (out4,HIGH);
    digitalWrite (out8,LOW);
    break;
    case 5:
    digitalWrite (out1,HIGH);
    digitalWrite (out2,LOW);
    digitalWrite (out4,HIGH);
    digitalWrite (out8,LOW);
    break;
    case 6:
    digitalWrite (out1,LOW);
    digitalWrite (out2,HIGH);
    digitalWrite (out4,HIGH);
    digitalWrite (out8,LOW);
    break;
    case 7:
    digitalWrite (out1,HIGH);
    digitalWrite (out2,HIGH);
    digitalWrite (out4,HIGH);
    digitalWrite (out8,LOW);
    break;
    case 8:
    digitalWrite (out1,LOW);
    digitalWrite (out2,LOW);
    digitalWrite (out4,LOW);
    digitalWrite (out8,HIGH);
    break;
    case 9:
    digitalWrite (out1,HIGH);
    digitalWrite (out2,LOW);
    digitalWrite (out4,LOW);
    digitalWrite (out8,HIGH);
    break;
  }
  delay(50);
}
//========================================================================================
