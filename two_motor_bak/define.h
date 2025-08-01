#ifndef __GLOBAL_DEFINE__
#define __GLOBAL_DEFINE__

//mega2560中断引脚:
//  2 3 21  20  19  18 //int0 int1 int2 int3 int4 int5 

int left1_counter = 0;//左前测速计数器
int left2_counter = 0;//左后测速计数器

int right1_counter = 0;//右前测速计数器
int right2_counter = 0;//右后测速计数器

//int last_right_counter = 0;//上一次右侧测速计数器
int handle_request = 0;
#define RECV_BUFFER_SIZE	40
#define SEND_MESSAGE_SIZE	30
#define WHEEL_L 0
#define WHEEL_R 1

#define START_MOVE 0x01
#define STOP_MOVE 0x02
#define SET_SPEED 0x03
#define GET_SPEED 0x04
unsigned char recv_buffer[RECV_BUFFER_SIZE];
unsigned char request_buffer[SEND_MESSAGE_SIZE];
unsigned int msg_length=0;
unsigned char msg_id=0;
unsigned char msg_new=0;
unsigned int msg_count=0;
unsigned char ptr=0;
unsigned char check_sum=0;
unsigned char uart_new_msg=0;
int rf_value=0;

#define PS2_DAT        13  //14    
#define PS2_CMD        11  //15
#define PS2_SEL        10  //16
#define PS2_CLK        12  //17
//#define pressures   true
#define pressures   false
//#define rumble      true
#define rumble      false


double time_interval=100;

double left_Setpoint1, left_Input1, left_Output1;
double left_Setpoint2, left_Input2, left_Output2;
double right_Setpoint1, right_Input1, right_Output1;
double right_Setpoint2, right_Input2, right_Output2;
//double Kp=4, Ki=5, Kd=0.1;
//double Kp=2, Ki=5, Kd=1;
double Kp=1, Ki=5, Kd=0.1;//kd不动 kp变大会震荡
#endif
