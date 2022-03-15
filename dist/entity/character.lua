local new;
local set_room;

new = function(name)
  local character = game.new_entity(name, "character");

  game.add_component(character, game.components.location, {})
  game.add_component(character, game.components.inventory, {})

  return character
end

set_room = function(character, room)
  if not game.has_component(character, game.components.location) then
    log.error("Attempt to set room for character [" .. character.name .. "] failed as they do not have location component")

    return
  end

  local location = game.get_component(character, game.components.location)

  location.room_uuid = room.uuid

end

get_room = function(character)
  if not game.has_component(character, game.components.location) then
    log.error("Attempt to get room for character [" .. character.name .. "] failed as they do not have location component")

    return
  end

  local location = game.get_component(character, game.components.location);

  return game.get_entity(location.room_uuid)
end

return {
  new = new,
  set_room = set_room,
  get_room = get_room
}