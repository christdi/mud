local TYPE = "communicate"

local scope = {
  ROOM = 1
}

local new

new = function(origin, what, scope)
  local event = {}

  event.type = TYPE
  event.origin = origin
  event.what = what
  event.scope = scope

  return event
end

return {
  TYPE = TYPE,
  scope = scope,
  new = new
}