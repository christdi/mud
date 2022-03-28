default_narrator = require('narrator/default')

local register
local deregister

register = function()
  default_narrator.register()
end

deregister = function()
  default_narrator.deregister()
end

return {
  register = register,
  deregister = deregister
}