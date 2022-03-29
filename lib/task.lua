local new
local all

new = function(name, func)
  local schedule
  local cancel
  
  local name = name or "Unnamed Task"
  local task

  schedule = function(execute_in)
    task = game.schedule_task(name, execute_in, func)

    log.info("Scheduled task [" .. task.name .."]")
  end

  cancel = function()
    game.cancel_task(task)

    log.info("Cancelled task [" .. task.name .. "]")
  end

  local interface = {
    schedule = schedule,
    cancel = cancel
  }

  return interface
end

all = function()
  return game.get_tasks()
end

return {
  new = new,
  all = all
}