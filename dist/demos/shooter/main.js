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
    var pos2 = Registry.GetPosition2(entity);
    if(pos2 != null)
    {
      // pos2 seems to be not working?
      print('dog is good');
      var vec = pos2.GetPos();
      print('cat is also good');
      vec.y = vec.y + dt * 20;
      print('e');
      pos2.SetPos(vec);
      print('as');
      pos2.vec = vec;
      print('f');
    }
  });
});
