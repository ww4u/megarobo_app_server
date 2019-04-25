#dst_addr=192.168.1.127
#dst_addr=192.168.1.54
dst_addr=192.168.1.226

ssh root@$dst_addr mkdir /home/megarobo/appserver

scp ./build-megaappserver-raspberry-Release/megaappserver root@$dst_addr:/home/megarobo/appserver/megaappserver
scp ./startappserver.sh root@$dst_addr:/home/megarobo/appserver/startappserver.sh
scp  ./sysstart.sh root@$dst_addr:/home/megarobo/MRH-T/sysstart.sh 

# scp  root@192.168.1.54:/home/megarobo/appserver/startappserver.sh ./startappserver.sh

# scp  root@192.168.1.54:/home/megarobo/MRH-T/sysstart.sh ./sysstart.sh


