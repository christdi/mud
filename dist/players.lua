local send
local send_all

send = function(p, msg)
  player.send(p, msg)
end

send_all = function(msg)
  for _, v in pairs(game.players) do
    player.send(v, msg)
  end
end

return {
  send = send,
  send_all = send_all
}