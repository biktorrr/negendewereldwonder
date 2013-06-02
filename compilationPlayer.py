#!/usr/bin/env python

from os.path import expanduser
import time
import subprocess
import shlex
import os
from glob import glob
import logging
import sys
from shutil import copy
from os.path import join,basename

videoCompilationPath = expanduser("~")+"/videoCompilation"
repeat = 1

class CompilationPlayer:
	videoCompilationPath = None
	repeat = None
	
	def __init__(self, videoCompilationPath, repeat):
		self.videoCompilationPath = videoCompilationPath
		self.repeat = repeat

	def updatePlaylist(self):
		out = open(self.videoCompilationPath+"/playlist.m3u", "w");

		for videoFile in glob(self.videoCompilationPath+"/*.avi"):
			for i in range(self.repeat):
				out.write(videoFile+"\n")

		out.close()

	def main(self):
		logging.basicConfig(
		    filename=expanduser("~")+'/mplayercontroller.log', 
		    filemode='w', 
		    format='%(asctime)s %(message)s', 
		    level=logging.DEBUG
		)

		while True:
			self.updatePlaylist()
			command = 'cvlc -f -L -q --no-osd --no-video-title-show "%s/playlist.m3u"' % (self.videoCompilationPath)
			print command
			subprocess.call(shlex.split(command))

if __name__ == "__main__":
    CompilationPlayer(videoCompilationPath, repeat).main()

