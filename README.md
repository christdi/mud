# Project
## Introduction

This repository contains an incomplete (and unnamed) framework for writing MUD games in Lua.

Some core functionality is still missing but it is technically possible to write games.

The dist directory contains a sample game used during development as an example of how a game might be written.

## Requirements
* Linux - Windows not currently supported.
* Lua 5.3
* SQLite3

## Features

* Text based interface via MUD client
* Persistence via SQLite
* Lua API for developing MUDs
* Entity Component System (currently missing the system part)
* Task system for scheduled tasks (not currently exposed to Lua)
* Event system for submitting events to players and entities (entities outstanding)
* Narration system for translating events to output for players

## Future

This is still very much in the toy project category but is starting to mature.  Future development might include the following

* Introduction of 'actions' to define how entiies can interact with the game
* Completion of 'system' part of Entity Component System framework
* Exposing tasks to Lua
* Telnet protocol implementation
* MUD specific protocol like GMCP
* Extract common functionality like networking and data structures to their own libraries