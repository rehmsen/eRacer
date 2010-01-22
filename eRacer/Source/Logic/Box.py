from Game import Entity
from Main import Main
import eRacer

class Box(Entity):
  def __init__(self, game):
    Entity.__init__(self, game)

    self.box = eRacer.Box(True, 20)
    self.graphics = game.graphics.scene.CreateMovingGeometry("test")
    self.graphics.visible = False
        
    def load(r):
      if r:
        print 'Failed to load mesh!!'
        return
      self.graphics.visible = True
      
    #game.io.LoadMeshAsync(load, self.graphics, "Resources/Ship_06.x")   
    game.io.LoadMeshAsync(load, self.graphics, "Resources/MeshFromOBJ.x")
    
  def Tick(self, time):
     pos = self.box.GetPosition()
     self.graphics.SetPosition(pos)
     print pos.x , pos.y, pos.z
