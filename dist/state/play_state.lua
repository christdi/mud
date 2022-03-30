local interface

local on_enter
local on_exit
local on_input
local on_output
local on_event

on_enter = function(p)
  local plr = lunac.player.get(p)

  plr.send("You are now playing!\n\r")

  commands.execute("look", p)
end

on_exit = function(p)
  local plr = lunac.player.get(p)

  plr.send("You are no longer playing.\n\r")
end

on_input = function(p, arg)
   local cmd, arguments = one_argument(arg)

   if not commands.execute(cmd, p, arguments) then
    commands.execute("go", p, cmd)
   end
end

on_output = function(p, output)
  local plr = lunac.player.get(p)
  local entity = plr.get_entity()

  if lunac.component.name.has(entity) then
    local name = lunac.component.name.get(entity)

    plr.send("\n\n\r[bgreen]" .. name.name .. " >[reset] ")
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