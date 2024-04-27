echo "Welcome to sgx's homework"
echo "开始安装摄像头环境"
sudo apt install libcamera-tools libcamera-v4l2 motion -y 
sudo rm /etc/motion/motion.conf
sudo mv motion.conf /etc/motion/motion.conf
sudo libcamerify motion 
echo "可在 <本地IP地址>:8080 中查看"
echo "---------------------------------------"
echo "开始配置 wiringPi 环境"
echo "是否配置？Y or N"
if [$0 == N]
then 
    echo "任务结束"
else
wget https://github.com/WiringPi/WiringPi/releases/download/3.2/wiringpi_3.2_arm64.deb
sudo dpkg -i wiringpi_3.2_arm64.deb
rm wiringpi_3.2_arm64.deb
fi