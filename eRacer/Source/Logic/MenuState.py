from Core.Globals     import *
from Core.Menu        import *
from Game.State       import State
  
from Camera           import Camera, CirclingCamera, OrthographicCamera
from Quad             import Quad
from HudQuad          import HudQuad
from MenuMapping      import *
from Graphics.View    import View, HudView
from GameSettings     import GameSettings
from Sound.Music      import Music

class MenuState(State):
  menuNav = None
  menuSel = None
  
  bgFileNames = []
  
  def __init__(self):
    State.__init__(self)
    self.selected = 0
    
    self.view = HudView([self.scene])

    if MenuState.menuNav == None:
      MenuState.menuNav = cpp.SoundFx();
      MenuState.menuNav.isLooping  = False
      MenuState.menuNav.is3D     = False
      MenuState.menuNav.isPaused = True
      game().sound.sound.LoadSoundFx("MenuNav.wav", self.menuNav)

    if MenuState.menuSel == None:
      MenuState.menuSel = cpp.SoundFx();
      MenuState.menuSel.isLooping  = False
      MenuState.menuSel.is3D     = False
      MenuState.menuSel.isPaused = True
      game().sound.sound.LoadSoundFx("MenuSelect.wav", self.menuSel)

    self.menu = []
    self.menuTop = 50
    self.menuLeft = 100
    self.bg = None
    self.bgFileNames = []
  
  def Tick(self, time):
    State.Tick(self, time)
    game().graphics.views.append(self.view)
    
    if not self.active:
      return
    
    y = self.menuTop  

    for i,m in enumerate(self.menu):
      yOffset = m.draw(self.view, self.menuLeft, y, i == self.selected)
      y += yOffset

  
  def setBackground(self, filename):
    if self.bg:
      self.view.Remove(self.bg.graphics)
      self.view.Remove(self.ui.graphics)
    
    self.bgFileNames.append(filename)
    
    self.bg = HudQuad("background", filename, 0, 0, 800, 600, True)
    self.view.Add(self.bg)
    self.view.Add(self.ui) # must be after the bg

      
  def MenuUpEvent(self):
    self.ChangeMenu(-1)
  
  def ChangeMenu(self, delta):
    game().sound.sound.PlaySoundFx(self.menuNav)
    self.selected = (self.selected+delta) % len(self.menu)
    while not self.menu[self.selected].enabled:
      self.selected = (self.selected+delta) % len(self.menu)    
  
  def MenuDownEvent(self):
    self.ChangeMenu(+
    1)

  def MenuLeftEvent(self):
    method = getattr(self.menu[self.selected], 'MenuLeftEvent', None)
    if method: 
      game().sound.sound.PlaySoundFx(self.menuNav)
      method()

  def MenuRightEvent(self):
    method = getattr(self.menu[self.selected], 'MenuRightEvent', None)
    if method: 
      game().sound.sound.PlaySoundFx(self.menuNav)
      method()

  def MenuSelectEvent(self):
    method = getattr(self.menu[self.selected], 'MenuSelectEvent', None)
    if method: 
      game().sound.sound.PlaySoundFx(self.menuSel)
      method()
      
  def CharacterTypedEvent(self, character):
    method = getattr(self.menu[self.selected], 'CharacterTypedEvent', None)
    if method: 
      game().sound.sound.PlaySoundFx(self.menuNav)
      method(character)

  def KeyPressedEvent(self, key):
    method = getattr(self.menu[self.selected], 'KeyPressedEvent', None)
    if method: 
      game().sound.sound.PlaySoundFx(self.menuNav)
      method(key)

  def Menu_Exit(self):
    game().event.QuitEvent()  
    
    
class MainMenuState(MenuState):
  MAPPING = MainMenuMapping
  
  def __init__(self):
    MenuState.__init__(self)
    logo = HudQuad("Logo","eRacerXLogoNegative.png", 30, 35, 535, 212)
    self.view.Add(logo)
    # self.view.Add(HudQuad("TextBox", Config.UI_TEXTURE, 20,110,760,420, False))
    self.music = Music("Terran5.ogg")
    
    self.menu = [
      ApplyMenuItem('New Game', self.Menu_New_Game),
      ApplyMenuItem('High Scores', self.Menu_High_Scores),
      ApplyMenuItem('Credits', self.Menu_Credits),
      ApplyMenuItem('Exit', self.Menu_Exit)
    ]
    self.menuTop = 240

  def Activate(self):
    State.Activate(self)
    
  def Menu_High_Scores(self):
    game().PushState(HighScoreState())
    
  def Menu_Credits(self):
    game().PushState(CreditsState())
    
  def Menu_New_Game(self):
    game().PushState(SetupGameMenuState())
    
  def Tick(self, time):
    y = 350
    for i in ['Don Ha', 'John Stuart', 'Michael Blackadar', 'Tom Flanagan', 'Ole Rehmsen']:
      self.view.WriteString(
        i, Config.FONT, 28, 500, y
      )
      y += 30
    MenuState.Tick(self, time)


    
class SetupGameMenuState(MenuState):
  MAPPING = MainMenuMapping

  def __init__(self):
    MenuState.__init__(self)
    
    self.menuTop = 170
    self.ui = HudQuad("TextBox", Config.UI_TEXTURE, 20, 110, 760, 420)
    self.view.Add(self.ui)
    self.settings = GameSettings()
    
    aiPlayerOptions = []
    for i in range(GameSettings.MAX_AIS+1):
      aiPlayerOptions.append((str(i),i))
    
    trackOptions = []
    for i,trackName in enumerate(self.settings.availableTrackNames):
      trackOptions.append((trackName,i))
    
    self.menu = [
      ApplyMenuItem('Start', self.Menu_Start, 36),
      ApplyMenuItem('Setup Players...', self.Menu_Setup_Players),
      SelectMenuItem('AI Players', self.Menu_AI_Players, aiPlayerOptions, self.settings.nAIs),
      SelectMenuItem('Track', self.Menu_Track, trackOptions, self.settings.trackIndex),
      SelectMenuItem('Lap Count', self.Menu_Lap_Count, map(lambda x: (str(x[1]),x[0]) , enumerate(GameSettings.LAP_COUNTS)), self.settings.nLapsIndex),
      ApplyMenuItem('Back', self.Menu_Back),
    ]
    self.updateBackground()
    
  def updateBackground(self):
    self.setBackground('Trackbg-%s.png' % self.settings.track.__name__)    
    
  def Menu_Start(self):
    self.parent.music.Pause()
    if self.bg:
      self.view.Remove(self.bg.graphics)
    for filename in self.bgFileNames:
      print "trying to free",filename
      game().io.UnloadTexture(filename)
    
    game().PushState(LoadScreenState(self.settings))
        
  def Menu_AI_Players(self, value):
    self.settings.nAIs = value[1]
    
  def Menu_Track(self, value):
    self.settings.trackIndex = value[1]
    self.updateBackground()

  def Menu_Lap_Count(self, value):
    self.settings.nLapsIndex = value[1]    
    
  def Menu_Setup_Players(self):
    print 'setupplayer'
    game().PushState(SetupPlayersMenuState(self.settings))    
    
  def Menu_Back(self):
    game().PopState()
    
class SetupPlayersMenuState(MenuState):
  MAPPING = MainMenuMapping
  
  def __init__(self, settings):
    MenuState.__init__(self)  
    
    self.ui = HudQuad("UI Overlay", Config.UI_TEXTURE, 20, 110, 760, 420)
    self.view.Add(self.ui)
    
    self.settings = settings
   
    self.UpdateMenu()  
    self.setBackground('Trackbg-%s.png' % self.settings.track.__name__)    
  

  def Tick(self, time):
    State.Tick(self, time)
    game().graphics.views.append(self.view)
    
    if not self.active:
      return
    
    quadrants = [
      (100, 240),
      (400, 240), 
      (100, 340), 
      (400, 340)
    ]

    for (x,y), menu in zip(quadrants,self.pmenu):
      for i,m in enumerate(menu):
        yOffset = m.draw(self.view, x, y, m is self.menu[self.selected])
        y += yOffset
    
    y = 170
    for i,m in enumerate(self.mmenu):
      m.draw(self.view, self.menuLeft, y, m is self.menu[self.selected])
      y += 250

  def UpdateMenu(self):
    self.menu  = []
    self.mmenu = []
    self.pmenu = [[],[],[],[]]

    humanPlayerOptions = []
    for i,num in enumerate(self.settings.availablePlayerNums):
      humanPlayerOptions.append((str(num),i))   

    self.mmenu.append(SelectMenuItem('Human Players', self.Menu_Human_Players, humanPlayerOptions, self.settings.nPlayersIndex))
    self.mmenu.append(ApplyMenuItem('Done', self.Menu_Back))
    self.settings.update_players()
    
    fontsize = 24
    lineheight = 24
    padding = 10

    for playerId,player in enumerate(self.settings.playersIndices):
      self.pmenu[playerId].append(InputMenuItem('Name', self.settings.set_player_name, playerId, player.name, labelwidth=100))
      self.pmenu[playerId][-1].fontsize = fontsize
      self.pmenu[playerId][-1].lineheight = lineheight
    
      mappingOptions = []
      for i,mapping in enumerate(self.settings.availableMappings):
        s = mapping and mapping.__name__.replace('Mapping','') or 'None'
        mappingOptions.append((s,playerId,i))   
    
      self.pmenu[playerId].append(SelectMenuItem('Controls', self.Menu_Controls, mappingOptions, player.mappingIndex, labelwidth=100))
      self.pmenu[playerId][-1].fontsize = fontsize
      self.pmenu[playerId][-1].lineheight = lineheight
      
      textureOptions = []

      for i,textureName in enumerate(GameSettings.TEXTURE_NAMES):
        textureOptions.append((textureName, playerId, i))
      
      self.pmenu[playerId].append(SelectMenuItem('Color', self.Menu_Color, textureOptions, player.textureIndex, labelwidth=100))
      self.pmenu[playerId][-1].fontsize = fontsize
      self.pmenu[playerId][-1].lineheight = lineheight + padding
      
    for j in range(playerId+1, 4):
		  self.pmenu[j].append(InputMenuItem('Name', 'Player', j, 'Player', labelwidth=100, enabled=False))
		  self.pmenu[j][-1].fontsize = fontsize
		  self.pmenu[j][-1].lineheight = lineheight
	      
		  self.pmenu[j].append(SelectMenuItem('Controls', self.Menu_Controls, mappingOptions, 0, labelwidth=100, enabled=False))
		  self.pmenu[j][-1].fontsize = fontsize
		  self.pmenu[j][-1].lineheight = lineheight
	      
		  self.pmenu[j].append(SelectMenuItem('Color', self.Menu_Color, textureOptions, 0, labelwidth=100, enabled=False))
		  self.pmenu[j][-1].fontsize = fontsize
		  self.pmenu[j][-1].lineheight = lineheight + padding
    
    # copy all elements linearly for input logic
    self.menu = self.mmenu[:1] \
      + self.pmenu[0]  \
      + self.pmenu[1]  \
      + self.pmenu[2]  \
      + self.pmenu[3]  \
      + self.mmenu[1:] \
    
  def MenuSelectEvent(self):
    # if not MenuState.MenuSelectEvent(self):
    game().PopState()

  def Menu_Human_Players(self, value):
    self.settings.nPlayersIndex = value[1]      
    self.UpdateMenu()

    
  def Menu_Controls(self, value):
    self.settings.set_player_mapping_index(value[1], value[2])
    
  def Menu_Color(self, value):
    self.settings.set_player_texture_index(value[1], value[2])
  
  
  def Menu_Back(self):
    game().PopState()    

class PauseMenuState(MenuState):
  MAPPING = PauseMenuMapping
  music = None  
  
  @classmethod
  def PreloadMusic(cls):
    if not cls.music:
      cls.music = Music("SwanLakeShort.mp3",volume=128)
      cls.music.Pause()
  
  def __init__(self):
    MenuState.__init__(self)
    self.menu = [
      ApplyMenuItem('Continue',self.Menu_Continue),
      ApplyMenuItem('Restart race', self.Menu_Restart_race),
      ApplyMenuItem('Main menu',self.Menu_Main_menu),
      ApplyMenuItem('Exit',self.Menu_Exit),
    ]
    self.menuTop = 190
    self.view.Add(HudQuad("TextBox", Config.UI_TEXTURE, 20, 110, 760, 420))
    pause = HudQuad("PauseHeadline", "pause_glow.png", 300, 110, 110, 30)
    pause.SetCenter(350,125)
    self.view.Add(pause)
    self.view.name = 'Pause HudView'
    if not PauseMenuState.music:
      PauseMenuState.music = Music("SwanLakeShort.mp3", volume=255)
      
    
  def Activate(self):
    game().simspeed = 0.
    MenuState.Activate(self)
    self.parent.music.Pause()
    self.music.Unpause()

    
  def Deactivate(self):
    game().simspeed = 1.
    MenuState.Deactivate(self)
    self.music.Pause()
    if self.parent and self.parent.music:
      self.parent.music.Unpause()
    
  def UnPauseEvent(self):
    game().PopState()
    
  def Menu_Continue(self):
    game().PopState()

  def Menu_Main_menu(self):
    self.parent = None
    while not game().states[-1].__class__ is MainMenuState:
      game().PopState()
      
    game().states[-1].music.Restart()
    
  def Menu_Restart_race(self):
    while not game().states[-1].__class__ is GameState:
      game().PopState()
    
    gamestate = game().states[-1]
    load = LoadScreenState(gamestate.settings, gamestate)
    game().PushState(load)
          
  def Tick(self, time):
    self.parent.Tick(time)
    MenuState.Tick(self, time)
      
from GameState      import GameState, LoadScreenState
from HighScoreState import HighScoreState
from CreditsState   import CreditsState  
