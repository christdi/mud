observable_archetype = require('dist/archetype/observable')
goable_archetype = require('dist/archetype/goable')
teleportable_archetype = require('dist/archetype/teleportable')

local register

register = function()
  observable_archetype.register()
  goable_archetype.register()
  teleportable_archetype.register()
end

return {
  register = register
}