#include "sgx.h"
void moto_run(double x,double y);
void direct(double speed){
    moto_run(-speed,speed);
}
void stop(){
    // digitalWrite(GPIO22, LOW);
    // digitalWrite(GPIO23,LOW);
    // digitalWrite(GPIO24, LOW);
    // digitalWrite(GPIO25,LOW);
    moto_run(0,0);
}
void left(double speed){
    // digitalWrite(GPIO22,LOW );
    // digitalWrite(GPIO23,HIGH);
    // digitalWrite(GPIO24, LOW);
    // digitalWrite(GPIO25,HIGH);
    moto_run(-speed,-speed);
}
void right(double speed){
    // digitalWrite(GPIO22, HIGH);
    // digitalWrite(GPIO23,LOW);
    // digitalWrite(GPIO24, HIGH);//
    // digitalWrite(GPIO25,LOW);
    moto_run(speed,speed);
}
void moto_run(double x,double y){           //电机控制部分
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
const int base=112.5;
int fd,fd_M;
void flash(){
    int data=wiringPiI2CReadReg8(fd,1);
    int s1=data&1,s2=(data>>1)&1,s3=(data>>2)&1,s4=(data>>3)&1;
    if(s1 == 0&& s2 == 0&& s3 == 1&&s4 == 0)  //0010
      err = -0.65625;
    if(s1 == 0&& s2 == 0&& s3 == 0&&s4 == 1)  //0001
      err = -2.28875;
    if(s1 == 0&& s2 == 1&& s3 == 0&&s4 == 0)  //0100
      err = 0.65625;
    if(s1 == 1&& s2 == 0&& s3 == 0&&s4 == 0)  //1000
      err = 2.28875;
    if(s1 ==1 && s2==1 && s3==0 &&s4==0){   //1100 
        err=3.375;
    }
    if(s1 == 0&& s2 == 0&& s3 == 1&&s4 == 1){ //0011
        err=-3.375;
    }
    if(s1 == 0&& s2 == 1&& s3 == 1&&s4 == 1){ //0111
        err=-3.75;
    }
    if(s1 == 1&& s2 == 1&& s3 == 1&&s4 == 0){
        err=3.75;
    }
    printf("%d %d %d %d\n",s1,s2,s3,s4);
    // err = (s1 - s4) * 3 + (s2 - s3);
    err_sum += err;
    err_diff = err - last_err;
    last_err = err;
}
double  cacu_pwm()            //pid运算部分
{
    double output;
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
    fd_M = wiringPiI2CSetup(DEVICE_ID);
    wiringPiI2CWriteReg8(fd_M, REG_POWER_CTL, 0b00001000);
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
double getPitch(){
    int dataX = wiringPiI2CReadReg16(fd_M, REG_DATA_X_LOW);
    dataX = -(~(int16_t)dataX + 1);
    int dataY = wiringPiI2CReadReg16(fd_M, REG_DATA_Y_LOW);
    dataY = -(~(int16_t)dataY + 1);
    int dataZ = wiringPiI2CReadReg16(fd_M, REG_DATA_Z_LOW);
    dataZ = -(~(int16_t)dataZ + 1);
    double pitch=atan2((- dataY) , sqrt(dataX * dataX + dataZ * dataZ)) * 57.3;
    return pitch;
}
