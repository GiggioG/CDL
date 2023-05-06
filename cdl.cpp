#pragma once
#include <windows.h>
#include <iostream>
#include <algorithm>
#include <string>

namespace CDL {
SMALL_RECT canvas;
COORD canvasSize;
COORD topLeftCoord = {0, 0};
int linePlotScaleFactor = 1;

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
    CInfoAttrib getCInfoAttrib() const {
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
    CHAR_INFO getCInfo() const {
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
void drawChar(const Char& c, COORD coord, bool screenCoords = false){
    CHAR_INFO ci = c.getCInfo();

    if(!screenCoords){
        coord.X -= topLeftCoord.X;
        coord.Y -= topLeftCoord.Y;
    }

    SMALL_RECT rect;
    rect.Left = canvas.Left + coord.X;
    rect.Top = canvas.Top + coord.Y;
    rect.Right = rect.Left;
    rect.Bottom = rect.Top;

    if(rect.Left < canvas.Left || rect.Right > canvas.Right){ return; }
    if(rect.Top < canvas.Top || rect.Bottom > canvas.Bottom){ return; }

    WriteConsoleOutput(GetStdHandle(STD_OUTPUT_HANDLE), &ci, {1, 1}, {0, 0}, &rect);
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
        chars = new Char;
        *chars = c;
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

void drawTexture(const Texture& t, COORD coord, bool screenCoords=false){
    CHAR_INFO* ci = new CHAR_INFO[t.dims.X*t.dims.Y];
    for(int i = 0; i < t.dims.X*t.dims.Y; i++){
        ci[i] = t.chars[i].getCInfo();
    }

    if(!screenCoords){
        coord.X -= topLeftCoord.X;
        coord.Y -= topLeftCoord.Y;
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
Texture* axesTexture;

void calculateAxesTexture(){
    int x0 = -topLeftCoord.X;
    int y0 = -topLeftCoord.Y;
    int dx, dy;
    for(int x = 0; x < axesTexture->dims.X; x++){
        for(int y = 0; y < axesTexture->dims.Y; y++){
            dx = x-x0;
            dy = y-y0;

            char c = '\0';
            if(x == x0){ c = '|'; }
            if(y == y0){ c = '-'; }
            if(x == x0 && y == y0){ c = '+'; }

            Colour bg = BLACK;//(!((dx%2==0) ^ (dy%2==0) )?DARK_GREY:BLACK);
            if(dx%5 == 0 && y == y0){ c = '+'; }
            if(dy%5 == 0 && x == x0){ c = '+'; }

            axesTexture->chars[y*axesTexture->dims.X + x] = Char(c, Palette(GREY, bg));
        }
    }
}

void init(SMALL_RECT rect) {
    canvas = rect;
    canvasSize.X = canvas.Right - canvas.Left + 1;
    canvasSize.Y = canvas.Bottom - canvas.Top + 1;

    blankTexture = new Texture(canvasSize);
    for (int i = 0; i <= canvas.Bottom; i++) {
        std::cout << '\n';
    }

    axesTexture = new Texture(canvasSize);
    calculateAxesTexture();
}

void setTopLeft(COORD topLeft){
    topLeftCoord = topLeft;
    calculateAxesTexture();
}
void exit(){
    delete blankTexture;
    delete axesTexture;
}

void clearCanvas(){
    drawTexture(*blankTexture, {0, 0}, true);
}
void drawAxes(){
    drawTexture(*axesTexture, {0, 0}, true);
}
void plot(COORD point, bool screenCoords=false){
    drawChar(Char('@'), point, screenCoords);
}
void line(COORD a, COORD b, bool screenCoords=false){
    if(a.X > b.X){ std::swap(a, b); }
    int ax = a.X, ay = a.Y;
    int bx = b.X, by = b.Y;

    ax *= linePlotScaleFactor, ay *= linePlotScaleFactor;
    bx *= linePlotScaleFactor, by *= linePlotScaleFactor;

    int dx = bx - ax;
    int dy = by - ay;
    double d;
    if(dx != 0){
        d = ((double)dy/(double)dx);
    }else{
        d = 0;
    }
    int u;
    if(ay <= by){u = 1;}
    else{u = -1;}

    if(ax == bx){
        for(int y = ay; y != by+u; y += u){
            plot({ax, y}, screenCoords);
        }
        return;
    }

    double y = ay;
    double prevy = y;
    for(int x = ax; x <= bx; x++){
        int ry = round(y);
        int rp = round(prevy);

        Palette pal = Palette(abs(x%5)+1, BLACK);
        plot({x, ry}, screenCoords);

        bool skip = true;
        for(int i = rp; i != ry; i += u){
            if(skip){ skip = false; continue; }
            plot({x, i}, screenCoords);
        }

        prevy = y;
        y += d;
    }
}

void setLinePlotScaleFactor(int factor){
    linePlotScaleFactor = factor;
}
}  // namespace CDL

// #include <bits/stdc++.h>

// int main(){
//    SMALL_RECT rect;
//    rect.Top = 0;
//    rect.Left = 0;
//    rect.Bottom = 40;
//    rect.Right = 50;
//    CDL::init(rect);

//    CDL::setTopLeft({-20, -20});

//    CDL::drawAxes();
//    std::vector<COORD> v = {
//        {-1, 2},
//        {1, -1},
//        {0, -3},
//        {3, -1},
//        {1, 2}
//    };
//    int n = v.size();
//    for(int i = 0; i < n; i++){
//        COORD a = v[i];
//        COORD b = v[(i+1)%n];
//        CDL::line(a, b);
//    }

//    INPUT_RECORD irInBuf[128];
//    HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
//    DWORD fdwMode = ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT;
//    SetConsoleMode(hStdin, fdwMode);
//    DWORD cNumRead;
//    while(true){
//        ReadConsoleInput(
//                    hStdin,      // input buffer handle
//                    irInBuf,     // buffer to read into
//                    1,         // size of read buffer
//                    &cNumRead
//        );
//        if(cNumRead){
//            INPUT_RECORD ev = irInBuf[0];
//            if(ev.EventType != MOUSE_EVENT){ continue; }
//            //std::cout << ev.Event.MouseEvent.dwMousePosition.X << " " << ev.Event.MouseEvent.dwMousePosition.Y << '\n';
//            CDL::drawAxes();
//            COORD c = ev.Event.MouseEvent.dwMousePosition;
//            c.X += CDL::topLeftCoord.X;
//            c.Y += CDL::topLeftCoord.Y;
//            CDL::line({0, 0}, c);
//        }
//    }

//    return 0;
// }
