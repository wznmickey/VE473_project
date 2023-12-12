#! /bin/sh
a=$(sudo udevadm info /dev/video0 | grep P: | awk -F '/' '{ print $12 }')
b=$(sudo udevadm info /dev/video2 | grep P: | awk -F '/' '{ print $12 }')

#1-1.3.2 or 1-1.3.4

if [[ "$a" = "1-1.3.2" ]]; then
    ./main 2 0 # video0-> camera2 , video2 -> camera1
else 
    ./main 0 2 # video0-> camera1 , video2 -> camera2 
fi


