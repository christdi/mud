local TYPE = "player_connected"

local new

new = function(player)
  local event = {}

  event.type = TYPE
  event.player = player

  return event
end

return {
  TYPE = TYPE,
  new = new
}