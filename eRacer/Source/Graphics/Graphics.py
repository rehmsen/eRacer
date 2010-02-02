from Game.Module    import Module
from Window         import Window
import eRacer

class Graphics(Module):
  def __init__(self, game):
    Module.__init__(self, game)
    
    self.window   = Window("Test")
    self.graphics = eRacer.GraphicsLayer.GetGraphicsInstance()
    
    # get pointers
    
    self.hwnd = self.window.hwnd
    self.hwnd.disown()
    self.hinst = self.window.hinst
    self.hinst.disown()
    
    self.graphics.Init(self.hwnd)
    
    self.d3d = self.graphics.GetDevice()
    self.d3d.disown()
    
    #self.views = []
    self.scene = None
    self.camera = None


  def Init(self):
    Module.Init(self)
    
  def Tick(self, time):
    Module.Tick(self, time)
    self.window.Poll()
    
    #print self.camera, self.scene
    self.graphics.RenderFrame(self.camera.camera, self.scene)
    self.window.SetTitle("eRacerX - %.2f FPS" % time.Fps())
    
    
    #while self.views:
    #  self.graphics.RenderView(self.views.pop())
    
    
    self.window.SetTitle("eRacerX - %.2f FPS" % time.Fps())
  
  def Quit(self):
    Module.Quit(self)
    self.graphics.Shutdown()
    # TODO close window
    