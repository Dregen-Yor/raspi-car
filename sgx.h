#ifndef _WIRING_PI
#define _WIRING_PI
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

#include <fcntl.h>
#include <i2c/smbus.h>
#include <linux/i2c-dev.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <i2c/smbus.h>
#endif

#ifndef __BME280_H
#define __BME280_H

/* edit device DEV_ID, DEV_PATH and LOCAL_HASL as needed */
#define DEV_ID 0x77
#define DEV_PATH "/dev/i2c-1"
#define LOCAL_HASL 25.0 /* local height above sea level  m */

#define IDENT 0xD0
#define SOFT_RESET 0xE0
#define CTRL_HUM 0xF2
#define STATUS 0xF3
#define CTRL_MEAS 0xF4
#define CONFIG 0xF5

#define DATA_START_ADDR 0xF7
#define DATA_LENGTH 8

/* sea level pressure conversion constants */
#define G 9.80665   /* acceleration due to gravity  m/s2 */
#define M 0.0289644 /* molar mass of Earth's air  kg/mol */
#define T 288.15    /* standard temperature  K */
#define R 8.3144598 /* universal gas constant  J/(mol·K) */

double sta2sea(double station_press);

#endif

#ifndef __COMPENSATION_H
#define __COMPENSATION_H

#include <stdint.h>

#define CAL_DATA0_START_ADDR 0x88
#define CAL_DATA0_LENGTH 25
#define CAL_DATA1_START_ADDR 0xE1
#define CAL_DATA1_LENGTH 7

double BME280_compensate_T_double(int32_t adc_T);
double BME280_compensate_P_double(int32_t adc_P);
double BME280_compensate_H_double(int32_t adc_H);
void setCompensationParams(int fd);

#endif


#ifdef __cplusplus
extern "C"{
#endif

    void direct(double speed);
    void left(double speed);
    void right(double speed);
    void stop();
    void moto_run(double x,double y);
    void trace();
    double getDis();
    void flash();
 
#ifdef __cplusplus
}
#endif

