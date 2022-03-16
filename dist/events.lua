player_connected_event = require('event/player_connected')
character_looked_event = require('event/character_looked')
communicate_event = require('event/communicate')
moved_event = require('event/moved')
log.info(dump(moved_event))

local dispatch

dispatch = function(e)
  game.event(e)
end

return {
  dispatch = dispatch
}