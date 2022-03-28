login_state = require('state/login_state')
play_state = require('state/play_state')
lua_state = require('state/lua_state')

local register
local deregister

register = function()
  login_state.register()
  play_state.register()
  lua_state.register()
end

deregister = function()
  login_state.deregister()
  play_state.deregister()
  lua_state.deregister()
end

return {
  register = register,
  deregister = deregister
}