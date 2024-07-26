# Rygame - Raylib meets Pygame

A collection of helper functions to work with Raylib in the same style as Pygame.

These are functions that I created while working on other projects and I decided to extract them into a library to have
them available to all my Raylib projects.  
After some time, I decided to follow some concepts from Pygame to organize this library.

It has also some useful functions to deal with TMX files (**Tiled** software).

### Dependencies

- Raylib  
  Main library to manage window, inputs, screen  
  Compile and install raylib https://github.com/raysan5/raylib
- TMX  
  Parse `.tmx` files (**Tiled**)  
  Compile and install TMX https://github.com/baylej/tmx
- File `raylib-tmx.h` from https://github.com/RobLoach/raylib-tmx    
  It contains helper functions to integrate TMX with Raylib.    
  Add this file to your include path.    
