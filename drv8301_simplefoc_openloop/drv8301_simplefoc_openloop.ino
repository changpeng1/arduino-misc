// Open loop motor control example
#include <SimpleFOC.h>
#include <DRV8301.h>

// Motor instance
BLDCMotor motor = BLDCMotor(7);//7/11/21
BLDCDriver3PWM driver = BLDCDriver3PWM(PA0, PA1, PA2);
// DRV8301 gate_driver = DRV8301(MOSI, MISO, SCLK, CS, EN_GATE, FAULT);
// DRV8301 gate_driver = DRV8301(PB5, PA6, PA5, PA4, PA3, PE9);
DRV8301 gate_driver = DRV8301(PB15, PB14, PB13, PD8, PB11, PB10);

void setup()
{
    // driver config
    // power supply voltage [V]
    driver.voltage_power_supply = 12;
    // driver.voltage_limit = 12;
    driver.init();
    gate_driver.begin(PWM_INPUT_MODE_3PWM);
    // link the motor and the driver
    motor.linkDriver(&driver);

    // limiting motor movements
    motor.voltage_limit = 1;   // [V]
    motor.velocity_limit = 50; // [rad/s]

    // open loop control config
    motor.controller = MotionControlType::velocity_openloop;

    // init motor hardware
    motor.init();

    Serial.begin(115200);
    Serial.println("Motor ready!");
    _delay(1000);
    int id = gate_driver.get_id();
    Serial.print("ID is:");
    Serial.println(id);
    SimpleFOCDebug::enable(&Serial);
}

float target_velocity = 10; // [rad/s]

void loop()
{                                                                           
    // open loop velocity movement
    // using motor.voltage_limit and motor.velocity_limit
    motor.move(target_velocity);

    // receive the used commands from serial
    serialReceiveUserCommand();
  // if(gate_driver.is_fault())
  {
    // int fault = gate_driver.read_fault();
    // Serial.println(fault);
}
    // _delay(1000);
}

// utility function enabling serial communication with the user to set the target values
// this function can be implemented in serialEvent function as well
void serialReceiveUserCommand()
{
    // a string to hold incoming data
    static String received_chars;

    while (Serial.available())
    {
        // get the new byte:
        char inChar = (char)Serial.read();
        // add it to the string buffer:
        received_chars += inChar;
        // end of user input
        if (inChar == '\n')
        {
            // change the motor target
            target_velocity = received_chars.toFloat();
            Serial.print("Target velocity ");
            Serial.println(target_velocity);

            // reset the command buffer
            received_chars = "";
        }
    }
}
