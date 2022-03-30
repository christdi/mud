lunac.player.send_all("\n\rShutdown scheduled.  Game will shut down in 10 seconds\n\r");

lunac.task.new("shutdown", function() game.shutdown() end).schedule(10)