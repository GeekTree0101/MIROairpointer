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
#include <stdio.h>
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
#define DATA_MOTION "D"
#define DATA_ZOOMIN "C"
#define DATA_DRAWING "B"
#define DATA_PASSPAGE "A"

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
        char buff[7];

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
            
            sprintf(buff, "%s/%d/%d/",DATA_PASSPAGE,X,Y);
            Serial1.println(buff); 

            zoomin_flag = ~zoomin_flag + 2;

            delay(1000);
        }

        if(digitalRead(Drawing_button) == LOW){

            sprintf(buff, "%s/%d/%d/",DATA_DRAWING ,X,Y);
            Serial1.println(buff); 

            drawing_flag = ~drawing_flag + 2;

            delay(1000);
            
        }

        if(digitalRead(ZoomIn_button) == LOW ){             //ZoomIn Function

            sprintf(buff, "%s/%d/%d/",DATA_ZOOMIN ,X,Y);
            Serial1.println(buff); 
            delay(1000);       
            
        }

        if(digitalRead(Motion_button) == LOW){              //Motion Control

            sprintf(buff, "%s/%d/%d/",DATA_MOTION ,X,Y);
            Serial1.println(buff); 
        }
        else{
        
            if( drawing_flag == true || zoomin_flag == true ){

            sprintf(buff, "%s/%d/%d/","N",X,Y);
            Serial1.println(buff); 
            }
        }
    }
  
    free(Data_Stack);                                     // Data-Stack memory free
}


void Button_setup(){                                      //Pull-up Digital Button;

    //FIXME : Must script base on DDR, PORT register

    pinMode(Drawing_button,INPUT);  //DDRE.6 0b 0X00 0000 X:1  (1 = INPUT)
    pinMode(Click_button ,INPUT);   //DDRB.4 0b 000X 0000 X:1
    pinMode(ZoomIn_button,INPUT);   //DDRB.5 0b 00X0 0000 X:1
    pinMode(Motion_button,INPUT);   //DDRB.6 0b 0X00 0000 X:1
    pinMode(Lazer_button,OUTPUT);   //DDRB.7 0b X000 0000 X:0  (0 = OUTPUT)
    
    //DDRE = 0x40;
    //DDRB = 0x70;  

    digitalWrite(Drawing_button,HIGH); //PORTE.6 0b0X00 0000 X:1
    digitalWrite(Click_button ,HIGH);  //PORTB.4
    digitalWrite(ZoomIn_button,HIGH);  //PORTB.5
    digitalWrite(Motion_button,HIGH);  //PORTB.6 0bXXXX 0000 X:1 (1 = HIGH)

    //PORTB = 0xF0;
    //PORTE = 0x40;
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
