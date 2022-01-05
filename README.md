# MUD
## Introduction
This repository contains an incomplete MUD engine.  It was initially created because I had some downtime and was feeling nostalgic for the MUD codebases that I'd cut my coding teeth on when I was younger.

Currently not much use for an actual game but you might have fun tinkering with it.

## Requirements
* Linux - Sorry, not cross platform! I'm lazy and don't want to deal with Windows.
* Lua 5.1 - I like Lua, so I've embedded it even when it's entirely unnecessary.  I'd like to do more with it.
* SQLite3 - All the cool kids are using SQLite.

## Features
It's pretty basic so far.
* Text based interface via Telnet or a MUD client
  * Supports ANSI terminal codes
  * Doesn't currently implement the actual telnet protocol or any MUD specific ones.
* Text output templating system
* Limited persistence via sqlite
* Scripting via Lua (technically, just the config file so far)
* Rudimentry ECS (entity component system) model
* Task scheduling

# Future
It's pretty much a toy project, some ideas of things I'd like to try implementing:
  * Supprt Telnet option negotiations, at least basic ones
  * Support for a MUD protocol like GMCP
  * Support scripting via a MUD engine API
