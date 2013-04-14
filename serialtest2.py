import serial
import time
import subprocess

ser = serial.Serial("/dev/tty.usbmodemfa141",9600)

def dostuff():
    print "python is now copying files and doing cool stuff, this will take some secs"
    command = "/Applications/ffmpegX.app/Contents/Resources/ffmpeg -f image2 -r 12 -i fotos/IMG_%05d.JPG -vcodec mpeg4 -y movie.mp4"

 #   command = "/Applications/ffmpegX.app/Contents/Resources/ffmpeg -f image2 -r 12 -i fotos/image%03d.jpg -vcodec mpeg4 -y movie.mp4"
    p = subprocess.Popen(command, shell=True,stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    output = p.communicate()[0]
    print "python is done making the movie"
    playMovie()

def playMovie():
    print "python is now playing the movie"
    command = "/Applications/VLC.app/Contents/MacOS/VLC movie.mp4"
    p = subprocess.Popen(command, shell=True,stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
 #  output = p.communicate()[0]
    print "python is done playing the movie"   

while 1:

    try:
        x = ser.read()

    except serial.serialutil.SerialException:
        pass
    
    if (x=="1"):
        print "arduino ready"
        
    elif (x=="2"):
        print "arduino button pushed"
        
    elif (x=="3"):
        print "arduino turning and snapping"
       
    elif (x=="5"): #was 4
        print "arduino turning back"
        dostuff()

    #elif (x=="5"):
    #    print "arduino cycle ready"
     
        
        
