print('Hello world');

IsDown = function(key) {
  return key.state > 0.1
}
WasDown = function(key) {
  return key.last_state > 0.1
}
JustPressed = function(key) {
  return IsDown(key) && !WasDown(key);
}

Types = {
  Pos2: Registry.GetPosition2Id(),
  Player: Registry.New("Player"),
  MoveUp: Registry.New("MoveUp")
};

time = 0;
bark = 0;
Systems.AddUpdate("bark", function(dt) {
  time += dt;
  if(time>1){
    time -= 1;
    bark += 1;
    print('Bark!', bark);
  }
});

Systems.AddUpdate("move up", function(dt) {
  ents = Registry.Entities([Types.Pos2, Types.MoveUp]);
  ents.forEach(function(entity) {
    var pos2 = Registry.GetPosition2(entity);
    if(pos2 != null)
    {
      var speed = 250;

      var vec = pos2.vec;
      vec.y = vec.y + dt * speed;
      pos2.vec = vec;
    }
  });
});


shotTemplate = Templates.Find("shot")
Systems.AddUpdate("player", function(dt) {
  ents = Registry.Entities([Types.Pos2, Types.Player]);
  ents.forEach(function(entity) {
    var pos2 = Registry.GetPosition2(entity);
    if(pos2 != null)
    {
      var speed = 150;

      var vertical = Input.up.state - Input.down.state;
      var horizontal = Input.right.state - Input.left.state;

      var vec = pos2.vec;
      if(JustPressed(Input.fire))
      {
        if(!shotTemplate)
        {
          print("no shot");
        }
        else
        {
          var shot = shotTemplate.Create();
          var p2 = Registry.GetPosition2(shot);
          if(p2 != null)
          {
            var v = p2.vec;
            v.x = vec.x;
            v.y = vec.y;
          }
        }
      }
      vec.y = vec.y + dt * speed * vertical;
      vec.x = vec.x + dt * speed * horizontal;
    }
  });
});
