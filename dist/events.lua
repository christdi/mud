character_looked_event = require('event/character_looked')
communicate_event = require('event/communicate')
moved_event = require('event/moved')

local dispatch

dispatch = function(e)
  game.event(e)
end

return {
  dispatch = dispatch
}