// #if 1
#include <windows.h>
#include <iostream>
#include <string>
void draw(CHAR_INFO* texture, COORD texture_size, COORD screen_coords){
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SMALL_RECT screen_pos = {
        screen_coords.X, 
        screen_coords.Y, 
        screen_coords.X + texture_size.X,
        screen_coords.Y + texture_size.Y 
    };
    WriteConsoleOutput(hConsole, texture, texture_size, {0, 0}, &screen_pos);
}
CHAR_INFO* toTexture(std::string text, WORD colors){
    CHAR_INFO* ptr = new CHAR_INFO[text.length()];
    for(int i = 0; i < text.length(); i++){
        ptr[i].Char.AsciiChar = text[i];
        ptr[i].Attributes = colors;
    }
    return ptr;
}
void dr(int x, int y, UCHAR col){
    COORD t_size = {1,1};
    COORD crd = {x, y};
    CHAR_INFO* ptr = toTexture("#", col);
    draw(ptr, t_size, crd);
}
void cl(int x, int y){
    COORD t_size = {1,1};
    COORD crd = {x, y};
    CHAR_INFO* ptr = toTexture(" ", 0x00);
    draw(ptr, t_size, crd);
}
int main(){
    HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
    SetConsoleMode(hStdin, ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT);
    INPUT_RECORD rec2;
    INPUT_RECORD rec;
    DWORD read;
    while(true){
        rec2 = rec;
        ReadConsoleInput(hStdin, &rec, 1, &read);
        if(read){
            if(rec.EventType == MOUSE_EVENT){
                if(rec.Event.MouseEvent.dwButtonState == FROM_LEFT_1ST_BUTTON_PRESSED){
                    dr(rec.Event.MouseEvent.dwMousePosition.X, rec.Event.MouseEvent.dwMousePosition.Y, 0xbb);
                }
                if(rec.Event.MouseEvent.dwButtonState == RIGHTMOST_BUTTON_PRESSED){
                    dr(rec.Event.MouseEvent.dwMousePosition.X, rec.Event.MouseEvent.dwMousePosition.Y, 0xcc);
                }
                if(rec.Event.MouseEvent.dwButtonState == FROM_LEFT_2ND_BUTTON_PRESSED){
                    cl(rec.Event.MouseEvent.dwMousePosition.X, rec.Event.MouseEvent.dwMousePosition.Y);
                }
            }
        }
    }
    return 0;
}
// #endif