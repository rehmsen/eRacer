from Core.Globals import *
from Mapping      import Mapping, E

class MenuMapping(Mapping):
  def KeyPressedEvent(self, key):
    if key == KEY.UP:     return E.MenuUpEvent()
    if key == KEY.DOWN:   return E.MenuDownEvent()
    if key == KEY.RETURN: return E.MenuSelectEvent()
    
  def GamepadStick1AbsoluteEvent(self, x, y):
    if y >  900.0:        return E.MenuUpEvent()
    if y < -900.0:        return E.MenuDownEvent()
    
  def GamepadButtonPressedEvent(self, button):
    if button == eRacer.BUTTON_A: return E.MenuSelectEvent()
    
class MainMenuMapping(MenuMapping):
  def KeyPressedEvent(self, key):
    if key == KEY.ESCAPE:     return E.QuitEvent()
    return MenuMapping.KeyPressedEvent(self, key)    

class PauseMenuMapping(MenuMapping):
  def KeyPressedEvent(self, key):
    if key == KEY.ESCAPE:     return E.UnPauseEvent()
    return MenuMapping.KeyPressedEvent(self, key)
    
  def GamepadButtonPressedEvent(self, button):
    if button == eRacer.BUTTON_START:  return E.UnPauseEvent()
    return MenuMapping.GamepadButtonPressedEvent(self, button)
    