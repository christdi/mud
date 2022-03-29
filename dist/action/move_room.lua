lunac.entity.character.wrap(entity):set_room(data.to)
events.dispatch(moved_event.new(entity, data.portal))

return true