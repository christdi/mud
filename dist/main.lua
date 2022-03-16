entities = require ('entities')
events = require('events')
states = require('states')
narrators = require('narrators')
components = require('components')
archetypes = require('archetypes')

function main()
  game.players = {}
  game.config = {}

  log.info("Demo MUD initialising")

  states.register()
  components.register()
  archetypes.register()
  narrators.register()

  game.config.default_room = room_entity.new("Valhalla", "Home of the Gods", "This is the place where gods come to celebrate and rejoice with fallen warriors.")
end

function entities_loaded(entities)
end


function player_connected(p)
  game.players[p.uuid] = {}

  default_narrator.use(p)
  login_state.use(p)
end


function player_disconnected(p)
  game.players[p.uuid] = nil
end


function player_input(p, what)
end