from Core.Globals import game, cpp

class Event(cpp.Event):
  def __init__(self, game):
    cpp.Event.__init__(self)
    self.listeners = {}

  def Register(self, obj, event=None):
    #print 'Register', event, obj
    try:
      if callable(obj):
        event = obj.__name__
        func = obj
      else:
        func = getattr(obj, event)
      self.listeners.setdefault(event, []).append(func)
    except:
      print 'Error registering event!', obj, event
      import traceback
      traceback.print_exc()
      
  def UnRegister(self, obj, event=None):
    if callable(obj):
      event = obj.__name__
      func = obj
    else:
      func = getattr(obj, event)
    self.listeners[event].remove(obj)
    
    

  def __getattribute__(self, attr):
    if attr.endswith('Event'):
      def f(*args, **kwargs):
        # print '%s%r%r' % (attr, args, kwargs)
        for f in self.listeners.get(attr, []):
          try:
            # print f
            f(*args, **kwargs)
          except:
            import traceback
            print 'Error calling listener callback: %s %r' % (attr, f)
            traceback.print_exc()
        try:
          s = game().states[-1]
          if hasattr(s, attr): getattr(s, attr)(*args, **kwargs)
        except:
          import traceback
          print 'Error calling state top'
          traceback.print_exc()
          raise
        try:        
          s = game().states[-1].mapping
          if s and hasattr(s, attr): getattr(s, attr)(*args, **kwargs)
        except:
          import traceback
          print 'Error calling mapping'
          traceback.print_exc() 
          raise
        return 0
      return f

    return object.__getattribute__(self, attr)
