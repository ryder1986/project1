while  [ 1 ] 
do 
   sleep 1
   echo file ;
ffmpeg        -re       -i  09_30_26.264  -r 100     -s   960x540     -c:v  libx264   -f  flv  rtmp://localhost/live/960x540
done 
