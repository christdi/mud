local initialise
local execute

local _commands

initialise = function(commands)
  _commands = commands 
end

execute = function(name, player, arguments)
  for _, v in ipairs(_commands) do
    if v.name:lower() == name:lower() then
      game.do_command(player, v, arguments or "")

      return true
    end
  end

  return false
end

return {
  initialise = initialise,
  execute = execute
}