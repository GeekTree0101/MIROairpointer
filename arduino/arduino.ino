
/*
    MIRO airpointer project
    Copyright(C) 2016 MIRO
    provider : GeekTree0101
    update : 2016.10.24s
*/
//#include "Arduino.h"
#include <Wire.h>
#include <Mouse.h>
#include <Keyboard.h>
#include <MPU6050.h>


//Definition or const value area

//#define BUTTON_IO PORTB
//#define BUTTON_DIR DDRB

//Function area

void Button_setup();
void MPU_setup();
void Mouse_interface_setup();
void Keyboard_interface_setup();
void MPU_action_control(int16_t* value_array);

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
    Acc_value = (int16_t*)malloc(2*sizeof(int16_t));
    

    while(1){
    
        Wire.beginTransmission(0x68);         //Begin MPU
        Wire.write(0x3B);
        Wire.endTransmission(false);             //Sustain connection
        Wire.requestFrom(0x68, 14, true);

        Acc_value[0] = Wire.read() << 8 | Wire.read();  // X pos data
        Acc_value[1] = Wire.read() << 8 | Wire.read();  // Y pos data

        MPU_action_control(Acc_value);

        if(digitalRead(8) == LOW){              //Button Click

            Mouse.click(MOUSE_LEFT);
        }

        if(digitalRead(9) == LOW){              //Gyro action

            Mouse.move(Acc_value[0],Acc_value[1],0);
        }

        delay(100);
    }


    free(Acc_value);
}


// Function area
void Button_setup(){                        //Pull-up Digital Button;

    //XXX : Must script base on DDR, PORT register

    pinMode(8,INPUT);
    pinMode(9,INPUT);

    digitalWrite(8,HIGH);
    digitalWrite(9,HIGH);
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


void MPU_action_control(int16_t* value_array){                  //mouse pointer core


    // Must script algorythm under O(n) 



}
