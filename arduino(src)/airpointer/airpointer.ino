/***********************************
    Title : MIRO airpointer project
    Copyright(C) 2016 MIRO KyongPook Univ
     
    하현수, 박민규, 황인득,  이재훈, 이동은
    update : 2016.11.15. 22:05
***********************************/

/**********************************
       [+] Import Library
**********************************/

#include <Wire.h>
#include <MPU6050.h>
/**********************************
    [+] Global & Const Variables
**********************************/

#define Drawing_button 7                //Button Pin (PULL-UP Button)
#define Click_button 8
#define ZoomIn_button 9
#define Motion_button 10
#define Lazer_button 11

#define key_press_delay 30              //key & Mouse Motion delay time
#define mouse_press_delay 50
#define Hardware_delay 10               //Hardware Delay

// Data protocol
#define DATA_MOTION 1
#define DATA_ZOOMIN 2
#define DATA_DRAWING 3
#define DATA_PASSPAGE 4

/**********************************
        [+] Function
***********************************/

void Button_setup();                    //Pull-up Digital Button
void MPU_setup();                       //MPU6050 Init Setup
short Check_X(int Data);                //Calculate X position
short Check_Y(int Data);                //Calculate Y position

boolean drawing_flag = false;
boolean zoomin_flag = false;

void setup(){                               //Hardware Setup

    Serial1.begin(9600);                     //bluetooth serial 9600 baudrate
    Button_setup();
    MPU_setup();
    delay(2000);
    
}

void loop(){                                //Main Loop Proc

    //NOTE : Must import Low Battery System Cuz, overhead very big in this loop
    
    int16_t* Data_Stack;
    Data_Stack = (int16_t*)malloc(7*sizeof(int16_t));
    

    while(1){
        
        short X = 0;
        short Y = 0;

        Wire.beginTransmission(0x68);         //Begin MPU
        Wire.write(0x3B);
        Wire.endTransmission(false);             //Sustain connection
        Wire.requestFrom(0x68, 14, true);

        Data_Stack[0] = Wire.read() << 8 | Wire.read();  // X acc data
        Data_Stack[1] = Wire.read() << 8 | Wire.read();  // Y acc data
        Data_Stack[2] = Wire.read() << 8 | Wire.read();   // Z acc data
        Data_Stack[3] = Wire.read() << 8 | Wire.read();   // Temp
        Data_Stack[4] = Wire.read() << 8 | Wire.read();   // X gyro data select
        Data_Stack[5] = Wire.read() << 8 | Wire.read();   // Y gyro data
        Data_Stack[6] = Wire.read() << 8 | Wire.read();   // Z gyro data select

        X = Check_X(Data_Stack[6]);
        Y = Check_Y(Data_Stack[4]);
        

        if(digitalRead(Click_button) == LOW){              //Click Function
            
            Serial1.print(4);
            Serial1.print('/');
            Serial1.print(X);
            Serial1.print('/');
            Serial1.print(Y);
            Serial1.print('/');
            Serial1.print('&');    

            zoomin_flag = ~zoomin_flag + 2;

            delay(1000);
        }

        if(digitalRead(Drawing_button) == LOW){

            Serial1.print(3);
            Serial1.print('/');
            Serial1.print(X);
            Serial1.print('/');
            Serial1.print(Y);
            Serial1.print('/');
            Serial1.print('&');

            drawing_flag = ~drawing_flag + 2;

            delay(1000);
            
        }

        if(digitalRead(ZoomIn_button) == LOW ){             //ZoomIn Function

            Serial1.print(2);
            Serial1.print('/');
            Serial1.print(X);
            Serial1.print('/');
            Serial1.print(Y);
            Serial1.print('/');
            Serial1.print('&'); 
            delay(1000);       
            
        }

        if(digitalRead(Motion_button) == LOW){              //Motion Control

            Serial1.print(1);
            Serial1.print('/');
            Serial1.print(X);
            Serial1.print('/');
            Serial1.print(Y);
            Serial1.print('/');
            Serial1.print('&');
                  
        }
        else{
        
            if( drawing_flag == true || zoomin_flag == true ){

                Serial1.print(7);
                Serial1.print('/');
                Serial1.print(X);
                Serial1.print('/');
                Serial1.print(Y);
                Serial1.print('/');
                Serial1.print('&');

            }
        }
    }
  
    free(Data_Stack);                                     // Data-Stack memory free
}


void Button_setup(){                                      //Pull-up Digital Button;

    //XXX : Must script base on DDR, PORT register
    pinMode(Drawing_button,INPUT);
    pinMode(Click_button ,INPUT);
    pinMode(ZoomIn_button,INPUT);
    pinMode(Motion_button,INPUT);
    pinMode(Lazer_button,OUTPUT);

    digitalWrite(Drawing_button,HIGH);
    digitalWrite(Click_button ,HIGH);
    digitalWrite(ZoomIn_button,HIGH);
    digitalWrite(Motion_button,HIGH);
}

void MPU_setup(){                           //MPU6050 Init Setup

    Wire.begin();
    Wire.beginTransmission(0x68);
    Wire.write(0x6B);
    Wire.write(0);
    Wire.endTransmission(true);
    delay(30);
}

short Check_X(int Data) {

            short Xval;  

            if(Data > 1000) {

              Xval = -2-Data / 1000;
            }
            else if( Data < -1000) {
            
              Xval = 2-Data / 1000;
            }
            else if(Data > 500 && Data < 1000)  {                
              
              Xval = -3;               
            }
            else if(Data > -1000 && Data < -500) {
              
              Xval = 3;
            }
            else {
                
              Xval = 0;
            }
            

            return Xval;
}


short Check_Y(int Data) {
  
            short Yval;

            if(Data > 1000) {
            
              Yval = Data / 1000;
            }
            else if(Data < -1000){
            
              Yval = Data / 1000;
            }
            else if(Data > 500 && Data < 1000)  {                
              
              Yval = 1;               
            }
            else if(Data > -1000 && Data < -500) {
                
                Yval = -1;
             }
            else {
                
                Yval = 0;
            }

            return Yval;
 }
