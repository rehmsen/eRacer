eRacer

INSTALLATION


This program has been tested on Windows 7 and Windows XP with an 
nVidia Geforce 8-series graphics card or higher. It is known to not
work with ATi graphics cards.


PREREQUISITES

The following must be installed:
  
  Visual Studio 2008
  Microsoft DirectX SDK (August 2009)
  NVIDIA PhysX SDK v2.8.1
  Python 2.6
  SWIG-1.3.40

The following environment variables
must be set (your paths may vary):
  
  DXSDK_DIR = F:\Program Files (x86)\Microsoft DirectX SDK (August 2009)
  PHYSX_DIR = F:\Program Files (x86)\NVIDIA Corporation\NVIDIA PhysX SDK\v2.8.1
  PYTHON_DIR= F:\Python26\
  SWIG_DIR  = P:\swigwin

  

BUILDING

Build the project eRacer/eRacer.sln. 


RUNNING

To run the program, run eRacer/run-release.py or eRacer/run-debug.py with python.


CONTROLS

In Menu:
  
  ESC, B-button                 Go back
  UP-DOWN, left control stick   Move
  ENTER, A-button               Select Item


In Game:
  
  ESC                           Pause

  W, right trigger:             Accelerate
  S, left trigger:              Reverse
  Left Shift, B-button:		      Brake
  A-D, left control stick:      Steering

  C, Y-button                   Cycle cameras (3rd person, in car, and free flying)
  ARROWS                        Move flying camera

  R, A-button                   Reload Config/eRacer.cnf file
  TAB, X-button                 Toggle debug variables
  SPACEBAR                      RAWR!


BINARIES

Binary distributions are included for Windows XP and Windows 7.
You may or may not still need the MSVCRT10 Runtime and/or DirectX Runtime installed.

