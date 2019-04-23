sleep 30

export QT_ROOT=/opt/QtARM_5.10.0_openGL
#export QT_QPA_PLATFORM=linuxfb:fb="/dev/fb0"
export QT_QPA_PLATFORM=eglfs
export QT_QPA_PLATFORM_PLUGIN_PATH=$QT_ROOT/plugins/platforms
export QT_QPA_FONTDIR=$QT_ROOT/fonts
export QT_QPA_GENERIC_PLUGINS=tslib

export TSLIB_ROOT=/opt/tslib1.4
export TSLIB_TSDEVICE=/dev/input/touchscreen0
export TSLIB_TSEVENTTYPE=input
export TSLIB_FBDEVICE=/dev/fb0
export TSLIB_CONSOLEDEVICE=none
export TSLIB_PLUGINDIR=/opt/tslib1.4/lib/ts
export TSLIB_CONFFILE=/opt/tslib1.4/etc/ts.conf

export QWS_MOUSE_PROTO=tslib:/dev/input/event3
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$QT_ROOT/lib:$TSLIB_ROOT/lib

export QT_QPA_FB_TSLIB=1
export TSLIB_CALIBFILE=/etc/pointercal


cd /home/megarobo/appserver
chmod a+x ./megaappserver 
./megaappserver > ./log 2>&1 &

