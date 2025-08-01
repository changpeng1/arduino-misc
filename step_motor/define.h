#ifndef __GLOBAL_DEFINE__
#define __GLOBAL_DEFINE__

//mega2560中断引脚:
//  2 3 21  20  19  18 //int0 int1 int2 int3 int4 int5 


bool should_left_stop = true;
bool should_right_stop = true;

unsigned long left_duration = 1000;
unsigned long right_duration = 1000;

int left_counter = 0;//左前测速计数器

int right_counter = 0;//右前测速计数器

//int last_right_counter = 0;//上一次右侧测速计数器
int handle_request = 0;
#define RECV_BUFFER_SIZE	40
#define SEND_MESSAGE_SIZE	120
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


double time_interval=1000;

#endif
