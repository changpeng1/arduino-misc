/**
 *
 * Position/angle motion control example
 * Steps:
 * 1) Configure the motor and magnetic sensor
 * 2) Run the code
 * 3) Set the target angle (in radians) from serial terminal
 *
 */
#include <SimpleFOC.h>
#include <DRV8301.h>
MagneticSensorI2C sensor1 = MagneticSensorI2C(AS5600_I2C);
TwoWire Wire1(PB11, PB10);
MagneticSensorI2C sensor2 = MagneticSensorI2C(AS5600_I2C);

// BLDC motor & driver instance
BLDCMotor motor1 = BLDCMotor(7);//7,11
BLDCDriver3PWM driver1 = BLDCDriver3PWM(PA8,PB0,PB1,PB3);

BLDCMotor motor2 = BLDCMotor(7);//7,11
BLDCDriver3PWM driver2 = BLDCDriver3PWM(PA0,PA1,PA2,PA3);

// angle set point variable
float target_angle = 0;
// instantiate the commander
Commander command = Commander(Serial);
void doTarget(char* cmd) { command.scalar(&target_angle, cmd); }

void setup() {

  // initialise magnetic sensor hardware
  sensor1.init(&Wire1);
  sensor2.init();
  // link the motor to the sensor
  motor1.linkSensor(&sensor1);
  // motor2.linkSensor(&sensor2);

  // driver config
  // power supply voltage [V]
  driver1.voltage_power_supply = 12;
  driver1.init();
  motor1.linkDriver(&driver1);

  // choose FOC modulation (optional)
  motor1.foc_modulation = FOCModulationType::SpaceVectorPWM;

  // set motion control loop to be used
  motor1.controller = MotionControlType::angle;

  // contoller configuration
  // default parameters in defaults.h

  // velocity PI controller parameters
  motor1.PID_velocity.P = 0.02f;//调节此参数，减少震荡
  motor1.PID_velocity.I = 2;//调节此参数，减少震荡
  motor1.PID_velocity.D = 0;
  // maximal voltage to be set to the motor
  motor1.voltage_limit = 3; //xxd:0.2,2804:3

  // velocity low pass filtering time constant
  // the lower the less filtered
  motor1.LPF_velocity.Tf = 0.01f;
  // angle P controller
  motor1.P_angle.P = 20;
  // maximal velocity of the position control
  motor1.velocity_limit = 20;

  // use monitoring with serial
  Serial.begin(115200);
  // comment out if not needed
  motor1.useMonitoring(Serial);


  // initialize motor
  motor1.init();
  // align sensor and start FOC
  motor1.initFOC();



    driver2.voltage_power_supply = 12;
  // driver2.init();
  // link the motor and the driver
  // motor2.linkDriver(&driver2);

  // choose FOC modulation (optional)
  motor2.foc_modulation = FOCModulationType::SpaceVectorPWM;

  // set motion control loop to be used
  motor2.controller = MotionControlType::angle;

  // contoller configuration
  // default parameters in defaults.h

  // velocity PI controller parameters
  motor2.PID_velocity.P = 0.02f;//调节此参数，减少震荡
  motor2.PID_velocity.I = 2;//调节此参数，减少震荡
  motor2.PID_velocity.D = 0;
  // maximal voltage to be set to the motor
  motor2.voltage_limit = 3; //xxd:0.2,2804:3

  // velocity low pass filtering time constant
  // the lower the less filtered
  motor2.LPF_velocity.Tf = 0.01f;

  // angle P controller
  motor2.P_angle.P = 20;
  // maximal velocity of the position control
  motor2.velocity_limit = 20;

  // use monitoring with serial
  Serial.begin(115200);
  // comment out if not needed
  motor1.useMonitoring(Serial);
    motor2.useMonitoring(Serial);


  // initialize motor
  motor1.init();
  // align sensor and start FOC
  motor1.initFOC();
    // motor2.init();
  // align sensor and start FOC
  // motor2.initFOC();

  // add target command T
  command.add('T', doTarget, "target angle");

  Serial.println(F("Motor ready."));
  Serial.println(F("Set the target angle using serial terminal:"));
  SimpleFOCDebug::enable(&Serial);
  _delay(1000);
}


void loop() {

  motor1.loopFOC();
  // motor2.loopFOC();

  sensor2.update();
  target_angle = sensor2.getAngle();
  motor1.move(target_angle);
  // motor2.move(target_angle);


  // function intended to be used with serial plotter to monitor motor variables
  // significantly slowing the execution down!!!!
  // motor.monitor();

  // user communication
  // command.run();
  serialReceiveUserCommand();
}

#if 1
void serialReceiveUserCommand() {
  
  // a string to hold incoming data
  static String received_chars;
  
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the string buffer:
    received_chars += inChar;
    // end of user input
    if (inChar == '\n') {
      
      // change the motor target
      target_angle = received_chars.toFloat();
      Serial.print("Target velocity: ");
      Serial.println(target_angle);
      
      // reset the command buffer 
      received_chars = "";
    }
  }
}
#endif