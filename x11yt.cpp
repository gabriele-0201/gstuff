#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>

#include <stdio.h>
#include <stdlib.h>
#include <iostream>

Display *dis;
int screen;
Window win;
GC gc;
unsigned long black, white, red, blue;

enum Pos {
    TOP_LEFT,
    TOP_RIGHT,
    BOTTOM_LEFT,
    BOTTOM_RIGHT
};

struct Style {
    int background, borderColor, textColor;
    int duration; // milliseconds
    int padding; // %
    int border; // px
    Pos position;

    int paddingInside; // px
} style;

struct Point {
    int x, y;
} corner;

std::string strText;

void init();
void close();
unsigned long RGB(int r, int g, int b);
void loadConfig(std::string name);

int main() {

    // Load Config
    style.background = RGB(0, 0, 0);
    style.borderColor = RGB(255, 0, 0);
    style.borderColor = RGB(255, 255, 255);
    style.duration = 1000;
    style.padding = 4;
    style.border = 2;
    style.position = Pos::TOP_LEFT;

    strText = "prova";

    init();
    XEvent event;

    while (1)
    {
        XNextEvent(dis, &event);
        if(event.type==ButtonPress)
            close();
    }
    
    return 0;
}

void init() {
    
    dis=XOpenDisplay((char *)0);
    screen=DefaultScreen(dis);
    
    black=BlackPixel(dis, screen);
    white=WhitePixel(dis, screen);
    red=RGB(255,0,0);
    blue=(0,0,255);

    // calc dimension of the window
    int wWidth = 100, wHeight = 100;
    
    // calc corner near angle
    Screen*  s = DefaultScreenOfDisplay(dis);
    corner.x = ((s -> width) * style.padding) / 100; // x : width = padding : 100
    corner.y = ((s -> height) * style.padding) / 100;

    switch(style.position) {
        case Pos::TOP_LEFT: // default
            break;
        case Pos::TOP_RIGHT:
            corner.x = (s -> width) - wWidth - corner.x;
            break;
        case Pos::BOTTOM_LEFT:
            corner.y = (s -> height) - wHeight - corner.y;
            break;
        case Pos::BOTTOM_RIGHT:
            corner.x = (s -> width) - wWidth - corner.x;
            corner.y = (s -> height) - wHeight - corner.y;
            break;
    }

    // Calc size text
    XTextItem text;
    text.chars = &strText[0];
    text.nchars = strText.length();
    text.delta = 1;
    //text.font = None;
    
    // Create the window
    win=XCreateSimpleWindow(dis, DefaultRootWindow(dis), corner.x, corner.y, wWidth, wHeight, style.border ,style.borderColor, style.background);
    
    XSetStandardProperties(dis, win, "Howdy", "Hi", None, NULL, 0, NULL);
    XSelectInput(dis, win, ExposureMask | ButtonPressMask | KeyPressMask);
    
    gc=XCreateGC(dis, win, 0,0);
    
    XSetBackground(dis,gc,white);
    XSetForeground(dis,gc,black);

    // draw text
    //XDrawText(dis, win, gc, 0, 0, strText, strText.length());
    XDrawString(dis, win, gc, 5, 5, &strText[0], strText.length());
    
    // Set non managed window
    XSetWindowAttributes set_attr;
    set_attr.override_redirect = True;
    XChangeWindowAttributes(dis, win, CWOverrideRedirect, &set_attr);
    
    // Map the window on screen
    XMapWindow(dis, win);
    
    // on top
    XClearWindow(dis, win);
    XMapRaised(dis, win);
}

void close() {
    XFreeGC(dis, gc);
    XDestroyWindow(dis, win);
    XCloseDisplay(dis);
    exit(0);
}

unsigned long RGB(int r, int g, int b) {
    return b + (g<<8) + (r<<16);
}

