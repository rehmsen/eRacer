import threading
import random
import time as _time

from Core.Globals   import *
from Game.State     import State

from GameMapping    import *
from MenuState      import PauseMenuState
from GameEndState   import GameEndState

# Entities
from Track      import Track
from Vehicle    import Vehicle
from Shadow     import Shadow
from Meteor     import Meteor, MeteorManager
from Quad       import Quad
from HudQuad    import HudQuad
from PlayerInterface  import PlayerInterface
# from CoordinateCross  import CoordinateCross


# AI stuff
from AI.Behavior import PlayerBehavior, AIBehavior
from AI.Raceline import Raceline

# View stuff
from Graphics.View    import View
from Graphics.SkyBox  import SkyBox




# TODO
# need a lock so that loading of IO does not happen
# inside BegineScene()/EndScene() pairs, or else the 
# driver will freak out
class LoadingState(State):
  def __init__(self, func):
    State.__init__(self)    
    def load():
      # do loading function
      func()
      # wait for all asyncronous loads to finish
      game().io.LoadAsyncEvent(self.Loaded)
    
    self.thread = threading.Thread(target=load)
    self.thread.start()
    
  def Activate(self):
    game().simspeed = 0.
  
  def Deactivate(self):
    game().simspeed = 1.    
    
  def Loaded(self, none):
    game().PopState()
    
  def Tick(self, time):
    State.Tick(self, time)
    game().graphics.graphics.WriteString(
      "Loading...", 
      "Verdana", 32, Point3(300,220,0)
    )    
##############################################


class GameState(State):
  AI_MODEL_NUMS = [2,3,4,5,6,8,4]
  AI_NAMES = ["Arthur Dent", "Ford Prefect", "Zaphod Beeblebrox", "Marvin", "Trillian", "Slartibartfast"]
  
  def __init__(self, track='Track1', nPlayers=1, nAIs=3):
    State.__init__(self)
    self.loaded = False
    
    self.laps   = 2 # TODO CONST
    self.stats  = {}
    self.gameOver = False

    
    self.load(track,nPlayers,nAIs)
    
  def Activate(self):
    State.Activate(self)
    #if not self.loaded:
    #  game().PushState(LoadingState(self.load))
    print "Activate game state"

  def Deactivate(self):
    State.Deactivate(self)    
    # self.sound.isPaused = True
    # game().sound.sound.UpdateSoundFx(self.sound)
    
  def Pop(self):
    self.meteorManager.Release()
    del self.meteorManager
    
    for i in self.entities.values():
      self.Remove(i)
        
    self.vehicleList = []
    self.stats = {}
    del self.track

    # print '*******'
    # import gc
    # print gc.collect()
    
    # print '\n\n'.join(map(repr,gc.get_referrers(self)))
    # print '*******'
    # print '\n\n'.join(gc.garbage)
    
    # if self in gc.garbage:
    #   print 'AAAAAAAAHHHHHHH'
    
 
  def AddVehicle(self, isAI):
      n = len(self.vehicleList)    
      x = (n % 3 - 1)*15
      z = (3-(n / 3))*-15
      
      vehicle    = self.Add(Vehicle(
        isAI and random.choice(self.AI_NAMES) or "Player1",    
        self.track, 
        Matrix(Point3(x, 3, z)) * self.startOrientation, 
        (not isAI) and 1 or self.AI_MODEL_NUMS.pop()
      ))
      self.Add(Shadow(vehicle))
      self.vehicleList.append(vehicle)
      if isAI:
        AIBehavior(vehicle, self.track)
      else:
        PlayerBehavior(vehicle)
        vehicle.Backwards = False
      
      return vehicle                
    

  def load(self, track, nPlayers, nAIs):
    # testing stuff
    # game().sound.PlaySound2D("jaguar.wav")
    print "GameState::load begin"
    scene = cpp.Scene()
    self.scene = scene
    
    # TODO
    # can we render a fake loading screen here until the real one works?
    
    random.shuffle(self.AI_MODEL_NUMS)
    
    self.track = self.Add(Track(track))
    self.vehicleList = []

    frame = self.track.GetFrame(-30.0)
    self.startOrientation = Matrix(frame.position, frame.up, frame.fw)
    
    forwardMat = Matrix(ORIGIN, -PI/2.0, X)
    
    startFrame = self.track.GetFrame(0.0)
    
    # TODO: this should load "StartLine.x" but it is not appearing properly
    finishLineTransform = Matrix(30, 1, 3) * Matrix(startFrame.position+startFrame.up, startFrame.up, startFrame.fw)
    self.Add(Model('Finish Line','FinishLine.x',None,finishLineTransform))

    self.skybox = SkyBox()
    
    for i in range(nAIs):
      self.AddVehicle(True)

    self.interfaces = []
    viewports = self.SetupViewports(nPlayers)
    
    for viewport in viewports:
      player = self.AddVehicle(False)
      pi = PlayerInterface(self, player, viewport)
      pi.AddRenderable(self.scene)
      pi.AddRenderable(self.skybox)
      self.interfaces.append(pi)


    self.SetupInputMapping(nPlayers)

       
    self.meteorManager = MeteorManager(self)

    for i in range(CONSTS.NUM_METEORS):
      self.meteorManager.spawnRandom()
    
    self.lastMeteorTime = 0
    
    #self.sound = cpp.SoundFx();
    #self.sound.looping  = True
    #self.sound.is3D     = False
    #self.sound.isPaused = False
    #game().sound.sound.LoadSoundFx("Adventure.mp3", self.sound)
    
    game().time.Zero()
    self.loaded = True
  

  def SetupViewports(self, nPlayers):  
    w = game().graphics.width
    h = game().graphics.height
    
    w2 = w/2
    h2 = h/2
    if nPlayers==1:
      return [(0,0,w,h)]
    elif nPlayers==2:
      return [
        (0,   0,  w, h2),
        (0,   h2, w, h2),
      ]
    elif nPlayers==4:
      return [
        (0,   0,  w2, h2),
        (w2,  0,  w2, h2),
        (0,   h2, w2, h2),
        (w2,  h2, w2, h2),
      ]
      
  def SetupInputMapping(self, nPlayers):
    if nPlayers == 1:
      self.mapping = GameMapping([
          Keyboard1Mapping(self.interfaces[0]),
          KeyboardDebugMapping(None),
          Gamepad1Mapping(self.interfaces[0]),
          GamepadDebugMapping(None), 
                                 ])
    if nPlayers == 2:
      self.mapping = GameMapping([
          Keyboard1Mapping(self.interfaces[0]),
          Keyboard2Mapping(self.interfaces[1]),
                                 ])
  
  def Tick(self, time):
    
    # int SetOrientation3D(const Point3& listenerPos, const Vector3& listenerVel, const Vector3& atVector, const Vector3& upVector); //For 3D sound
    cam = self.interfaces[0].view.camera
    # TODO camera velocity
    game().sound.sound.SetOrientation3D(cam.GetPosition(), Point3(0,0,0), cam.GetLookAt(), cam.GetUp())
    
    _time.sleep(CONSTS.SLEEP_TIME)
    

    self.vehicleList.sort(key = lambda vehicle:vehicle.trackpos, reverse=True)
    
    for place,vehicle in enumerate(self.vehicleList):
      vehicle.place = place+1
      vehicle.lapRatio = vehicle.lapcount <= self.laps and vehicle.trackpos / self.track.dist % 1.0 or 1.0
    
    for interface in self.interfaces:
      interface.Tick(time)
      game().graphics.views.append(interface.view)
      game().graphics.views.append(interface.hud)

    
    if (not self.gameOver) and CONSTS.AIMED_METEOR_INTERVAL:
      self.lastMeteorTime += time.game_delta
      if self.lastMeteorTime > CONSTS.AIMED_METEOR_INTERVAL*time.RESOLUTION:
        self.lastMeteorTime = 0
        self.meteorManager.spawnTargeted(self.player)
    
    self.meteorManager.Tick(time)
    
    State.Tick(self, time)
      
  def LapEvent(self, vehicle, lap):
    self.stats.setdefault(vehicle, []).append(game().time.get_seconds())
    
    # if lap == self.laps+1:
    #   if vehicle == self.player:
    #     self.gameOver = True
    #     game().PushState(GameEndState(self.stats))
        
    #   vehicle.Brake(1)
    
  def ReloadConstsEvent(self):
    game().config.read()
    game().event.ReloadedConstsEvent()
    
  def PauseEvent(self):
    game().PushState(PauseMenuState())

  def PlayJaguarSoundEvent(self):
    game().sound.PlaySound2D("jaguar.wav")
    
  def KeyPressedEvent(self, key):
    if key == KEY.HOME:
      game().simspeed = 1.0
      
  def ObstacleAheadEvent(self, vehicleId, obstacleId):
    vehicle = self.entities[vehicleId]
    obstacle = self.entities[obstacleId]
    vehicle.obstacles.append(obstacle)
    
      

