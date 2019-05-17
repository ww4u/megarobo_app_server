rm -r ./appserver
mkdir ./appserver
mkdir ./appserver/appserver

cp ./readme.txt ./appserver/readme.txt
cp ./ig.pdf	./appserver/installguide.pdf
cp ./build-megaappserver-raspberry-Release/megaappserver ./appserver/appserver/megaappserver
cp ./startappserver.sh ./appserver/appserver/startappserver.sh
cp ./sysstart.sh ./appserver/sysstart.sh
cp ./install.bat ./appserver/install.bat
