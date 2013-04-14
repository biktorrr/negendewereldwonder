
import serial
import time
import subprocess

ser = serial.Serial("COM3",9600)

def dostuff():
    print "python is now copying files and doing cool stuff, this will take some secs"
    command = "ffmpeg -f image2 -r 12 -i fotos/image%03d.jpg -vcodec mpeg4 -y movie.mp4"
    p = subprocess.Popen(command, shell=True,stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    output = p.communicate()[0]
    print "python is done"

while 1:
    x = ser.read()
    
    if (x=="1"):
        print "arduino ready"
        
    elif (x=="2"):
        print "arduino button pushed"
        
    elif (x=="3"):
        print "arduino turning and snapping"
       
    elif (x=="4"):
        print "arduino turning back"
        dostuff()

    elif (x=="5"):
        print "arduino cycle ready"
        
    
