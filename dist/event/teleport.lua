local TYPE = "teleport"

local new

new = function(entity, from, to)
  local event = {}

  event.type = TYPE
  event.entity = entity
  event.from = from
  event.to = to

  return event
end

return {
  TYPE = TYPE,
  new = new
}