description_component = require('component/description')
inventory_component = require('component/inventory')
location_component = require('component/location')

register = function()
  description_component.register()
  inventory_component.register()
  location_component.register()
end

return {
  register = register
} 