local register
local use
local interface

local on_enter
local on_exit
local on_input
local on_output
local on_event

local state

-- Register this state
register = function()
   state = game.register_state(interface)
end

-- Switches a player to this state
--
-- p - instance of player userdata
use = function(p)
   player.set_state(p, state)
end

on_enter = function(p)
  player.send(p, "You are now playing!\n\r")
end

on_exit = function(p)
  player.send(p, "You are no longer playing.\n\r")
end

on_input = function(p, arg)
   local cmd, arguments = one_argument(arg)

   if not command.execute(p, cmd, arguments) then
      player.send(p, "Sorry! [bcyan]" .. arg .. "[reset] isn't a known command.\n\r");
   end
end

on_output = function(p, output)
  local entity = player.get_entity(p)

  if name_component.has(entity) then
    local name = name_component.get(entity)

    player.send(p, "\n\n\r[bgreen]" .. name.name .. " >[reset] ")
  end
end

on_event = function(p, event)
  player.narrate(p, event);
end

interface = {
  register = register,
  use = use,
  on_enter = on_enter,
  on_exit = on_exit,
  on_input = on_input,
  on_output = on_output,
  on_event = on_event
}

return interface