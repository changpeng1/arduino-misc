#ifndef __GLOBAL_DEFINE__
#define __GLOBAL_DEFINE__
int last_left_state =0;//保存上一次左侧引脚数据
int last_right_state = 0;//保存上一次右侧引脚数据
int left_counter = 0;//左侧测速计数器
int last_left_counter = 0;//上一次左侧测速计数器
int right_counter = 0;//右侧测速计数器
int last_right_counter = 0;//上一次右侧测速计数器
int handle_request = 0;
#define RECV_BUFFER_SIZE	40
#define SEND_MESSAGE_SIZE	10
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

  #define PS2_DAT       52 // 13  //14    
#define PS2_CMD        51 //11  //15
#define PS2_SEL        53 //10  //16
#define PS2_CLK        50 //12  //17
//#define pressures   true
#define pressures   false
//#define rumble      true
#define rumble      false
#endif
