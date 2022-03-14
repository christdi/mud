function on_enter(p)
  player.send(p, "Lua Interpreter\n\n\r")
end

function on_input(p, arg)
  local f, error = load(arg)

  if error ~= nil then
    player.send(p, "[bred]" .. error .. "[reset]\n\r")
  else
    local result = f()
    player.send(p, "[bgreen]" .. arg .. " -> " .. tostring(result) .. "[reset]\n\r")
  end
end

function on_output(p)
  player.send(p, "> ")
end

return {
  on_enter = on_enter,
  on_output = on_output,
  on_input = on_input
}