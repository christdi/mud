local archetypes = {}

local register
local get_entities_can_describe_in_room

register = function()
  archetypes.can_describe_in_room = game.register_archetype(components_module.components.location, components_module.components.description)
end

get_entities_can_describe_in_room = function()
  return game.get_archetype_entities(archetypes.can_describe_in_room)
end

return {
  register = register,
  get_entities_can_describe_in_room = get_entities_can_describe_in_room
}