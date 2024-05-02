echo "Welcome to sgx's homework"
echo "换源"

echo "开始安装摄像头环境"
sudo apt install libcamera-tools libcamera-v4l2 motion -y 
sudo rm /etc/motion/motion.conf
sudo cp motion.conf /etc/motion/motion.conf
sudo libcamerify motion 
echo "之后可通过指令 sudo libcamerify motion 启动"
echo "可在 <本地IP地址>:8080 中查看"
echo "---------------------------------------"
echo "开始配置 wiringPi 环境"
echo "是否配置？Y or N"
read tmp
if [$tmp == 'N']
then 
    echo "任务结束"
else
sudo dpkg -i wiringpi_3.2_arm64.deb
rm wiringpi_3.2_arm64.deb
fi
