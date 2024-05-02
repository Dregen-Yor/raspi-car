void moto_run(int x,int y);
void direct(){
    moto_run(-100,100);
}
void stop(){
    // digitalWrite(GPIO22, LOW);
    // digitalWrite(GPIO23,LOW);
    // digitalWrite(GPIO24, LOW);
    // digitalWrite(GPIO25,LOW);
    moto_run(0,0);
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
     if(x>0){
        softPwmWrite(GPIO22,x); 
        softPwmWrite(GPIO23,0); 
    }
    if(x<0){
        softPwmWrite(GPIO22,0); 
        softPwmWrite(GPIO23,-x); 
    }
    if(y>0){
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
    // if(s1 == 0&& s2 == 0&& s3 == 1&&s4 == 0)  //0010
    //   err = 1;
    // if(s1 == 0&& s2 == 0&& s3 == 0&&s4 == 1)  //0001
    //   err = 2;
    // if(s1 == 0&& s2 == 1&& s3 == 0&&s4 == 0)  //0100
    //   err = -1;
    // if(s1 == 1&& s2 == 0&& s3 == 0&&s4 == 0)  //1000
    //   err = -2;
    err = (s4 - s1) * 3 + (s3 - s2);
    err_sum += err;
    err_diff = err - last_err;
    last_err = err;
}
int  cacu_pwm()            //pid运算部分
{
    int output;
    double kp=60,ki=0,kd=12;    //pid需要根据实际情况进行调节
    output = kp*err + ki*err_sum + kd*err_diff;    
    last_err = err;
    return output;
}

void trace(){
    flash();
    moto_run(-(base+cacu_pwm()),base-cacu_pwm());
}