sudo dmesg -C
sudo rmmod driver
cd driver
make clean
make
sudo insmod driver.ko
cd ../user
make