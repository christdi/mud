local interface

local on_enter
local on_input
local on_output

on_enter = function(p)
  local player = lunac.player.get(p)

  player.sendln("Lua Interpreter (enter .quit to quit)\n")
end

on_input = function(p, arg)
  local player = lunac.player.get(p)

  if arg == ".quit" then
    player.set_state(game.state.play)
    
    return
  end

  local env = {}  
  setmetatable(env, { __index = _ENV })

  env.p = player

  local f, error = load(arg, 'chunk', 't', env)

  if error ~= nil then
    player.sendln("[bred]" .. error .. "[reset]")
  else
    local result = f()
    player.sendln("[bgreen]" .. arg .. " -> " .. tostring(result) .. "[reset]")
  end
end

on_output = function(p)
  local player = lunac.player.get(p)

  player.send("> ")
end

interface = {
  on_enter = on_enter,
  on_output = on_output,
  on_input = on_input
}

return interface