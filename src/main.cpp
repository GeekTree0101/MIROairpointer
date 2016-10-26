
/***********************************

    Title : MIRO airpointer project
    Copyright(C) 2016 MIRO KyongPook Univ
     
    하현수, 박민규, 황인득,  이재훈, 이동은
    update : 2016.10.25 23:58

***********************************/

/**********************************

       [+] Import Library

**********************************/

#include <Wire.h>
#include <Mouse.h>
#include <Keyboard.h>
#include <MPU6050.h>

/**********************************

    [+] Global & Const Variables

**********************************/

#define BUTTON_IO PORTB                 //Button Bport I/O Register
#define BUTTON_DIR DDRB                 //Button Bport DIR Register

#define Z_key 122                       //KeyValue Data
#define O_key 79
#define H_key 72

#define Drawing_button 7                //Button Pin (PULL-UP Button)
#define Click_button 8
#define ZoomIn_button 9
#define Motion_button 10

#define key_press_delay 30              //key & Mouse Motion delay time
#define mouse_press_delay 80
#define Hardware_delay 10               //Hardware Delay

boolean flag = false;                   //Drawing Flag

/**********************************

        [+] Function

***********************************/

void Button_setup();                    //Pull-up Digital Button
void MPU_setup();                       //MPU6050 Init Setup
void Mouse_interface_setup();           //Mouse Init Setup
void Keyboard_interface_setup();        //Keyboard Init Setup
void Click_event();                     //Click event Function
void Drawing_event();                   //Drawing event Function
void ZoomIn_event();                    //Zoomin event Function

void setup(){                               //Hardware Setup

    Button_setup();
    MPU_setup();
    Mouse_interface_setup();
    Keyboard_interface_setup();
}

void loop(){                                //Main Loop Proc

    //XXX : Must import Low Battery System Cuz, overhead very big in this loop
    
    int16_t* Data_Stack;
    Data_Stack = (int16_t*)malloc(6*sizeof(int16_t));
    

    while(1){
    
        Wire.beginTransmission(0x68);         //Begin MPU
        Wire.write(0x3B);
        Wire.endTransmission(false);             //Sustain connection
        Wire.requestFrom(0x68, 14, true);

        Data_Stack[0] = Wire.read() << 8 | Wire.read();  // X pos data
        Data_Stack[1] = Wire.read() << 8 | Wire.read();  // Y pos data

        //HACK : If you need more special function then using these data
        Data_Stack[2] = Wire.read() << 8 | Wire.read();   
        Data_Stack[3] = Wire.read() << 8 | Wire.read();
        Data_Stack[4] = Wire.read() << 8 | Wire.read();  
        Data_Stack[5] = Wire.read() << 8 | Wire.read();  
        
        if(digitalRead(Click_button) == LOW){              //Click Function

            Click_event();
        }

        if(digitalRead(Drawing_button) == LOW){            //Drawing Function

            Drawing_event();
        }

        if(digitalRead(ZoomIn_button) == LOW){             //ZoomIn Function

            ZoomIn_event();
        }

        if(digitalRead(Motion_button) == LOW){              //Motion Control
            
            //TODO : Don't make it as Function
            //       Due to OverHead during make a new stack on the memory

            short X = 0;
            short Y = 0;
            
            if(Data_Stack[0] > 10000){                      // Left/Right Motion
                                     
              X = 5 + (Data_Stack[0] % 10000) / 1000;
            } 
            else if(Data_Stack[0] < -10000){

              X = -5 + (Data_Stack[0] % 10000) / 1000;
            }
            
            if(Data_Stack[1] > 10000){                      // Up/Down Motion
                                      
               Y = 5 + (Data_Stack[1] % 10000) / 1000;
            }
            else if(Data_Stack[1] < -10000){

              Y = -5 + (Data_Stack[1] % 10000) / 1000;
            }            

            /* 
            Serial.print(X);                              //Debug : X,Y pos Serial Debug
            Serial.print("|");
            Serial.print(Y);
            Serial.println("");
            */

            if(flag){                                     // Flag = true : Drawing
            
              Mouse.click(MOUSE_LEFT);                    
            }
            
            Mouse.move(X,Y,0);                            // Mouse Pointer moving method
        }

        delay(Hardware_delay);                            // Loop-Proc delay time
    }


    free(Data_Stack);                                     // Data-Stack memory free
}


void Button_setup(){                        //Pull-up Digital Button;

    //XXX : Must script base on DDR, PORT register
    pinMode(Drawing_button,INPUT);
    pinMode(Click_button ,INPUT);
    pinMode(ZoomIn_button,INPUT);
    pinMode(Motion_button,INPUT);

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

void Mouse_interface_setup(){               //Mouse Init Setup

    Mouse.begin();
    delay(30);
}

void Keyboard_interface_setup(){            //Keyboard Init Setup

    Keyboard.begin();
    delay(30);
}

void Click_event(){                         //Click event Function

    Mouse.click(MOUSE_LEFT);
    delay(500);
}

void Drawing_event(){                       //Drawing event Function
    
    Mouse.click(MOUSE_RIGHT);
    delay(mouse_press_delay);
    Keyboard.press(O_key);
    delay(key_press_delay);
    Keyboard.release(O_key);          
    delay(key_press_delay);
    Keyboard.press(H_key);
    delay(key_press_delay);
    Keyboard.release(H_key);  

    if(flag){
        flag = false;
    }
    else{
        flag = true;
    }
    delay(1000);

}

void ZoomIn_event(){                       //Zoomin event Function

    Mouse.click(MOUSE_RIGHT);
    delay(mouse_press_delay);
    Keyboard.press(Z_key);
    delay(key_press_delay);
    Keyboard.release(Z_key);
}

