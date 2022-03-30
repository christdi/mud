local interface

local on_enter
local on_input
local on_output

on_enter = function(p)
  local player = lunac.player.get(p)
  player.send("Lua Interpreter\n\n\r")
end

on_input = function(p, arg)
  local player = lunac.player.get(p)
  local f, error = load(arg)

  if error ~= nil then
    player.send("[bred]" .. error .. "[reset]\n\r")
  else
    local result = f()
    player.send("[bgreen]" .. arg .. " -> " .. tostring(result) .. "[reset]\n\r")
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