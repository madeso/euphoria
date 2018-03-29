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
      var speed = 50;

      var vertical = Input.up - Input.down;
      var horizontal = Input.right - Input.left;

      var vec = pos2.vec;
      vec.y = vec.y + dt * speed * vertical;
      vec.x = vec.x + dt * speed * horizontal;
      pos2.vec = vec;
    }
  });
});
