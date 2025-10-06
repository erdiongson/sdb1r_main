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

#define motor_x_speed 80000 // 8000, 2000
#define motor_y_speed 800  // 4800, 1000
#define motor_z_speed 80000  // 4800, 1000

#define motor_x_Acceleration 100000//8000 // 4800, 3200
#define motor_y_Acceleration 1000 //4000 // 2400 1600 // 3200
#define motor_z_Acceleration 100000//4000 // 2400 1600 // 3200


#define MAXXMM 400 //190 (S) //max x/col dimension in mm
#define MAXYMM 330 //250 (S) //max y/row dimension in mm

//(S)
//#define STEPS_PER_UNIT_X 400L // (X step motor specs: lead screw is 4mm, 1600 pulse/rev(8 microsteps driver), 4mm/1600 = 0.0025mm, 1mm/0.0025 = 400 steps)
//#define STEPS_PER_UNIT_Y 320L // (Y step motor specs: lead screw is 10mm, 3200 pulse/rev(16 microsteps driver), 10mm/3200 = 0.003125mm, 1mm/0.003125 = 320 steps/unit)
//20240917: erdiongson - Use the STEPS_PER_UNIT_Y 160L if we are using the new motor. use this moving forward
//#define STEPS_PER_UNIT_Y 160L // (Y step motor specs: lead screw is 20mm, 1600 pulse/rev(8 microsteps driver), 20mm/3200 = 0.00625mm, 1mm/0.00625 = 160 steps/unit)

//(L)
//20240625: erdiongson - Trying out 3200 pulse per revolution (v2.05L)
#define STEPS_PER_UNIT_X 300 // (X step motor specs: lead screw is 10mm, 3200 pulse/rev(16 microsteps driver), 10mm/3200 = 0.003125mm, 1mm/0.003125 = 320 steps/unit)
//#define STEPS_PER_UNIT_X 160L // (X step motor specs: lead screw is 10mm, 3200 pulse/rev(16 microsteps driver), 10mm/3200 = 0.003125mm, 1mm/0.003125 = 320 steps/unit)
#define STEPS_PER_UNIT_Y 100L // (Y step motor specs: lead screw is 10mm, 3200 pulse/rev(16 microsteps driver), 10mm/3200 = 0.003125mm, 1mm/0.003125 = 320 steps/unit)
//20240917: erdiongson - Use the STEPS_PER_UNIT_Y 160L if we are using the new motor. use this moving forward
//#define STEPS_PER_UNIT_Y 160L // (Y step motor specs: lead screw is 20mm, 1600 pulse/rev(8 microsteps driver), 20mm/3200 = 0.00625mm, 1mm/0.00625 = 160 steps/unit)

#define STEPS_PER_UNIT_Z 30L

//Change MAX_TUBES_X and MAX_TUBES_Y depending on the use;
//for (L) 300x300 - MAX_TUBES_X 33; MAX_TUBES_Y 33
//for (S) 200x300 - MAX_TUBES_X 20; MAX_TUBES_Y 27
#define MAX_TUBES_X 42//try054 33
#define MAX_TUBES_Y 33
#define MAX_ZDIP 2000 // 20cm

#define SUPER_PASSWORD "superXQ"
#define INITIAL_PASSWORD "init1234"

// Defines the number of "dispenses" performed during priming
#define PRIME_DISPENSE_NUM 2

// Defines if the dispenser should home the Z-axis during homing
#define EXPERIMENTAL_Z_HOMING true 

// Defines if the dispenser should perform a Z-Dip during a dispense action
#define EXPERIMENTAL_Z_DIP true 

// Defines if the dispenser should respond to Z-movement requests via PLC commands
#define EXPERIMENTAL_Z_AXIS_MOVEMENT_API true 

