random_teleport_system = require('system/random_teleport')

local register
local deregister
local systems

register = function()
  random_teleport_system.register()
end

deregister = function()
  random_teleport_system.deregister()
end

systems = function()
  return { random_teleport_system }
end

return {
  register = register,
  deregister = deregister,
  systems = systems
}