import threading
import random
import colorsys
import time as _time

from Core.Globals   import *
from Core.Config    import Config
from Game.State     import State

from GameMapping    import *
from MenuState      import PauseMenuState
from GameEndState   import GameEndState
from GameSettings   import GameSettings
from PlayerInterface  import PlayerInterface

# Entities
from Track      import Track
from Vehicle    import Vehicle
from Shadow     import Shadow
from Meteor     import Meteor, MeteorManager
from Quad       import Quad
from HudQuad    import HudQuad
from Ring       import Ring

# AI stuff
from AI.Behavior import PlayerBehavior, AIBehavior
from AI.Raceline import Raceline

# View stuff
from Graphics.View    import View, HudView
from Graphics.SkyBox  import SkyBox

from Sound.Music import Music
    

class LoadScreenState(State):
  
  def __init__(self, settings, game=None):
    State.__init__(self)
    self.view = HudView([self.scene])
    self.game = game
    
    
    self.mappingQuads = []
    nPlayers = settings.nPlayers
    w = 800
    h = 600

    self.mappingCoords = []
    self.nameStringCoords = []
    
    if nPlayers==1:
      self.mappingCoords.append((0,0,w,h))
      self.nameStringCoords.append((20,20))
    elif nPlayers==2:
      self.mappingCoords.append((w/4,0,w/2,h/2))
      self.mappingCoords.append((w/4,h/2,w/2,h/2))
      self.nameStringCoords.append((20,20))
      self.nameStringCoords.append((20,h/2+20))
    elif nPlayers>2:
      self.mappingCoords.append((0,0,w/2,h/2))
      self.mappingCoords.append((w/2,0,w/2,h/2))
      self.mappingCoords.append((0,h/2,w/2,h/2))
      self.mappingCoords.append((w/2,h/2,w/2,h/2))
      self.nameStringCoords.append((20,     20))
      self.nameStringCoords.append((w/2+20, 20))
      self.nameStringCoords.append((20,     h/2+20))
      self.nameStringCoords.append((w/2+20, h/2+20))
    
    self.names = []
    for i,player in enumerate(settings.players):
      self.mappingQuads.append(HudQuad("%sMapping" % player.name,player.mapping.IMAGE, *self.mappingCoords[i]))
      self.names.append(player.name)
      
    for quad in self.mappingQuads:
      self.view.Add(quad)
    self.settings = settings

    self.isLoaded = False
    PauseMenuState.PreloadMusic()
    
  def Tick(self, time):
    State.Tick(self, time)
    if not self.isLoaded:
      game().graphics.views.append(self.view)
      game().graphics.force = True
      self.isLoaded = True
      if self.settings.nPlayers==1:
        t = 500
      else:
        t = 250
      self.view.WriteString('Loading...',Config.FONT, 60, 290, t)
      for i in range(self.settings.nPlayers):
        self.view.WriteString(self.names[i], Config.FONT, 30, *self.nameStringCoords[i])

    else:
      game().PopState()
      if not self.game:
        game().PushState(GameState(self.settings))
      else:
        self.game.Release()
        self.game.load(self.game.settings)
        

class GameState(State):
  def __init__(self, settings):
    State.__init__(self)
    self.loaded = False
    self.music = None
    self.load(settings)

 

  def load(self, settings):
    if self.music != None:
      self.music.Pause()
    self.settings = settings
   
    self.laps   = self.settings.nLaps
    self.stats  = {}
    self.countdown = 4
    self.countsound = self.countdown
    self.gameStarted = False
    self.gameOver = False
 
    self.nPlayersRacing = self.settings.nPlayers
 
    self.freeAINames =  list(GameSettings.AI_NAMES)
    random.shuffle(self.freeAINames)
    
    track = Track(self.settings.track)
    self.vehicleList = []

    frame = track.GetFrame(-30.0)
    self.startOrientation = Matrix(frame.position, frame.up, frame.fw)
    
    forwardMat = Matrix(ORIGIN, -PI/2.0, X)
    
    startFrame = track.GetFrame(0.0)\
       
    finishLineTransform = Matrix(40, 8, 1) * Matrix(startFrame.position+startFrame.up*0.1+startFrame.fw*-3, startFrame.fw, -startFrame.up)
    self.Add(Quad('FinishLine','FinishLine2.png',finishLineTransform))
    finishLineTransform = Matrix(6.35, 4.0, 4.0) * Matrix(startFrame.position+startFrame.up*0.1+startFrame.fw*-3, startFrame.up, startFrame.fw)
    self.Add(StaticModel('FinishLine','FinishLine.x',finishLineTransform))

    self.rings = []
    self.accuDelta = 0.
    self.ringStartingHue = 0.0


    for x in [200., 2200.]:
      for i in xrange(64):
        frame = track.GetFrame(x+10*i)
        tx = Matrix(3.0, 3.0, 6.0) * Matrix(frame.position, frame.up, frame.fw)
        ring = Ring("Ring", "Ring1.x", "Ring1.x", tx)
        self.rings.append(ring)
        self.Add(ring)
    self.track = self.Add(track)
      
    
    self.skybox = SkyBox()

    self.interfaces = []
    viewports = self.SetupViewports(self.settings.nPlayers)
    
    self.playerVehicles = [self.AddVehicle(player) for player in self.settings.players] 

    for i in range(self.settings.nAIs):
      self.AddVehicle(None)

    for i,vehicle in enumerate(self.playerVehicles):
      pi = PlayerInterface(self, vehicle, viewports[i])
      pi.AddRenderable(self.skybox)
      pi.AddRenderable(self.scene)
      self.interfaces.append(pi)

    self.SetupInputMapping()
       
    self.meteorManager = MeteorManager(self)
    
    self.lastMeteorTime = 0
    
    self.countFx = cpp.SoundFx();
    self.countFx.isLooping  = False
    self.countFx.is3D     = False
    self.countFx.isPaused = True
    self.countFx.volume = 20
    game().sound.sound.LoadSoundFx("Countdown.wav", self.countFx)

##    self.goFx = cpp.SoundFx();
##    self.goFx.isLooping  = False
##    self.goFx.is3D     = False
##    self.goFx.isPaused = True
##    game().sound.sound.LoadSoundFx("Go.wav", self.goFx)

    if self.music is None:
      self.music = Music(track.music, volume=20)
      self.music.Pause()
        
    self.boostbeams = []
    for i in xrange(16):
      beam = Model('StealBeam%d'%i, 'boostStealBeam.x', None, IDENTITY)
      beam.active = False
      beam.graphics.m_texOffset.v = random.random()
      self.boostbeams.append(self.Add(beam))
    
    game().io.LoadTexture(Config.UI_TEXTURE)
    
    game().time.Zero()
    self.loaded = True
  
  def BoostStealEvent(self, stealer, stealee, tx):
    for b in self.boostbeams:
      if b.active: continue
      b.active = True
      b.graphics.visible = True
      b.transform = tx
      b.graphics.m_texOffset.v += game().time.game_delta * -0.5 / game().time.RESOLUTION
      return
      
  def AddVehicle(self, player = None):
    if player and game().debug: print vars(player)
    
    n = len(self.vehicleList)    
    x = (n % 3 - 1)*15
    z = ((n / 3))*-15
    
    vehicle    = self.Add(Vehicle(
      player and player.name or self.freeAINames.pop(),    
      self.track, 
      Matrix(Point3(x, 7, z)) * self.startOrientation, 
      player and player.textureId or self.settings.RandomTextureId()
    ))
    vehicle.finishPlace = -1
    vehicle.lapBugCount = 0
    vehicle.isAI = player==None
    self.Add(Shadow(vehicle))
    self.vehicleList.append(vehicle)
    vehicle.isShutoff = True
    vehicle.Brake(1)
    if player:
      vehicle.sound.priority = 200
      PlayerBehavior(vehicle)
      vehicle.Backwards = False #???
    else:
      AIBehavior(vehicle, self.track, self)
    
    return vehicle                
      

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
    elif nPlayers>2:
      return [
        (0,   0,  w2, h2),
        (w2,  0,  w2, h2),
        (0,   h2, w2, h2),
        (w2,  h2, w2, h2),
      ]
      
  def SetupInputMapping(self):
    mappings = []
    
    for mapping in self.settings.debugMappings:
      mappings.append(mapping(None))
        
    for i,player in enumerate(self.settings.players):
      mappings.append(player.mapping(self.interfaces[i]))
      
    if self.settings.nPlayers==1 and self.settings.players[0].mapping != Keyboard1Mapping:
      mappings.append(Keyboard1Mapping(self.interfaces[0]))
      
    self.mapping = GameMapping(mappings)
    
  
  
  def Tick(self, time):
    delta = float(time.game_delta) / time.RESOLUTION
    self.countdown = self.countdown - delta
    if self.gameStarted == False:
      if math.ceil(self.countdown) < self.countsound:
        self.countsound = self.countsound - 1;
        if self.countdown > 0 and self.countdown <= 3:
          game().sound.sound.PlaySoundFx(self.countFx)
        #if self.countsound == 0:
        # game().sound.sound.PlaySoundFx(self.goFx)
        
    if self.gameStarted == False and self.countdown <=0:
      self.music.Unpause()
      self.gameStarted = True
      for vehicle in self.vehicleList:
        vehicle.isShutoff = False
        vehicle.Brake(0)
        self.stats.setdefault(vehicle, []).append(game().time.get_seconds())
        
      for i in range(CONSTS.NUM_METEORS):
        self.meteorManager.spawnRandom()

        

    for b in self.boostbeams:
      b.active = False
      b.graphics.visible = False
    
    # int SetOrientation3D(const Point3& listenerPos, const Vector3& listenerVel, const Vector3& atVector, const Vector3& upVector); //For 3D sound
    if len(self.interfaces) > 0:
      for i,interface in enumerate(self.interfaces):
        cam = interface.view.camera
        game().sound.sound.SetOrientation3DB(cam.GetPosition(), Point3(0,0,0), cam.GetLookAt(), cam.GetUp(), i, len(self.interfaces))

      #cam = self.interfaces[0].view.camera
      #---------------------------------------------------
      #game().sound.sound.SetOrientation3D(cam.GetPosition(), Point3(0,0,0), cam.GetLookAt(), cam.GetUp())
      
    
    _time.sleep(CONSTS.SLEEP_TIME)
    
    self.vehicleList.sort(key = lambda vehicle:vehicle.trackpos, reverse=True)
    
    for place,vehicle in enumerate(self.vehicleList):
      vehicle.place = place+1
      vehicle.lapRatio = vehicle.lapcount <= self.laps and vehicle.trackpos / self.track.dist % 1.0 or 1.0
    
    for interface in self.interfaces:
      interface.Tick(time)
      game().graphics.views.append(interface.view)
      game().graphics.views.append(interface.hud)
      
    if self.gameStarted:
      self.handleBoostStealing(float(time.game_delta)/time.RESOLUTION)
    
    if (not self.gameOver) and CONSTS.AIMED_METEOR_INTERVAL and self.gameStarted:
      self.lastMeteorTime += time.game_delta
      if self.lastMeteorTime > CONSTS.AIMED_METEOR_INTERVAL*time.RESOLUTION:
        self.lastMeteorTime = 0
        self.meteorManager.spawnTargeted(random.choice(self.vehicleList))
    
    self.meteorManager.Tick(time)
    
    self.accuDelta += delta
    
    if self.accuDelta>0.02:
      self.ringStartingHue = (self.ringStartingHue + self.accuDelta*5) % 1.0
      self.accuDelta-=0.1

      h = self.ringStartingHue
      
      for ring in self.rings:
        rgb = colorsys.hsv_to_rgb(h, .8, 0.5)
        ring.graphics.setTint(Vector4(rgb[0],rgb[1],rgb[2],1.0))
        h=(h+0.05)%1.0
  

    
    State.Tick(self, time)
    
    
  def LapEvent(self, vehicle, lap):
    if vehicle.lapBugCount < lap:
      vehicle.lapBugCount+=1
      if lap != 1:
        self.stats.setdefault(vehicle, []).append(game().time.get_seconds())
      
    
    if lap == self.laps+1:
      vehicle.finishPlace = vehicle.place
      if not vehicle.isAI:
        self.nPlayersRacing-=1
        if self.nPlayersRacing == 0:
          self.gameOver = True
          game().PushState(GameEndState(self.stats, self))
        
      vehicle.Brake(1)
      vehicle.isShutoff = True
      
    
  def ReloadConstsEvent(self):
    game().config.read()
    game().event.ReloadedConstsEvent()
    
  def PauseEvent(self):
    game().PushState(PauseMenuState())
    
  def KeyPressedEvent(self, key):
    if key == KEY.HOME:
      game().simspeed = 1.0
      
  def ObstacleAheadEvent(self, vehicleId, obstacleId):
    vehicle = self.entities[vehicleId]
    obstacle = self.entities[obstacleId]
    vehicle.obstacles.append(obstacle)
    
  def handleBoostStealing(self, delta):
    stealAmount = CONSTS.STEALING_SPEED*delta
    if stealAmount < 0.0001:
      #print "must be paused, no boost"
      return
    for i in range(len(self.vehicleList)-1):
      a = self.vehicleList[i]
      for j in range(i+1, len(self.vehicleList)):
        b = self.vehicleList[j]
        ap = a.physics.GetPosition()
        bp = b.physics.GetPosition()
        
        if length(ap-bp) > CONSTS.MAX_STEALING_DISTANCE: continue
        #possible stealing for one of the cars
        
        atx = a.physics.GetTransform()
        btx = b.physics.GetTransform()
        
        def steal(stealer, stealee, vec):
          
          size = 0.2
          if stealee.boostFuel > stealAmount and stealer.boostFuel + stealAmount < 5:
            #actually take boost
            stealer.boostFuel += stealAmount
            stealee.boostFuel -= stealAmount
            size = 1.0
            
          beamTransform = Matrix(size, size, length(vec)) * Matrix(stealer.physics.GetPosition(), Y, vec)
          game().event.BoostStealEvent(stealer, stealee, beamTransform)  
          
        if dot(mul0(atx, Z), normalized(bp-ap)) > 0.5:  steal(a, b, bp-ap)
        if dot(mul0(btx, Z), normalized(ap-bp)) > 0.5:  steal(b, a, ap-bp)
                    
  def Release(self):
    self.loaded = False
    self.meteorManager.Release()
    del self.meteorManager
    
    for i in self.entities.values():
      self.Remove(i)
    del self.track
    
    
  def Activate(self):
    State.Activate(self)
    if self.gameStarted:
      pass
      self.music.Unpause()
    if game().debug:
      print "Activate game state"

  def Deactivate(self):
    State.Deactivate(self) 
    
  def Pop(self):
    self.Release()
    self.music.Pause()
    self.vehicleList = []
    self.stats = {}
      

