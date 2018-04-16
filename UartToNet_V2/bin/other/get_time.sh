Server_IP=$(grep -i "^FS_IP" /JKJN/sys.ini | cut -d = -f 2 )
echo "FS_IP:$Server_IP" >>/JKJN/log/RunLogntp.log
ntpdate $Server_IP >>/JKJN/log/RunLogntp.log
echo 3 > /proc/sys/vm/drop_caches
hwclock -w -u
