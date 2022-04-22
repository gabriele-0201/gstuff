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

    char* fontName;
    int fontSize;

    int interlineSpace; // px

    int winWidth; // Calc after
    int winHeight; // Calc after
    char** text;
    int nLines;

} style;

struct Point {
    int x, y;
} corner;

void init();
void close();
unsigned long RGB(int r, int g, int b);
//void loadConfig(std::string name);
XFontStruct* getFont();

int main(int argc, char **argv) {

    if(argc == 1) {
        std::cout << "Not specified text" <<std::endl;
        exit(0);
    }

    // Load Config
    style.background = RGB(0, 0, 0);
    style.borderColor = RGB(255, 0, 0);
    style.textColor = RGB(255, 255, 255);
    style.duration = 1000;
    style.padding = 4;
    style.border = 5;
    style.position = Pos::TOP_LEFT;
    style.paddingInside = 30;

    style.fontSize = 20;
    style.fontName = "roboto condensed";
    style.interlineSpace = 3;

    // Now I have to divide the first argument and all the others
    
    // skip at least the namefile
    style.nLines = argc - 1;
    int start = 1;
    if(strlen(argv[1]) > 2 && argv[1][0] == '-' && argv[1][0] == '-') {
        if(argc == 2) {
            std::cout << "Not specified text" <<std::endl;
            exit(0);
        }
        style.nLines = argc - 2; 
        start = 2;
    }
        //std::cout << argv[0] << " " <<argv[1] <<std::endl;
    
    // create a new array of char* with only the lines
    char* lines[style.nLines];
    for(int i = start; i < argc; ++i) 
        lines[i - start] = argv[i];

    style.text = lines;

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

    // Load font and calc size window
    XFontStruct* font = getFont();

    for(int i = 0; i < style.nLines; ++i) {
        int currentWidth = XTextWidth(font, style.text[i], strlen(style.text[i])) + (2 * style.paddingInside);
        style.winWidth = style.winWidth < currentWidth ? currentWidth : style.winWidth;
    }

    // ascent -> pixel up base line
    // descent -> pixel down base line
    int heightCharaceter = font -> ascent + font -> descent;
    style.winHeight =  style.nLines * (heightCharaceter + style.interlineSpace) + (2 * style.paddingInside);
    
    // calc corner near angle
    Screen*  s = DefaultScreenOfDisplay(dis);
    corner.x = ((s -> width) * style.padding) / 100; // x : width = padding : 100
    corner.y = ((s -> height) * style.padding) / 100;

    switch(style.position) {
        case Pos::TOP_LEFT: // default
            break;
        case Pos::TOP_RIGHT:
            corner.x = (s -> width) - style.winWidth - corner.x;
            break;
        case Pos::BOTTOM_LEFT:
            corner.y = (s -> height) - style.winHeight - corner.y;
            break;
        case Pos::BOTTOM_RIGHT:
            corner.x = (s -> width) - style.winWidth - corner.x;
            corner.y = (s -> height) - style.winHeight - corner.y;
            break;
    }

    // Create the window
    win=XCreateSimpleWindow(dis, DefaultRootWindow(dis), corner.x, corner.y, style.winWidth, style.winHeight, style.border ,style.borderColor, style.background);
    
    XSetStandardProperties(dis, win, "gsfuff", "", None, NULL, 0, NULL);
    XSelectInput(dis, win, ExposureMask | ButtonPressMask | KeyPressMask);
    
    gc=XCreateGC(dis, win, 0,0);
    
    XSetBackground(dis,gc,white);
    XSetForeground(dis,gc,black);

    // Set non managed window
    XSetWindowAttributes set_attr;
    set_attr.override_redirect = True;
    XChangeWindowAttributes(dis, win, CWOverrideRedirect, &set_attr);
    
    // Map the window on screen
    XMapWindow(dis, win);
    
    // on top
    XClearWindow(dis, win);
    XMapRaised(dis, win);
    
    // Text on the window
    XSetFont (dis, gc, font->fid);
    XSetForeground(dis,gc,style.textColor);

    for(int i = 0; i < style.nLines; ++i) {
        XDrawString(dis, win, gc, style.paddingInside, style.paddingInside + font -> ascent + (i * (font -> descent + style.interlineSpace + font -> ascent)) , style.text[i], strlen(style.text[i]));
    }

}

/* Set up the text font. */
XFontStruct* getFont()
{
    //const char * fontname = "-*-roboto condensed-medium-r-normal--0-0-0-0-p-0-adobe-standard";
    char fontname[255] = "-*-";
    //strncat("-*-", style.fontName, "-medium-r-normal--0-", sprintf("" ,style.fontSize * 10) ,"-0-0-p-0-iso10646-1");
    strcat(fontname, style.fontName);
    strcat(fontname, "-medium-r-normal--0-");
    // enough?
    char converted[10];
    sprintf(converted, "%d", style.fontSize * 10);
    strcat(fontname, converted);
    strcat(fontname,"-0-0-p-0-iso10646-1");

    XFontStruct * font = XLoadQueryFont (dis, fontname);
    /* If the font could not be loaded, revert to the "fixed" font. */
    if (! font) {
        fprintf (stderr, "unable to load font %s: using fixed\n", fontname);
        font = XLoadQueryFont (dis, "fixed");
    }

    return font;
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

