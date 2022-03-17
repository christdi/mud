local TYPE = "moved"

local new

new = function(entity, portal)
  local event = {}

  event.type = TYPE
  event.entity = entity
  event.portal = portal

  return event
end

return {
  TYPE = TYPE,
  new = new
}