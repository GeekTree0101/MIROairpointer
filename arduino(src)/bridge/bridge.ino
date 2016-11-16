/***********************************
    Title : MIRO airpointer project
    Copyright(C) 2016 MIRO KyongPook Univ
     
    하현수, 박민규, 황인득,  이재훈, 이동은
    update : 2016.11.16 15:44
***********************************/


/**********************************
       [+] Import Library
**********************************/

#include <Mouse.h>
#include <Keyboard.h>
#include <unistd.h>

/**********************************
    [+] Global & Const Variables
**********************************/

// Data protocol
// reference : airpointer.ino
#define DATA_MOTION 1
#define DATA_ZOOMIN 2
#define DATA_DRAWING 3
#define DATA_PASSPAGE 4

#define key_press_delay 30              //key & Mouse Motion delay time
#define mouse_press_delay 50

boolean Drawing_flag = false;                   //Drawing Flag
boolean Zoom_flag = false;                      //Zoom Flag
boolean Serial_control_flag = false;            //recved serial data control flag

unsigned int function_state = 99;
short X = 0;
short Y = 0;

/**********************************
        [+] Function
***********************************/

void Mouse_interface_setup();           //Mouse Init Setup
void Keyboard_interface_setup();        //Keyboard Init Setup
void ZoomIn_start();
void ZoomIn_cancel();
void ZoomIn_event();
void Drawing_start();
void Drawing_cancel();
void Drawing_event();

void setup(){                               //Hardware Setup

    //NOTE : arduino micro(atmega32u4) using Serial1 
    Serial1.begin(9600);
    Mouse_interface_setup();
    Keyboard_interface_setup();
}

void loop(){                                //Main Loop Proc

    char node[4] ={ ' ' };   //char buffer
    
    unsigned short node_index = 0;
    unsigned short output_pos = 0;

    while(1){
    
    if(Serial1.available()){
      
            char buff = Serial1.read();

            if(buff == '&'){                    //exit
                
              digitalWrite(5,LOW);
              Serial_control_flag = true;
              break;
            }
            else if(buff == '/'){                    //slash

               String str = node;      
              
               switch(output_pos){
                   case 0 : function_state =  str.toInt();
                       break;
                   case 1 : X = str.toInt();
                       break;
                   case 2 : Y = str.toInt();
                       break;
               }
               Serial.print(function_state);
               Serial.print('\t');
               Serial.print(X);
               Serial.print('\t');
               Serial.println(Y);      
              
               // init
               node_index = 0;               
               node[0] = ' ';
               node[1] = ' ';
               node[2] = ' ';

               //count up
               output_pos++; 
             
            }
            else{
               node[node_index] = buff;
               node_index++;
            }
        }
                                     // start bit == * endif
    }                                  //available endif

    
    if (Serial_control_flag) {
        
        switch(function_state){

            //Zoomin proc
            case DATA_ZOOMIN:  
                
                ZoomIn_event();        //Zoomin flag toogle function
                
                if(Zoom_flag == true){      
                    ZoomIn_start();
                }
                else{
                    ZoomIn_cancel();
                }
                break;

            //Drawing proc
            case DATA_DRAWING:

                Drawing_event();       //Drawing flag toogle function
            
                if(Drawing_flag == true){
                    Drawing_start();
                }
                else{
                    Drawing_cancel();
                } 

                break;

            //Passpage proc
            case DATA_PASSPAGE:

                Mouse.click(MOUSE_LEFT);
                delay(500);
                break;

            case DATA_MOTION:

                if(Drawing_flag == true){

                  Mouse.press(MOUSE_LEFT);
                }
                else if(Zoom_flag == true){

                  Mouse.press(MOUSE_LEFT);
                }
                
                Mouse.move(X,Y,0);

              break;

            default :

                if(Zoom_flag == true || Drawing_flag == true ){     //Zoom or Drawing active state
                    
                    Mouse.move(X,Y,0);
                }
                else{

                    Mouse.release(MOUSE_ALL);
                }
                
        }

        function_state = 99;
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
