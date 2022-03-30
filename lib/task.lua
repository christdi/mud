local new
local all

new = function(name, func)
  local schedule
  local cancel
  
  local name = name or "Unnamed Task"
  local task

  schedule = function(execute_in)
    task = lunac.api.game.schedule_task(name, execute_in, func)

    lunac.api.log.info("Scheduled task [" .. task.name .."]")
  end

  cancel = function()
    lunac.api.game.cancel_task(task)

    lunac.api.log.info("Cancelled task [" .. task.name .. "]")
  end

  local interface = {
    schedule = schedule,
    cancel = cancel
  }

  return interface
end

all = function()
  return lunac.api.game.get_tasks()
end

return {
  new = new,
  all = all
}