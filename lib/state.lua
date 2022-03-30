local new

new = function(impl)
  if not impl then error("implementation must be specified") end

  local _impl = impl or {}

  local deregister
  local get_instance

  local _interface
  local _instance

  deregister = function()
    lunac.api.game.deregister_state(_instance)
  end

  get_instance = function()
    return _instance
  end

  _interface = {
    deregister = deregister,
    get_instance = get_instance
  }

  setmetatable(_interface, {
    __index = function(_, v)
      if _impl[v] then
        return _impl[v]
      end

      return rawget(_interface, v)
    end
  })

  _instance = lunac.api.game.register_state(_interface)

  return _interface
end

return {
  new = new
}