local interface

local on_enter
local on_input
local on_output

on_enter = function(p)
  player.send(p, "Lua Interpreter\n\n\r")
end

on_input = function(p, arg)
  local f, error = load(arg)

  if error ~= nil then
    player.send(p, "[bred]" .. error .. "[reset]\n\r")
  else
    local result = f()
    player.send(p, "[bgreen]" .. arg .. " -> " .. tostring(result) .. "[reset]\n\r")
  end
end

on_output = function(p)
  player.send(p, "> ")
end

interface = {
  on_enter = on_enter,
  on_output = on_output,
  on_input = on_input
}

return interface