local interface

local on_enter
local on_exit
local on_input
local on_output
local on_event

on_enter = function(p)
  local plr = lunac.player.get(p)

  plr.sendln("You are now playing!")

  plr.execute("look")
end

on_exit = function(p)
  local plr = lunac.player.get(p)

  plr.sendln("You are no longer playing.")
end

on_input = function(p, arg)
  local plr = lunac.player.get(p)

  local cmd, arguments = one_argument(arg)
  local result = plr.execute(cmd, arguments);

  if (result.none == true or result.invalid == true) then
    plr.sendln("You don't know how to do that.")
  elseif (result.multiple == true) then
    plr.sendln("You know " .. result.count .. " ways to do that.  Use \"" .. cmd .. ":x\" to select which one")
  end
end

on_output = function(p, output)
  local plr = lunac.player.get(p)
  local entity = plr.get_entity()

  if game.component.name.has(entity) then
    local name = game.component.name.get(entity)

    plr.send("[bgreen]" .. name.name .. " >[reset] ")
  end
end

on_event = function(p, event)
  local plr = lunac.player.get(p)
  plr.narrate(event)
end

interface = {
  on_enter = on_enter,
  on_exit = on_exit,
  on_input = on_input,
  on_output = on_output,
  on_event = on_event
}

return interface