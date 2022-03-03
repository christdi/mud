local type = {
  PLAYER_CONNECTED = 1
}

local function player_connected()
  local event = {}

  event.type = type.PLAYER_CONNECTED

  game.event(event)
end

return {
  player_connected = player_connected,
  type = type
}