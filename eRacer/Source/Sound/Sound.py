from Game.Module import Module
import eRacer as cpp
import os

class Sound(Module):
  def __init__(self):
    Module.__init__(self)
    self.sound = cpp.Sound()
    self.sound.Init()

  def PlaySound2D(self, file):
    self.sound.PlaySound2D(os.path.join(self.sound.SOUND_FOLDER,file))

  def Tick(self, time):
    Module.Tick(self, time)
    self.sound.Update()
    
    
  def Quit(self):
    Module.Quit(self)
    #this method does not exist anymore. Don or Tom should check whether it needs replacement
    #self.sound.StopMusic()
    self.sound.Shutdown()
