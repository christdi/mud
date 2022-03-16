local register
local use
local interface

local narrate

local narrator

local moved_handler = require('narrator/default/moved')
local communicate_handler = require('narrator/default/communicate')
local character_looked_handler = require('narrator/default/communicate')

register = function()
  narrator = game.register_narrator(interface)
end

use = function(p)
  player.set_narrator(p, narrator)
end

narrate = function(plr, event)
  local handlers = {
    { event = character_looked_event.TYPE, handler = character_looked_handler },
    { event = communicate_event.TYPE, handler = communicate_handler },
    { event = moved_event.TYPE, handler = moved_handler }
  }

  for _, v in ipairs(handlers) do
    if v.event == event.type then
      v.handler(plr, event)

      return
    end
  end
end

interface = {
  register = register,
  use = use,
  narrate = narrate
}

return interface