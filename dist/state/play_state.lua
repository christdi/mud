local interface

local on_enter
local on_exit
local on_input
local on_output
local on_event

on_enter = function(p)
  player.send(p, "You are now playing!\n\r")

  commands.execute("look", p)
end

on_exit = function(p)
  player.send(p, "You are no longer playing.\n\r")
end

on_input = function(p, arg)
   local cmd, arguments = one_argument(arg)

   if not commands.execute(cmd, p, arguments) then
    commands.execute("go", p, cmd)
   end
end

on_output = function(p, output)
  local entity = player.get_entity(p)

  if lunac.component.name.has(entity) then
    local name = lunac.component.name.get(entity)

    player.send(p, "\n\n\r[bgreen]" .. name.name .. " >[reset] ")
  end
end

on_event = function(p, event)
  player.narrate(p, event);
end

interface = {
  on_enter = on_enter,
  on_exit = on_exit,
  on_input = on_input,
  on_output = on_output,
  on_event = on_event
}

return interface