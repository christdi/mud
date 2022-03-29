local location = lunac.component.location.get(data.entity)

location.room_uuid = data.to.uuid

lunac.event.teleport.dispatch({
    entity = data.entity, 
    from = data.from, 
    to = data.to
})

return true