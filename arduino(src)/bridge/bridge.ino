/***********************************
    Title : MIRO airpointer project
    Copyright(C) 2016 MIRO KyongPook Univ
     
    하현수, 박민규, 황인득,  이재훈, 이동은
    update : 2016.12.05 12:56
***********************************/

/**********************************
       [+] Import Library
**********************************/

#include <Wire.h>
#include <MPU6050.h>
#include <stdio.h>
/**********************************
    [+] Global & Const Variables
**********************************/

#define Motion_button 12
#define ZoomIn_button 11
#define Drawing_button 10             
#define Next_Page_button 9
#define Back_Page_button 8
#define STATE_LED 7
#define Lazer_button 6

#define key_press_delay 30              //key & Mouse Motion delay time
#define mouse_press_delay 50
#define Hardware_delay 10               //Hardware Delay
#define LOCK_DRURATION 200
// Data protocol
#define DATA_MOTION 1
#define DATA_ZOOMIN 2
#define DATA_DRAWING 3
#define DATA_PASSPAGE 4
#define DATA_BACKPAGE 5
#define DATA_DEFAULT 7
#define DATA_UPDATE 8
/**********************************
        [+] Function
***********************************/

void Button_setup();                    //Pull-up Digital Button
void MPU_setup();                       //MPU6050 Init Setup
short Check_X(int Data);                //Calculate X position
short Check_Y(int Data);                //Calculate Y position

boolean drawing_flag = false;
boolean zoomin_flag = false;

//Program Locker
unsigned int lock = 0;
boolean lock_check  = false;
unsigned short count_update = 0;

void setup(){                               //Hardware Setup

    Serial1.begin(9600);                     //bluetooth serial 9600 baudrate
    Button_setup();
    MPU_setup();
    PORTE = 0x40;
    delay(2000);
    PORTE = 0x00;
}

void loop(){                                //Main Loop Proc

    //NOTE : Must import Low Battery System Cuz, overhead very big in this loop
    
    int16_t* Data_Stack;
    Data_Stack = (int16_t*)malloc(7*sizeof(int16_t));
    

    while(1){


        /*
         * Locker : 카운트 레지스터를 이용한 프로그래밍 적인 딜레이 연출
         * No delay() API
        */
        if(lock_check == true){
              
              lock++;
              
              if(lock > LOCK_DRURATION){
                lock = 0;
                lock_check = false;
                PORTE = 0x00;
              }      
        }
        
        short X = 0;
        short Y = 0;
        String packet = "";

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
       

        if(digitalRead(Next_Page_button) == LOW && lock == 0){              //Click Function

            packet = packet + "*";
            packet = packet  + DATA_PASSPAGE; 
            packet = packet + "/";
            packet = packet + X ;
            packet = packet + "/";
            packet = packet + Y;
            packet = packet + "/";
            packet = packet + "*";
            Serial1.println(packet); 
            
            lock_check = true;
            lock++;
            PORTE = 0x40;
        }
        else if(digitalRead(Back_Page_button) == LOW && lock == 0){              //Click Function
            
            packet = packet + "*";
            packet = packet  + DATA_BACKPAGE; 
            packet = packet + "/";
            packet = packet + X ;
            packet = packet + "/";
            packet = packet + Y;
            packet = packet + "/";
            packet = packet + "*";
            Serial1.println(packet); 
            
            lock_check = true;
            lock++;
            PORTE = 0x40; 
        }
        else if(digitalRead(Drawing_button) == LOW  && lock == 0){

            drawing_flag = ~drawing_flag + 2;
            
            packet = packet + "*";
            packet = packet  + DATA_DRAWING ; 
            packet = packet + "/";
            packet = packet + drawing_flag ;
            packet = packet + "/";
            packet = packet + drawing_flag;
            packet = packet + "/";
            packet = packet + "*";
            Serial1.println(packet);
            
            lock++;
            lock_check = true;
            PORTE = 0x40;
            
        }
        else if(digitalRead(ZoomIn_button) == LOW  && lock == 0){             //ZoomIn Function

            zoomin_flag = ~zoomin_flag + 2;
            
            packet = packet + "*";
            packet = packet  + DATA_ZOOMIN; 
            packet = packet + "/";
            packet = packet + zoomin_flag ;
            packet = packet + "/";
            packet = packet + zoomin_flag;
            packet = packet + "/";
            packet = packet + "*";
            Serial1.println(packet); 

            lock++;
            lock_check = true;
            PORTE = 0x40;
        }
        else if(digitalRead(Motion_button) == LOW){              //Motion Control

            packet = packet + "*";
            packet = packet  + DATA_MOTION; 
            packet = packet + "/";
            packet = packet + X ;
            packet = packet + "/";
            packet = packet + Y;
            packet = packet + "/";
            packet = packet + "*";
            Serial1.println(packet); 
            
        }
        else{
       
            if( drawing_flag == true || zoomin_flag == true ){

            packet = packet + "*";
            packet = packet  + DATA_DEFAULT; 
            packet = packet + "/";
            packet = packet + X ;
            packet = packet + "/";
            packet = packet + Y;
            packet = packet + "/";
            packet = packet + "*";
            Serial1.println(packet); 
            
            }
        
        }

        count_update ++;
        if(count_update >= 50){
          
            packet = packet + "*";
            packet = packet  + DATA_UPDATE; 
            packet = packet + "/";
            packet = packet + zoomin_flag ;
            packet = packet + "/";
            packet = packet + drawing_flag ;
            packet = packet + "/";
            packet = packet + "*";
            Serial1.println(packet); 

            count_update = 0;
          
        }

       
    }
  
    free(Data_Stack);                                     // Data-Stack memory free
}


void Button_setup(){                                      //Pull-up Digital Button;

    //FIXME : Must script base on DDR, PORT register

    pinMode(Drawing_button,INPUT);  
    pinMode(Next_Page_button ,INPUT);   
    pinMode(ZoomIn_button,INPUT);   
    pinMode(Motion_button,INPUT);   
    pinMode(Back_Page_button , INPUT);
    pinMode(Lazer_button,OUTPUT);  
    pinMode(STATE_LED,OUTPUT);  

    digitalWrite(Drawing_button,HIGH); 
    digitalWrite(Next_Page_button ,HIGH);  
    digitalWrite(ZoomIn_button,HIGH);  
    digitalWrite(Back_Page_button, HIGH);
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
            

            return -Xval;
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
