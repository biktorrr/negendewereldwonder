#!/usr/bin/env python

import os
from glob import glob
from shutil import copy
from os.path import join,basename
import logging

# settings

videoPath = "/home/synergique/Dropbox/video/"
videoCompilationPath = "/home/synergique/videoCompilation"
remotevideoCompilationPath = "marloes:/home/synergique/videoCompilation"
length = 2

# NO EDITING BELOW THIS LINE

def main(videoPath, videoCompilationPath, remotevideoCompilationPath, length):
	logging.basicConfig(
	    filename='/home/synergique/createvideocompilation.log', 
	    filemode='w', 
	    format='%(asctime)s %(message)s', 
	    level=logging.DEBUG
	)
	
	logging.info('updating video compilation path %s' % (videoCompilationPath))

	if not os.path.exists(videoCompilationPath):
		os.makedirs(videoCompilationPath)

	availableVideos = filter(os.path.isfile, glob(videoPath + "/*"))
	availableVideos.sort(key=lambda x: -os.path.getmtime(x))
	availableVideos = map(basename, availableVideos[:length])

	compilationVideos = filter(os.path.isfile, glob(videoCompilationPath + "/*"))
	compilationVideos = map(basename, compilationVideos)

	toRemove = set(compilationVideos) - set(availableVideos)
	toAdd = set(availableVideos) - set(compilationVideos)

	changed = False

	for video in toRemove:
		logging.info('removing %s' % (video))
		os.remove(join(videoCompilationPath, video))
		changed = True

	for video in toAdd:
		logging.info('adding %s' % (video))
		copy(join(videoPath, video), join(videoCompilationPath, video))
		changed = True

	if True:
		command = "rsync -auvz --delete %s/ %s/" % (videoCompilationPath, remotevideoCompilationPath)
		logging.info(command)
		os.popen(command).read()
	

if __name__ == "__main__":
	main(videoPath, videoCompilationPath, remotevideoCompilationPath, length)