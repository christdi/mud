local location = location_component.get(data.entity)
location.room_uuid = data.to.uuid

events.dispatch(teleport_event.new(data.entity, data.from, data.to))

return true