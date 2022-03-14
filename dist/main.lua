login_state_module = require('login_state')
play_state_module = require('play_state')
lua_state_module = require('lua_state')
narrator_module = require('narrator')
event_module = require('event')

function main()
  game.players = {}
  game.components = {}
  game.narrators = {}
  game.states = {}

  log.info("Demo MUD initialising")

  game.components.location = game.register_component();
  game.components.has_inventory = game.register_component()

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