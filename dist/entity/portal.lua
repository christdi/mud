local new;

new = function(name, origin, destination, short_description, long_description, tags)
  local portal = game.new_entity(name, "portal");

  location_component.add(portal, {
    room_uuid = origin.uuid
  })

  room_ref_component.add(portal, {
    ref = destination.uuid
  })

  description_component.add(portal, {
    short = short_description,
    long = long_description
  })

  tag_component.add(portal, tags)

  return portal
end

return {
  new = new
}