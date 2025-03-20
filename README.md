# 树莓派智能小车

### 参考代码时不忘点个小星星再看

## 所用材料
- L289N 电机驱动模块
- GA25-370 12V 282转电机4个 [购买连接](https://item.taobao.com/item.htm?_u=k3hol22g88f2&id=693298985042&spm=a1z09.2.0.0.19812e8dsYZmqG)
- I2C协议 红外四路寻迹  [购买连接](https://e.tb.cn/h.6bcGMTHxDyDge5J?tk=b9RFerzNhHn)
- BME280 室温，气压，湿度检测模块
- CSI 摄像头 [购买连接](https://item.taobao.com/item.htm?spm=a1z09.2.0.0.19812e8dsYZmqG&id=600312132103&_u=k3hol22g8fca)
- 12V电池   [购买连接](https://item.taobao.com/item.htm?_u=k3hol22g9733&id=573013194357&spm=a1z09.2.0.0.19812e8dsYZmqG)
- 超声波模块  [购买链接（只含底座不含模块）](https://item.taobao.com/item.htm?_u=k3hol22g2598&id=576417178269&spm=a1z09.2.0.0.19812e8dsYZmqG)
- 大号车轮85mm [购买连接](https://detail.tmall.com/item.htm?id=670160152244&spm=a1z09.2.0.0.19812e8dsYZmqG&_u=k3hol22g9527)
- 亚克力底盘底板双层 [购买链接](https://item.taobao.com/item.htm?spm=a1z09.2.0.0.19812e8dsYZmqG&id=653879212101&_u=k3hol22gd878)

## 代码解释（粗略）
`main.cpp` 为程序主入口，`sgx.h` 中定义了各个引脚的地址，`move.c` 中定义了控制小车运动的所有函数。

`raspi-car-control` 目录下为网页端控制页面，包括一个摄像头画面显示，模式选择，周围温度湿度气压以及小车静止倾斜角、前方障碍物距离参数，通过监控 `W`,`A`,`S`,`D` 案件进行手动控制。

## 实现效果
- 实时画面传输
- PID算法四路寻迹
- 超声波避障
- 网页端遥控

## 技术栈
- [wiringPi](https://github.com/WiringPi/WiringPi)
- [opencv](https://github.com/opencv/opencv)
- [oat++](https://github.com/oatpp/oatpp)
- [frp](https://github.com/fatedier/frp)
- [Vue](https://github.com/vuejs/vue)
- [cmake](https://github.com/Kitware/CMake)

## 参考代码
- [BME280](https://github.com/davebm1/c-bme280-pi)