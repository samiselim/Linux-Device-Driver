case $1 in 
     insert)
        make clean
        make
        sudo insmod GPIO.ko
        sudo chmod 777 /dev/SamiGPIO_Dev
        ;;
     remove)
        sudo rmmod GPIO
        make clean 
        ;;
     test)
        sudo rmmod GPIO
        make clean
        make
        sudo insmod GPIO.ko
        sudo cat /dev/SamiGPIO_Dev
        sudo rmmod GPIO
        sudo dmesg

        ;; 

esac
