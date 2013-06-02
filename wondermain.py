#!/usr/bin/env python

from os.path import expanduser
import serial
from serial.serialutil import SerialException
import time
import subprocess
import os
from glob import glob
import string
import datetime
import errno
import atexit
import logging
import sys
from shutil import copy
from os.path import join,basename

# settings
imageDownloadPath = expanduser("~")+"/wonder/negendewereldwonder/images"
videoPath = expanduser("~")+"/Dropbox/video/"
imagePath = expanduser("~")+"/Dropbox/images/"

# NO EDITING BELOW THIS LINE

class WonderMain:
    # process holders
    playerProcess = None

    # paths
    imageDownloadPath = None
    videoPath = None
    imagePath = None

    # the video we're currently working on
    videoName = None
    videoFile = None

    # devices
    arduinoDevice = None
    # serial connection
    ser = None

    # serial handlers
    serialHandlers = {
        "1": "arduinoInit",
        "2": "arduinoReset",
        "3": "arduinoReady",
        "4": "arduinoButtonPushed",
        "5": "arduinoTurningAndSnapping",
        "6": "arduinoDone",
        "7": "arduinoBlinking",
        "8": "arduinoWaiting",
        "9": "arduinoEncoderDone",
    }

    def __init__(self, imageDownloadPath, videoPath, imagePath):
        self.imageDownloadPath = imageDownloadPath
        self.ensureDirectory(self.imageDownloadPath)

        self.videoPath = videoPath
        self.ensureDirectory(self.videoPath)

        self.imagePath = imagePath
        self.ensureDirectory(self.imagePath)

    def openSerial(self):
        for i in [0, 1, 2]:
            self.arduinoDevice = "/dev/ttyACM%s" % (i)
            if not os.path.exists(self.arduinoDevice):
                continue

            try:
                self.ser = serial.Serial(self.arduinoDevice, 9600)
                return
            except:
                pass

    def fetchCameraImages(self):
        # uses cwd to set working directory to destpath
        logging.info('starting image download')
        command = 'exec gphoto2 --get-all-files'
        logging.info(command)
        
        return subprocess.Popen(command, cwd=self.imageDownloadPath, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)

    def removeCameraImages(self):
        logging.info('starting remover')
        command = 'exec gphoto2 --recurse --delete-all-files'
        logging.info(command)
        
        return subprocess.Popen(command, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)

    def renameImages(self):
        # *.JPG files (copied from camera) are renamed to incrementing imgNNNN.jpg
        files = glob(self.imageDownloadPath+"/*.JPG")
        files.sort()
        count = 0
        for source in files:
            count = count + 1
            dest = "%s/img%s.jpg" % (self.imageDownloadPath, string.zfill(count,4))

            try:
                os.rename(source, dest)
            except:
                logging.info("error: didn't rename")
    
    def backupImages(self):
        # create directory to store this video's images
        destPath = self.imagePath+'/'+self.videoName
        self.ensureDirectory(destPath)

        for f in glob(self.imageDownloadPath+"/*.jpg"):
            copy(f, join(destPath, basename(f)))

    def createVideo(self):
        self.videoFile = "%s/%s.avi" % (self.videoPath, self.videoName)
        # command = 'avconv -r 18 -i "%s/img%%4d.jpg" -vcodec mpeg4 -b 7000k "%s"' % (self.imageDownloadPath, self.videoFile)
        command = 'avconv -loop 1 -r 18 -i "%s/img%%4d.jpg" -vcodec mpeg4 -b 7000k -frames 400 "%s"' % (self.imageDownloadPath, self.videoFile)

        logging.info(command)
        
        return subprocess.Popen(command, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)

    def playVideo(self):
        if self.playerProcess is not None:
            logging.info('killing player')
            playerProcess.kill()
        
        command = 'exec cvlc -f -L -q --no-osd --no-video-title-show "%s"' % (self.videoFile)
        logging.info(command)
        
        playerProcess = subprocess.Popen(command, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)

    def generateDate(self):
        # the filename of the video is the current time
        return datetime.datetime.now().strftime("%d-%m-%Y om %H:%M")

    def ensureDirectory(self, path):
        try:
            os.makedirs(path)
        except OSError as exception:
            if exception.errno != errno.EEXIST:
                raise

    def emptyDirectory(self, path):
        files = glob(path+'/*')
        for f in files:
            os.remove(f)

    def arduinoInit(self):
        logging.info('arduino init')

    def arduinoReset(self):
        logging.info('arduino reset')

    def arduinoReady(self):
        logging.info('arduino ready')

    def arduinoButtonPushed(self):
        logging.info('arduino button pushed')

    def arduinoTurningAndSnapping(self):
        logging.info('arduino turning and snapping')

    def arduinoDone(self):
        self.videoName = "9e wereldwonder gemaakt op %s" % (self.generateDate())

        logging.info('fetching images from camera')
        (out, err) = self.fetchCameraImages().communicate()
        logging.info(out)
        logging.info(err)

        logging.info('removing images from camera')
        removeProcess = self.removeCameraImages()

        logging.info('renaming images')
        self.renameImages()

        logging.info('backup (export) images')
        self.backupImages()

        logging.info('creating video')
        (out, err) = self.createVideo().communicate()
        logging.info(out)
        logging.info(err)

        self.ser.write('9') # encoder done

        logging.info('start video playback')
        self.playVideo()

        logging.info('removing local images')
        self.emptyDirectory(self.imageDownloadPath)

        logging.info('waiting for camera image remover to complete')
        (out, err) = removeProcess.communicate()
        logging.info(out)
        logging.info(err)

        logging.info('done')
        self.ser.write('2') # reset arduino

    def arduinoBlinking(self):
        logging.info('arduino blinking')

    def arduinoWaiting(self):
        logging.info('arduino waiting')

    def arduinoEncoderDone(self):
        logging.info('arduino encoder done')

    def initialize(self):
        logging.info('creating directories')
        self.ensureDirectory(self.imageDownloadPath)
        self.ensureDirectory(self.videoPath)
        self.ensureDirectory(self.imagePath)

        logging.info('connecting to arduino')
        self.openSerial()

        logging.info('removing local images')
        self.emptyDirectory(self.imageDownloadPath)

        logging.info('removing images from camera')
        (out, err) = self.removeCameraImages().communicate()
        logging.info(out)
        logging.info(err)

        # tell arduino we're ready
        logging.info('ready')
        self.ser.write('1') # we're done

    def cleanup(self):
        if self.ser is not None:
            self.ser.close()

    def run(self):
        logging.basicConfig(
            filename=expanduser("~")+'/wondermain.log', 
            filemode='w', 
            format='%(asctime)s %(message)s', 
            level=logging.DEBUG
        )

        atexit.register(self.cleanup)

        logging.info('initializing')
        self.initialize()

        while True:
            try:
                logging.info('waiting for arduino')

                x = self.ser.read()
            except serial.serialutil.SerialException:
                raise

            if x in self.serialHandlers:
                getattr(self, self.serialHandlers[x])()
            else:
                logging.info('Arduino command "%s" not understood' % (x))

if __name__ == "__main__":
    m = WonderMain(imageDownloadPath, videoPath, imagePath)
    m.run()
