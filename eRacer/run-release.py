import os
import sys
import time
# paths to code
sys.path.append(os.path.join(sys.path[0], 'Source'))
sys.path.append(os.path.join(sys.path[0], 'Release'))
sys.path.append(os.path.join(sys.path[0], 'Source\\Swig'))
sys.path.append('.')

# set path to load fmod.dll
os.environ['PATH'] += ';Lib'

#print sys.path
#print os.environ['PATH']

def run():
  m = None
  try:
    from Main import Main
    m = Main()
    m.Init()
    m.Run()
  except:
    if m: m.Quit()
    import traceback
    import msvcrt
    traceback.print_exc()
    print 'Press any key to close'
    msvcrt.getch()
  
if __name__ == '__main__':
  run()    
