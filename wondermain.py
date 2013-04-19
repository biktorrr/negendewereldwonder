import serial
import time
import subprocess
import os
import glob
import string
import datetime
import errno
import gphoto

ser = serial.Serial("/dev/ttyACM0",9600)

# external programs
ffmpeg = "ffmpeg"
play = "omxplayer"

def fetchCameraImages(dest_path):
	# copy images from camera using gphoto2
	# empties the destpath!!!
    dest_path = os.path.normpath(dest_path)
    ensureDirectory(dest_path)
    emptyDirectory(dest_path)
	
    if gphoto.detectcamera() == False:
        return "Camera not found"
	
    gphoto.resetusb()
	# uses cwd to set working directory to destpath
    p = subprocess.Popen(['gphoto2', '--get-all-files'],cwd=dest_path,stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    print p.communicate()[0]
    gphoto.resetusb()
    p = subprocess.Popen(['gphoto2', '--recurse', '--delete-all-files'],stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    print p.communicate()[0]
    gphoto.resetusb()
	
def renameImages(path):
    # *.JPG files (copied from camera) are renamed to incrementing imgNNNN.jpg
    path = os.path.normpath(path)
    files = glob.glob(path+"/*.JPG") 
    files.sort()
    count = 0
    for f in files:
        count = count + 1
        n = string.zfill(count,4) + ".jpg"
        print f, n, 
        try:
            os.rename(f, path+'/img' + n)
            print
        except:
            print "error: didn't rename"
    
def createVideo(src_path, dest_file):
	# make sure we have proper paths and the destination file does not exists yets
    src_path = os.path.normpath(src_path)
    if os.path.isfile(dest_file):
        return dest_file+" exists already"
    dest_dir = os.path.dirname(dest_file)
    if(dest_dir):
        ensureDirectory(dest_dir)
    command = ffmpeg +" -f image2 -r 6 -i " + src_path + "/img%4d.jpg -vcodec mpeg4 -b 2000k " + dest_file
    p = subprocess.Popen(command,shell=True,stdout=subprocess.PIPE,stderr=subprocess.STDOUT)
    print p.communicate()[0]

def playVideo(file):
    if not os.path.isfile(file):
        return file+" does not exist"
    subprocess.call([play, file])

def generateDate():
	# the filename of the video is the current time
    ts = datetime.datetime.now().strftime("%Y-%m-%dT%H-%M-%S")
    return ts

def ensureDirectory(path):
    try:
        os.makedirs(path)
    except OSError as exception:
        if exception.errno != errno.EEXIST:
            raise

def emptyDirectory(path):
    files = glob.glob(path+'/*')
    for f in files:
        os.remove(f)


# the whole chain
def doIt():
    imgDir = 'images'
    videoDir = 'videos'
    videoFile = videoDir+'/video'+generateDate()+'.avi'
    print 'copy photos from camera to '+imgDir
    fetchCameraImages(imgDir)
    print 'rename files in '+imgDir
    renameImages(imgDir)
    print 'create video '+videoFile+' from '+imgDir
    createVideo(imgDir, videoFile)
    playVideo(videoFile)

#doIt()

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
        doIt()

    #elif (x=="5"):
    #    print "arduino cycle ready"
     
        
       
