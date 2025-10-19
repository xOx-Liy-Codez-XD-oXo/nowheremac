# nowhere to go for the mac
it's just an sdl1 port of nowhere to go, but it compiles on mac os tiger and leopard and you'd need to change it slightly to get it to work on something else.

<img width="1034" height="537" alt="Picture 9" src="https://github.com/user-attachments/assets/9d868b7d-bf5b-4465-aa00-cb0199df797d" />
(you have to change a line to get a windowed mode)

## building
you need xcode. 2.5 will do, and older versions might work too. you need a copy of sdl1. tigerbrew has it, but if you're not on a G4 with altivec, you might need to compile it yourself. once you have it, copy `/usr/local/Cellar/sdl/1.2.15/lib/libSDL-1.2.0.dylib` to the nowheremac directory renamed `libSDL.dylib`. the bash scripts wont have the right permissions after downloading so you need to chmod +x both build and run. after that, run `./build`. it'll make a neat application with an icon and everything 
```
brew install sdl
git clone https://github.com/xOx-Liy-Codez-XD-oXo/nowheremac
cd nowheremac
cp /usr/local/Cellar/sdl/1.2.15/lib/libSDL-1.2.0.dylib libSDL.dylib
chmod +x build
chmod +x run
./build
```
## licenses
math_3d.h is licensed under the MIT license so heres a copy of that

```
The MIT License (MIT)

Copyright (c) 2016 arkanis

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```
"nowhere to go w/ jane remover" by juno is licensed under creative commons BY-NC-SA 3.0 and a shortened version is included in this repo
