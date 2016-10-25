

/*
    MIRO airpointer project
    Copyright(C) 2016 MIRO
    provider : GeekTree0101
    update : 2016.10.24s
*/
#include <Wire.h>
#include <Mouse.h>
#include <Keyboard.h>
#include <MPU6050.h>


//Definition or const value area

//#define BUTTON_IO PORTB
//#define BUTTON_DIR DDRB
boolean flag = false;
//Function area

void Button_setup();
void MPU_setup();
void Mouse_interface_setup();
void Keyboard_interface_setup();

//Setup

void setup(){

    Button_setup();
    MPU_setup();
    Mouse_interface_setup();
    Keyboard_interface_setup();
}

//Main loop proc

void loop(){

    //XXX : Must import Low Battery System Cuz, overhead very big in this loop
    int16_t* Acc_value;
    Acc_value = (int16_t*)malloc(6*sizeof(int16_t));
    

    while(1){
    
        Wire.beginTransmission(0x68);         //Begin MPU
        Wire.write(0x3B);
        Wire.endTransmission(false);             //Sustain connection
        Wire.requestFrom(0x68, 14, true);

        Acc_value[0] = Wire.read() << 8 | Wire.read();  // X pos data
        Acc_value[1] = Wire.read() << 8 | Wire.read();  // Y pos data
        Acc_value[2] = Wire.read() << 8 | Wire.read();
        Acc_value[3] = Wire.read() << 8 | Wire.read();
        Acc_value[4] = Wire.read() << 8 | Wire.read();  // z acc
        Acc_value[5] = Wire.read() << 8 | Wire.read();  //
        
        if(digitalRead(8) == LOW){              //Button Click

            Mouse.click(MOUSE_LEFT);
            delay(500);
        }

        if(digitalRead(7) == LOW){
           Mouse.click(MOUSE_RIGHT);
           delay(80);
           Keyboard.press(79);
           delay(30);
           Keyboard.release(79);          
           delay(30);
           Keyboard.press(72);
           delay(30);
           Keyboard.release(72);  

           if(flag){
            flag = false;
           }
           else{
            flag = true;
           }
           delay(1000);
        }

        if(digitalRead(9) == LOW){

           Mouse.click(MOUSE_RIGHT);
           delay(80);
           Keyboard.press(122);
           delay(30);
           Keyboard.release(122);
        }

        if(digitalRead(10) == LOW){              //Gyro action
            int X = 0;
            int Y = 0;
            
            if(Acc_value[0] > 10000){                      //left right
              X = 5 + (Acc_value[0] % 10000) / 1000;
            } 
            else if(Acc_value[0] < -10000){
              X = -5 + (Acc_value[0] % 10000) / 1000;
            }
            
            if(Acc_value[1] > 10000){                       // up down
               Y = 5 + (Acc_value[1] % 10000) / 1000;
            }
            else if(Acc_value[1] < -10000){
              Y = -5 + (Acc_value[1] % 10000) / 1000;
            }            

            Serial.print(X);
            Serial.print("|");
            Serial.print(Y);
            Serial.println("");
            if(flag){
              Mouse.click(MOUSE_LEFT);
            }
            Mouse.move(X,Y,0);   
        }

        delay(10);
    }


    free(Acc_value);
}


// Function area
void Button_setup(){                        //Pull-up Digital Button;

    //XXX : Must script base on DDR, PORT register
    pinMode(7,INPUT);
    pinMode(8,INPUT);
    pinMode(9,INPUT);
    pinMode(10,INPUT);

    digitalWrite(7,HIGH);
    digitalWrite(8,HIGH);
    digitalWrite(9,HIGH);
    digitalWrite(10,HIGH);
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
