ofxPhoto (libgphoto2)
=====================

This addon uses the gphoto2 library to take pictures remotely
from your camera and use it directly in openFrameworks.

See http://www.openframeworks.cc/ and http://www.gphoto.org/ for
more Information.

Is my camera supported
----------------------

First check here if your camera is on this list: http://gphoto.org/doc/remote/
After this install gphoto2 for example (on ubuntu) with

sudo apt-get install gphoto2

and test if your camera works with this command

gphoto2 --capture-image-and-download

If this works for you, the addon should work too.

Install Instructions
--------------------

Download the addon from here: http://github.com/benben/ofxPhoto/downloads
Or fork it from here: http://github.com/benben/ofxPhoto
Copy the addon in the addons folder and the example in your apps folder.

**Mac notes (thanks to jesus gollonet)**

The included libgphoto2 library doesn't work on mac. Don't add it to your project.
You'll have to compile the library yourself and link from your project.

Here's how I've made it work using macports:

- to compile libgphoto2 in 32 bit, you'll have to tell macports to compile for i386
- open the file macports.conf (it's in /opt/local/etc/macports/ in my system)
- uncomment the line that says

    build_arch			i386
		
- then install libgphoto2
	
	sudo port install libgphoto2
	
- if everything went well, edit the file Project.xconfig in your Xcode project to be like this:

    OTHER_LDFLAGS = $(OF_CORE_LIBS) /opt/local/lib/libgphoto2.dylib
    
    HEADER_SEARCH_PATHS = $(OF_CORE_HEADERS) /opt/local/include/gphoto2
	
- and you should be fine

Basic Usage
-----------

Plug in your camera and turn it on.
A splash screen should appear where you have to unmount the camera from the system.
Launch the photoExample, press Spacebar and watch the Console.
After a few seconds a picture should appear on the screen.
If the addon doesn't work, but the gphoto2 capture command above, please report it.

Other
-----

Please test it, if you can!
Any Comments, Criticism, Suggestions are very welcome here: 

http://www.openframeworks.cc/forum/viewtopic.php?f=10&t=3474

If you have any question, feel free to ask! 

License
-------

libgphoto2 is released under the LGPL. See for more here: http://www.gphoto.org/proj/libgphoto2/

(The MIT License)

Copyright © 2011 Benjamin Knofe

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the ‘Software’), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED ‘AS IS’, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.