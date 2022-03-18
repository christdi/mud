players.send_all("\n\rShutdown scheduled.  Game will shut down in 60 seconds\n\r");

tasks.schedule("shutdown warning", 30, function()
  players.send_all("\n\rShutdown scheduled.  Game will shut down in 30 seconds\n\r");
end)

tasks.schedule("shutdown", 60, function()
  game.shutdown();
end)