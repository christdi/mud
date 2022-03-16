local register
local entities
local matches

local a

register = function()
  a = game.register_archetype(location_component.component(), description_component.component())
end

entities = function(filter)
  local entities = game.get_archetype_entities(a)

  if filter ~= nil then
    local keys = {}

    for k, v in ipairs(entities) do
      if filter(v) == false then
        table.insert(keys, k);
      end
    end

    for _, v in ipairs(keys) do
      table.remove(entities, v)
    end
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