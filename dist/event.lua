local player_connected
local character_looked

local type = {
  PLAYER_CONNECTED = 1,
  CHARACTER_LOOKED = 2
}

player_connected = function()
  local event = {}

  event.type = type.PLAYER_CONNECTED

  game.event(event)
end

character_looked = function(character)
  local event = {}

  event.type = type.CHARACTER_LOOKED
  event.character = character

  game.event(event)
end

return {
  player_connected = player_connected,
  character_looked = character_looked,
  type = type
}