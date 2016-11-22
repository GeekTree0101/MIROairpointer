/***********************************
    Title : MIRO airpointer project
    Copyright(C) 2016 MIRO KyongPook Univ
     
    하현수, 박민규, 황인득,  이재훈, 이동은
    update : 2016.11.16 15:44
***********************************/


/**********************************
       [+] Import Library
**********************************/

#include <Mouse.h>                            // Mouse lib
#include <Keyboard.h>                         // Keyboard lib
#include <unistd.h>                           // string to int lib

/**********************************
    [+] Global & Const Variables
**********************************/

//function state data
#define DATA_MOTION 1
#define DATA_ZOOMIN 2
#define DATA_DRAWING 3
#define DATA_PASSPAGE 4

// HID delay
#define key_press_delay 30             
#define mouse_press_delay 50

// Process control flag
boolean Drawing_flag = false;                   // Drawing Flag
boolean Zoom_flag = false;                      // Zoom Flag
boolean Serial_control_flag = false;            // received serial data control flag
boolean mouse_press_flag = false;

// Global variable
unsigned int function_state = 99;               // function state base on state data
short X = 0;                                    // HID mouse X position
short Y = 0;                                    // HID mouse Y position

/**********************************
        [+] Function
***********************************/

void Mouse_interface_setup();                   //Mouse Init Setup
void Keyboard_interface_setup();                //Keyboard Init Setup

//Zoom function
void ZoomIn_start();
void ZoomIn_cancel();
void ZoomIn_event();

//Drawing function
void Drawing_start();
void Drawing_cancel();
void Drawing_event();

void setup(){                                  //Hardware Setup

    //NOTE : arduino micro(atmega32u4) using Serial1 

    Serial1.begin(9600);
    Mouse_interface_setup();
    Keyboard_interface_setup();
}

void loop(){                                   // Main Loop Proc

    char node[4] ={ ' ' };                     // char buffer
    
    unsigned short node_index = 0;
    unsigned short output_pos = 0;

    while(1){
    
        Serial1.flush();
        
        int count = 0;
    
        while(1){
        
            if(Serial1.available()){
        
                String str = Serial1.readStringUntil('/');
    
                if(count == 0){
                    function_state = str.toInt();
                    count++;
                }
                else if(count == 1){
                    X = str.toInt();
                    count++;
                }
                else if(count == 2){
                    Y = str.toInt();
                    Serial_control_flag = true;
                    count = 0;
                    break;
                }
            }
                                
        }                               

    
        if (Serial_control_flag) {                 // Reality HID control section
        
            switch(function_state){

                // Zoomin control section
                case DATA_ZOOMIN:  
                
                    ZoomIn_event();                // Zoomin flag toogle function
                    
                    if(Zoom_flag == true){      
                        ZoomIn_start();
                    }
                    else{
                        ZoomIn_cancel();
                    }
                    break;

                // Drawing control section
                case DATA_DRAWING:

                    Drawing_event();               // Drawing flag toogle function
            
                    if(Drawing_flag == true){
                        Drawing_start();
                    }
                    else{
                        Drawing_cancel();
                    } 

                    break;

                // Passpage control section
                case DATA_PASSPAGE:

                    Mouse.click(MOUSE_LEFT);
                    delay(500);
                    break;

                // Motion controll section
                case DATA_MOTION:                                       

                    if(Drawing_flag == true && mouse_press_flag == false){

                        Mouse.press(MOUSE_LEFT);
                        mouse_press_flag = ~mouse_press_flag + 2;
                    }
                    else if(Zoom_flag == true && mouse_press_flag == false){

                        Mouse.press(MOUSE_LEFT);
                        mouse_press_flag = ~mouse_press_flag + 2;
                    }
                
                    Mouse.move(X,Y,0);

                    break;
                default :

                    if( Drawing_flag == true || Zoom_flag == true ){     //Zoom or Drawing active state

                        if(mouse_press_flag == true){

                            Mouse.release(MOUSE_LEFT); 
                            mouse_press_flag = false;
                        }

                    
                        Mouse.move(X,Y,0);
                    }
                    else{

                        if(mouse_press_flag == true){
                        
                            Mouse.release(MOUSE_ALL);
                            mouse_press_flag = false;
                        }

                    
                    }
                
            }

            function_state = 99;
        }
    }
}

void Drawing_event(){                       //Drawing event Function

  if(Zoom_flag == false){
    Drawing_flag = ~Drawing_flag + 2; 
  }

}

void ZoomIn_event() {                     //Zoomin function toggling
  
  if(Drawing_flag == false){
     Zoom_flag = ~Zoom_flag + 2;
  }
  
}

void Drawing_start() {                   //Drawing start function
  
    Keyboard.press(KEY_LEFT_CTRL);
    delay(key_press_delay);
    Keyboard.press('p');
    delay(key_press_delay);
    Keyboard.release('p');
    delay(key_press_delay);
    Keyboard.release(KEY_LEFT_CTRL);
    delay(key_press_delay);
    
    delay(30);
}


void Drawing_cancel() {                  //Drawing cancel function
  
    Keyboard.press(KEY_LEFT_CTRL);
    delay(key_press_delay);
    Keyboard.press('a');
    delay(key_press_delay);
    Keyboard.release('a');
    delay(key_press_delay);
    Keyboard.release(KEY_LEFT_CTRL);
    delay(key_press_delay);

    delay(30);
}

void ZoomIn_start(){                       //Zoomin event Function

     Keyboard.press(KEY_LEFT_CTRL);
     delay(key_press_delay);
     Mouse.move(0,0,3);
     delay(key_press_delay);
     Keyboard.release(KEY_LEFT_CTRL);
     
}

void ZoomIn_cancel() {
    Keyboard.write(0x30);
    delay(key_press_delay);
}

void Mouse_interface_setup(){               //Mouse Init Setup

    Mouse.begin();
    delay(30);
}

void Keyboard_interface_setup(){            //Keyboard Init Setup

    Keyboard.begin();
    delay(30);
}
