#[MotionMachine](http://motionmachine.org)

MotionMachine is a C++ software toolkit for rapid prototyping of motion feature extraction and motion-based interaction design. It encapsulates the complexity of motion capture data processing into an intuitive and easy-to-use set of APIs, associated with the openFrameworks environment for visualisation. MotionMachine is a new framework designed for “sense-making”, i.e. enabling the exploration of motion-related data so as to develop new kinds of analysis pipelines and/or interactive applications.

# How to install ?
ofxMotionMachine is an openframeworks addon. 
1. Download version of OpenFrameworks from official website http://www.openframeworks.cc. The last version of MotionMachine works with 0.9.8 OF version, others might not work.
2. Clone MotionMachine repository on your computer, or download the zip and unzip it in folder addons of openframeworks.
3. you can generate a motionMachine project with the app:
		of_v0.9.8_vs_release\addons\ofxMotionMachine\projectGenerator-*-MoMa\projectGenerator
	This generator is a customized version of the OF project generator. It selects the appropriate addons and copy the good template for new project.

Dependencies:	
We add in this ofAddon the following modified libraries:
Armadillo C++ Linear Algebra Library version 4.450 http://arma.sourceforge.net/
ofxUi https://github.com/rezaali/ofxUI (deprecated library)

# Tutorials

Work in progress...

You can find a series of tutorials for installation and first use of MotionMachine here:



Old version of MotionMachine (the following explanations are deprecated)
----------------------------------------------------------------------


[![MotionMachine Tutorials](http://i.imgur.com/7NVZphS.png)](https://youtu.be/-00fcnDebVE?list=PLBdzvc6iEN7WqJqM1oi1nwyGaLZHu_QB3 "MotionMachine Tutorials")






Mac OS (Xcode)
--------
1. Clone MotionMachine repository on your computer, or download the zip.
2. Download OSX version version of OpenFrameworks from official website http://www.openframeworks.cc. The last version of MotionMachine works with 0.9.2 OF version, others might not work.
3. Copy libs and export folders from OF main folder to your MotionMachine folder "libs/oF". Leave the addons folder as it is.
4. Done. You can check if it works by running apps/examples/mmEmptyExample/mmEmptyExample.xcodeproj with xcode.

Windows (Visual Studio 2015)
--------
1. Clone MotionMachine repository on your computer, or download the zip.
2. Download Windows Visual Studio version version of OpenFrameworks from official website http://www.openframeworks.cc. The last version of MotionMachine works with 0.9.2 OF version, others might not work.
3. Copy libs and export folders from OF main folder to your MotionMachine folder "libs/oF". Leave the addons folder as it is.
4. Done. You can check if it works by running apps/examples/mmEmptyExample/mmEmptyExample.sln with Visual Studio 2015.

Linux (CMake)
--------
On Linux, MotionMachine (MoMa) is composed of two parts, as opposed to what is done in OSX and Windows where Xcode/VS configs take care of everything as one big project. The two parts are:

1. libmoma, a shared library based on the files in `./libs/MoMa/{core,features,parsers}`. You can use it to load, access and process mocap data in your applications independently of openFrameworks. It gets installed into /usr/local/{lib,include}.
2. ofxMoMa, an openFrameworks (oFx) addon to make it easy to use MoMa into oFx. It goes into /path/to/openframeworks/addons/

Installing MotionMachine has been tested on Ubuntu 14.04 and 16.04. Note that on Ubuntu 14.04, it requires to install some tools manually, from source (notably armadillo, CMake, GCC 5.x, etc.) because the versions available in the official repositories are outdated. The installation process is the following:

1. Clone MotionMachine repository on your computer, or download the zip.
2. Go to ./MotionMachine/libs/compiled/project/linux/ and follow the instructions in README.md

# External links

C3D-to-csv file converter developed from the MotionMachine parser: https://omid.al/projects/c3d_to_csv.html
