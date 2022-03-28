local state
local switch

state = function(impl)
  local impl = impl or {}

  local deregister
  local switch

  local interface
  local instance

  deregister = function()
    game.deregister_state(instance)
  end

  switch = function(p)
    player.set_state(p, instance)
  end

  interface = {
    deregister = deregister,
    switch = switch
  }

  setmetatable(interface, {
    __index = function(_, v)
      return impl[v]
    end
  })

  instance = game.register_state(interface)

  return interface
end

return state