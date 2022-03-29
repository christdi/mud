local new

new = function(name, impl)
  local deregister
  local enable
  local disable
  local execute

  local name = name or "anonymous"
  local impl = impl or {}

  local system

  deregister = function()
    log.info("Deregistering " .. name .. " system")

    game.deregister_system(system)
  end

  enable = function()
    log.info("Enabling " .. name .. " system")

    system = game.enable_system(system)
  end

  disable = function()
    log.info("Disabling " .. name .. " system")

    system = game.disable_system(system)
  end

  local interface = {
    deregister = deregister,
    enable = enable,
    disable = disable
  }

  setmetatable(interface, {
    __index = function(_, v)

      return impl[v]
    end
  })

  log.info("Registering " .. name .. " system")
  system = game.register_system(name, interface)
  
  return interface
end

return {
  new = new
}
