local register
local deregister
local use
local interface

local narrate

local narrator

local moved_handler = require('dist/narrator/default/moved')
local communicate_handler = require('dist/narrator/default/communicate')
local character_looked_handler = require('dist/narrator/default/communicate')
local teleport_handler = require('dist/narrator/default/teleport')

register = function()
  narrator = game.register_narrator(interface)
end

deregister = function()
  game.deregister_narrator(narrator)
end

use = function(p)
  player.set_narrator(p, narrator)
end

narrate = function(plr, event)
  local handlers = {
    { event = character_looked_event.TYPE, handler = character_looked_handler },
    { event = communicate_event.TYPE, handler = communicate_handler },
    { event = moved_event.TYPE, handler = moved_handler },
    { event = teleport_event.TYPE, handler = teleport_handler }
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
  deregister = deregister,
  use = use,
  narrate = narrate
}

return interface