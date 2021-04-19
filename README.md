# Abstract Realm
A Real-Time Engine

![Current State](https://files.catbox.moe/f5ta3q.gif)

# Dependencies

Vulkan. (Use the VULKAN_PATH env variable). You will also need the debug libs https://files.lunarg.com/.

bgfx, bimg, bimg_decode, and bx libraries are setup as separate projects... They need to be built for the engine to function with bgfx...
(Currently bgfx is not really setup)

## So whats the point of this engine?

### Update 4-14-2021: So far I'm treating this engine design like a learning tool so its going to pretty much be the breadboard of game engines; where you can literally poke and see everything during runtime (with some caveats ofc, if you poke too much the performance will be impacted)

This is a portfolio piece I want to make. This is not intended to be a full-fledged engine. More so a showcase of what I know in c++

The intention is to use pretty much close to standard c++ style with my own flavoring and really strict taste on spacial alignment of code. I'll do my best to make a super clean design implementation of the structure of the engine but first priority is a working MVP. No optimization, just staright up getting this thing working with the features I want. I'll figure out the rest after.

## Whats your references?

Other than what I knew from previous expereince:

This book: 

![Game Engine Architecture, Third Edition](https://i.imgur.com/oMnHm74.png) + More

Documentation for various APIs.

A bunch of Youtube... (I'll note what I used in the wiki)

And more likely more cause this isn't the smallest engine at the same time...

## C++17 Used with MSVC and Clang-CL. 
