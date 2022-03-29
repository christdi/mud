local initialise;
local set_room;

initialise = function(self, name, short_description, long_description)
  self.name.name = name
  self.description.short = short_description
  self.description.long = long_description

  return self
end

set_room = function(character, room)
  local room_component = character.location;
  room_component.room_uuid = room.uuid
end

get_room = function(character)
  return lunac.entity.room.wrap(game.get_entity(character.location.room_uuid))
end

return {
  initialise = initialise,
  set_room = set_room,
  get_room = get_room
}