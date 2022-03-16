login_state_module = require('state/login_state')
play_state_module = require('state/play_state')
lua_state_module = require('state/lua_state')
character_module = require ('entity/character')
room_module = require('entity/room')
event_module = require('event')
narrator_module = require('narrator')
components_module = require('components')
archetypes_module = require('archetypes')

function main()
  game.players = {}
  game.narrators = {}
  game.states = {}
  game.config = {}

  log.info("Demo MUD initialising")

  components_module.register()
  archetypes_module.register()

  game.narrators.default = game.register_narrator(narrator_module)

  game.states.login = game.register_state(login_state_module)
  game.states.play = game.register_state(play_state_module)
  game.states.lua = game.register_state(lua_state_module)

  game.config.default_room = room_module.new("Valhalla", "Home of the Gods", "This is the place where gods come to celebrate and rejoice with fallen warriors.")
end

function entities_loaded(entities)
end


function player_connected(p)
  game.players[p.uuid] = {}

  player.set_narrator(p, game.narrators.default)
  player.set_state(p, game.states.login)
end


function player_disconnected(p)
  game.players[p.uuid] = nil
end


function player_input(p, what)
end