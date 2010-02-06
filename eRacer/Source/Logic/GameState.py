from Core.Globals   import *
from Game.State     import State

from MenuState  import PauseMenuState
from Box        import Box
from Plane      import Plane
from Track      import Track
from Ship       import Ship
from Vehicle    import Vehicle
from Camera     import ChasingCamera, FirstPersonCamera
from Starfield  import Starfield
from GameMapping    import GameMapping

class GameState(State):
  MAPPING = GameMapping
  def __init__(self):
    State.__init__(self)
    self.load()
    
  def load(self):
    # testing stuff
   # game().sound.PlaySound2D("jaguar.wav")
    
    scene = eRacer.Scene()
    self.scene = scene
    game().graphics.graphics.m_scene = scene
    
    # TODO this should be in some 
    # map-loading code with a progress bar
    self.scene.LoadSkyBox('skybox2.x')
    
    
    self.player = Vehicle(self.scene)

    self.cameras = []
    self.cameras.append(ChasingCamera(self.player))
    self.cameras.append(FirstPersonCamera())
    self.cameraIndex = 0
    
    game().logic.Add(self.player)
    
    for camera in self.cameras:
      game().logic.Add(camera)
    
    
    
    game().logic.Add(Ship(scene))
    game().logic.Add(Track(scene))
    game().logic.Add(Plane(scene))    
    
    self.starfield1 = Starfield(scene, self.camera, 1024, 1000.0)
    self.starfield2 = Starfield(scene, self.camera, 1024, 100.0)
    self.starfield3 = Starfield(scene, self.camera, 1024, 20.0)
    
    
    game().logic.Add(self.starfield1)
    game().logic.Add(self.starfield2)
    game().logic.Add(self.starfield3)

    self.boxcount = 1
    game().time.Zero()
    
  def get_camera(self):
    return self.cameras[self.cameraIndex]
    
  camera = property(get_camera)   
    
  def CameraChangedEvent(self):
    # print "Camera ",self.cameraIndex+1," out of ",len(self.cameras)
    self.cameraIndex+=1
    if(self.cameraIndex>=len(self.cameras)): self.cameraIndex=0
    self.starfield1.camera = self.camera
    self.starfield2.camera = self.camera
    self.starfield3.camera = self.camera
      
    
    
  def Tick(self, time):
    State.Tick(self, time)

    game().graphics.scene  = self.scene
    game().graphics.camera = self.camera
    #game().graphics.views.append(self.view)
    
    if time.seconds > self.boxcount:
      self.boxcount += max(self.boxcount+1, 20)
      #game().logic.Add(Box(self.scene))
      
  def PauseEvent(self):
    game().PushState(PauseMenuState())

  def KeyPressedEvent(self, key):   
    if key == KEY.SPACE:
      game().sound.PlaySound2D("jaguar.wav")          
    
    if key == KEY.R:
      game().config.read()
      game().event.ReloadConstsEvent()        
      
  def GamepadButtonPressedEvent(self, button):
    if button == eRacer.BUTTON_START:
      game().PushState(PauseMenuState())