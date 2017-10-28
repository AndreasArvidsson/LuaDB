# LuaMongo
Lua CLI client for MongoDB

## Background
I use MongoDB extensively and quite often with the Mongo CLI and JavaScript to do more complex data aggregations and migrations.
On large collections this takes a long time and JavaScript is a huge bottleneck so I started looking for something faster.

## Why Lua?
Pure and simple: with [LuaJIT](http://luajit.org/) Lua is the fastests scripting lanugage available.     

LuaJit is in most scenarios on par or at most a few percent behind pure C code. For a scripting language this is amazing.


## Setup
**Include directories**
* $(ProjectRoot)/lib/bson
* $(ProjectRoot)/lib/mongoc
* $(ProjectRoot)/lib/luajit
* $(ProjectRoot)/lib/libbson

**Linker library directories**
* $(ProjectRoot)/lib/bson
* $(ProjectRoot)/lib/mongoc
* $(ProjectRoot)/lib/luajit
