#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>

#include <stdio.h>
#include <stdlib.h>

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

    int winWidth; // Calc after
    int winHeight; // Calc after
    char* text;
} style;

struct Point {
    int x, y;
} corner;

void init();
void close();
unsigned long RGB(int r, int g, int b);
//void loadConfig(std::string name);
XFontStruct* geFont();

int main() {

    // Load Config
    style.background = RGB(0, 0, 0);
    style.borderColor = RGB(255, 0, 0);
    style.borderColor = RGB(255, 255, 255);
    style.duration = 1000;
    style.padding = 4;
    style.border = 5;
    style.position = TOP_LEFT;
    style.paddingInside = 30;

    style.fontSize = 15;
    style.fontName = "roboto condensed";

    style.text = "daskdhasjkldhasjldhasljkdhadjasdjkha das jhdljkas hdlkja hsdjklas hdjklh asljkdhajkld hasjkld h";

    init();
    XEvent event;
    char text[255];

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
    //int wWidth = 300, wHeight = 200;
    
    // calc corner near angle
    Screen*  s = DefaultScreenOfDisplay(dis);
    corner.x = ((s -> width) * style.padding) / 100; // x : width = padding : 100
    corner.y = ((s -> height) * style.padding) / 100;

    switch(style.position) {
        case TOP_LEFT: // default
            break;
        case TOP_RIGHT:
            corner.x = (s -> width) - style.winWidth - corner.x;
            break;
        case BOTTOM_LEFT:
            corner.y = (s -> height) - style.winHeight - corner.y;
            break;
        case BOTTOM_RIGHT:
            corner.x = (s -> width) - style.winWidth - corner.x;
            corner.y = (s -> height) - style.winHeight - corner.y;
            break;
    }

    // Calc size text
    //XTextItem text;
    //text.chars = &strText[0];
    //text.nchars = strText.length();
    //text.delta = 1;
    //text.font = None;
    
    XFontStruct* font = geFont();

    style.winWidth = XTextWidth(font, style.text, strlen(style.text)) + (2 * style.paddingInside);

    // ascent -> pixel up base line
    // descent -> pixel down base line
    int heightCharaceter = font -> ascent + font -> descent;
    style.winHeight =  heightCharaceter + (2 * style.paddingInside);
    
    // Create the window
    win=XCreateSimpleWindow(dis, DefaultRootWindow(dis), corner.x, corner.y, style.winWidth, style.winHeight, style.border ,style.borderColor, style.background);
    
    XSetStandardProperties(dis, win, "Howdy", "Hi", None, NULL, 0, NULL);
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
    //set_up_text();
    XSetFont (dis, gc, font->fid);

    XSetForeground(dis,gc,white);
    XDrawString(dis, win, gc, style.paddingInside, style.paddingInside + font -> ascent, style.text, strlen(style.text));

}

/* Set up the text font. */
XFontStruct* geFont()
{
    //const char * fontname = "-*-roboto condensed-medium-r-normal--0-0-0-0-p-0-adobe-standard";
    const char * fontname = strcat("-*-", style.fontName, "-medium-r-normal--0-", asjkdasjkldh ,"-0-0-p-0-iso10646-1");
    XFontStruct * font = XLoadQueryFont (dis, fontname);
    /* If the font could not be loaded, revert to the "fixed" font. */
    if (! font) {
        fprintf (stderr, "unable to load font %s: using fixed\n", fontname);
        font = XLoadQueryFont (dis, "fixed");
    }
    //XSetFont (dis, gc, font->fid);

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

