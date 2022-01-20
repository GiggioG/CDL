#pragma once;
#include <windows.h>
#include <string>
#include <vector>
#include <fstream>
namespace CDL {
enum Colour {
    BLACK = 0x0000,
    DARK_BLUE = 0x0001,
    DARK_GREEN = 0x0002,
    DARK_CYAN = 0x0003,
    DARK_RED = 0x0004,
    DARK_MAGENTA = 0x0005,
    DARK_YELLOW = 0x0006,
    GREY = 0x0007,
    DARK_GREY = 0x0008,
    BLUE = 0x0009,
    GREEN = 0x000A,
    CYAN = 0x000B,
    RED = 0x000C,
    MAGENTA = 0x000D,
    YELLOW = 0x000E,
    WHITE = 0x000F
};

typedef UCHAR Palette;
Palette make_palette(Colour fg, Colour bg) {
    return fg | (bg << 4);
}
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
void draw_char(Char chr, COORD pos) {
    if (chr.chr == 0) {
        return;
    }
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CHAR_INFO cinfo;
    cinfo.Char.AsciiChar = chr.chr;
    cinfo.Attributes = chr.pal;

    SMALL_RECT rect = {pos.X, pos.Y, pos.X + 1, pos.Y + 1};
    WriteConsoleOutput(hConsole, &cinfo, {1, 1}, {0, 0}, &rect);
}
struct Texture {
    Char* chars;
    COORD dims;
    void clear() {
        delete[] chars;
    }
};
void draw_texture(Texture tex, COORD pos) {
    for (SHORT i = 0; i < tex.dims.X; i++) {
        for (SHORT j = 0; j < tex.dims.Y; j++) {
            int idx = j * tex.dims.X + i;
            COORD position = {pos.X + i, pos.Y + j};
            draw_char(tex.chars[idx], position);
        }
    }
}

Colour letter_to_colour(char hex){
    switch(hex){
        case 'k':return BLACK;
        case 'b':return DARK_BLUE;
        case 'g':return DARK_GREEN;
        case 'c':return DARK_CYAN;
        case 'r':return DARK_RED;
        case 'm':return DARK_MAGENTA;
        case 'y':return DARK_YELLOW;
        case 'w':return GREY;
        case 'K':return DARK_GREY;
        case 'B':return BLUE;
        case 'G':return GREEN;
        case 'C':return CYAN;
        case 'R':return RED;
        case 'M':return MAGENTA;
        case 'Y':return YELLOW;
        case 'W':return WHITE;
    }
}

void read_texture_file(std::string filename, COORD& dims, std::string& characters, std::string& foreground, std::string& background){
    std::fstream file;
    file.open(filename);
    short width, height;
    file >> width >> height;
    char tmp;
    for (short i = 0; i < height * width; i++) {
        file >> tmp;
        characters += tmp;
    }
    for (short i = 0; i < height * width; i++) {
        file >> tmp;
        foreground += tmp;
    }
    for (short i = 0; i < height * width; i++) {
        file >> tmp;
        background += tmp;
    }
    file.close();
    dims = {width, height};
}

Texture texture_from_strings(COORD dims, std::string characters, std::string foreground, std::string background){
    int width = dims.X;
    int height = dims.Y;
    Char* chars = new Char[width * height];
    for (int i = 0; i < height * width; i++){
        if(foreground[i] == '.' && background[i] != '.'){foreground[i] = background[i];}
        else if(foreground[i] != '.' && background[i] == '.'){background[i] = foreground[i];}
        else if(foreground[i] == '.' && background[i] == '.'){continue;}
        chars[i].chr = characters[i];
        chars[i].pal = make_palette(
            letter_to_colour(foreground[i]),
            letter_to_colour(background[i])
        );
    }
    Texture t;
    t.chars = chars;
    t.dims = dims;
    return t;
}

Texture texture_from_file(std::string filename) {
    COORD dims;
    std::string characters, foreground, background;
    read_texture_file(filename, dims, characters, foreground, background);
    return texture_from_strings(dims, characters, foreground, background);
}

}  // namespace CDL
