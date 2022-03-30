local define
local get
local all

define = function(impl, components)
  local impl = impl or {}
  local components = components or {}

  local wrap
  local new
  local get

  wrap = function(entity)
    setmetatable(entity, {
      __index = function(_, v)  
        for key, component in pairs(components) do
          if key == v then
            return component.get(entity)
          end
        end

        return impl[v]
      end
    })

    return entity
  end

  new = function()
    local entity = lunac.api.game.new_entity()

    for _, component in pairs(components) do
      component.add(entity, {})
    end

    return wrap(entity)
  end

  get = function(uuid)
    local entity = lunac.api.game.get_entity(uuid)

    return wrap(entity)
  end

  return {
    wrap = wrap,
    new = new,
    get = get
  }
end

get = function(uuid)
  if not uuid then error("uuid must be specified") end

  return lunac.api.game.get_entity(uuid)
end

all = function()
  return lunac.api.game.get_entities()
end

return {
  define = define,
  get = get,
  all = all
}
