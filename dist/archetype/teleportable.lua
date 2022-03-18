local register
local entities
local matches

local a

register = function()
  a = game.register_archetype(name_component.component(), location_component.component(), description_component.component())
end

entities = function(filter)
  local entities = game.get_archetype_entities(a)

  if filter ~= nil then
    filter_array(entities, filter)
  end

  return entities
end

matches = function(entity)
  return game.matches_archetype(entity, a)
end

return {
  register = register,
  entities = entities,
  matches = matches
}