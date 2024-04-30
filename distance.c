#include<wiringPi.h>
#include<stdio.h>
#include <sys/time.h> 
#define Trig_pin 4
#define Echo_pin 5
void systemInit(){
    wiringPiSetup();
    // 初始化发射引脚
    pinMode (Trig_pin, OUTPUT);
    // 初始化接收引脚
    pinMode (Echo_pin, INPUT);
}
// 测量 距离
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

int main (void)
{
    double dis; 
    systemInit();
	// 每秒 测试一次距离并输出
    while(1){  
        dis = getDis();  
        printf("distance = %0.2lf cm\n",dis);  
        delay(1000);  
    }  
	return 0;
}