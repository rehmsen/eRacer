from Core.Globals import *

class Track(Entity):
  def __init__(self, game):
    Entity.__init__(self, game)

    self.physics = eRacer.TriMesh()
    self.graphics = game.graphics.scene.CreateStaticGeometry("track")
    self.graphics.visible = False
        
    def load(r):
      if r:
        print 'Failed to load mesh!!'
        return
      self.graphics.visible = True
      self.physics.Initialize(self.graphics.mesh());
      
    game.io.LoadMeshAsync(load, self.graphics, "simple_track.x")   
    
  def Tick(self, time):
     Entity.Tick(self, time)