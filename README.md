# Project
## Introduction

This repository contains an incomplete (and unnamed) framework for writing MUD games in Lua.

Some core functionality is still missing but it is technically possible to write games.

This started as a toy pet project but it's actually grown into something that might actually be usable.  This means that all code isn't always to the same standard and can be inconsistent in terms of naming conventions and commenting.

The dist directory contains a sample game used during development as an example of how a game might be written.

## Requirements
* Linux - Windows not currently supported.
* Lua 5.3
* SQLite3

## Features

* Text based interface over TCP/IP
* Persistence via SQLite
* Lua API for developing MUDs
* Entity component system
* Task system for scheduled tasks
* Event system for submitting events to players and entities (entities outstanding)
* Narration system for translating events to output for players

## Future

This is still very much in the toy project category but is starting to mature.  Future development might include the following
* Telnet protocol implementation
* MUD specific protocol like GMCP
* Extract common functionality like networking and data structures to their own libraries