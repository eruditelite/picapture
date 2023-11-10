# picapture #

## Overview ##

I just have a ZWO ASI120MC-S to play with so far.  For this camera,
I'm planning on using the ZWO SDK on a raspberry pi.  That being the
case, install the SDK!  For my build system, put the
ASI_linux_mac_SDK* directory extracted from the downloaded SDK (see
https://www.zwoastro.com/downloads/developers) in /opt.  For example,

> sudo cp -r ASI_linux_mac_SDK_V1.31 /opt

Add the appropriate library directory to /etc/ld.so.conf and 'sudo
ldconfg'.  On a laptop etc., use lib/x64.  On the pi, lib/armv8.  See
README.txt in lib to allow camera access as a regular user.

## C ##

Originally the plan was to write this in C.  BUT, there are just too
many complications!  An example is in the C directory that can do the
following.

  * List the available cameras.
  * Display a camera's details.
  * Take a snapshot as a .ppm.

## Python ##

As a starting point, I got
[ZWOIPCam](https://github.com/grapeot/ZWOIPCam.git).  This handles
"focusing" just like I intended!  Running it on the pi streams video
to a web server.

The example above relies on
[python-zwoasi](https://github.com/python-zwoasi/python-zwoasi) to
access the camera.

I plan on creating a simpler version of the first example that allows
focusing (the video stream and capturing with options.

## Notes ##

  * Auto exposure isn´t available for snapshots, only for video.
