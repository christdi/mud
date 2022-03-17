character_entity.set_room(entity, data.to)

events.dispatch(moved_event.new(entity, data.portal))

return true