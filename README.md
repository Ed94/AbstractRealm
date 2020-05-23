# Abstract Realm
A Real-Time Engine

# Dependencies
Since the Vulkan-SDK is quite a bit of work to embed as a submodule (requires pretty much the entire Khronos suite of Vulkan repo), and even then, the Loader needs you to put in the vender stuff yourself) unfortuantely you have to manually setup it.

This project expects access to vulkan sdk at the following directory:
$(ProjectDir)PAL\ThirdParty\SDKs\LunarG_VulkanTools\

(This project is the raw vs solution, since this projet only supports Windows I haven't made a cmake to generate the project for other platforms.)


## So whats the point of this engine?

This is a portfolio piece I want to make. This is not intended to be a full-fledged engine. More so a showcase of what I know in c++

The intention is to use pretty much close to standard c++ style with my own flavoring and really strict taste on spacial alignment of code. I'll do my best to make a super clean design implementation of the structure of the engine but first priority is a working MVP. No optimization, just staright up getting this thing working with the features I want. I'll figure out the rest after.

## Whats your references?

Other than what I knew from previous expereince:

This book: 

![Game Engine Architecture, Third Edition](https://i.imgur.com/oMnHm74.png) + More

Documentation for various APIs.

A bunch of Youtube... (I'll note what I used in the wiki)

And more likely more cause this isn't the smallest engine at the same time...

## C++17 Used with MSVC. 

(As it stands I'm making the design abstract away from platform implementation but will not be adding support beyond what gets this working on Windows 10)
