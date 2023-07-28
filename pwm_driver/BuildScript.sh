#!/bin/bash

case $1 in 
    insert)
    make clean
    make
    sudo insmod pwm_driver.ko
    sudo chmod 777 /dev/pwm_device
    ;;
    rm)
    sudo rmmod pwm_driver
    make clean
    ;;
esac