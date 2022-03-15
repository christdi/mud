local new;

new = function(name)
  local room = game.new_entity(name, "room");

  game.add_component(room, game.components.inventory, {})

  return room
end

return {
  new = new
}