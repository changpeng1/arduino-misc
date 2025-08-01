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
// magnetic sensor instance - SPI
// MagneticSensorSPI sensor = MagneticSensorSPI(AS5147_SPI, 10);
// magnetic sensor instance - MagneticSensorI2C
MagneticSensorI2C sensor = MagneticSensorI2C(AS5600_I2C);
// magnetic sensor instance - analog output
// MagneticSensorAnalog sensor = MagneticSensorAnalog(A1, 14, 1020);

// BLDC motor & driver instance
BLDCMotor motor = BLDCMotor(7);//7,11
BLDCDriver3PWM driver = BLDCDriver3PWM(PA0, PA6, PA7);
// DRV8301 gate_driver = DRV8301(MOSI, MISO, SCLK, CS, EN_GATE, FAULT);
DRV8301 gate_driver = DRV8301(PA11, PA12, PA15, PB3, PB4, PB5);

LowsideCurrentSense current_sense  = LowsideCurrentSense(0.01, 10, PA3, PA4, PA5);
// angle set point variable
float target_angle = 20;
// instantiate the commander
Commander command = Commander(Serial);
void doTarget(char* cmd) { command.scalar(&target_angle, cmd); }

void setup() {

  // initialise magnetic sensor hardware
  sensor.init();
  // link the motor to the sensor
  motor.linkSensor(&sensor);

  // driver config
  // power supply voltage [V]
  driver.voltage_power_supply = 12;
  driver.init();
  gate_driver.begin(PWM_INPUT_MODE_3PWM);
  current_sense.linkDriver(&driver);
  // link the motor and the driver
  motor.linkDriver(&driver);

//如果设置了phase resistance，实际的voltage limit 是phase_resistance * current_limit,再设置voltage_limit无效
// limiting motor movements
	motor.phase_resistance = 5.57;//0.09;//5.57;//0.095; // [Ohm]
	// motor.phase_inductance = 0.0001; // [Ohm]
	motor.foc_modulation = FOCModulationType::SpaceVectorPWM;
	motor.current_limit = 0.08;//0.07;   // [Amps] - if phase resistance defined
	motor.voltage_limit = 1;   // [V] - if phase resistance not defined
	motor.velocity_limit = 50; // [rad/s] cca 50rpm

motor.voltage_sensor_align = 0.2;
motor.torque_controller = TorqueControlType::voltage;

  // choose FOC modulation (optional)
  motor.foc_modulation = FOCModulationType::SpaceVectorPWM;

  // set motion control loop to be used
  motor.controller = MotionControlType::angle;

  // contoller configuration
  // default parameters in defaults.h

  // velocity PI controller parameters
  motor.PID_velocity.P = 0.2f;
  motor.PID_velocity.I = 2;
  motor.PID_velocity.D = 0;
  // maximal voltage to be set to the motor
  // motor.voltage_limit = 1;//6;

  // velocity low pass filtering time constant
  // the lower the less filtered
  motor.LPF_velocity.Tf = 0.01f;

  // angle P controller
  motor.P_angle.P = 20;
  // maximal velocity of the position control
  // motor.velocity_limit = 20;

  // use monitoring with serial
  Serial.begin(115200);
  // comment out if not needed
  motor.useMonitoring(Serial);


  // initialize motor
  motor.init();
  
  // current sense init hardware
	current_sense.init();
	// link the current sense to the motor
	motor.linkCurrentSense(&current_sense);
  
  // align sensor and start FOC
  motor.initFOC();

  // add target command T
  command.add('T', doTarget, "target angle");

  Serial.println(F("Motor ready."));
  Serial.println(F("Set the target angle using serial terminal:"));
   SimpleFOCDebug::enable(&Serial);
  _delay(1000);
}


void loop() {

  // main FOC algorithm function
  // the faster you run this function the better
  // Arduino UNO loop  ~1kHz
  // Bluepill loop ~10kHz
  motor.loopFOC();

  // Motion control function
  // velocity, position or voltage (defined in motor.controller)
  // this function can be run at much lower frequency than loopFOC() function
  // You can also use motor.move() and set the motor.target in the code
  motor.move(target_angle);


  // function intended to be used with serial plotter to monitor motor variables
  // significantly slowing the execution down!!!!
  motor.monitor();
// Serial.println(current_sense.getDCCurrent());
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