local _systems = {}

local new
local all

new = function(name, impl)
  if not name or type(name) ~= "string" then error("system name must be specified") end
  
  local deregister
  local enable
  local disable
  local execute
  local get_name
  local get_uuid
  local is_enabled

  local _name = name
  local _impl = impl or {}
  local _system

  deregister = function()
    lunac.api.log.info("Deregistering " .. _name .. " system")

    lunac.api.game.deregister_system(_system)

    for i, system in ipairs(_systems) do
      if system == _system then
        table.remove(_systems, i)
        break
      end
    end

    _system = nil
  end

  enable = function()
    lunac.api.log.info("Enabling " .. _name .. " system")

    _system = lunac.api.game.enable_system(_system)
  end

  disable = function()
    lunac.api.log.info("Disabling " .. _name .. " system")

    _system = lunac.api.game.disable_system(_system)
  end

  get_name = function()
    return _name
  end

  get_uuid = function()
    return _system.uuid
  end

  is_enabled = function()
    return _system.enabled
  end

  local _interface = {
    deregister = deregister,
    enable = enable,
    disable = disable,
    get_name = get_name,
    get_uuid = get_uuid,
    is_enabled = is_enabled
  }

  setmetatable(_interface, {
    __index = function(_, v)
      if _impl[v] then
        return _impl[v]
      end

      return rawget(_interface, v)
    end
  })

  lunac.api.log.info("Registering " .. _name .. " system")
  _system = lunac.api.game.register_system(name, _interface)
  table.insert(_systems, _interface)

  return _interface
end

all = function()
  return _systems
end

return {
  new = new,
  all = all,
}
