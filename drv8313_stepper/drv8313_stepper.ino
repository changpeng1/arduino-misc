#include <SimpleFOC.h>

// Stepper motor
HybridStepperMotor motor = HybridStepperMotor(50);

// BLDC driver instance 
// SimpleFOCShield 
// BLDCDriver3PWM driver = BLDCDriver3PWM(PA8,PB0,PB1,PB3);
BLDCDriver3PWM driver = BLDCDriver3PWM(PB1,PB0,PA8,PB3);

// encoder instance
// Encoder encoder = Encoder(2, 3, 2048);
// // channel A and B callbacks
// void doA(){encoder.handleA();}
// void doB(){encoder.handleB();}

// inline current sensor instance
// ACS712-05B has the resolution of 0.185mV per Amp
// NOTE: LowsideCurrentSense sense is used because its faster than InlineCurrentSense class
// LowsideCurrentSense current_sense = LowsideCurrentSense(185.0f, A0, A2);

// commander communication instance
// Commander command = Commander(Serial);
//void doMotion(char* cmd){ command.motion(&motor, cmd); }
// void doMotor(char* cmd){ command.motor(&motor, cmd); }

// MagneticSensorSPI(int cs, float _cpr, int _angle_register)
// config           - SPI config
//  cs              - SPI chip select pin 
MagneticSensorSPI sensor = MagneticSensorSPI(AS5147_SPI, PC13);

// these are valid pins (mosi, miso, sclk) for 2nd SPI bus on storm32 board (stm32f107rc)
SPIClass SPI_2(PA7, PA6, PA5);

void setup() {

  // use monitoring with serial 
  Serial.begin(115200);
  // enable more verbose output for debugging
  // comment out if not needed
  SimpleFOCDebug::enable(&Serial);
sensor.init(&SPI_2);
  // initialize encoder sensor hardware
  // encoder.init();
  // encoder.enableInterrupts(doA, doB);
  // link the motor to the sensor
  motor.linkSensor(&sensor);

  // driver config
  // power supply voltage [V]
  driver.voltage_power_supply = 12;
  driver.init();
  // link driver
  motor.linkDriver(&driver);
  // link current sense and the driver
  // current_sense.linkDriver(&driver);

  // set control loop type to be used
  motor.controller = MotionControlType::velocity;
  motor.torque_controller = TorqueControlType::voltage;

  // SVPWM modulation type is much more efficient for hybrid stepper motors
  motor.foc_modulation = FOCModulationType::SpaceVectorPWM;


  // controller configuration based on the control type
  motor.PID_velocity.P = 0.05f;
  motor.PID_velocity.I = 1;
  motor.PID_velocity.D = 0;
  // default voltage_power_supply
  motor.voltage_limit = 5;

  // velocity low pass filtering time constant
  motor.LPF_velocity.Tf = 0.01f;

  // angle loop controller
  motor.P_angle.P = 20;
  // angle loop velocity limit
  motor.velocity_limit = 20;

  // comment out if not needed
  motor.useMonitoring(Serial);

  // current sense init and linking
  // current_sense.init();
  // motor.linkCurrentSense(&current_sense);

  // initialise motor
  motor.init();
  // align encoder and start FOC
  motor.initFOC();

  // subscribe motor to the commander
  // command.add('M', doMotor, "motor");
  
  // Run user commands to configure and the motor (find the full command list in docs.simplefoc.com)
  Serial.println("Motor ready.");

  _delay(1000);
}

float target_velocity = 0.0; // [rad/s]
void loop() {
  // iterative setting FOC phase voltage
  motor.loopFOC();

  // iterative function setting the outter loop target
  motor.move(target_velocity);
  // motor.setPhaseVoltage(2.0f, 0, 0);
  // motor.setPhaseVoltage(2.0f, 0, _PI_2);


  // motor monitoring
  // motor.monitor();

serialReceiveUserCommand();
  // user communication
  // command.run();
}


// float electrical_angle = 0;

//   void loop() {
//     motor.setPhaseVoltage(2.0f, 0, electrical_angle);

//     delay(500);

//     electrical_angle += _PI_2;
//     if (electrical_angle >= _2PI) {
//       electrical_angle -= _2PI;
//     }
//   }

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