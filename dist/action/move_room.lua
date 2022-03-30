game.entity.character.wrap(entity):set_room(data.to)
game.event.moved.dispatch({entity = entity, portal = data.portal})

return true