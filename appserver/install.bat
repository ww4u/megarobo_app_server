set addr=%1
scp -r ./appserver root@%addr%:/home/megarobo
scp ./sysstart.sh root@%addr%:/home/megarobo/MRH-T/sysstart.sh