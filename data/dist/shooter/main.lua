oldprint = print
print = function(...)
    local args = { n = select("#", ...), ... }
    local t = ""
    for i=1,args.n do
        t = t .. tostring(args[i])
    end
    oldprint(t)
end

print('Hello world')

IsDown = function(key) 
    return key.state > 0.1
end

WasDown = function(key) 
    return key.last_state > 0.1
end

JustPressed = function(key) 
    return IsDown(key) and not WasDown(key)
end

StarRandom = Math.NewRandom()

Types = {
    Pos2= Registry.GetPosition2Id(),
    Sprite= Registry.GetSpriteId(),
    Player= Registry.New("Player"),
    MoveUp= Registry.New("MoveUp"),
    Star= Registry.New("Star", function(args) 
        c = {}
        c.speed = args:GetNumber("speed") + StarRandom:NextRangeFloat(args:GetNumber("random_boost"))
        return c
    end),
    DestroyOutside= Registry.New("DestroyOutside"),
    TimeOut= Registry.New("TimeOut", function() 
        c = {}
        c.time = 4
        return c
    end)
}

-- todo: create some on init callback, to spawn the stars
-- or enter level callback
-- or init callback on entity that is directly destroyed, or keep spawning points
-- move sprite anchor to config

Systems.OnInit("place star", {Types.Pos2, Types.Star}, function(entity)
    local vec = Registry.GetPosition2vec(entity)
    local p = StarRandom:NextPoint2(Camera.GetRect())
    vec.x = p.x
    vec.y = p.y
end)

Systems.AddUpdate("star movement", function(dt) 
    local ents = Registry.Entities({Types.Sprite, Types.Star})
    for _, entity in pairs(ents) do
        local star = Registry.Get(entity, Types.Star)
        local vec = Registry.GetPosition2vec(entity)
        if vec ~= null then
            vec.y = vec.y - dt * star.speed;
            local vy = vec.y
            -- print("Moving star to ", vy)
            if vy < 0.0 then
                vec.x = StarRandom:NextRangeFloat(Camera.GetRect():GetWidth())
                vec.y = vec.y + Camera.GetRect():GetHeight()
                print("Reseting star to ", vec.x, " ", vec.y)
            end
        end
    end
end)


-- todo: new update function
-- Systems.OnUpdate("move up", [Types.Pos2, Types.MoveUp], function(dt, entities) { });

time = 0
bark = 0
Systems.AddUpdate("bark", function(dt) 
    time = time + dt
    if time>1 then
        time = time - 1
        bark = bark + 1
        print('Bark!', bark)
    end
end)

Systems.AddUpdate("move up", function(dt) 
    local ents = Registry.Entities({Types.Pos2, Types.MoveUp})
    for _, entity in pairs(ents) do
        local vec = Registry.GetPosition2vec(entity)
        if vec ~= null then
            local speed = 250
            vec.y = vec.y + dt * speed
        end
    end
end)

Systems.AddUpdate("time out", function(dt) 
    local ents = Registry.Entities({Types.TimeOut});
    for _, entity in pairs(ents) do
        local data = Registry.Get(entity, Types.TimeOut)
        data.time = data.time - dt
        if data.time < 0 then
            print("Timeout")
            Registry.DestroyEntity(entity)
        end
    end
end)

Systems.AddUpdate("destroy outside", function (dt) 
    local ents = Registry.Entities({Types.Sprite, Types.Pos2, Types.DestroyOutside})
    for _, entity in pairs(ents) do
        local sp = Registry.GetSprite(entity)
        local p = Registry.GetPosition2(entity)
        if sp ~= null then
            local cam = Camera.GetRect()
            local r = sp.GetRect(p)
            if not cam:Contains(r) then
                Registry.DestroyEntity(entity)
            end
        end
    end
end)

shotTemplate = Templates.Find("shot")
Systems.AddUpdate("player", function(dt)
    ents = Registry.Entities({Types.Pos2, Types.Player})
    for _, entity in pairs(ents) do
        local vec = Registry.GetPosition2vec(entity)
        if vec ~= null then
            local speed = 150

            local vertical = Input.up.state - Input.down.state
            local horizontal = Input.right.state - Input.left.state

            if JustPressed(Input.fire) then
                if not shotTemplate then
                    print("no shot")
                else
                    local shot = shotTemplate:Create()
                    local v = Registry.GetPosition2vec(shot)
                    if v ~= null then
                        v.x = vec.x
                        v.y = vec.y
                    end
                end
            end
            vec.y = vec.y + dt * speed * vertical
            vec.x = vec.x + dt * speed * horizontal
        end
    end
end)
