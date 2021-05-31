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

is_down = function(key)
    return key.state > 0.1
end

was_down = function(key)
    return key.last_state > 0.1
end

just_pressed = function(key)
    return is_down(key) and not was_down(key)
end

star_random = math.new_random()

types = {
    pos2 = registry.get_position2_id(),
    sprite = registry.get_sprite_id(),
    player = registry.create("player"),
    move_up = registry.create("move_up"),
    star = registry.create("star", function(args)
        c = {}
        c.speed = args:get_number("speed") + star_random:next_range_float(args:get_number("random_boost"))
        return c
    end),
    destroy_outside = registry.create("destroy_outside"),
    timeout = registry.create("timeout", function()
        c = {}
        c.time = 4
        return c
    end)
}

-- todo: create some on init callback, to spawn the stars
-- or enter level callback
-- or init callback on entity that is directly destroyed, or keep spawning points
-- move sprite anchor to config

systems.on_init("place star", {types.pos2, types.star}, function(entity)
    local vec = registry.get_position2_vec(entity)
    local p = star_random:next_point2(camera.get_rect())
    vec.x = p.x
    vec.y = p.y
end)

systems.add_update("star movement", function(dt)
    local ents = registry.entities({types.sprite, types.star})
    for _, entity in pairs(ents) do
        local star = registry.get(entity, types.star)
        local vec = registry.get_position2_vec(entity)
        if vec ~= null then
            vec.y = vec.y - dt * star.speed;
            if vec.y < 0 then
                vec.x = star_random:next_range_float(camera.get_rect():get_width())
                vec.y = vec.y + camera.get_rect():get_height()
            end
        end
    end
end)


-- todo: new update function
-- Systems.OnUpdate("move up", [Types.Pos2, Types.MoveUp], function(dt, entities) { });

time = 0
bark = 0
systems.add_update("bark", function(dt)
    time = time + dt
    if time>1 then
        time = time - 1
        bark = bark + 1
        print('Bark!', bark)
    end
end)

systems.add_update("move up", function(dt)
    local ents = registry.entities({types.pos2, types.move_up})
    for _, entity in pairs(ents) do
        local vec = registry.get_position2_vec(entity)
        if vec ~= null then
            local speed = 250
            vec.y = vec.y + dt * speed
        end
    end
end)

systems.add_update("time out", function(dt)
    local ents = registry.entities({types.timeout});
    for _, entity in pairs(ents) do
        local data = registry.get(entity, types.timeout)
        data.time = data.time - dt
        if data.time < 0 then
            print("timeout!")
            registry.destroy_entity(entity)
        end
    end
end)

systems.add_update("destroy outside", function (dt)
    local ents = registry.entities({types.sprite, types.pos2, types.destroy_outside})
    for _, entity in pairs(ents) do
        local sp = registry.get_sprite(entity)
        local p = registry.get_position2(entity)
        if sp ~= null then
            local cam = camera.get_rect()
            local r = sp:get_rect(p)
            if not cam:contains(r) then
                registry.destroy_entity(entity)
            end
        end
    end
end)

shot_template = templates.find("shot")
systems.add_update("player", function(dt)
    ents = registry.entities({types.pos2, types.player})
    for _, entity in pairs(ents) do
        local vec = registry.get_position2_vec(entity)
        if vec ~= null then
            local speed = 150

            local vertical = input.up.state - input.down.state
            local horizontal = input.right.state - input.left.state

            vec.y = vec.y + dt * speed * vertical
            vec.x = vec.x + dt * speed * horizontal

            if just_pressed(input.fire) then
                if not shot_template then
                    print("bug: no shot")
                else
                    local shot = shot_template:create()
                    local v = registry.get_position2_vec(shot)
                    if v ~= null then
                        v.x = vec.x
                        v.y = vec.y
                    end
                end
            end
        end
    end
end)
