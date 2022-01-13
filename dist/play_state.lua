function on_enter(p)
  player.send(p, "You are now playing!\n\n\r")
end

function on_exit(p)
  player.send(p, "You are no longer playing.\n\r")
end

function on_input(p, arg)
   if arg:lower() == "quit" then
      player.disconnect(p);
   elseif arg:lower() == "shutdown" then
      game.shutdown();
   else
      player.send(p, "Sorry! [bcyan]" .. arg .. "[reset] isn't a known command.\n\r");
   end
end

function on_tick(p)
end

