#include"move.h"
int main(){
    init();

    while(1){
        delay(5);
        trace();
        if(getDis()<20){
            stop();
            break;
        }
    }
    return 0;
}
