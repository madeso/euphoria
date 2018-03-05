print('Hello world');

time = 0;
Systems.AddUpdate("bark", function(dt) {
  time += dt;
  if(time>1){
    time -= 1;
    print('Bark!');
  }
});

Systems.AddUpdate("move up", function(dt) {
  ents = Registry.Entities([Registry.GetPosition2Id()]);
  ents.forEach(function(entity) {
    var pos = Registry.GetPosition2(entity);
    if(pos != null)
    {
      // pos seems to be not working?
      var vec = pos.vec;
      vec.y = vec.y + dt * 20;
      pos.vec = vec;
    }
  });
});
