/**
 *
 * Velocity motion control example
 * Steps:
 * 1) Configure the motor and magnetic sensor
 * 2) Run the code
 * 3) Set the target velocity (in radians per second) from serial terminal
 *
 *
 * By using the serial terminal set the velocity value you want to motor to obtain
 *
 */
#include <SimpleFOC.h>
#include <DRV8301.h>
// magnetic sensor instance - SPI
// MagneticSensorSPI sensor = MagneticSensorSPI(AS5147_SPI, 10);
// magnetic sensor instance - MagneticSensorI2C
MagneticSensorI2C sensor = MagneticSensorI2C(AS5600_I2C);
// MagneticSensorAnalog sensor = MagneticSensorAnalog(A1, 14, 1020);

// BLDC motor & driver instance
BLDCMotor motor = BLDCMotor(7);
BLDCDriver3PWM driver = BLDCDriver3PWM(PA0, PA6, PA7);
// DRV8301 gate_driver = DRV8301(MOSI, MISO, SCLK, CS, EN_GATE, FAULT);
DRV8301 gate_driver = DRV8301(PA11, PA12, PA15, PB3, PB4, PB5);

LowsideCurrentSense current_sense  = LowsideCurrentSense(0.01, 10, PA3, PA4, PA5);

// velocity set point variable
float target_velocity = 10;
// instantiate the commander
Commander command = Commander(Serial);
void doTarget(char* cmd) { command.scalar(&target_velocity, cmd); }

void setup() {

  // initialise magnetic sensor hardware
  sensor.init();
  // link the motor to the sensor
  motor.linkSensor(&sensor);
  Serial.println(F("sensor ready."));
  // driver config
    // driver.pwm_frequency = 40000;
  // power supply voltage [V]
  driver.voltage_power_supply = 12;
  // driver.voltage_limit = 6;
  driver.init();
  gate_driver.begin(PWM_INPUT_MODE_3PWM);

  current_sense.linkDriver(&driver);
  // link the motor and the driver
  motor.linkDriver(&driver);


// limiting motor movements
	motor.phase_resistance = 5.57;//0.1;//5.57;//0.09; // [Ohm]
	// motor.phase_inductance = 0.0001; // [Ohm]
	motor.foc_modulation = FOCModulationType::SpaceVectorPWM;
	motor.current_limit = 0.8;//5;   // [Amps] - if phase resistance defined //50
	motor.voltage_limit = 1;//0.25;  //0.2 // [V] - if phase resistance not defined
	motor.velocity_limit = 50; // [rad/s] cca 50rpm

motor.voltage_sensor_align = 0.1;

motor.torque_controller = TorqueControlType::voltage;


  // set motion control loop to be used
  motor.controller = MotionControlType::velocity;

  // contoller configuration
  // default parameters in defaults.h

  // velocity PI controller parameters
  motor.PID_velocity.P = 0.1f;
  motor.PID_velocity.I = 2;//20
  motor.PID_velocity.D = 0;
  // default voltage_power_supply
  // motor.voltage_limit = 0.02;//6;
  // jerk control using voltage voltage ramp
  // default value is 300 volts per sec  ~ 0.3V per millisecond
  motor.PID_velocity.output_ramp = 1000;

  // velocity low pass filtering
  // default 5ms - try different values to see what is the best.
  // the lower the less filtered
  motor.LPF_velocity.Tf = 0.01f;

  // use monitoring with serial
  Serial.begin(115200);
  // comment out if not needed
  motor.useMonitoring(Serial);

  // initialize motor
  motor.init();
  // align sensor and start FOC

// current sense init hardware
	if(current_sense.init())
  Serial.println("Current sense init success!");
  else
  Serial.println("Current sense init failed!");
	// link the current sense to the motor
	motor.linkCurrentSense(&current_sense);

  motor.initFOC();

  // add target command T
  command.add('T', doTarget, "target velocity");

  Serial.println(F("Motor ready."));
  Serial.println(F("Set the target velocity using serial terminal:"));
  SimpleFOCDebug::enable(&Serial);
  _delay(1000);
}

void loop() {
  // main FOC algorithm function
  // the faster you run this function the better
  // Arduino UNO loop  ~1kHz
  // Bluepill loop ~10kHz
  motor.loopFOC();
  // Serial.println(sensor.getAngle());
  // Motion control function
  // velocity, position or voltage (defined in motor.controller)
  // this function can be run at much lower frequency than loopFOC() function
  // You can also use motor.move() and set the motor.target in the code
  motor.move(target_velocity);

  // function intended to be used with serial plotter to monitor motor variables
  // significantly slowing the execution down!!!!
  motor.monitor();
  // Serial.print(current_sense.getPhaseCurrents().a);
  // Serial.print("   ");
  // Serial.print(current_sense.getPhaseCurrents().b);
  // Serial.print("   ");
  //  Serial.println(current_sense.getDCCurrent());
  // user communication
  // if(gate_driver.is_fault())
  // {
  // int fault = gate_driver.read_fault();
  //   Serial.println(fault);
  // }
  // command.run();
  serialReceiveUserCommand();
}
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
      target_velocity = received_chars.toFloat();
      Serial.print("Target velocity: ");
      Serial.println(target_velocity);
      
      // reset the command buffer 
      received_chars = "";
    }
  }
}
