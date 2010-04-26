from Core.Globals     import *
from Game.State       import State
from Graphics.View    import View, HudView
from Core.Config      import Config

class CreditsState(State):
  SCROLLING_SPEED = 30 #pixels per second
  LEFT = 50
  
  def __init__(self):
    State.__init__(self)
    
    self.data = []
    for line in open(Config.CREDITS_FILE, 'r').readlines():
      if line.strip() == '':
        continue
      s = Struct()
      if line.startswith('h3. '):
        s.paddingTop = 40
        s.lineheight = 16
        s.fontsize = 24
        s.color = cpp.WHITE
        s.text = line[4::]
      else:
        s.paddingTop = 10
        s.lineheight = 20
        s.fontsize = 30
        s.color = cpp.GREEN
        s.text = line
        
        
      self.data.append(s)
      
      
    self.view = HudView([self.scene])
    self.top = 550
      
  def Tick(self, time):
    State.Tick(self, time)
 
    t = self.top

    self.view.WriteString("Credits", Config.FONT, 60, CreditsState.LEFT, t, cpp.WHITE)
    t += 60

    
    for i,line in enumerate(self.data):
      t += line.paddingTop + line.lineheight
      
      if t<-50:
        continue
      
      self.view.WriteString(line.text, Config.FONT, line.fontsize, CreditsState.LEFT, t+line.lineheight, line.color)
      
      if t>600:
        break
    
    self.top -= (float(time.wall_delta)/time.RESOLUTION)*CreditsState.SCROLLING_SPEED
    
    game().graphics.views.append(self.view)
    
  def KeyPressedEvent(self, key):
    if key == KEY.RETURN:
      game().PopState()
    elif key == KEY.ESCAPE:
      game().event.QuitEvent()  
