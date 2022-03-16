default_narrator = require('narrator/default')

local register

register = function()
  default_narrator.register()
end

return {
  register = register
}