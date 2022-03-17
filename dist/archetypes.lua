observable_archetype = require('archetype/observable')
goable_archetype = require('archetype/goable')

local register

register = function()
  observable_archetype.register()
  goable_archetype.register()
end

return {
  register = register
}