login_state = require('state/login_state')
play_state = require('state/play_state')
lua_state = require('state/lua_state')

local register

register = function()
  login_state.register();
  play_state.register();
  lua_state.register();
end

return {
  register = register
}