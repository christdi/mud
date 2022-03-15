login_state_module = require('state/login_state')
play_state_module = require('state/play_state')
lua_state_module = require('state/lua_state')
character_module = require ('entity/character')
room_module = require('entity/room')
event_module = require('event')
narrator_module = require('narrator')

function main()
  game.players = {}
  game.components = {}
  game.archetypes = {}
  game.narrators = {}
  game.states = {}

  log.info("Demo MUD initialising")

  game.components.location = game.register_component()
  game.components.inventory = game.register_component()

  game.archetypes.room = game.register_archetype(game.components.location, game.components.inventory)

  game.narrators.default = game.register_narrator(narrator_module)

  game.states.login = game.register_state(login_state_module)
  game.states.play = game.register_state(play_state_module)
  game.states.lua = game.register_state(lua_state_module)
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