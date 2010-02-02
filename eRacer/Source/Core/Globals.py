import os
import sys
import math

import eRacer

# global game object
__game = None
def _set_game(g):
  global __game
  __game = g;
  
def game():
  return __game

Vector3   = eRacer.D3DXVECTOR3
Vector4   = eRacer.D3DXVECTOR4
Point3    = eRacer.D3DXVECTOR3
Point4    = eRacer.D3DXVECTOR4

Matrix    = eRacer.CreateMatrix

ORIGIN 		= eRacer.cvar.ORIGIN
IDENTITY 	= eRacer.cvar.IDENTITY
X         = eRacer.cvar.X
Y         = eRacer.cvar.Y
Z         = eRacer.cvar.Z
RED       = eRacer.cvar.RED
BLUE      = eRacer.cvar.BLUE
GREEN     = eRacer.cvar.GREEN
WHITE     = eRacer.cvar.WHITE

length    = eRacer.abs
dot       = eRacer.dot
cross     = eRacer.cross
normalize = eRacer.normalize
mul1      = eRacer.mul1
mul0      = eRacer.mul0

CONSTS    = eRacer.Constants().g_Constants

from Game.Module  	import Module
from Game.Entity    import Entity

from Core.Event 	  import Event
from Input          import KEY
