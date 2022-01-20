# CDL
ConsoleDrawingLibrary is a c++ library for drawing in the console on windows.

## Documentation
### Overall info
CDL is a c++ library, contained in `cdl.h`. All of it is contained in the `CDL` namespace.
### Initialisation
You need to initialise CDL before using it. `CDL::init` requires the number of rows you will use.
```c++
CDL::init(4);
```
### CDL::Colour
Colour is a `UCHAR` that represents a colour. It's values are a part of this enum.
```c++
enum Colour {
    BLACK = 0x0000,         //k
    DARK_BLUE = 0x0001,     //b
    DARK_GREEN = 0x0002,    //g
    DARK_CYAN = 0x0003,     //c
    DARK_RED = 0x0004,      //r
    DARK_MAGENTA = 0x0005,  //m
    DARK_YELLOW = 0x0006,   //y
    GREY = 0x0007,          //w
    DARK_GREY = 0x0008,     //K
    BLUE = 0x0009,          //B
    GREEN = 0x000A,         //G
    CYAN = 0x000B,          //C
    RED = 0x000C,           //R
    MAGENTA = 0x000D,       //M
    YELLOW = 0x000E,        //Y
    WHITE = 0x000F          //W
};
```
### CDL::Palette
A palette is a pair of colour representing a foreground and background colour pair. They are made from `OR`-ing two `CDL::Colour`-s with the one representing the background being shifted left by 4 bits.
```c++
typedef UCHAR Palette;
Palette make_palette(Colour fg, Colour bg) {
    return fg | (bg << 4);
}
```
### CDL::Char
A `CDL::Char` represents a grouping of a `UCHAR` and a `CDL::Palette`. It is nearly equivalent to the windows api type `CHAR_INFO`. The character part can be `ascii 0`, that represents that the Char is "transparent" and shouldn't be drawn.
```c++
struct Char {
    UCHAR chr;
    Palette pal;
    Char(UCHAR _chr, Palette _pal)
        : chr(_chr), pal(_pal) {}
    Char() {
        chr = 0;
        pal = BLACK;
    }
};
```
A `CDL::Char` can be drawn at a given position, represented by a windows api `COORD` object with the `CDL::draw_char` function.
```c++
void draw_char(Char chr, COORD pos)
```
### CDL::Texture
Textures are the most complex structures in CDL. A `CDL::Texture` encapsualtes an array of `CDL::Char`-s and a `COORD` that describes the dimesions of the texture. It is required, because a texture with a text component "abcd" can be any of these:
```
abcd    a   ab
        b   cd   
        c
        d
```
It works like this:
```c++
struct Texture {
    Char* chars;
    COORD dims;
};
```
You can draw a texture to specific coordinates (they are of the top-left corner) with the `CDL::draw_texture` function.
```c++
void draw_texture(Texture tex, COORD pos)
```
### Texture info
A texture can be represented by a `COORD`(dimensions) and 3 `std::string`-s: one for the text content, one for the foreground colours and one for the background colours. The colours are in a format that is explained in the comment on [this](#cdlcolour) block of code.
You can read these 4 peaces of info with the `CDL::read_texture_file` function.
```c++
void read_texture_file(std::string filename, COORD& dims, std::string& characters, std::string& foreground, std::string& background)
```
It should be used like this:
```c++
COORD dims;
std::string characters, foreground, background;
read_texture_file("example.cdlt", dims, characters, foreground, background);
```
It reads from a file in a format explained [here](#the-texture-file-format).
The `CDL::texture_from_info` function creates a `CDL::Texture` from texture info.
```c++
Texture texture_from_info(COORD dims, std::string characters, std::string foreground, std::string background)
```
### The texture file format
The `CDL Texture` file format (extension `cdlt`) is structured in the following way:
On the first row are 2 numbers: the first one is the width of the texture and the second one is the height.
> For easier explaining purposes they will be henceforth called W and H.
This row is followed by 3 matrices seperated by newlines: H rows of W characters.
The first one represents the `characters` aspect of the texture info.
The second one is the `foreground`.
And the third one is the `background`.
Here is an example:
```cdlt
2 3

AB
CD
EF

gM
Br
Yc

kk
ww
kk
```
and it represents this texture:
![CDLT example](https://i.imgur.com/Db5aZAm.png)
### Other things
The `CDL::letter_to_colour` function converts a letter encoded like [here](#cdlcolour) to a `CDL::Colour` object.
```c++
Colour letter_to_colour(char letter)
```