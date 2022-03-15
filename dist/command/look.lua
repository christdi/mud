local character = player.get_entity(p)
local room = character_module.get_room(character)

player.send(p, "You look around and see...\n\r")
player.send(p, room.name .. "\n\r");

event_module.character_looked(character)