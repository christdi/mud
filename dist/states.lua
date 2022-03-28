login_state = require('dist/state/login_state')
play_state = require('dist/state/play_state')
lua_state = require('dist/state/lua_state')

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