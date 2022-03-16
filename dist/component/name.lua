local register
local add
local get
local has
local entities
local component

local c

register = function()
  c = game.register_component()
end

add = function(entity, data)
  data.entity = entity.uuid;

  game.add_component(entity, c, data)
end

get = function(entity)
  return game.get_component(entity, c)
end

has = function(entity)
  return game.has_component(entity, c)
end

entities = function(filter)
  local entities = game.get_component_entities(c)

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

component = function()
  return c
end

return {
  register = register,
  add = add,
  get = get,
  has = has,
  entities = entities,
  component = component
}