from Core.Globals import *
import ConfigParser

import os
import os.path

class Config(object):
  CONSTANTS_FILE  = 'Config/Constants.cnf'
  DEBUG_FILE      = 'Config/Debug.cnf'
  GLOBAL_FILE     = 'Config/Settings.cnf'
  USER_FOLDER     = 'User/'
  USER_FILE       = USER_FOLDER+'Settings.cnf'
  USER_STATS      = USER_FOLDER+'Statistics.cnf'
  CREDITS_FILE    = 'Config/Credits.txt'
  
  
  FONT = 'Sony Sketch EF'
  DEBUG_FONT = 'Verdana'  
  UI_TEXTURE = 'futureui2-large.png'
  
  def __init__(self):
    self.read()

  def read(self):
    constantsFiles = [self.CONSTANTS_FILE]
    if game().debug:
      constantsFiles.append(self.DEBUG_FILE)
    self.constants = self.readFile(constantsFiles)
    self.parseConstants(self.constants)
    self.settings = self.readFile([self.GLOBAL_FILE, self.USER_FILE])
    self.user = self.readFile(self.USER_FILE)
    
        
  def readFile(self, file):
    cp = ConfigParser.ConfigParser()
    cp.read(file)
    return cp

  def parseConstants(self, cp):
    for k, v in cp.items('CONSTS'):
      k = k.upper()
      try:
        c = getattr(CONSTS, k)
        r = getattr(cp, 'get%s' % type(c).__name__)('CONSTS', k)
        setattr(CONSTS, k, r)
        if game().debug:
          print 'Set %s \t= %r' % (k,r)
      except:
        print 'Failed to set %s' % k
        import traceback
        traceback.print_exc()
    
    
  def save(self):
    if not os.path.exists(self.USER_FOLDER):
      os.mkdir(self.USER_FOLDER)

    with open(self.USER_FILE, 'wb') as file:
      self.user.write(file)
      
  def set_setting(self, key, value, section='GENERAL'):
    if not self.user.has_section(section):
      self.user.add_section(section)
    self.user.set(section, key, value)
    self.settings.set(section, key, value)
    self.save()
    
  def get_setting(self, key, section='GENERAL'):
    return self.settings.get(section, key)
    
