#include "sgx.h"
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
void setCompensationParams(int FD) {
    uint8_t calData0[25];
    uint8_t calData1[7];

    /* read calibration data */
    i2c_smbus_read_i2c_block_data(FD, CAL_DATA0_START_ADDR, CAL_DATA0_LENGTH, calData0);
    i2c_smbus_read_i2c_block_data(FD, CAL_DATA1_START_ADDR, CAL_DATA1_LENGTH, calData1);

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
int FD = 0;
uint8_t dataBlock[8];
int32_t temp_int = 0;
int32_t press_int = 0;
int32_t hum_int = 0;
double station_press = 0.0;
int init_BME(){
    

    /* open i2c comms */
    if ((FD = open(DEV_PATH, O_RDWR)) < 0) {
        perror("Unable to open i2c device");
        return 1;
    }

    /* configure i2c slave */
    if (ioctl(FD, I2C_SLAVE, DEV_ID) < 0) {
        perror("Unable to configure i2c slave device");
        close(FD);
        return 2;
    }

    /* check our identification */
    if (i2c_smbus_read_byte_data(FD, IDENT) != 0x60) {
        perror("device ident error");
        close(FD);
        return 3;
    }

    /* device soft reset */
    i2c_smbus_write_byte_data(FD, SOFT_RESET, 0xB6);
    usleep(50000);

    /* read and set compensation parameters */
    setCompensationParams(FD);

    /* humidity o/s x 1 */
    i2c_smbus_write_byte_data(FD, CTRL_HUM, 0x1);

    /* filter off */
    i2c_smbus_write_byte_data(FD, CONFIG, 0);

    /* set forced mode, pres o/s x 1, temp o/s x 1 and take 1st reading */
    i2c_smbus_write_byte_data(FD, CTRL_MEAS, 0x25);
}
void  getAll(double *p){
    if ((i2c_smbus_read_byte_data(FD, STATUS) & 0x9) != 0) {
            printf("%s\n", "Error, data not ready");
            return ;
        }

        /* read data registers */
        i2c_smbus_read_i2c_block_data(FD, DATA_START_ADDR, DATA_LENGTH, dataBlock);

        /* awake and take next reading */
        i2c_smbus_write_byte_data(FD, CTRL_MEAS, 0x25);

        /* get raw temp */
        temp_int = (dataBlock[3] << 16 | dataBlock[4] << 8 | dataBlock[5]) >> 4;
        press_int = (dataBlock[0] << 16 | dataBlock[1] << 8 | dataBlock[2]) >> 4;


     station_press = BME280_compensate_P_double(press_int) / 100.0;
     hum_int = dataBlock[6] << 8 | dataBlock[7];
     p[0]=BME280_compensate_T_double(temp_int);
     p[1]=station_press;
     p[2]=BME280_compensate_H_double(hum_int);
     p[3]=getDis();
}
double getP(){
    if ((i2c_smbus_read_byte_data(FD, STATUS) & 0x9) != 0) {
            printf("%s\n", "Error, data not ready");
            return 0;
        }

        /* read data registers */
        i2c_smbus_read_i2c_block_data(FD, DATA_START_ADDR, DATA_LENGTH, dataBlock);

        /* awake and take next reading */
        i2c_smbus_write_byte_data(FD, CTRL_MEAS, 0x25);

        press_int = (dataBlock[0] << 16 | dataBlock[1] << 8 | dataBlock[2]) >> 4;


     station_press = BME280_compensate_P_double(press_int) / 100.0;
        return station_press;
    
}
double getH(){
    if ((i2c_smbus_read_byte_data(FD, STATUS) & 0x9) != 0) {
            printf("%s\n", "Error, data not ready");
            return 0;
        }

        /* read data registers */
        i2c_smbus_read_i2c_block_data(FD, DATA_START_ADDR, DATA_LENGTH, dataBlock);

        /* awake and take next reading */
        i2c_smbus_write_byte_data(FD, CTRL_MEAS, 0x25);


        /* get raw humidity */
        hum_int = dataBlock[6] << 8 | dataBlock[7];
        return BME280_compensate_H_double(hum_int);
    
}
// int main(void) {
    
//     // uint8_t dataBlock[8];
//     // int32_t temp_int = 0;
//     // int32_t press_int = 0;
//     // int32_t hum_int = 0;
//     // double station_press = 0.0;

//     // /* open i2c comms */
//     // if ((FD = open(DEV_PATH, O_RDWR)) < 0) {
//     //     perror("Unable to open i2c device");
//     //     return 1;
//     // }

//     // /* configure i2c slave */
//     // if (ioctl(FD, I2C_SLAVE, DEV_ID) < 0) {
//     //     perror("Unable to configure i2c slave device");
//     //     close(FD);
//     //     return 2;
//     // }

//     // /* check our identification */
//     // if (i2c_smbus_read_byte_data(FD, IDENT) != 0x60) {
//     //     perror("device ident error");
//     //     close(FD);
//     //     return 3;
//     // }

//     // /* device soft reset */
//     // i2c_smbus_write_byte_data(FD, SOFT_RESET, 0xB6);
//     // usleep(50000);

//     // /* read and set compensation parameters */
//     // setCompensationParams(FD);

//     // /* humidity o/s x 1 */
//     // i2c_smbus_write_byte_data(FD, CTRL_HUM, 0x1);

//     // /* filter off */
//     // i2c_smbus_write_byte_data(FD, CONFIG, 0);

//     // /* set forced mode, pres o/s x 1, temp o/s x 1 and take 1st reading */
//     // i2c_smbus_write_byte_data(FD, CTRL_MEAS, 0x25);

//     // for (;;) {
//         /* Sleep for 1 second for demonstration purposes.
//          * Data can be streamed with a sleep time down
//          * to 10 ms [usleep(10000)] with oversampling set at x1.
//          * See section 9, appendix B of the Bosch technical
//          * datasheet for details on measurement time calculation.
//          */
//         // sleep(1);

//         /* check data is ready to read */
//         if ((i2c_smbus_read_byte_data(FD, STATUS) & 0x9) != 0) {
//             printf("%s\n", "Error, data not ready");
//             return 0;
//         }

//         /* read data registers */
//         i2c_smbus_read_i2c_block_data(FD, DATA_START_ADDR, DATA_LENGTH, dataBlock);

//         /* awake and take next reading */
//         i2c_smbus_write_byte_data(FD, CTRL_MEAS, 0x25);

//         /* get raw temp */
//         temp_int = (dataBlock[3] << 16 | dataBlock[4] << 8 | dataBlock[5]) >> 4;

//         /* get raw pressure */
//         press_int = (dataBlock[0] << 16 | dataBlock[1] << 8 | dataBlock[2]) >> 4;

//         /* get raw humidity */
//         hum_int = dataBlock[6] << 8 | dataBlock[7];

//         /* calculate and print compensated temp. This function is called first, as it also sets the
//          * t_fine global variable required by the next two function calls
//          */
//         // printf("%.2f", BME280_compensate_T_double(temp_int));

//         // station_press = BME280_compensate_P_double(press_int) / 100.0;

//         /* calculate and print compensated press */
//         // printf("%.2f", station_press);

//         // /* calculate and print compensated humidity */
//         printf("%.2f", BME280_compensate_H_double(hum_int));
    

//     return 0;
// }

double sta2sea(double station_press) {
    return station_press * exp((-M * G * -LOCAL_HASL) / (R * T));
}
