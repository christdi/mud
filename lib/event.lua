character_looked_event = require('dist/event/character_looked')
communicate_event = require('dist/event/communicate')
moved_event = require('dist/event/moved')
teleport_event = require('dist/event/teleport')

local dispatch
local define

define = function(impl)
  impl = impl or {}

  
end

dispatch = function(e)
  game.event(e)
end

return {
  dispatch = dispatch
}