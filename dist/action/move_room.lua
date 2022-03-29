lunac.entity.character.wrap(entity):set_room(data.to)
lunac.event.moved.dispatch({entity = entity, portal = data.portal})

return true