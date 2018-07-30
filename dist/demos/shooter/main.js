print('Hello world');

IsDown = function(key) {
  return key.state > 0.1;
};

WasDown = function(key) {
  return key.last_state > 0.1;
};

JustPressed = function(key) {
  return IsDown(key) && !WasDown(key);
};

Types = {
  Pos2: Registry.GetPosition2Id(),
  Sprite: Registry.GetSpriteId(),
  Player: Registry.New("Player"),
  MoveUp: Registry.New("MoveUp"),
  Star: Registry.New("Star"),
  DestroyOutside: Registry.New("DestroyOutside"),
  TimeOut: Registry.New("TimeOut", function() {
    c = {};
    c.time = 4;
    return c;
  })
};

// todo: create some on init callback, to spawn the stars
// or enter level callback
// or init callback on entity that is directly destroyed, or keep spawning points
// move sprite anchor to config

StarRandom = Math.NewRandom();

// todo: make sure init runs as expected
Systems.OnInit("place star", [Types.Pos2, Types.Star], function(entity){
  var vec = Registry.GetPosition2vec(entity);
  var p = StarRandom.NextPoint2(Camera.GetRect());
  vec.x = p.x;
  vec.y = p.y;
});

Systems.AddUpdate("star movement", function(dt) {
    var ents = Registry.Entities([Types.Sprite, Types.Star]);
    ents.forEach(function(entity) {
        var vec = Registry.GetPosition2vec(entity);
        if(vec != null)
        {
            var speed = 350;
            vec.y = vec.y - dt * speed;
            if(vec.y < 0)
            {
              // todo: generate x based on width of screen
              var p = StarRandom.NextPoint2(Camera.GetRect());
              vec.x = p.x;
              // todo: move up based on sprite size and screen size
              vec.y += 300;
            }
        }
    });
});


// todo: new update function
// Systems.OnUpdate("move up", [Types.Pos2, Types.MoveUp], function(dt, entities) { });

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
  var ents = Registry.Entities([Types.Pos2, Types.MoveUp]);
  ents.forEach(function(entity) {
    var vec = Registry.GetPosition2vec(entity);
    if(vec != null)
    {
      var speed = 250;

      vec.y = vec.y + dt * speed;
    }
  });
});

Systems.AddUpdate("time out", function(dt) {
  var ents = Registry.Entities([Types.TimeOut]);
  ents.forEach(function(entity) {
    var data = Registry.Get(entity, Types.TimeOut);
    data.time -= dt;
    if(data.time < 0)
    {
      print("Timeout");
      Registry.DestroyEntity(entity);
    }
  });
});

Systems.AddUpdate("destroy outside", function (dt) {
  var ents = Registry.Entities([Types.Sprite, Types.Pos2, Types.DestroyOutside]);
  ents.forEach(function (entity) {
    var sp = Registry.GetSprite(entity);
    var p = Registry.GetPosition2(entity);
    if(sp != null)
    {
      var cam = Camera.GetRect();
      var r = sp.GetRect(p);
      if(!cam.Contains(r))
      {
        print("Removed outside");
        Registry.DestroyEntity(entity);
      }
    }
  });
});

shotTemplate = Templates.Find("shot");
Systems.AddUpdate("player", function(dt) {
  ents = Registry.Entities([Types.Pos2, Types.Player]);
  ents.forEach(function(entity) {
    var vec = Registry.GetPosition2vec(entity);
    if(vec != null)
    {
      var speed = 150;

      var vertical = Input.up.state - Input.down.state;
      var horizontal = Input.right.state - Input.left.state;

      if(JustPressed(Input.fire))
      {
        if(!shotTemplate)
        {
          print("no shot");
        }
        else
        {
          var shot = shotTemplate.Create();
          var v = Registry.GetPosition2vec(shot);
          if(v != null)
          {
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
