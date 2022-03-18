entities = require ('entities')
components = require('components')
systems = require('systems')
commands = require('commands')
actions = require('actions')
events = require('events')
states = require('states')
narrators = require('narrators')
archetypes = require('archetypes')

function main()
  game.players = {}
  game.config = {}

  log.info("Demo MUD initialising")

  states.register()
  components.register()
  systems.register()
  archetypes.register()
  narrators.register()

  game.config.default_room = room_entity.new("Valhalla", "Home of the Gods", "This is the place where gods come to celebrate and rejoice with fallen warriors.")
  game.config.second_room = room_entity.new("Hel", "Recluse of the Damned", "This is the place where lost souls come to be forgotten.")

  portal_entity.new("Portal to Hel", game.config.default_room, game.config.second_room, "a portal to hel", "An icy portal from which a cold wind blows", { "portal", "hel"} )
  portal_entity.new("Portal to Valhalla", game.config.second_room, game.config.default_room, "a portal to valhalla", "A glowing portal which rings with the sound of battle", { "portal", "valhalla"} )
end

function shutdown()
  systems.deregister()
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
  login_state.use(p)
end


function player_disconnected(p)
  game.players[p.uuid] = nil
end


function player_input(p, what)
end