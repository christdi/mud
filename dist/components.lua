description_component = require('component/description')
inventory_component = require('component/inventory')
location_component = require('component/location')
room_ref_component = require('component/room_ref')
tag_component = require('component/tag')

register = function()
  description_component.register()
  inventory_component.register()
  location_component.register()
  room_ref_component.register()
  tag_component.register()
end

return {
  register = register
} 