/***********************************
    Title : MIRO airpointer project
    Copyright(C) 2016 MIRO KyongPook Univ
     
    하현수, 박민규, 황인득,  이재훈, 이동은
    update : 2016.10.25 23:58
***********************************/

/**********************************
       [+] Import Library
**********************************/

#include <Mouse.h>
#include <Keyboard.h>
#include <SoftwareSerial.h>

SoftwareSerial BT = SoftwareSerial(0,1);
/**********************************
    [+] Global & Const Variables
**********************************/

#define CLICK_BUTTON 0                //Button Pin (PULL-UP Button)
#define DRAWING_BUTTON 1
#define ZOOMIN_BUTTON 2
#define MOTION_BUTTON 3
#define DEFAULT 4

#define key_press_delay 30              //key & Mouse Motion delay time
#define mouse_press_delay 50
#define Hardware_delay 10               //Hardware Delay

boolean Drawing_flag = false;                   //Drawing Flag
boolean Zoom_flag = false;                      //Zoom Flag

String inputString = "";
boolean stringComplete = false;  // whether the string is complete
unsigned int function_state = 99;
short X = 0;
short Y = 0;


/**********************************
        [+] Function
***********************************/

void Mouse_interface_setup();           //Mouse Init Setup
void Keyboard_interface_setup();        //Keyboard Init Setup
void Click_event();                     //Click event Function
void ZoomIn_event();                    //Zoomin event Function
void ZoomIn_start();
void ZoomIn_cancel();
void Drawing_event();                  //Drawing event Function
void Drawing_start();
void Drawing_cancel();

void setup(){                               //Hardware Setup
    //FIXME : 블루투스 Baudrate설정 재검토 필요 및 펌테크 인터페이스로 내부수정필요
    BT.begin(9600);
    inputString.reserve(200);
    Mouse_interface_setup();
    Keyboard_interface_setup();
}

void loop(){                                //Main Loop Proc

  if (stringComplete) {

        
        if(function_state == CLICK_BUTTON){              //Click Function

            Click_event();
        }

        if(function_state == DRAWING_BUTTON){
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
                    
                Mouse.move(X,Y,0);
            }

        }

        if(function_state == ZOOMIN_BUTTON){             //ZoomIn Function
            
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
            Mouse.move(X,Y,0);
                
        }
            
        if(function_state == MOTION_BUTTON){              //Motion Control
            

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
  
          Mouse.move(X,Y,0);                            // Mouse Pointer moving method
            
        }
        else{
          
          if(Drawing_flag==true){
           
              Mouse.release(MOUSE_LEFT);
          }
          else if(Zoom_flag==true){
           
              Mouse.release(MOUSE_LEFT);
          }
        }

        function_state = 99;
        stringComplete = false;
        delay(Hardware_delay);                            // Loop-Proc delay time

  } 

  
}

/*
  SerialEvent occurs whenever a new data comes in the
 hardware serial RX.  This routine is run between each
 time loop() runs, so using delay inside loop can delay
 response.  Multiple bytes of data may be available.
 */
void serialEvent() {

  boolean data_type_toggle = true;
  int count = 0;

  //example 0/234/23\n

  while (BT.available()) {

    if(data_type_toggle){

      int temp = (int)BT.read();   // 0 -> 234 -> 23

      if(count == 0){
        
        function_state = temp;         // 0
      }
      else if(count == 1){
        X = temp;                      // 234
      }
      else if(count == 2){
        Y = temp;                      // 23
      }

      count++;
      data_type_toggle = false;
    }
    else{

      char temp = (char)BT.read();  // / -> / -> \n
      
      if(temp == '/'){
        
        count++;
        data_type_toggle = true;
      }
      else{                                    //end serial line \n
        count = 0;
        stringComplete  = true;
      }

    }

  }
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
