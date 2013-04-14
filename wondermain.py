import serial
import time
import subprocess
import os
import glob
import string
import datetime

ser = serial.Serial("/dev/tty.usbmodemfa141",9600)


def photoRename(ts):
    
    os.makedirs(ts)
    # make new folder

    #get all the jpg file names in the current folder
    files = glob.glob("fotos/*.JPG") 
    #sort the list
    files.sort()
    count = 0
    # and rename each file
    for f in files:
        count = count + 1
        n = string.zfill(count,4) + ".jpg"
        print f, n, 
        try:
            os.rename(f, ts + '/img' + n)
            print
        except:
            print "error: didn't rename"


def makeAndPlayVideo():

    videofolder = "videos"
    ts = datetime.datetime.now().strftime("%Y-%m-%dT%H%M%S")
    # get timestamp
    videofile = videofolder + "/" + ts + ".mp4"

    photoRename(ts)
    # rename and move images from fotos to timestamped folder
    
    print "python is now copying files and doing cool stuff, this will take some secs"
    command = "ffmpeg -f image2 -r 12 -i " + ts + "/img%04d.jpg -vcodec mpeg4 -y " + videofile
 #   command = "/Applications/ffmpegX.app/Contents/Resources/ffmpeg -f image2 -r 12 -i " + ts + "/img%04d.jpg -vcodec mpeg4 -y " + videofile

    p = subprocess.Popen(command, shell=True,stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    output = p.communicate()[0]
    print "python is done making the movie"
    playMovie(videofile)



def playMovie(videofile):
    print "python is now playing the movie"
    command = "vlc " + videofile
#   command = "/Applications/VLC.app/Contents/MacOS/VLC "+videofile

    p = subprocess.Popen(command, shell=True,stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
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
        makeAndPlayVideo()

    #elif (x=="5"):
    #    print "arduino cycle ready"
     
        
        

