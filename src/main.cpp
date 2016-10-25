#include <Wire.h>


/*
    MIRO airpointer project
    Copyright(C) 2016 MIRO
    provider : GeekTree0101
    update : 2016.10.24s
*/
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

        Serial.print(Acc_value[0]);
        Serial.print("\t");
        Serial.print(Acc_value[1]);
        Serial.print("\t");
        Serial.print(Acc_value[2]);
        Serial.print("\t");
        Serial.print(Acc_value[3]);
        Serial.print("\t");
        Serial.print(Acc_value[4]);
        Serial.print("\t");
        Serial.println(Acc_value[5]);
        
        if(digitalRead(8) == LOW){              //Button Click

            Mouse.click(MOUSE_LEFT);
        }

        if(digitalRead(9) == LOW){              //Gyro action
            int X = 0;
            int Y = 0;
            if(Acc_value[0] > 10000){
              X = 5;
            }
            else if(Acc_value[0] < -10000){
              X = -5;
            }
            
            if(Acc_value[1] > 10000){
              Y = -5;
            }
            else if(Acc_value[1] < -10000){
              Y = 5;
            }            


            Mouse.move(X,Y,0);   
        }

        delay(50);
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
