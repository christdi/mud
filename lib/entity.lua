local define

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
    local entity = game.new_entity()

    for _, component in pairs(components) do
      component.add(entity, {})
    end

    return wrap(entity)
  end

  get = function(uuid)
    local entity = game.get_entity(uuid)

    return wrap(entity)
  end

  return {
    wrap = wrap,
    new = new,
    get = get
  }
end

return {
  define = define
}
