#include<wiringPi.h>
#include <softPwm.h>
#include<wiringPiI2C.h>
#include <sys/time.h> 
#define Trig_pin 4
#define Echo_pin 5
#define GPIO22 22
#define GPIO23 23
#define GPIO24 24
#define GPIO25 25
#define EN0 2 //左轮控速
#define EN1 3 //右轮控速
#define SDA1 8
#define SCL1 9
#define TRACE 0x78 