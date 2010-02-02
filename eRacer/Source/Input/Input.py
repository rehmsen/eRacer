from Game import Module
import eRacer

class Input(Module):
  def __init__(self, game):
    Module.__init__(self, game)
    self.input = eRacer.Input()
    self.input.Init(game.graphics.hwnd, game.graphics.hinst) 
    
  def Tick(self, time):
    Module.Tick(self, time)
    self.input.Update()