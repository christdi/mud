observable_archetype = require('archetype/observable')
goable_archetype = require('archetype/goable')
teleportable_archetype = require('archetype/teleportable')

local register

register = function()
  observable_archetype.register()
  goable_archetype.register()
  teleportable_archetype.register()
end

return {
  register = register
}