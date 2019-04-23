#!/bin/bash
filename="GreatShot"
set -x

#LED_GREEN2->GPIO4
#LED_RED1->GPIO3
#LED_GREEN1->GPIO2
led_light()
{
	#create directory gpio4
	echo $1 > /sys/class/gpio/export 
	#setting gpio4 direct
	echo out > /sys/class/gpio/gpio$1/direction
	#set the value
	echo $2 > /sys/class/gpio/gpio$1/value
}

net_check()
{
	eth0_enable=$(cat /sys/class/net/eth0/carrier)
	
	if [ "$eth0_enable"x = "1"x ];then
		ifconfig wlan0 down
		ifconfig eth0 down					
		ifconfig eth0  up
	fi
}

start_program()
{
	cd /home/megarobo
	if [ -f "./MRH-T/update.src/$filename" ];then
	echo "$filename is found.\n" > /dev/ttyAMA0
	cp -rf ./MRH-T/$filename ./MRH-T/update.back &&
	cp -rf ./MRH-T/update.src/$filename ./MRH-T &&
	chmod 755 ./MRH-T/$filename &&
	rm -rf ./MRH-T/update.src/$filename &&
	sync &&
	sync &&
	sync
	else
	echo "$filename is not found." > /dev/ttyAMA0
	fi	

	if [ -x "./MRH-T/$filename" ];then
	./MRH-T/$filename > /dev/ttyAMA0 2>&1 &
	else
		echo "Do not exist $filename \n" >/dev/ttyAMA0 
	fi
}

start_update()
{
    PATCH=/home/megarobo/update/sysupdate.sh
    if [ -e $PATCH ];then
	chmod a+x $PATCH
	/bin/bash -x $PATCH
    else
	echo "Do not exist $filename \n" > /dev/ttyAMA0 
    fi
}

start_qt()
{
    QTAPP=/home/megarobo/MCT/startQt.sh
    if [ -e $QTAPP ];then
	chmod a+x $QTAPP
	/bin/bash -x $QTAPP &
    else
	echo "Do not exist $filename \n" > /dev/ttyAMA0
    fi
}

start_appserver()
{
    APPSERVER=/home/megarobo/appserver/startappserver.sh
    if [ -e $APPSERVER ];then
	chmod a+x $APPSERVER
	/bin/bash -x $APPSERVER &
    else
	echo "Do not exist $filename \n" > /dev/ttyAMA0
    fi
}
#################################################

#net_check

start_update

start_program

start_qt

start_appserver

#led_light 16 0

