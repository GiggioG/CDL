# CDL
ConsoleDrawingLibrary is a c++ library for drawing in the console on windows.

## Documentation
### Overall info
CDL is a c++ library, contained in `cdl.cpp`. All of it is contained in the `CDL` namespace.
### Initialisation and exiting
You need to initialise CDL before using it. `CDL::init` requires a `SMALL_RECT` of the part of the console window you will use.
```cpp
SMALL_RECT rect;
rect.Left = 1;
rect.Top = 1;
rect.Right = 45;
rect.Bottom = 15;
CDL::init(rect);
```
The rect is saved in `CDL::canvas`, and its size in `CDL::canvasSize`.  
You also must call `CDL::exit` before the end, to free memory used by the library.
### Setting the top left corner
You can set what coordinate coresponds to the top left corner
of the RECT you are using.
```cpp
CDL::setTopLeft({-5, -5});
```
It is saved in `CDL::topLeftCorner`.
### CDL::Colour
Colour is a `UCHAR` that represents a colour. You have the following pre-defined; There aren't any other possible colours.
```cpp
BLACK        = 0x0;
DARK_BLUE    = 0x1;
DARK_GREEN   = 0x2;
DARK_CYAN    = 0x3;
DARK_RED     = 0x4;
DARK_MAGENTA = 0x5;
DARK_YELLOW  = 0x6;
GREY         = 0x7;
DARK_GREY    = 0x8;
BLUE         = 0x9;
GREEN        = 0xA;
CYAN         = 0xB;
RED          = 0xC;
MAGENTA      = 0xD;
YELLOW       = 0xE;
WHITE        = 0xF;
```
### CDL::Palette
A palette is a pair of colours representing a foreground and background colour pair. The default palette is white on black.
```cpp
/// Palette(Colour foreground, Colour background);
CDL::Palette p = CDL::Palette(CDL::CYAN, CDL::BLACK);
```
It can be converted from/to the `CHAR_INFO.Attributes`:
```cpp
CDL::Palette p = CDL::Palette((0x0<<4) | 0xb);
```
```cpp
CHAR_INFO ci;
ci.Attributes = CDL::Palette(CDL::CYAN, CDL::BLACK).getCInfoAttrib();
```
You an also get it from a string, which specifies the foreground first and the background second.
```cpp
CDL::Palette p = CDL::Palette("b0"); // case insensitive
```
### CDL::Char
A `CDL::Char` represents a grouping of a `char` and a `CDL::Palette`. It is nearly equivalent to the windows api type `CHAR_INFO`. The default `CDL::Char` is `'\0'` with the default `CDL::Palette`.  
You can get a `CDL::Char` from a `char`, with the default palette:
```cpp
CDL::Char c = CDL::Char('c');
```
Or with a palette:
```cpp
CDL::Char c = CDL::Char('c', CDL::Palette(CDL::CYAN, CDL::BLACK));
```
You can also convert it from/to `CHAR_INFO`:
```cpp
CDL::Char c = CDL::Char('c');
CHAR_INFO ci = c.getCInfo();
```
```cpp
CHAR_INFO ci;
CDL::Char c = CDL::Char(ci);
```
And you can also use a 3-character string, representing it. The first one is the character and the second and third ones are the palette.
```cpp
CDL::Char c = CDL::Char("cB0"); // palette - case insensitive
```

You can aquire a `Char` array with `CDL::charsFromText`, which accepts a `std::string` and an optional `CDL::Palette` (default if ommited).
```cpp
Char* carr1 = charsFromText("abcd");
Char* carr2 = charsFromText("abcd", CDL::Palette("b0"));
```
### CDL::Texture
The Texture contains an array of `CDL::Char` and a `COORD`, which records the dimensions of the texture. The default texture is empty and is `0x0`.  
If you initialise a `CDL::Texture` with just dimensions, then it will fill up with default `CDL::Char`:
```cpp
CDL::Texture t = CDL::Texture({3, 2});
```
You can of course give it dimensions and a Char array:
```cpp
Char* chrs = new Char[6]; // aquire it somehow
CDL::Texture t = CDL::Texture({3, 2}, chrs);
```
If you pass just one Char, then it will be repeated:
```cpp
CDL::Texture t = CDL::Texture({3, 2}, CDL::Char("cb0"));
```
...and if you past only the Char, 1x1 is assumed:
```cpp
CDL::Texture t = CDL::Texture(CDL::Char("cb0"));
```
Textures, like Palettes and Chars can be initialised by a special string format, which is:
```
<width>,<height>,<width*height Chars>
```
example:
```cpp
CDL::Texture t = CDL::Texture("2,3,.01,6e/b2:fd'f0'45");
//or from a buffer, with an extra option to delete[] it
char* buff; // aquire it somehow
CDL::Texture t = CDL::Texture(buff, true); // true = delete[] the buffer
```
for ![image of the texture](https://i.imgur.com/AYqVeL0.png)
### Loading from files
`CDL` includes the general-purpose function
```cpp
char* CDL::readFile(std::string filename)
```
The buffer it returns has size `<fileSize>+1` and the extra one is a `\0`.


This function is utilised by
```cpp
Texture CDL::loadTexture(std::string filename)
```
## Drawing things
To draw a texture, you need to call
### Clearing
To clear the canvas (fill it with default `Char`), call 
```cpp
void CDL::clear()
```

### Textures
```cpp
void CDL::drawTexture(const Texture& t, COORD coord, bool screenCoords=0)
```
```cpp
CDL::Texture t; // aquire somehow
CDL::drawTexture(t, {1, 2});
```
If `screenCoords` is set, then it will ignore setTopLeft.

### Chars
```cpp
void CDL::drawChar(Char c, COORD coord, bool screenCoords=0);
```

## Plotting
### Axes
You can use this function to draw coordinate axes on the screen.
```cpp
void CDL::drawAxes();
```

### Points
You can use this function to plot a point (draw a '@' with the default Palette).
```cpp
void CDL::plot(COORD point, bool screenCoords=false);
```

### Lines
You can use this function to plot a line. 
```cpp
void CDL::line(COORD a, COORD b, bool screenCoords=false);
```
The coordinates of the points are multiplied by `CDL::linePlotScaleFactor`. You can change this factor with the `CDL::setLinePlotScaleFactor` function.
```cpp
void setLinePlotScaleFactor(int factor);
```