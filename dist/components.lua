local component

component = function()
  local register
  local add
  local get
  local has
  local entities
  local component

  local c

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
      filter_array(entities, filter)
    end

    return entities
  end

  component = function()
    return c
  end

  c = game.register_component();

  return {
    add = add,
    get = get,
    has = has,
    entities = entities,
    component = component
  }
end

return component;
