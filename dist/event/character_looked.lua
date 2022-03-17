local TYPE = "character_looked"

local new

new = function(character)
  local event = {}

  event.type = TYPE
  event.character = character

  return event
end

return {
  TYPE = TYPE,
  new = new
}