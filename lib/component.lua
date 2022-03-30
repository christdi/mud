local new

new = function(extension)
  extension = extension or {}

  local register
  local add
  local get
  local has
  local entities
  local component
  local index;

  local c

  add = function(entity, data)
    data.entity = entity.uuid;

    lunac.api.game.add_component(entity, c, data)
  end

  get = function(entity)
    return lunac.api.game.get_component(entity, c)
  end

  has = function(entity)
    return lunac.api.game.has_component(entity, c)
  end

  entities = function(filter)
    local entities = lunac.api.game.get_component_entities(c)

    if filter ~= nil then
      filter_array(entities, filter)
    end

    return entities
  end

  component = function()
    return c
  end

  index = function()
  end

  c = lunac.api.game.register_component();

  local interface = {
    add = add,
    get = get,
    has = has,
    entities = entities,
    component = component
  }

  setmetatable(interface, {
    __index = function(_, v)
      return extension[v]
    end
  })

  return interface
end

return {
  new = new
};
