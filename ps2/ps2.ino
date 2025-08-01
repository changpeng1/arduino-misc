#include <PS2X_lib.h>
PS2X ps2x; // create PS2 Controller Class
 #define PS2_DAT       52 // 13  //14    
#define PS2_CMD        51 //11  //15
#define PS2_SEL        53 //10  //16
#define PS2_CLK        50 //12  //17
//#define pressures   true
#define pressures   false
//#define rumble      true
#define rumble      false
int error = 0;
byte type = 0;
byte vibrate = 0;
void setup() {
  // put your setup code here, to run once:
    Serial.begin(9600);
    while(Serial.read()>= 0){}
    //setup PS2
    delay(300);
    error = ps2x.config_gamepad(PS2_CLK, PS2_CMD, PS2_SEL, PS2_DAT, pressures, rumble);

    if(error == 0){
        Serial.println("Found Controller, configured successful ");
    }  
    else if(error == 1)
        Serial.println("No controller found, check wiring, see readme.txt to enable debug. visit www.billporter.info for troubleshooting tips");

    else if(error == 2)
        Serial.println("Controller found but not accepting commands. see readme.txt to enable debug. Visit www.billporter.info for troubleshooting tips");

    else if(error == 3)
        Serial.println("Controller refusing to enter Pressures mode, may not support it. ");

    //  Serial.print(ps2x.Analog(1), HEX);

    type = ps2x.readType(); 
    switch(type) {
        case 0:
            Serial.print("Unknown Controller type found ");
            break;
        case 1:
            Serial.print("DualShock Controller found ");
            break;
        case 2:
            Serial.print("GuitarHero Controller found ");
            break;
        case 3:
            Serial.print("Wireless Sony DualShock Controller found ");
            break;
    }
}

void loop() {
  // put your main code here, to run repeatedly:
 if(error == 1) //skip loop if no controller found
        return; 
    if(type == 2){ //Guitar Hero Controller

    }
    else { //DualShock Controller
        ps2x.read_gamepad(false, vibrate); //read controller and set large motor to spin at 'vibrate' speed

        if(ps2x.Button(PSB_START))         //will be TRUE as long as button is pressed
            Serial.println("Start is being held");
        if(ps2x.Button(PSB_SELECT))
            Serial.println("Select is being held");      

        if(ps2x.Button(PSB_PAD_UP)) {      //will be TRUE as long as button is pressed
            Serial.print("Up held this hard: ");
            Serial.println(ps2x.Analog(PSAB_PAD_UP), DEC);

        }
        if(ps2x.Button(PSB_PAD_RIGHT)){
            Serial.print("Right held this hard: ");
            Serial.println(ps2x.Analog(PSAB_PAD_RIGHT), DEC);
        }
        if(ps2x.Button(PSB_PAD_LEFT)){
            Serial.print("LEFT held this hard: ");
            Serial.println(ps2x.Analog(PSAB_PAD_LEFT), DEC);

        }
        if(ps2x.Button(PSB_PAD_DOWN)){
            Serial.print("DOWN held this hard: ");
            Serial.println(ps2x.Analog(PSAB_PAD_DOWN), DEC);
        }   

        vibrate = ps2x.Analog(PSAB_CROSS);  //this will set the large motor vibrate speed based on how hard you press the blue (X) button
        if (ps2x.NewButtonState()) {        //will be TRUE if any button changes state (on to off, or off to on)
            if(ps2x.Button(PSB_L3))
                Serial.println("L3 pressed");
            if(ps2x.Button(PSB_R3))
                Serial.println("R3 pressed");
            if(ps2x.Button(PSB_L2))
                Serial.println("L2 pressed");
            if(ps2x.Button(PSB_R2))
                Serial.println("R2 pressed");
            if(ps2x.Button(PSB_TRIANGLE))
                Serial.println("Triangle pressed");        
        }

        if(ps2x.ButtonPressed(PSB_CIRCLE))               //will be TRUE if button was JUST pressed
            Serial.println("Circle just pressed");
        if(ps2x.NewButtonState(PSB_CROSS))               //will be TRUE if button was JUST pressed OR released
            Serial.println("X just changed");
        if(ps2x.ButtonReleased(PSB_SQUARE))              //will be TRUE if button was JUST released
            Serial.println("Square just released");     

        if(ps2x.Button(PSB_L1) || ps2x.Button(PSB_R1)) { //print stick values if either is TRUE
            Serial.print("Stick Values:");
            Serial.print(ps2x.Analog(PSS_LY), DEC); //Left stick, Y axis. Other options: LX, RY, RX  
            Serial.print(",");
            Serial.print(ps2x.Analog(PSS_LX), DEC); 
            Serial.print(",");
            Serial.print(ps2x.Analog(PSS_RY), DEC); 
            Serial.print(",");
            Serial.println(ps2x.Analog(PSS_RX), DEC); 
        }

    }
    delay(50);  
}
