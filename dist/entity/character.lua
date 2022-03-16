local new;
local set_room;

new = function(name, short_description, long_description)
  local character = game.new_entity(name, "character");

  components_module.add_location(character, {})
  components_module.add_inventory(character, {})
  components_module.add_description(character, {
    short = short_description,
    long = long_description
  })

  return character
end

set_room = function(character, room)
  if not components_module.has_location(character) then
    log.error("Attempt to set room for character [" .. character.name .. "] failed as they do not have location component")

    return
  end

  local location = components_module.get_location(character)

  location.room_uuid = room.uuid
end

get_room = function(character)
  if not components_module.has_location(character) then
    log.error("Attempt to get room for character [" .. character.name .. "] failed as they do not have location component")

    return
  end

  local location = components_module.get_location(character);

  return game.get_entity(location.room_uuid)
end

return {
  new = new,
  set_room = set_room,
  get_room = get_room
}