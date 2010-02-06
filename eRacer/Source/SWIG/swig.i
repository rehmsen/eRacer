#pragma SWIG nowarn=454,503
%module(directors="1") eRacer
%{

/* Includes the header in the wrapper code */

// Core
#include <d3dx9math.h>
#include "..\Core\Math.h"
#include "..\Core\Event.h"
#include "..\Core\Time.h"
#include "..\Core\Consts.h"


//Game
#include "..\Game\Module.h"
#include "..\Game\Game.h"
#include "..\Game\State.h"


// IO
#include "..\IO\IO.h"


// Graphics
#include "..\Graphics\Camera.h"
#include "..\Graphics\Scene.h"
#include "..\Graphics\Geometry.h"
#include "..\Graphics\StaticGeometry.h"
#include "..\Graphics\MovingGeometry.h"
#include "..\Graphics\GraphicsLayer.h"
#include "..\Graphics\Window.h"


// Sound
#include "..\Sound\SoundLayer.h"


// Physics
#include "..\Physics\PhysicsLayer.h"
#include "..\Physics\PhysicsObject.h"
#include "..\Physics\Box.h"
#include "..\Physics\Plane.h"
#include "..\Physics\TriMesh.h"

// Input
#include "..\Input\Device.h"
#include "..\Input\Mouse.h"
#include "..\Input\Keyboard.h"
#include "..\Input\Gamepad.h"
#include "..\Input\Input.h"



// Test
#include "..\Test\Test.h"

%}

/* 
Parse the header file to generate wrappers 
These are the classes that get exposed to Python
*/


// SWIG
%include "std_string.i"


// Core
%include "..\Core\d3dx.h"
%include "..\Core\Math.h"
%feature("director") Event;
%include "..\Core\Event.h"
%include "..\Core\Time.h"
%include "..\Core\Consts.h"



// Game
%feature("director") Module;
%include "..\Game\Module.h"
%feature("director") Game;
%include "..\Game\Game.h"
%include "..\Game\State.h"


// IO
%feature("director") IO;
%include "..\IO\IO.h"


// Graphics
%include "..\Graphics\Camera.h"
%include "..\Graphics\AxisAlignedBoundingBox.h"
%include "..\Graphics\Spatial.h"
%include "..\Graphics\Geometry.h"
%include "..\Graphics\StaticGeometry.h"
%include "..\Graphics\MovingGeometry.h"
%include "..\Graphics\Scene.h"
%include "..\Graphics\GraphicsLayer.h"
%include "..\Graphics\Window.h"


// Sound
%include "..\Sound\SoundLayer.h"


// Physics
%include "..\Physics\PhysicsLayer.h"
%include "..\Physics\PhysicsObject.h"
%include "..\Physics\Box.h"
%include "..\Physics\Plane.h"
%include "..\Physics\TriMesh.h"


// Input
%include "..\Input\Device.h"
%include "..\Input\Mouse.h"
%include "..\Input\Keyboard.h"
%include "..\Input\Gamepad.h"
%include "..\Input\Input.h"



// Test
%include "..\Test\Test.h"
