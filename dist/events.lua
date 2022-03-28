character_looked_event = require('dist/event/character_looked')
communicate_event = require('dist/event/communicate')
moved_event = require('dist/event/moved')
teleport_event = require('dist/event/teleport')

local dispatch

dispatch = function(e)
  game.event(e)
end

return {
  dispatch = dispatch
}