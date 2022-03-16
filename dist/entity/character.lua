local new;
local set_room;

new = function(name, short_description, long_description)
  local character = game.new_entity();

  name_component.add(character, {
    name = name
  })

  location_component.add(character, {})

  inventory_component.add(character, {})

  description_component.add(character, {
    short = short_description,
    long = long_description
  })

  return character
end

set_room = function(character, room)
  if not location_component.has(character) then
    log.error("Attempt to set room for character [" .. character.uuid .. "] failed as they do not have location component")

    return
  end

  local location = location_component.get(character)

  location.room_uuid = room.uuid
end

get_room = function(character)
  if not location_component.has(character) then
    log.error("Attempt to get room for character [" .. character.uuid .. "] failed as they do not have location component")

    return
  end

  local location = location_component.get(character);

  return game.get_entity(location.room_uuid)
end

return {
  new = new,
  set_room = set_room,
  get_room = get_room
}