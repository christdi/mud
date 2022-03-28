players = require('dist/players')
entities = require ('dist/entities')
systems = require('dist/systems')
commands = require('dist/commands')
actions = require('dist/actions')
events = require('dist/events')
narrators = require('dist/narrators')
archetypes = require('dist/archetypes')
tasks = require('dist/tasks')

function main()
  game.players = {}
  game.config = {}

  log.info("Demo MUD initialising")

  login_state = state(require('dist/state/login_state'))
  play_state = state(require('dist/state/play_state'))
  lua_state = state(require('dist/state/lua_state'))

  description_component = component(require('dist/component/description'))
  inventory_component = component()
  location_component = component()
  room_ref_component = component()
  tag_component = component()
  name_component = component()
  room_component = component()

  description_component.save()

  systems.register()
  archetypes.register()
  narrators.register()

  game.config.default_room = room_entity.new("Valhalla", "Home of the Gods", "This is the place where gods come to celebrate and rejoice with fallen warriors.")
  game.config.second_room = room_entity.new("Hel", "Recluse of the Damned", "This is the place where lost souls come to be forgotten.")
  game.config.third_room = room_entity.new("Mount Celestia", "Home of the Platinum Dragon", "This is the place where the truly good come to live in harmony.")

  portal_entity.new("Portal to Hel", game.config.default_room, game.config.second_room, "a portal to hel", "An icy portal from which a cold wind blows", { "portal", "hel"} )
  portal_entity.new("Portal to Valhalla", game.config.second_room, game.config.default_room, "a portal to valhalla", "A glowing portal which rings with the sound of battle", { "portal", "valhalla"} )

  portal_entity.new("Portal to Mount Celestia", game.config.default_room, game.config.third_room, "a portal to mount celestia", "A glowing portal through which tall mountains are visible", {"portal", "mount", "celestia"})
  portal_entity.new("Portal to Valhalla", game.config.third_room, game.config.default_room, "a portal to valhalla", "A glowing portal which rings with the sound of battle", { "portal", "valhalla"} )
end

function shutdown()
  systems.deregister()
  narrators.deregister()
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

  default_narrator.use(p)
  login_state.switch(p)
end


function player_disconnected(p)
  game.players[p.uuid] = nil
end


function player_input(p, what)
end