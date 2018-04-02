print('Hello world');

Types = {
  Pos2: Registry.GetPosition2Id(),
  Player: Registry.New("Player")
};

time = 0;
Systems.AddUpdate("bark", function(dt) {
  time += dt;
  if(time>1){
    time -= 1;
    print('Bark!');
  }
});
Systems.AddUpdate("move up", function(dt) {
  ents = Registry.Entities([Types.Pos2, Types.Player]);
  ents.forEach(function(entity) {
    var pos2 = Registry.GetPosition2(entity);
    if(pos2 != null)
    {
      var speed = 150;

      var vertical = Input.up - Input.down;
      var horizontal = Input.right - Input.left;

      var vec = pos2.vec;
      vec.y = vec.y + dt * speed * vertical;
      vec.x = vec.x + dt * speed * horizontal;
      pos2.vec = vec;
    }
  });
});
