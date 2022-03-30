commands = require('dist/commands')
actions = require('dist/actions')

game = {
  state = {},
  system = {},
  component = {},
  archetype = {},
  event = {},
  narrator = {},
  entity = {},
  config = {}
}

function main()
  lunac.api.log.info("Demo MUD initialising")

  game.state.login = lunac.state.new(require('dist/state/login_state'))
  game.state.play = lunac.state.new(require('dist/state/play_state'))
  game.state.lua = lunac.state.new(require('dist/state/lua_state'))

  game.system.random_tp = lunac.system.new("Random Teleport", require('dist/system/random_teleport'))
  game.system.random_tp.disable()

  game.component.description = lunac.component.new(require('dist/component/description'))
  game.component.inventory = lunac.component.new(require('dist/component/inventory'))
  game.component.location = lunac.component.new(require('dist/component/location'))
  game.component.room_ref = lunac.component.new(require('dist/component/room_ref'))
  game.component.tag = lunac.component.new(require('dist/component/tag'))
  game.component.name = lunac.component.new(require('dist/component/name'))
  game.component.room = lunac.component.new(require('dist/component/room'))

  game.archetype.goable = lunac.archetype.define('goable', game.component.location, game.component.room_ref, game.component.tag)
  game.archetype.observable = lunac.archetype.define('observable', game.component.location, game.component.description)
  game.archetype.teleportable = lunac.archetype.define('teleportable', game.component.name, game.component.location, game.component.description)

  game.event.character_looked = lunac.event.define('character_looked')
  game.event.moved = lunac.event.define('moved')
  game.event.communicate = lunac.event.define('communicate', require('dist/event/communicate'))
  game.event.teleport = lunac.event.define('teleport')

  game.narrator.standard = lunac.narrator.define('standard')
  game.narrator.standard.on(game.event.communicate, require('dist/narrator/standard/communicate'))
  game.narrator.standard.on(game.event.character_looked, require('dist/narrator/standard/character_looked'))
  game.narrator.standard.on(game.event.moved, require('dist/narrator/standard/moved'))
  game.narrator.standard.on(game.event.teleport, require('dist/narrator/standard/teleport'))

  game.entity.character = lunac.entity.define(require('dist/entity/character'), { name = game.component.name, location = game.component.location, inventory = game.component.inventory, description = game.component.description })
  game.entity.room = lunac.entity.define(require('dist/entity/room'), { room = game.component.room, inventory = game.component.inventory, description = game.component.description} )
  game.entity.portal = lunac.entity.define(require('dist/entity/portal'), { location = game.component.location, room_ref = game.component.room_ref, description = game.component.description, tag = game.component.tag })

  game.config.default_room = game.entity.room.new()
  game.config.default_room:initialise("Home of the gods", "This is the place where gods come come to celebrate and rejoice with fallen warriors.")

  game.config.second_room = game.entity.room.new()
  game.config.second_room:initialise("Recluse of the Damned", "This is the place where lost souls come to be forgotten.")

  game.config.third_room = game.entity.room.new()
  game.config.third_room:initialise("Home of the Platinum Dragon", "This is the place where the truly good come to live in harmony.")

  game.entity.portal.new():initialise("Portal to Hel", game.config.default_room, game.config.second_room, "a portal to hel", "An icy portal from which a cold wind blows", { "portal", "hel" })
  game.entity.portal.new():initialise("Portal to Valhalla", game.config.second_room, game.config.default_room, "a portal to valhalla", "A glowing portal which rings with the sound of battle", { "portal", "valhalla" } )
  game.entity.portal.new():initialise("Portal to Mount Celestia", game.config.default_room, game.config.third_room, "a portal to mount celestia", "A glowing portal through which tall mountains are visible", {"portal", "mount", "celestia" })
  game.entity.portal.new():initialise("Portal to Valhalla", game.config.third_room, game.config.default_room, "a portal to valhalla", "A glowing portal which rings with the sound of battle", { "portal", "valhalla" } )
end

function shutdown()
  game.system.random_tp.deregister();
  game.narrator.standard.deregister();
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
  local plr = lunac.player.new(p)

  plr.set_state(game.state.login)
  plr.set_narrator(game.narrator.standard)
end


function player_disconnected(p)
  lunac.player.remove(p)
end


function player_input(p, what)
end