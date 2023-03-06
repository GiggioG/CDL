#pragma once
#include <windows.h>
#include <iostream>
#include <algorithm>
#include <string>

namespace CDL {
SMALL_RECT canvas;
COORD canvasSize;

typedef UCHAR Colour;
const Colour BLACK        = 0x0;
const Colour DARK_BLUE    = 0x1;
const Colour DARK_GREEN   = 0x2;
const Colour DARK_CYAN    = 0x3;
const Colour DARK_RED     = 0x4;
const Colour DARK_MAGENTA = 0x5;
const Colour DARK_YELLOW  = 0x6;
const Colour GREY         = 0x7;
const Colour DARK_GREY    = 0x8;
const Colour BLUE         = 0x9;
const Colour GREEN        = 0xA;
const Colour CYAN         = 0xB;
const Colour RED          = 0xC;
const Colour MAGENTA      = 0xD;
const Colour YELLOW       = 0xE;
const Colour WHITE        = 0xF;

typedef WORD CInfoAttrib;
CInfoAttrib toCInfoAttrib(Colour foreground, Colour background){
    return foreground | (background << 4);
}
struct Palette{
    Colour fg = WHITE;
    Colour bg = BLACK;
    Palette(){}
    Palette(Colour foreground, Colour background){
        fg = foreground;
        bg = background;
    }
    Palette(CInfoAttrib cia){
        fg = cia & 0b1111;
        bg = cia >> 4;
    }
    Palette(const char* palData){
        // look at ascii table for reference
        fg = palData[0]-'0';
        if(fg > 9){ fg -= 7; }
        if(fg > 15){ fg -= 32; }
        bg = palData[1]-'0';
        if(bg > 9){ bg -= 7; }
        if(bg > 15){ bg -= 32; }
    }
    CInfoAttrib getCInfoAttrib(){
        return toCInfoAttrib(fg, bg);
    }
};

struct Char{
    char chr  = '\0';
    Palette pal = {WHITE, BLACK};
    Char(){}
    Char(char c){
        chr = c;
    }
    Char(char c, Palette p){
        chr = c;
        pal = p;
    }
    Char(CHAR_INFO ci){
        chr = ci.Char.AsciiChar;
        pal = Palette(ci.Attributes);
    }
    Char(const char* chrData){
        chr = chrData[0];
        pal = Palette(&chrData[1]);
    }
    CHAR_INFO getCInfo(){
        CHAR_INFO ret;
        ret.Char.AsciiChar = chr;
        ret.Attributes = pal.getCInfoAttrib();
        return ret;
    }
};
Char* charsFromText(std::string txt){
    Char* arr = new Char[txt.length()];
    for(int i = 0; i < txt.length(); i++){
        arr[i].chr = txt[i];
    }
    return arr;
}
Char* charsFromText(std::string txt, Palette p){
    Char* arr = new Char[txt.length()];
    for(int i = 0; i < txt.length(); i++){
        arr[i].chr = txt[i];
        arr[i].pal = p;
    }
    return arr;
}
struct Texture{
    Char* chars = nullptr;
    COORD dims = {0, 0};
    Texture(){}
    Texture(COORD dim){
        dims = dim;
        chars = new Char[dim.X*dim.Y];
    }
    Texture(COORD dim, Char* chrs){
        dims = dim;
        chars = chrs;
    }
    Texture(COORD dim, Char chr){
        dims = dim;
        chars = new Char[dim.X*dim.Y];
        for(int i = 0; i < dims.X*dims.Y; i++){
            chars[i] = chr;
        }
    }
    Texture(Char c){
        dims = {1, 1};
        chars = &c;
    }
    Texture(const char* txtData, bool deleteBuffer=false){
        int idx = 0;

        std::string sWidth;
        while(txtData[idx] != ','){
            sWidth += txtData[idx++];
        }
        dims.X = stoi(sWidth);
        idx++;

        std::string sHeight;
        while(txtData[idx] != ','){
            sHeight += txtData[idx++];
        }
        dims.Y = stoi(sHeight);
        idx++;

        int total = dims.X*dims.Y;
        chars = new Char[total];
        for(int i = 0; i < total; i++){
            chars[i] = Char(&txtData[idx + 3*i]);
        }

        if(deleteBuffer){
            delete[] txtData;
        }
    }
    ~Texture(){
        delete[] chars;
    }
};


char* readFile(std::string filename){
    HANDLE hFile = CreateFileA(filename.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    DWORD fileSize = GetFileSize(hFile, NULL);
    char* buff = new char[fileSize+1];
    buff[fileSize] = '\0';
    ReadFile(hFile, buff, fileSize, NULL, NULL);
    CloseHandle(hFile);
    return buff;
}

Texture loadTexture(std::string filename){
    return Texture(readFile(filename), true);
}

void drawTexture(const Texture& t, COORD coord){
    CHAR_INFO* ci = new CHAR_INFO[t.dims.X*t.dims.Y];
    for(int i = 0; i < t.dims.X*t.dims.Y; i++){
        ci[i] = t.chars[i].getCInfo();
    }

    SMALL_RECT rect;
    rect.Left = canvas.Left + coord.X;
    rect.Top = canvas.Top + coord.Y;
    rect.Right = canvas.Left + (coord.X + t.dims.X);
    rect.Bottom = canvas.Top + (coord.Y + t.dims.Y);

    COORD topLeft = {0, 0};

    SMALL_RECT drawRect;
    drawRect.Left = std::max(rect.Left, canvas.Left);
    drawRect.Top = std::max(rect.Top, canvas.Top);
    if(drawRect.Left != rect.Left){ topLeft.X += (drawRect.Left - rect.Left); }
    if(drawRect.Top != rect.Top){ topLeft.Y += (drawRect.Top - rect.Top); }

    drawRect.Right = std::min(rect.Right, canvas.Right);
    drawRect.Bottom = std::min(rect.Bottom, canvas.Bottom);

    WriteConsoleOutput(GetStdHandle(STD_OUTPUT_HANDLE), ci, t.dims, topLeft, &drawRect);
    delete[] ci;
}

Texture* blankTexture;

void init(SMALL_RECT rect) {
    canvas = rect;
    canvasSize.X = canvas.Right - canvas.Left + 1;
    canvasSize.Y = canvas.Bottom - canvas.Top + 1;
    blankTexture = new Texture(canvasSize);
    for (int i = 0; i <= canvas.Bottom; i++) {
        std::cout << '\n';
    }
}

void exit(){
    delete blankTexture;
}

void clear(){
    drawTexture(*blankTexture, {0, 0});
}
}  // namespace CDL