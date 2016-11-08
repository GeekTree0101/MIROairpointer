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
/**********************************
    [+] Global & Const Variables
**********************************/

#define BUTTON_IO PORTB                 //Button Bport I/O Register
#define BUTTON_DIR DDRB                 //Button Bport DIR Register

#define Drawing_button 7                //Button Pin (PULL-UP Button)
#define Click_button 8
#define ZoomIn_button 9
#define Motion_button 10

#define key_press_delay 30              //key & Mouse Motion delay time
#define mouse_press_delay 50
#define Hardware_delay 10               //Hardware Delay

boolean Drawing_flag = false;                   //Drawing Flag
boolean Zoom_flag = false;                      //Zoom Flag

/**********************************
        [+] Function
***********************************/

void Button_setup();                    //Pull-up Digital Button
void MPU_setup();                       //MPU6050 Init Setup
void Mouse_interface_setup();           //Mouse Init Setup
void Keyboard_interface_setup();        //Keyboard Init Setup
void Click_event();                     //Click event Function
void ZoomIn_event();                    //Zoomin event Function
void ZoomIn_start();
void ZoomIn_cancel();
void Drawing_event();                  //Drawing event Function
void Drawing_start();
void Drawing_cancel();
short Check_X(int Data);
short Check_Y(int Data);

void setup(){                               //Hardware Setup

    Serial.begin(9600);
    Button_setup();
    Mouse_interface_setup();
    Keyboard_interface_setup();
}

void loop(){                                //Main Loop Proc

 
        
        if(digitalRead(Click_button) == LOW){              //Click Function

            Click_event();
        }

        if(digitalRead(Drawing_button) == LOW){
          delay(300);
          Drawing_event();
          if(Drawing_flag==true)
             Drawing_start();
          else
             Drawing_cancel();
        }


        if(Drawing_flag==true){            //Drawing Function
            
            Keyboard.releaseAll();
            
            if(Zoom_flag == false) {
       
                short X = 0;
                short Y = 0;
                
                X = Check_X(Data_Stack[6]);
                Y = Check_Y(Data_Stack[4]);
                
                Mouse.move(X,Y,0);

            }

        }

        if(digitalRead(ZoomIn_button) == LOW){             //ZoomIn Function
            delay(300);
            ZoomIn_event();
            if(Zoom_flag==true)
                ZoomIn_start();
             else {
                ZoomIn_cancel();
             }
          }

           if(Zoom_flag==true) {

              Keyboard.releaseAll();

                short X = 0;
                short Y = 0;
                
                X = Check_X(Data_Stack[6]);
                Y = Check_Y(Data_Stack[4]);
                
                Mouse.move(X,Y,0);
                
            }
            

        if(digitalRead(Motion_button) == LOW){              //Motion Control
            
            //TODO : Don't make it as Function
            //       Due to OverHead during make a new stack on the memory

            if(Drawing_flag==true){
              Mouse.press(MOUSE_LEFT);
            }
            else if(Zoom_flag==true) {
              Mouse.press(MOUSE_LEFT);
              }
            else if(Drawing_flag==true && Zoom_flag==true) {
              Keyboard.releaseAll();
              Mouse.release(MOUSE_ALL);
              }
 
            short X = 0;
            short Y = 0;
            
            X = Check_X(Data_Stack[6]);
            Y = Check_Y(Data_Stack[4]);
            
            Mouse.move(X,Y,0);                            // Mouse Pointer moving method
            
        }
        else{
          if(Drawing_flag==true)
              Mouse.release(MOUSE_LEFT);
          else if(Zoom_flag==true)
              Mouse.release(MOUSE_LEFT);
        }

        delay(Hardware_delay);                            // Loop-Proc delay time

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

  if(Drawing_flag==true) {
    Drawing_flag = false;
    }
  else if(Drawing_flag==false && Zoom_flag==false) {
    Drawing_flag = true;
  }  
}

void Drawing_start() {
  
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

void Drawing_cancel() {
  
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

void ZoomIn_event() {

  if(Zoom_flag) {
    Zoom_flag = false;
    }
  else {
    Zoom_flag = true;
  }  
  
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