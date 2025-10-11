#define LIMIT_S_X_MIN 46 // 32
#define LIMIT_S_Y_MIN 32 // 33
#define LIMIT_S_Z_MIN 36

//#define LIMIT_S_X_MAX 18
//#define LIMIT_S_Y_MAX 19
#define LIMIT_S_X_MAX 22 //johari 20240610
#define LIMIT_S_Y_MAX 44 //johari 20240610
#define LIMIT_S_Z_MAX 38


#define MOTOR_X_CW 27
#define MOTOR_X_CLK 25

#define MOTOR_Y_CW 29
#define MOTOR_Y_CLK 31

#define MOTOR_z_CW 35
#define MOTOR_Z_CLK 37

//#define MOTOR_X_SPEED 8000//, 2000
//#define MOTOR_Y_SPEED 4800//, 1000

//#define MOTOR_X_ACCELERATION 8000 // 4800, 3200
//#define MOTOR_Y_ACCELERATION 4000 // 2400 1600 // 3200

#define MOTOR_X_SPEED 80000 // 8000, 2000
#define MOTOR_Y_SPEED 800  // 4800, 1000
#define MOTOR_Z_SPEED 80000  // 4800, 1000

#define MOTOR_X_ACCELERATION 100000//8000 // 4800, 3200
#define MOTOR_Y_ACCELERATION 1000 //4000 // 2400 1600 // 3200
#define MOTOR_Z_ACCELERATION 100000//4000 // 2400 1600 // 3200


#define MAXXMM 400 //190 (S) //max x/col dimension in mm
#define MAXYMM 330 //250 (S) //max y/row dimension in mm

//(S)
//#define STEPS_PER_UNIT_X 400L // (X step motor specs: lead screw is 4mm, 1600 pulse/rev(8 microsteps driver), 4mm/1600 = 0.0025mm, 1mm/0.0025 = 400 steps)
//#define STEPS_PER_UNIT_Y 320L // (Y step motor specs: lead screw is 10mm, 3200 pulse/rev(16 microsteps driver), 10mm/3200 = 0.003125mm, 1mm/0.003125 = 320 steps/unit)
//20240917: erdiongson - Use the STEPS_PER_UNIT_Y 160L if we are using the new motor. use this moving forward
//#define STEPS_PER_UNIT_Y 160L // (Y step motor specs: lead screw is 20mm, 1600 pulse/rev(8 microsteps driver), 20mm/3200 = 0.00625mm, 1mm/0.00625 = 160 steps/unit)

//(L)
//20240625: erdiongson - Trying out 3200 pulse per revolution (v2.05L)
#define STEPS_PER_UNIT_X 402L // (X step motor specs: lead screw is 10mm, 3200 pulse/rev(16 microsteps driver), 10mm/3200 = 0.003125mm, 1mm/0.003125 = 320 steps/unit)
//#define STEPS_PER_UNIT_X 160L // (X step motor specs: lead screw is 10mm, 3200 pulse/rev(16 microsteps driver), 10mm/3200 = 0.003125mm, 1mm/0.003125 = 320 steps/unit)
#define STEPS_PER_UNIT_Y 160L // (Y step motor specs: lead screw is 10mm, 3200 pulse/rev(16 microsteps driver), 10mm/3200 = 0.003125mm, 1mm/0.003125 = 320 steps/unit)
//20240917: erdiongson - Use the STEPS_PER_UNIT_Y 160L if we are using the new motor. use this moving forward
//#define STEPS_PER_UNIT_Y 160L // (Y step motor specs: lead screw is 20mm, 1600 pulse/rev(8 microsteps driver), 20mm/3200 = 0.00625mm, 1mm/0.00625 = 160 steps/unit)

#define STEPS_PER_UNIT_Z 390L

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

#define DEBUG 0

#define FWVER "4.0"