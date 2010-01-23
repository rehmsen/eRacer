#pragma SWIG nowarn=454,503
%module(directors="1") eRacer
%{

/* Includes the header in the wrapper code */

#include <d3dx9math.h>

#include "..\Core\Event.h"
#include "..\Core\Time.h"
#include "..\Core\Module.h"

#include "..\IO\IO.h"

#include "..\Graphics\Camera.h"
#include "..\Graphics\Scene.h"
#include "..\Graphics\Geometry.h"
#include "..\Graphics\StaticGeometry.h"
#include "..\Graphics\MovingGeometry.h"
#include "..\Graphics\GraphicsLayer.h"
#include "..\Graphics\Window.h"

#include "..\Sound\SoundLayer.h"

#include "..\Physics\PhysicsLayer.h"
#include "..\Physics\PhysicsObject.h"
#include "..\Physics\Box.h"
#include "..\Physics\Plane.h"

#include "..\Input\Keyboard.h"

%}

/* Parse the header file to generate wrappers */

%include "std_string.i"


%include "..\Core\d3dx.h"
%include "..\Core\Math.h"

%feature("director") Event;
%include "..\Core\Event.h"
%include "..\Core\Time.h"
%include "..\Core\Module.h"

%feature("director") IO;
%include "..\IO\IO.h"

%include "..\Graphics\Camera.h"
%include "..\Graphics\AxisAlignedBoundingBox.h"
%include "..\Graphics\Spatial.h"
%include "..\Graphics\Geometry.h"
%include "..\Graphics\StaticGeometry.h"
%include "..\Graphics\MovingGeometry.h"
%include "..\Graphics\Scene.h"
%include "..\Graphics\StaticGeometry.h"
%include "..\Graphics\MovingGeometry.h"
%include "..\Graphics\GraphicsLayer.h"
%include "..\Graphics\Window.h"

%include "..\Sound\SoundLayer.h"

%include "..\Physics\PhysicsLayer.h"
%include "..\Physics\PhysicsObject.h"
%include "..\Physics\Box.h"
%include "..\Physics\Plane.h"

%include "..\Input\Keyboard.h"
