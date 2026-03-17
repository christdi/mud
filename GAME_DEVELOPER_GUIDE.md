# Game Developer Guide

This guide covers everything needed to write a game on top of the MUD engine, based solely on what the C engine exposes to Lua. No additional Lua libraries are assumed.

---

## How the engine works

The engine manages TCP networking, a game loop, an Entity-Component-System (ECS), player sessions, command dispatch, and SQLite persistence. Your game is written entirely in Lua.

The engine calls into your Lua code at defined points via **hooks** — functions you register using `lunac.api.hooks`. You call back into the engine via the **`lunac.api`** table the engine injects into your Lua state.

### Game loop order

Each tick the engine runs these phases in sequence:

1. **Poll network** — accept new connections, read pending input from clients
2. **Dispatch events** — route queued events to players via narrators and states
3. **Execute tasks** — run any scheduled tasks whose time has elapsed
4. **Update systems** — call `execute()` on all enabled ECS systems
5. **Flush output** — send buffered output to all connected clients
6. **Sleep** — wait out the remainder of the tick

---

## Configuration

Create `config.lua` in the working directory you run the binary from:

```lua
game_script      = "src/main.lua"   -- entry point for your game (required)
lib_script       = "lib/main.lua"   -- path to any Lua library loaded before your script
game_port        = 5000             -- TCP port to listen on
database_file    = "game.db"        -- SQLite database path
ticks_per_second = 5                -- game loop rate
```

The engine loads `lib_script` first, then `game_script`. If you have no library, set `lib_script` to a file that simply returns.

---

## Entry point

Your `game_script` is executed once at startup. At the top level of the file, register hooks using `lunac.api.hooks` and perform any other setup the engine needs before it finishes initialising.

```lua
lunac.api.hooks.on_startup(function()
  -- register states, systems, etc.
end)
```

The engine runs `on_startup` after all data has been loaded and all other API registrations are complete. Hook registration calls made at the top level of your script (outside any function) are processed as the file loads, so they are in place before the engine calls any of them.

---

## Hooks

Hooks are registered by passing a function to the corresponding `lunac.api.hooks` method. All hooks are optional — if not registered the engine skips the call silently. Registering the same hook a second time replaces the first registration.

---

### `lunac.api.hooks`

#### `on_startup(fn)`

`fn` is called once after the engine has fully initialised and all data has been loaded from the database. This is the right place to register states, narrators, and systems.

`fn` signature: `function()`

#### `on_shutdown(fn)`

`fn` is called when the engine is shutting down cleanly (after `lunac.api.game.shutdown()` is called or the process receives a signal).

`fn` signature: `function()`

---

#### `on_entities_loaded(fn)`

`fn` is called after entities are loaded from the database.

`fn` signature: `function(entities)` — `entities` is an array of entity tables.

Each entry: `{ uuid = "...", _ptr = <userdata>, _type = <number> }`

#### `on_commands_loaded(fn)`

`fn` signature: `function(commands)` — array of command tables.

Each entry: `{ uuid = "...", name = "...", script = "script-uuid", _ptr = <userdata>, _type = <number> }`

#### `on_command_groups_loaded(fn)`

`fn` signature: `function(groups)` — array of command group tables.

Each entry: `{ uuid = "...", description = "...", commands = { "uuid", ... }, _ptr = <userdata>, _type = <number> }`

`commands` is an array of UUID strings for the commands belonging to this group.

#### `on_actions_loaded(fn)`

`fn` signature: `function(actions)` — array of action tables.

Each entry: `{ uuid = "...", name = "...", script = "script-uuid", _ptr = <userdata>, _type = <number> }`

---

#### `on_player_connected(fn)`

`fn` is called when a TCP connection is accepted and a player session is created. This is the right place to assign an initial state.

`fn` signature: `function(player)` — `player` is a player table (see [Player table](#player-table)).

#### `on_player_disconnected(fn)`

`fn` is called when a connection is closed or the client hangs up.

`fn` signature: `function(player)` — `player` is a player table.

#### `on_player_input(fn)`

`fn` is called when raw input arrives from a player **before** it is routed to the player's current state.

`fn` signature: `function(player, input)`

| Argument | Type | Description |
|---|---|---|
| `player` | lightuserdata | Raw player pointer (not a table) |
| `input` | string | Raw input string from the client |

> **Note:** Unlike the other player hooks, `player` here is a lightuserdata, not a table. You cannot pass it directly to most `lunac.api.player.*` functions without wrapping it. In practice you will rarely need this hook — player state `on_input` handles input for the normal case.

---

### State hooks

A **state** controls what happens to a player at a given moment: their login screen, character selection, playing the game, etc. You create a state as a Lua table and register it with `lunac.api.game.register_state()`. The returned userdata handle is what you pass to `lunac.api.player.set_state()`.

The engine calls named functions on your state table. All state hooks receive a **player table** (see [Player table](#player-table)).

#### `on_enter(player)`

Called when a player transitions into this state.

#### `on_exit(player)`

Called when a player leaves this state (either by transitioning to another state or disconnecting).

#### `on_input(player, input)`

Called when the player sends input while in this state.

| Argument | Type |
|---|---|
| `player` | table |
| `input` | string |

#### `on_output(player, output)`

Called when text is about to be sent to the player. Allows the state to intercept or transform output before it is delivered.

| Argument | Type |
|---|---|
| `player` | table |
| `output` | string |

#### `on_event(player, event)`

Called when an event is dispatched to the player while they are in this state.

| Argument | Type | Description |
|---|---|---|
| `player` | table | |
| `event` | lightuserdata | Pointer to the internal event struct |

#### `on_gmcp(player, topic [, message])`

Called when a GMCP message arrives from the client.

| Argument | Type | Description |
|---|---|---|
| `player` | table | |
| `topic` | string | GMCP topic string, e.g. `"Core.Hello"` |
| `message` | table or absent | JSON payload decoded to a Lua table; absent if no data was sent |

The JSON message is decoded and passed as a nested Lua table. If the client sent no payload, the third argument is not present.

**Example state:**

```lua
local LoginState = {
  on_enter = function(player)
    lunac.api.player.disable_echo(player)
    lunac.api.player.send(player, "Password: ")
  end,

  on_input = function(player, input)
    if lunac.api.player.authenticate(player, "username", input) then
      lunac.api.player.enable_echo(player)
      lunac.api.player.set_state(player, GameState)
    else
      lunac.api.player.send(player, "Wrong password.\r\n")
    end
  end,

  on_exit = function(player)
    lunac.api.player.enable_echo(player)
  end
}

LoginState = lunac.api.game.register_state(LoginState)
```

---

### Narrator hooks

A **narrator** translates events into text sent to a player. Create a narrator as a Lua table with a `narrate` function, and register it with `lunac.api.game.register_narrator()`.

#### `narrate(player, event_data)`

Called when an event is routed to a player whose narrator is this one.

| Argument | Type | Description |
|---|---|---|
| `player` | table | Player table |
| `event_data` | table | The Lua table that was originally passed to `lunac.api.game.event()` |

```lua
local WorldNarrator = {
  narrate = function(player, event)
    if event.type == "say" then
      lunac.api.player.send(player, event.actor .. " says: " .. event.message .. "\r\n")
    end
  end
}

WorldNarrator = lunac.api.game.register_narrator(WorldNarrator)
```

---

### System hooks

A **system** runs logic against entities every tick. Create a Lua table with an `execute` function and register it with `lunac.api.game.register_system()`. Systems are disabled by default — call `lunac.api.game.enable_system()` after registering.

#### `execute()`

Called once per tick for every enabled system. No arguments.

```lua
local RegenSystem = lunac.api.game.register_system("Health Regen", {
  execute = function()
    -- query entities and update component data here
  end
})

lunac.api.game.enable_system(RegenSystem)
```

---

### Task callbacks

Tasks are not hooks in the same sense — you pass a function directly when scheduling. The engine calls it once when the delay has elapsed.

```lua
lunac.api.game.schedule_task("Respawn", 30, function()
  -- runs 30 seconds from now
end)
```

---

## API reference

All engine functions are available under `lunac.api`.

---

### `lunac.api.hooks`

| Function | Arguments | Description |
|---|---|---|
| `on_startup(fn)` | function | Called once after full engine initialisation |
| `on_shutdown(fn)` | function | Called when the engine shuts down cleanly |
| `on_entities_loaded(fn)` | function | Called with an array of entity tables after DB load |
| `on_commands_loaded(fn)` | function | Called with an array of command tables after DB load |
| `on_command_groups_loaded(fn)` | function | Called with an array of command group tables after DB load |
| `on_actions_loaded(fn)` | function | Called with an array of action tables after DB load |
| `on_player_connected(fn)` | function | Called with a player table when a connection is accepted |
| `on_player_disconnected(fn)` | function | Called with a player table when a connection closes |
| `on_player_input(fn)` | function | Called with (lightuserdata, string) when a player sends input |

Calling any of these a second time replaces the previous registration.

---

### `lunac.api.game`

#### Entities

| Function | Arguments | Returns | Description |
|---|---|---|---|
| `get_entities()` | — | table (array) | All entities currently loaded in the game |
| `new_entity()` | — | entity table | Creates a new entity in memory (not yet persisted) |
| `get_entity(uuid)` | string | entity table | Retrieves an entity by UUID; errors if not found |
| `save_entity(entity)` | entity table | — | Persists entity to the database |
| `delete_entity(entity)` | entity table | — | Removes entity from all components and deletes from database |

#### Components

Components are opaque handles. You register one to get a handle, then attach data tables to entities using that handle.

| Function | Arguments | Returns | Description |
|---|---|---|---|
| `register_component()` | — | lightuserdata | Registers a new component type; store the returned handle |
| `has_component(entity, component)` | table, lightuserdata | boolean | True if entity has the component attached |
| `add_component(entity, component, data)` | table, lightuserdata, table | — | Attaches `data` to `entity` under `component` |
| `get_component(entity, component)` | table, lightuserdata | table or nil | Returns the data table attached to this entity, or nil |
| `get_component_entities(component)` | lightuserdata | table (array) | All entity tables that have this component |

#### Archetypes

An archetype is a named combination of components. It lets you query all entities that have every component in the set.

| Function | Arguments | Returns | Description |
|---|---|---|---|
| `register_archetype(comp, ...)` | lightuserdata... | lightuserdata | Registers an archetype from one or more component handles |
| `get_archetype_entities(archetype)` | lightuserdata | table (array) | All entity tables matching this archetype |
| `matches_archetype(entity, archetype)` | table, lightuserdata | boolean | True if entity has all components in the archetype |

#### States

| Function | Arguments | Returns | Description |
|---|---|---|---|
| `register_state(state_table)` | table | userdata | Stores a reference to the state table; returns a handle |
| `deregister_state(handle)` | userdata | — | Releases the state reference |

The returned handle is what you pass to `lunac.api.player.set_state()`.

#### Narrators

| Function | Arguments | Returns | Description |
|---|---|---|---|
| `register_narrator(narrator_table)` | table | userdata | Stores a reference to the narrator table; returns a handle |
| `deregister_narrator(handle)` | userdata | — | Releases the narrator reference |

#### Systems

| Function | Arguments | Returns | Description |
|---|---|---|---|
| `register_system(name, table)` | string, table | system table | Registers a system (disabled by default) |
| `deregister_system(system)` | system table | — | Removes system from the engine |
| `enable_system(system)` | system table | system table | Starts running the system each tick |
| `disable_system(system)` | system table | system table | Stops running the system |

The system table returned by `register_system` has: `{ uuid, name, enabled, _ptr, _type }`.

#### Tasks

| Function | Arguments | Returns | Description |
|---|---|---|---|
| `schedule_task(name, seconds, fn)` | string, number, function | task table | Schedules `fn` to run after `seconds` real seconds |
| `cancel_task(task)` | task table | — | Cancels a pending task |
| `get_tasks()` | — | table (array) | All pending tasks |

Delays are in **real seconds** (wall-clock time), not ticks.

The task table has: `{ uuid, name, execute_at, _ptr, _type }` where `execute_at` is a Unix timestamp.

#### Events

| Function | Arguments | Returns | Description |
|---|---|---|---|
| `event(data_table)` | table | — | Submits a Lua table as an event to the broker |

The table you pass is what your narrator's `narrate` function receives verbatim. The engine does not inspect it — structure it however your game requires.

#### Actions

| Function | Arguments | Returns | Description |
|---|---|---|---|
| `do_action(entity, action, data)` | table, table, table | 2 values | Executes an action script; return values are defined by the script |

#### Misc

| Function | Arguments | Returns | Description |
|---|---|---|---|
| `shutdown()` | — | — | Initiates a clean engine shutdown |

---

### `lunac.api.player`

All functions take a **player table** as their first argument unless noted.

| Function | Arguments | Returns | Description |
|---|---|---|---|
| `send(player, message)` | table, string | — | Sends text to the player's output buffer |
| `send_gmcp(player, topic)` | table, string | — | Sends a GMCP message with no payload |
| `send_gmcp(player, topic, message)` | table, string, string | — | Sends a GMCP message; `message` must be a JSON string |
| `authenticate(player, username, password)` | table, string, string | boolean | Returns true if credentials match a user in the database |
| `get_entity(player)` | table | entity table or nil | Returns the entity currently associated with the player |
| `set_entity(player, entity)` | table, table | — | Associates an entity with the player |
| `get_entities(player)` | table | table (array) | Entity tables owned by this player's user account |
| `set_state(player, handle)` | table, userdata | — | Transitions player to a new state; fires `on_exit` then `on_enter` |
| `set_narrator(player, handle)` | table, userdata | — | Sets the player's active narrator |
| `narrate(player, event)` | table, lightuserdata | — | Passes an event directly to the player's narrator |
| `disable_echo(player)` | table | — | Sends telnet IAC WILL ECHO (suppresses terminal echo — use before password prompts) |
| `enable_echo(player)` | table | — | Sends telnet IAC WONT ECHO (restores terminal echo) |
| `disconnect(player)` | table | — | Marks the player for disconnection; takes effect at end of tick |
| `add_command_group(player, uuid)` | table, string | — | Grants the player access to a command group by UUID |
| `remove_command_group(player, uuid)` | table, string | — | Revokes access to a command group |
| `get_commands(player, name)` | table, string | table (array) | Commands accessible to the player matching `name` |
| `execute_command(player, command, args)` | table, table, string | — | Executes a command on the player's behalf |

---

### `lunac.api.db`

Direct SQLite3 access. Follows the prepare / bind / step / finalize pattern.

| Function | Arguments | Returns | Description |
|---|---|---|---|
| `prepare(sql)` | string | lightuserdata | Prepares a statement; errors on failure |
| `bind(stmt, index, value)` | userdata, number, string\|number | — | Binds a value to a `?` parameter; index is **1-based** |
| `step(stmt)` | userdata | boolean | Advances to the next row; `true` if a row is available, `false` when done |
| `column_text(stmt, index)` | userdata, number | string | Returns the text value of a column; index is **0-based** |
| `finalize(stmt)` | userdata | — | Finalises and frees the statement |

```lua
local stmt = lunac.api.db.prepare("SELECT uuid FROM characters WHERE user_uuid = ?")
lunac.api.db.bind(stmt, 1, player.user_uuid)
while lunac.api.db.step(stmt) do
  local uuid = lunac.api.db.column_text(stmt, 0)
  -- do something with uuid
end
lunac.api.db.finalize(stmt)
```

> Bind index is 1-based (SQLite convention). Column index is 0-based (SQLite convention).

---

### `lunac.api.log`

All functions take a single string message.

```lua
lunac.api.log.trace("message")
lunac.api.log.debug("message")
lunac.api.log.info("message")
lunac.api.log.warn("message")
lunac.api.log.error("message")
```

---

### `lunac.api.script`

| Function | Arguments | Returns | Description |
|---|---|---|---|
| `available()` | — | table (array) | All scripts registered in the database |

Each entry: `{ uuid = "...", path = "...", groups = { "group-name", ... } }`

---

## Data structures

These are the table shapes the engine produces and passes to your hooks and API calls. The `_ptr` field is an opaque internal pointer — do not modify it. `_type` is an integer discriminator used internally.

### Player table

```lua
{
  uuid      = "string",   -- player session UUID
  user_uuid = "string",   -- user account UUID (used for entity ownership)
  username  = "string",   -- authenticated username, or empty string
  _ptr      = userdata,
  _type     = number
}
```

### Entity table

```lua
{
  uuid  = "string",
  _ptr  = userdata,
  _type = number
}
```

### Command table

```lua
{
  uuid   = "string",
  name   = "string",
  script = "string",   -- UUID of the script that implements this command
  _ptr   = userdata,
  _type  = number
}
```

### Command group table

```lua
{
  uuid        = "string",
  description = "string",
  commands    = { "uuid", "uuid", ... },   -- array of command UUIDs in this group
  _ptr        = userdata,
  _type       = number
}
```

### Action table

```lua
{
  uuid   = "string",
  name   = "string",
  script = "string",   -- UUID of the script that implements this action
  _ptr   = userdata,
  _type  = number
}
```

### System table

```lua
{
  uuid    = "string",
  name    = "string",
  enabled = boolean,
  _ptr    = userdata,
  _type   = number
}
```

### Task table

```lua
{
  uuid       = "string",
  name       = "string",
  execute_at = number,   -- Unix timestamp when the task will run
  _ptr       = userdata,
  _type      = number
}
```

---

## Concepts and constraints

### Single-threaded

The game loop runs on a single thread. Never block inside a hook, system, or task — doing so stalls the entire server for all players.

### States

A player always has at most one active state. Calling `set_state()` transitions the player: the engine calls `on_exit` on the outgoing state, then `on_enter` on the new one. It is safe to call `set_state()` from within `on_input`.

States are the primary tool for managing player flow: login screen → character selection → in-game, etc.

### Narrators

A narrator converts raw events into player-visible text. A player's narrator is called whenever an event is dispatched to them (via the event broker during the dispatch phase) or when `lunac.api.player.narrate()` is called directly.

The event table your code passes to `lunac.api.game.event()` is the same table your narrator's `narrate()` receives. Use whatever fields make sense for your game.

### ECS

The engine uses an Entity-Component-System. Entities are just UUIDs. Components are opaque handles with associated data tables. Archetypes let you query all entities that have a given set of components. Systems run each tick to process those entities.

A typical pattern:

```lua
-- register component types at startup
local Health = lunac.api.game.register_component()
local Position = lunac.api.game.register_component()

-- register an archetype for entities that have both
local Living = lunac.api.game.register_archetype(Health, Position)

-- create an entity and attach components
local entity = lunac.api.game.new_entity()
lunac.api.game.add_component(entity, Health, { current = 100, max = 100 })
lunac.api.game.add_component(entity, Position, { x = 0, y = 0 })
lunac.api.game.save_entity(entity)

-- query in a system
local RegenSystem = lunac.api.game.register_system("Regen", {
  execute = function()
    for _, e in ipairs(lunac.api.game.get_archetype_entities(Living)) do
      local h = lunac.api.game.get_component(e, Health)
      h.current = math.min(h.current + 1, h.max)
    end
  end
})
lunac.api.game.enable_system(RegenSystem)
```

### Commands and command groups

Commands and command groups are stored in the database. Players only have access to commands in groups you explicitly assign to them via `add_command_group`. A typical pattern is to assign a basic group on connection and a broader one after authentication.

### Echo suppression

Call `disable_echo` before prompting for a password, and `enable_echo` once the player has submitted it. This sends telnet negotiation sequences to the client — clients that do not support telnet will ignore them safely.

### Task timing

Task delays are in **real seconds** of wall-clock time, not game ticks. At 5 ticks/second a single tick is 200ms, but tasks scheduled for `30` seconds will fire approximately 30 seconds later regardless of tick rate.
