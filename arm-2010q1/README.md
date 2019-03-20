thư mục này chứa compile cho uboot và uclinux. 

Dưới đây là lệnh để export đường dẫn compile vào môi trường build

cd compile_uclinux/arm-2010q1/bin
export PATH=$PATH:pwd
export CROSS_COMPILE=arm-uclinuxeabi-
export ARCH=arm
