lunac.player.sendln_all("\n\rShutdown scheduled.  Game will shut down in 10 seconds\n\r");

lunac.task.new("shutdown", function() lunac.api.game.shutdown() end).schedule(10)