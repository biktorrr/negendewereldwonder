#!/usr/bin/python

import subprocess

global_usb_port = None
usbresetpath = '/home/pi/usbreset'

def execute(command):
	
	if detectcamera() == False:
		return "Camera not found"
	
	resetusb()	
	gphotocommand = ['sudo', 'gphoto2'] + command
	p = subprocess.Popen(gphotocommand)
	gphoto_response  = p.communicate()[0]
	#gphoto_response = subprocess.check_output(gphotocommand)	
	resetusb()
	
	return gphoto_response
	
def get_list(gphotoCommand):
	gphotoresponse = execute(gphotoCommand).decode('utf-8')
	if gphotoresponse == "Camera not found":
		return {'error': gphotoresponse}
	else:
		return gphotoresponse
		
def resetusb():	
	if global_usb_port != None:
		#subprocess.Popen(['sudo', usbresetpath, '/dev/bus/usb/' + global_usb_port])
		return True
	else:
		return False
	
def detectcamera():
	p = subprocess.Popen(['sudo', 'gphoto2', '--auto-detect'], stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
	gphoto_detect  = p.communicate()[0]
	#gphoto_detect = subprocess.check_output(['sudo', 'gphoto2', '--auto-detect'])
	
	if gphoto_detect == None:
		return False
		
	usb_device = gphoto_detect.split(":")
	if len(usb_device) < 2:
		return False
	else:
		usb_device = usb_device[1].strip().replace(",","/")
		global global_usb_port
		global_usb_port = usb_device
		return True		
	
	
