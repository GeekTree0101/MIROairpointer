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
#define MPU 0x68
//Function area

void Button_setup();
void MPU_setup();
void Mouse_interface_setup();
void Keyboard_interface_setup();
void MPU_action_control(int16_t* value_array);

//Setup

void setup(){
    Serial.begin(9600);
    Button_setup();
    MPU_setup();
    Mouse_interface_setup();
    Keyboard_interface_setup();
}

//Main loop proc

void loop(){

    //XXX : Must import Low Battery System Cuz, overhead very big in this loop
    int16_t Gyro_value[2] = {0,0};

    Wire.beginTransmission(MPU);         //Begin MPU
    Wire.write(0x3B);
    Wire.endTransmission(false);             //Sustain connection
    Wire.requestFrom(MPU, 14, true);

    int16_t accX = Wire.read() << 8|Wire.read();  // X pos data
    int16_t accY = Wire.read() << 8|Wire.read();  // Y pos data
    int16_t accZ = Wire.read() << 8|Wire.read();  // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
    int16_t tmp = Wire.read() << 8|Wire.read();  // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
    Gyro_value[0] = Wire.read() << 8|Wire.read();  // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
    Gyro_value[1] = Wire.read() << 8|Wire.read();  // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
    int16_t gyro_z =Wire.read() << 8|Wire.read();  // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)
  
    MPU_action_control(Gyro_value);

    if(digitalRead(8) == LOW){              //Button Click

        Mouse.click(MOUSE_LEFT);
    }

    if(digitalRead(9) == LOW){              //Gyro action

        int X = 10;
        int Y = 10;

        Serial.print(X);
        Serial.print("|");
        Serial.print(Y);
        Serial.println("*");

        if(Gyro_value[0] > 0){

            X = -10;
        }

        if(Gyro_value[1] > 0){

            Y = -10;
        }

        Mouse.move(X,Y,0);
    }

    delay(100);
    

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
    Wire.beginTransmission(MPU);
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

