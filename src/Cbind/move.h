#include "sgx.h"
void moto_run(int x,int y);
void direct(){
    moto_run(-100,100);
}
void stop(){
    digitalWrite(GPIO22, LOW);
    digitalWrite(GPIO23,LOW);
    digitalWrite(GPIO24, LOW);
    digitalWrite(GPIO25,LOW);
    // moto_run(0,0);
}
void left(){
    // digitalWrite(GPIO22,LOW );
    // digitalWrite(GPIO23,HIGH);
    // digitalWrite(GPIO24, LOW);
    // digitalWrite(GPIO25,HIGH);
    moto_run(100,100);
}
void right(){
    // digitalWrite(GPIO22, HIGH);
    // digitalWrite(GPIO23,LOW);
    // digitalWrite(GPIO24, HIGH);//
    // digitalWrite(GPIO25,LOW);
    moto_run(-100,-100);
}
void moto_run(int x,int y){           //电机控制部分
     if(x>=0){
        softPwmWrite(GPIO22,x); 
        softPwmWrite(GPIO23,0); 
    }
    if(x<0){
        softPwmWrite(GPIO22,0); 
        softPwmWrite(GPIO23,-x); 
    }
    if(y>=0){
        softPwmWrite(GPIO24,y); 
        softPwmWrite(GPIO25,0); 
    }
      if(y<0)
    {
        softPwmWrite(GPIO24,0); 
        softPwmWrite(GPIO25,-y); 
    }
}
double err,last_err,err_diff,err_sum;
const int base=100;
int fd;
void flash(){
    int data=wiringPiI2CReadReg8(fd,1);
    int s1=data&1,s2=(data>>1)&1,s3=(data>>2)&1,s4=(data>>3)&1;
    if(s1 == 0&& s2 == 0&& s3 == 1&&s4 == 0)  //0010
      err = -0.65625;
    if(s1 == 0&& s2 == 0&& s3 == 0&&s4 == 1)  //0001
      err = -2.21875;
    if(s1 == 0&& s2 == 1&& s3 == 0&&s4 == 0)  //0100
      err = 0.65625;
    if(s1 == 1&& s2 == 0&& s3 == 0&&s4 == 0)  //1000
      err = 2.21875;
    if(s1 ==1 && s2==1 && s3==0 &&s4==0){   //1100 
        err=3.375;
    }
    if(s1 == 0&& s2 == 0&& s3 == 1&&s4 == 1){ //001& 0101
        err=-3.375;
    }
    if(s1 == 0&& s2 == 1&& s3 == 1&&s4 == 1){ //0111
        err=-3.75;
    }
    if(s1 == 1&& s2 == 1&& s3 == 1&&s4 == 0){
        err=3.75;
    }
    // err = (s1 - s4) * 3 + (s2 - s3);
    err_sum += err;
    err_diff = err - last_err;
    last_err = err;
}
int  cacu_pwm()            //pid运算部分
{
    int output;
    double kp=60.75,ki=0,kd=10;    //pid需要根据实际情况进行调节
    output = kp*err + ki*err_sum + kd*err_diff;    
    last_err = err;
    return output;
}

void trace(){
    flash();
    moto_run(-(base+cacu_pwm()),base-cacu_pwm());
}
void init(){
    wiringPiSetup();
    fd=wiringPiI2CSetup(TRACE);
    pinMode(GPIO22, OUTPUT); // 设置gpio0为输出模式
    pinMode(GPIO23, OUTPUT);
    pinMode(GPIO24, OUTPUT);
    pinMode(GPIO25, OUTPUT);
    pinMode (Trig_pin, OUTPUT);
    pinMode (Echo_pin, INPUT);
    pinMode(EN0,OUTPUT);
    digitalWrite(EN0,HIGH);
    softPwmCreate(GPIO22, 150, 150);
    softPwmCreate(GPIO23, 150, 150);
    softPwmCreate(GPIO24, 150, 150);
    softPwmCreate(GPIO25, 150, 150);
    softPwmCreate(EN0, 100, 100);
    pinMode(EN1,OUTPUT);
    softPwmCreate(EN1, 100, 100);
     if ((fd = open(DEV_PATH, O_RDWR)) < 0) {
        perror("Unable to open i2c device");
        return;
    }

    /* configure i2c slave */
    if (ioctl(fd, I2C_SLAVE, DEV_ID) < 0) {
        perror("Unable to configure i2c slave device");
        close(fd);
        return;
    }

    /* check our identification */
    if (i2c_smbus_read_byte_data(fd, IDENT) != 0x60) {
        perror("device ident error");
        close(fd);
        return;
    }

    /* device soft reset */
    i2c_smbus_write_byte_data(fd, SOFT_RESET, 0xB6);
    usleep(50000);

    /* read and set compensation parameters */
    setCompensationParams(fd);

    /* humidity o/s x 1 */
    i2c_smbus_write_byte_data(fd, CTRL_HUM, 0x1);

    /* filter off */
    i2c_smbus_write_byte_data(fd, CONFIG, 0);

    /* set forced mode, pres o/s x 1, temp o/s x 1 and take 1st reading */
    i2c_smbus_write_byte_data(fd, CTRL_MEAS, 0x25);
}
double getDis(){
    struct timeval start_time;
    struct timeval stop_time;

    digitalWrite (Trig_pin, LOW); 
    delay(10); // 延时10 毫秒  等待让电平稳定
    digitalWrite (Trig_pin, HIGH);
    delayMicroseconds(10); // 给一个10us的高电平
    // 这里发现 delayMicroseconds()，这个函数是不准确的，实际是大于10us，具体结果如何，需要示波器测量一下。
    digitalWrite (Trig_pin, LOW);

    while(!(digitalRead(Echo_pin) == 1));      
    gettimeofday(&start_time, NULL);           // 获取当前时间 开始接收到返回信号的时候     
    while(!(digitalRead(Echo_pin) == 0));      
    gettimeofday(&stop_time, NULL);           // 获取当前时间 结束信号

    double start, stop;
    start = start_time.tv_sec * 1000000 + start_time.tv_usec;   //微秒级的时间  
    stop  = stop_time.tv_sec * 1000000 + stop_time.tv_usec;
    double dis = (stop - start) / 1000000 * 34000 / 2;  //计算时间差求出距离
    // 这里测试的 距离 实际就是上面时序图的回响时间长度乘以声速的结果。
    return dis;
}
static int32_t t_fine = 0;

static uint16_t dig_T1 = 0;
static int16_t dig_T2 = 0;
static int16_t dig_T3 = 0;

static uint16_t dig_P1 = 0;
static int16_t dig_P2 = 0;
static int16_t dig_P3 = 0;
static int16_t dig_P4 = 0;
static int16_t dig_P5 = 0;
static int16_t dig_P6 = 0;
static int16_t dig_P7 = 0;
static int16_t dig_P8 = 0;
static int16_t dig_P9 = 0;

static uint8_t dig_H1 = 0;
static int16_t dig_H2 = 0;
static uint8_t dig_H3 = 0;
static int16_t dig_H4 = 0;
static int16_t dig_H5 = 0;
static int8_t dig_H6 = 0;

/* The following three compensation functions are reproduced from Appendix A: Section 8.1,
 * Compensation formulas in double precision floating point, of the Bosch Sensortec technical
 * datasheet.
 *
 * https://www.bosch-sensortec.com/media/boschsensortec/downloads/datasheets/bst-bme280-ds002.pdf
 *
 */

/* Returns temperature in DegC, double precision. Output value of “51.23”
 * equals 51.23 DegC. t_fine carries fine temperature as global value
 */
double BME280_compensate_T_double(int32_t adc_T) {
    double var1, var2, T__;
    var1 = (((double)adc_T) / 16384.0 - ((double)dig_T1) / 1024.0) * ((double)dig_T2);
    var2 = ((((double)adc_T) / 131072.0 - ((double)dig_T1) / 8192.0) *
            (((double)adc_T) / 131072.0 - ((double)dig_T1) / 8192.0)) *
           ((double)dig_T3);
    t_fine = (int32_t)(var1 + var2);
    T__ = (var1 + var2) / 5120.0;
    return T__;
}

/* Returns pressure in Pa as double. Output value of “96386.2” equals 96386.2 Pa
 * = 963.862 hPa
 */
double BME280_compensate_P_double(int32_t adc_P) {
    double var1, var2, p;
    var1 = ((double)t_fine / 2.0) - 64000.0;
    var2 = var1 * var1 * ((double)dig_P6) / 32768.0;
    var2 = var2 + var1 * ((double)dig_P5) * 2.0;
    var2 = (var2 / 4.0) + (((double)dig_P4) * 65536.0);
    var1 = (((double)dig_P3) * var1 * var1 / 524288.0 + ((double)dig_P2) * var1) / 524288.0;
    var1 = (1.0 + var1 / 32768.0) * ((double)dig_P1);
    /* avoid exception caused by division by zero */
    if (var1 == 0.0) {
        return 0;
    }
    p = 1048576.0 - (double)adc_P;
    p = (p - (var2 / 4096.0)) * 6250.0 / var1;
    var1 = ((double)dig_P9) * p * p / 2147483648.0;
    var2 = p * ((double)dig_P8) / 32768.0;
    p = p + (var1 + var2 + ((double)dig_P7)) / 16.0;
    return p;
}

/* Returns humidity in %rH as as double. Output value of “46.332”
 * represents 46.332 %rH
 */
double BME280_compensate_H_double(int32_t adc_H) {
    double var_H;
    var_H = (((double)t_fine) - 76800.0);
    var_H = (adc_H - (((double)dig_H4) * 64.0 + ((double)dig_H5) / 16384.0 * var_H)) *
            (((double)dig_H2) / 65536.0 *
             (1.0 + ((double)dig_H6) / 67108864.0 * var_H *
                        (1.0 + ((double)dig_H3) / 67108864.0 * var_H)));
    var_H = var_H * (1.0 - ((double)dig_H1) * var_H / 524288.0);
    if (var_H > 100.0)
        var_H = 100.0;
    else if (var_H < 0.0)
        var_H = 0.0;
    return var_H;
}

/* Read calibration data and determine trimming parameters */
void setCompensationParams(int fd) {
    uint8_t calData0[25];
    uint8_t calData1[7];

    /* read calibration data */
    i2c_smbus_read_i2c_block_data(fd, CAL_DATA0_START_ADDR, CAL_DATA0_LENGTH, calData0);
    i2c_smbus_read_i2c_block_data(fd, CAL_DATA1_START_ADDR, CAL_DATA1_LENGTH, calData1);

    /* trimming parameters */
    dig_T1 = calData0[1] << 8 | calData0[0];
    dig_T2 = calData0[3] << 8 | calData0[2];
    dig_T3 = calData0[5] << 8 | calData0[4];

    dig_P1 = calData0[7] << 8 | calData0[6];
    dig_P2 = calData0[9] << 8 | calData0[8];
    dig_P3 = calData0[11] << 8 | calData0[10];
    dig_P4 = calData0[13] << 8 | calData0[12];
    dig_P5 = calData0[15] << 8 | calData0[14];
    dig_P6 = calData0[17] << 8 | calData0[16];
    dig_P7 = calData0[19] << 8 | calData0[18];
    dig_P8 = calData0[21] << 8 | calData0[20];
    dig_P9 = calData0[23] << 8 | calData0[22];

    dig_H1 = calData0[24];
    dig_H2 = calData1[1] << 8 | calData1[0];
    dig_H3 = calData1[2];
    dig_H4 = calData1[3] << 4 | (calData1[4] & 0xF);
    dig_H5 = calData1[5] << 4 | (calData1[4] >> 4);
    dig_H6 = calData1[6];
}

int fd = 0;
uint8_t dataBlock[8];
int32_t temp_int = 0;
int32_t press_int = 0;
int32_t hum_int = 0;
double station_press = 0.0;

double getT()
{
    // sleep(1);

    /* check data is ready to read */
    if ((i2c_smbus_read_byte_data(fd, STATUS) & 0x9) != 0)
    {
        printf("%s\n", "Error, data not ready");
        return 0.0;
    }

    /* read data registers */
    i2c_smbus_read_i2c_block_data(fd, DATA_START_ADDR, DATA_LENGTH, dataBlock);

    /* awake and take next reading */
    i2c_smbus_write_byte_data(fd, CTRL_MEAS, 0x25);

    /* get raw temp */
    temp_int = (dataBlock[3] << 16 | dataBlock[4] << 8 | dataBlock[5]) >> 4;

    /* calculate and print compensated temp. This function is called first, as it also sets the
     * t_fine global variable required by the next two function calls
     */
    return BME280_compensate_T_double(temp_int);
}
double getP()
{
    // sleep(1);

    /* check data is ready to read */
    if ((i2c_smbus_read_byte_data(fd, STATUS) & 0x9) != 0)
    {
        printf("%s\n", "Error, data not ready");
        return 0.0;
    }

    /* read data registers */
    i2c_smbus_read_i2c_block_data(fd, DATA_START_ADDR, DATA_LENGTH, dataBlock);

    /* awake and take next reading */
    i2c_smbus_write_byte_data(fd, CTRL_MEAS, 0x25);
    press_int = (dataBlock[0] << 16 | dataBlock[1] << 8 | dataBlock[2]) >> 4;

    station_press = BME280_compensate_P_double(press_int) / 100.0;
    return station_press;
}
double getH()
{
    // sleep(1);

    /* check data is ready to read */
    if ((i2c_smbus_read_byte_data(fd, STATUS) & 0x9) != 0)
    {
        printf("%s\n", "Error, data not ready");
        return 0.0;
    }

    /* read data registers */
    i2c_smbus_read_i2c_block_data(fd, DATA_START_ADDR, DATA_LENGTH, dataBlock);

    /* awake and take next reading */
    i2c_smbus_write_byte_data(fd, CTRL_MEAS, 0x25);
    /* get raw humidity */
    hum_int = dataBlock[6] << 8 | dataBlock[7];
    return BME280_compensate_H_double(hum_int);
}
// void init() {
    // if ((fd = open(DEV_PATH, O_RDWR)) < 0) {
    //     perror("Unable to open i2c device");
    //     return;
    // }

    // /* configure i2c slave */
    // if (ioctl(fd, I2C_SLAVE, DEV_ID) < 0) {
    //     perror("Unable to configure i2c slave device");
    //     close(fd);
    //     return;
    // }

    // /* check our identification */
    // if (i2c_smbus_read_byte_data(fd, IDENT) != 0x60) {
    //     perror("device ident error");
    //     close(fd);
    //     return;
    // }

    // /* device soft reset */
    // i2c_smbus_write_byte_data(fd, SOFT_RESET, 0xB6);
    // usleep(50000);

    // /* read and set compensation parameters */
    // setCompensationParams(fd);

    // /* humidity o/s x 1 */
    // i2c_smbus_write_byte_data(fd, CTRL_HUM, 0x1);

    // /* filter off */
    // i2c_smbus_write_byte_data(fd, CONFIG, 0);

    // /* set forced mode, pres o/s x 1, temp o/s x 1 and take 1st reading */
    // i2c_smbus_write_byte_data(fd, CTRL_MEAS, 0x25);

    // for (;;) {
    //     /* Sleep for 1 second for demonstration purposes.
    //      * Data can be streamed with a sleep time down
    //      * to 10 ms [usleep(10000)] with oversampling set at x1.
    //      * See section 9, appendix B of the Bosch technical
    //      * datasheet for details on measurement time calculation.
    //      */
    //     sleep(1);

    //     /* check data is ready to read */
    //     if ((i2c_smbus_read_byte_data(fd, STATUS) & 0x9) != 0) {
    //         printf("%s\n", "Error, data not ready");
    //         continue;
    //     }

    //     /* read data registers */
    //     i2c_smbus_read_i2c_block_data(fd, DATA_START_ADDR, DATA_LENGTH, dataBlock);

    //     /* awake and take next reading */
    //     i2c_smbus_write_byte_data(fd, CTRL_MEAS, 0x25);

    //     /* get raw temp */
    //     temp_int = (dataBlock[3] << 16 | dataBlock[4] << 8 | dataBlock[5]) >> 4;

    //     /* get raw pressure */
    //     press_int = (dataBlock[0] << 16 | dataBlock[1] << 8 | dataBlock[2]) >> 4;

    //     /* get raw humidity */
    //     hum_int = dataBlock[6] << 8 | dataBlock[7];

    //     /* calculate and print compensated temp. This function is called first, as it also sets the
    //      * t_fine global variable required by the next two function calls
    //      */
    //     printf("当前温度: %.2f°C  ", BME280_compensate_T_double(temp_int));

    //     station_press = BME280_compensate_P_double(press_int) / 100.0;

    //     /* calculate and print compensated press */
    //     printf("大气压强: %.2f hPa, sea level press: %.2f  ", station_press,
    //            sta2sea(station_press));

    //     /* calculate and print compensated humidity */
    //     printf("humidity: %.2f %%rH\n", BME280_compensate_H_double(hum_int));
    // }
// }

double sta2sea(double station_press) {
    return station_press * exp((-M * G * -LOCAL_HASL) / (R * T));
}