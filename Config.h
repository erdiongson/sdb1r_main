// ------- Disp(To be removed and change with UART serial command)
#define Motor_ON 41
#define Motor_UP 43
#define Motor_Dow 45
#define Vibrate 47

#define Limit_S_x_MIN 46 // 32
#define Limit_S_y_MIN 32 // 33
#define Limit_S_z_MIN 36

//#define Limit_S_x_MAX 18
//#define Limit_S_y_MAX 19
#define Limit_S_x_MAX 22 //johari 20240610
#define Limit_S_y_MAX 44 //johari 20240610
#define Limit_S_z_MAX 38


#define Motor_x_CW 27
#define Motor_x_CLK 25

#define Motor_y_CW 29
#define Motor_y_CLK 31

#define Motor_z_CW 35
#define Motor_z_CLK 37

//#define motor_x_speed 8000//, 2000
//#define motor_y_speed 4800//, 1000

//#define motor_x_Acceleration 8000 // 4800, 3200
//#define motor_y_Acceleration 4000 // 2400 1600 // 3200

#define motor_x_speed 8000 // 8000, 2000
#define motor_y_speed 80000  // 4800, 1000
#define motor_z_speed 80000  // 4800, 1000

#define motor_x_Acceleration 100000//8000 // 4800, 3200
#define motor_y_Acceleration 100000//4000 // 2400 1600 // 3200
#define motor_z_Acceleration 100000//4000 // 2400 1600 // 3200

// Defines the number of "dispenses" performed during priming
#define PRIME_DISPENSE_NUM 2

// Defines if the dispenser should home the Z-axis during homing
#define EXPERIMENTAL_Z_HOMING true 

// Defines if the dispenser should perform a Z-Dip during a dispense action
#define EXPERIMENTAL_Z_DIP true 

// Defines if the dispenser should respond to Z-movement requests via PLC commands
#define EXPERIMENTAL_Z_AXIS_MOVEMENT_API true 