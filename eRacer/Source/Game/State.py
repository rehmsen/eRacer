from Core.Globals import *

class State(cpp.State):
  MAPPING = None
  def __init__(self):
    self.parent   = None
    self.mapping  = self.MAPPING and self.MAPPING()
    self.active   = False
    self.entities = {}
    self.scene    = cpp.Scene()

    self.music          = cpp.SoundFx();
    self.music.looping  = True
    self.music.is3D     = False
    self.music.isPaused = False
    self.music.volume   = 24
    
  def Add(self, obj, scene=True):
    self.entities[obj.id] = obj
    g = getattr(obj, 'graphics', None)
    if scene and g: 
      if isinstance(g,list): 
        for i in g:
          self.scene.Add(i)
      else:
        self.scene.Add(g)
    return obj
    
  def Remove(self, obj):
    del self.entities[obj.id]
    g = getattr(obj, 'graphics', None)
    if g: self.scene.Remove(g)
    obj.Release()
    
  def Tick(self, time):
    for i in self.entities.itervalues():
      i.Tick(time)
    
  def Activate(self):
    self.active = True
    
  def Deactivate(self):
    self.active = False

  def Pop(self):
    pass
    
  def LoadMusic(self, filename):
    game().sound.sound.LoadSoundFx(filename, self.music)
    
  def PauseMusic(self):
    self.music.isPaused = True
    game().sound.sound.UpdateSoundFx(self.music)    
  