# picapture #

## Overview ##

I have two ZWO cameras, the ASI120MC-S and the ASI294MC.  For these
cameras, I am planning on using the ZWO SDK on a raspberry pi.  That
being the case, install the SDK!  For my build system, put the
ASI_linux_mac_SDK* directory extracted from the downloaded SDK (see
https://www.zwoastro.com/downloads/developers) in /opt.  For example,

> sudo cp -r ASI_linux_mac_SDK_V1.31 /opt

Add the appropriate library directory to /etc/ld.so.conf and 'sudo
ldconfg'.  On a laptop etc., use lib/x64.  On the pi, lib/armv8.  See
README.txt in lib to allow camera access as a regular user.

For Linux, use lib/x64/libASICamera2.so, and for Raspberry Pi, use
lib/armv8/libASICamera2.so.  Just set ZWO_ASI_LIB to the library.

## C ##

Originally the plan was to write this in C.  BUT, there are just too
many complications!  An example is in the C directory that can do the
following.

  * List the available cameras.
  * Display a camera's details.
  * Take a snapshot as a .ppm.

The complications have to do with creating and saving images, as well
as histograms etc.

## Python ##

### The Starting Point ###

As a starting point, I got
[ZWOIPCam](https://github.com/grapeot/ZWOIPCam.git).  This handles
"focusing" just like I intended!  Running it on the pi streams video
to a web server.

The example above relies on
[python-zwoasi](https://github.com/python-zwoasi/python-zwoasi) to
access the camera.

### picapture.py ###

#### Overview ####

First, get a list of cameras using the following command.  Note that
the environment variable, ZWO_ASI_LIB, must be set the the
libASICasmera2.so library for the current architecture!

		picapture.py -c list

The index of the camera will be in the first column, and after that
(separated by a colon) is the name of the camera.  Use it as the
agrument to -i (or --index) in all the following examples.

Next, to get a list of the available controls, use the following.

		picapture.py -i <camera> -c info

This will give the current value (saved in <camera name>.settings), a
description, the range, etc.  Properties (or whatever you want to call
unchangeable camera properties) are at the top.  Controls (things that
can be changed) are listed as follows.

		[Exposure/1/200]
		    DefaultValue: 14000
		    Description: 'Exposure Time(us)'
		    IsAutoSupported: True
		    IsWritable: True
		    MaxValue: 2000000000
		    MinValue: 64

Settings can be changed using the following.  Note that a window will
open with a capture using the settings, and the new setting will be
saved in the <camera name>.settings file.

		picapture.py -i <camera> -c adjust <list of 'name value' pairs>

For example, to change the exposure, do the following.

		picapture.py -i <camera> -c adjust Exposure 300

This sill, in addition to opening a window with a capture, print the
red, green, and blue histograms on the console.

## Examples/Tests ##

### ASI120MC-S ###

	rm -rf /tmp/ZWO_ASI120MC-S ; mkdir /tmp/ZWO_ASI120MC-S ; make clean all
	./picapture.py -i 0 -c adjust Exposure 2000 Gain 0
	./picapture.py -i 0 -c focus
	./picapture.py -i 0 -c capture 100 1.5 /tmp/ZWO_ASI120MC-S/test.png
	rm -f /tmp/ZWO_ASI120MC-S.avi
	cat /tmp/ZWO_ASI120MC-S/test*png | \
		ffmpeg -f image2pipe -r 1.5 -i - -c:v libx264 -pix_fmt yuv420p \
		/tmp/ZWO_ASI120MC-S.avi
	vlc /tmp/ZWO_ASI120MC-S.avi

The capture should take about 

/tmp/ZWO_ASI120MC-S will be about 75M and /tmp/ZWO_ASI120MC-S.avi about 2.5M.

### ASI294MC ###

	rm -rf /tmp/ZWO_ASI294MC ; mkdir /tmp/ZWO_ASI294MC ; make clean all
	./picapture.py -i 0 -s 0.25 -c adjust Exposure 800 Gain 0
	./picapture.py -i 0 -c capture 100 15 /tmp/ZWO_ASI294MC/test.png
	rm -f /tmp/ZWO_ASI294MC.avi
	cat /tmp/ZWO_ASI294MC/test*png | \
		ffmpeg -f image2pipe -r 0.07 -i - -c:v libx264 -pix_fmt yuv420p \
		/tmp/ZWO_ASI294MC.avi
	vlc /tmp/ZWO_ASI294MC.avi

The capture should take about 25 minutes.

/tmp/ZWO_ASI294MC should be about 684M.  Not sure about the -r option yet!

## Notes ##

  * Auto exposure isn´t available for snapshots, only for video.
  * To convert images (png in this case) to video, use something like

			cat <image files> | fmpeg -f image2pipe \
			    -r <same as capture period> i - -c:v libx264 \
				-pix_fmt yuv420p <output name/type (avi)>
