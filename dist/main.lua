function main()
  game.components = {}

  log.info("Demo MUD initialising")

  game.components.location = game.register_component();
  game.components.has_inventory = game.register_component()
end

function entities_loaded(entities)
end


function player_connected(p)
end


function player_disconnected(p)
end


function player_input(p, what)
end