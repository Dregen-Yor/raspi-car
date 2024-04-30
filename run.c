#include <stdio.h>
#include "sgx.h"
int fd;
void init(){
    wiringPiSetup();
    fd=wiringPiI2CSetup(TRACE);
    pinMode(GPIO22, OUTPUT); // 设置gpio0为输出模式
    pinMode(GPIO23, OUTPUT);
    pinMode(GPIO24, OUTPUT);
    pinMode(GPIO25, OUTPUT);
     pinMode (Trig_pin, OUTPUT);
    // 初始化接收引脚
    pinMode (Echo_pin, INPUT);
    pinMode(EN0,OUTPUT);
    digitalWrite(EN0,HIGH);
    softPwmCreate(EN0, 100, 100);
    pinMode(EN1,OUTPUT);
    softPwmCreate(EN1, 100, 100);
}
void direct(){
    digitalWrite(GPIO22,LOW );
    digitalWrite(GPIO23,HIGH);
    digitalWrite(GPIO24, HIGH);
    digitalWrite(GPIO25,LOW);
}
void stop(){
    digitalWrite(GPIO22, LOW);
    digitalWrite(GPIO23,LOW);
    digitalWrite(GPIO24, LOW);
    digitalWrite(GPIO25,LOW);
}
void left(){
    digitalWrite(GPIO22,LOW );
    digitalWrite(GPIO23,HIGH);
    digitalWrite(GPIO24, LOW);
    digitalWrite(GPIO25,HIGH);
}
void right(){
    digitalWrite(GPIO22, HIGH);
    digitalWrite(GPIO23,LOW);
    digitalWrite(GPIO24, HIGH);//
    digitalWrite(GPIO25,LOW);
}
void speed(int pos){
    softPwmWrite(EN0,pos);
    softPwmWrite(EN1,pos);
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
void trace(){
    while(1){
        int data=wiringPiI2CReadReg8(fd,1);
        int s1=data&1,s2=(data>>1)&1,s3=(data>>2)&1,s4=(data>>3)&1;
        // printf("%d %d %d %d\n",s1,s2,s3,s4);
        if(getDis()<30){
            left();
            printf("%lf\n",getDis());
            continue;
        }
        delay(10);
        if(s2&&s3){
            direct();
        }
        else if(s1&&!s4){
            left();
        }
        else if(!s1&&s4){
            right();
        }
        else if(!s1&&!s4&&s2&&!s3){
            right();
        }
        else if(!s1&&!s4&&!s2&&s3){
            left();
        }
        else {
            continue;
        }
    }
    
}
int main(){
    init();
    speed(50);
    trace();
        
    stop();
    return 0;
}
