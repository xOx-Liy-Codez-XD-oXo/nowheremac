# nowhere to go for the mac
it's just an sdl1 port of nowhere to go, but it compiles on mac os tiger and you'd need to change it slightly to get it to work on something else.
## building
you need xcode. 2.5 will do, and older versions might work too. you need a copy of sdl1. tigerbrew has it, but if you're not on a G4 with altivec, you might need to compile it yourself. once you have it, copy `/usr/local/Cellar/sdl/1.2.15/lib/libSDL-1.2.0.dylib` to the nowheremac directory renamed `libSDL.dylib`. after that, run `./build`. it'll make a neat application with an icon and everything
