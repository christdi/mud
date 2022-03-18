local schedule
local cancel
local tasks

schedule = function(name, execute_in, func)
  local task = game.schedule_task(name, execute_in, func);

  log.info("Scheduled task [" .. task.name .."]")
end

cancel = function(task)
  game.cancel_task(task);

  log.info("Cancelled task [" .. task.name .. "]")
end

tasks = function()
  return game.get_tasks()
end

return {
  schedule = schedule,
  cancel = cancel,
  tasks = tasks
}