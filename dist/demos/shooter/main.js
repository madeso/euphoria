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
    pos2 = Registry.GetPosition2(entity);
    pos = pos2.GetPositionRef();
    if(pos != null)
    {
      // pos seems to be not working?
      // print(pos.getY().toString());
      pos.y = pos.y + dt * 20;
    }
  });
});
