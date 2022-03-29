players = require('dist/players')
commands = require('dist/commands')
actions = require('dist/actions')

function main()
  game.players = {}
  game.config = {}

  log.info("Demo MUD initialising")

  lunac.state.login_state = lunac.state.new(require('dist/state/login_state'))
  lunac.state.play_state = lunac.state.new(require('dist/state/play_state'))
  lunac.state.lua_state = lunac.state.new(require('dist/state/lua_state'))

  lunac.system.random_tp_system = lunac.system.new("Random Teleport", require('dist/system/random_teleport'))
  lunac.system.random_tp_system.disable()

  lunac.component.description = lunac.component.new(require('dist/component/description'))
  lunac.component.inventory = lunac.component.new(require('dist/component/inventory'))
  lunac.component.location = lunac.component.new(require('dist/component/location'))
  lunac.component.room_ref = lunac.component.new(require('dist/component/room_ref'))
  lunac.component.tag = lunac.component.new(require('dist/component/tag'))
  lunac.component.name = lunac.component.new(require('dist/component/name'))
  lunac.component.room = lunac.component.new(require('dist/component/room'))

  lunac.archetype.goable = lunac.archetype.define('goable', lunac.component.location, lunac.component.room_ref, lunac.component.tag)
  lunac.archetype.observable = lunac.archetype.define('observable', lunac.component.location, lunac.component.description)
  lunac.archetype.teleportable = lunac.archetype.define('teleportable', lunac.component.name, lunac.component.location, lunac.component.description)

  lunac.event.character_looked = lunac.event.define('character_looked')
  lunac.event.moved = lunac.event.define('moved')
  lunac.event.communicate = lunac.event.define('communicate', require('dist/event/communicate'))
  lunac.event.teleport = lunac.event.define('teleport')

  lunac.narrator.standard = lunac.narrator.define('standard')
  lunac.narrator.standard.on(lunac.event.communicate, require('dist/narrator/standard/communicate'))
  lunac.narrator.standard.on(lunac.event.character_looked, require('dist/narrator/standard/character_looked'))
  lunac.narrator.standard.on(lunac.event.moved, require('dist/narrator/standard/moved'))
  lunac.narrator.standard.on(lunac.event.teleport, require('dist/narrator/standard/teleport'))

  lunac.entity.character = lunac.entity.define(require('dist/entity/character'), { name = lunac.component.name, location = lunac.component.location, inventory = lunac.component.inventory, description = lunac.component.description })
  lunac.entity.room = lunac.entity.define(require('dist/entity/room'), { room = lunac.component.room, inventory = lunac.component.inventory, description = lunac.component.description} )
  lunac.entity.portal = lunac.entity.define(require('dist/entity/portal'), { location = lunac.component.location, room_ref = lunac.component.room_ref, description = lunac.component.description, tag = lunac.component.tag })

  game.config.default_room = lunac.entity.room.new()
  game.config.default_room:initialise("Home of the gods", "This is the place where gods come come to celebrate and rejoice with fallen warriors.")

  game.config.second_room = lunac.entity.room.new()
  game.config.second_room:initialise("Recluse of the Damned", "This is the place where lost souls come to be forgotten.")

  game.config.third_room = lunac.entity.room.new()
  game.config.third_room:initialise("Home of the Platinum Dragon", "This is the place where the truly good come to live in harmony.")

  lunac.entity.portal.new():initialise("Portal to Hel", game.config.default_room, game.config.second_room, "a portal to hel", "An icy portal from which a cold wind blows", { "portal", "hel" })
  lunac.entity.portal.new():initialise("Portal to Valhalla", game.config.second_room, game.config.default_room, "a portal to valhalla", "A glowing portal which rings with the sound of battle", { "portal", "valhalla" } )
  lunac.entity.portal.new():initialise("Portal to Mount Celestia", game.config.default_room, game.config.third_room, "a portal to mount celestia", "A glowing portal through which tall mountains are visible", {"portal", "mount", "celestia" })
  lunac.entity.portal.new():initialise("Portal to Valhalla", game.config.third_room, game.config.default_room, "a portal to valhalla", "A glowing portal which rings with the sound of battle", { "portal", "valhalla" } )
end

function shutdown()
  lunac.system.random_tp_system.deregister();
  lunac.narrator.standard.deregister();
end

function entities_loaded(entities)
end

function commands_loaded(c)
  commands.initialise(c)
end

function actions_loaded(a)
  actions.initialise(a)
end


function player_connected(p)
  game.players[p.uuid] = p

  lunac.narrator.standard.use(p)
  lunac.state.login_state.switch(p)
end


function player_disconnected(p)
  game.players[p.uuid] = nil
end


function player_input(p, what)
end