#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/extensions/Xrandr.h>
#include <X11/Xft/Xft.h>

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <string.h>

#include <unistd.h>
#include <pwd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <errno.h>
#include <dirent.h>

// Uncomment next line to have debug messages printed to stdout
#define DEBUG

// Debug macros
#ifdef DEBUG
	#define debug_print(...) do { printf(__VA_ARGS__); } while(0);
#endif
#ifndef DEBUG
	#define debug_print(fmt, ...) do { } while(0);
#endif

#define USAGE "Usage: gstuff --myConfig \"first line\" \"second line\""


// Enums and structs
enum Pos {
    TOP_LEFT,
    TOP_RIGHT,
    BOTTOM_LEFT,
    BOTTOM_RIGHT,
    CENTER
};
struct Style {
    char* background= 	strdup("#323200");
	char* borderColor= 	strdup("#FFFF44");
	char* textColor=	strdup("#FFFFFF");

    int duration= 		2000; //milliseconds
    int padding= 		4;	//%
    int paddingInside=	15;	//px
    int border= 		5;	//px
    int interlineSpace=	3;	//px

    Pos position= 		Pos::BOTTOM_RIGHT;

    char* fontName= 	strdup("monospace");
    int fontSize= 		25;

	// The following entries will be calculated later
	// depending on the input
    int winWidth;
    int winHeight;
    char** text;
    int nLines;

} style;

struct Point {
    int x, y;
} corner;

struct StringSlice{
	int start;	// Inclusive
	int end;	// Not inclusive
};


// Global variables
int screen;
Display *dis;
Window win;
XSetWindowAttributes windowAttributes;
Visual *visual;
Colormap colormap;
XftColor color;
XftFont *font;


// Function declarations
void init();
void close();
void setFont();
void calcWindowDimension(XftFont* font);
void calcCornerPosition();
bool loadConfig(const std::string& configName);
void setWindowAttributes();


int main(int argc, char* argv[]) {

	// argc = 3;
	// argv[1] = "--bella";
	// argv[2] = "testo";

    if(argc <= 1) {
        printf(USAGE);
        exit(0);
    }

    // Default config name, Maybe not necessary?
	std::string configName = "config_template";
    
    // Skip at least the name of the executable
    style.nLines = argc - 1;
    int start = 1;
    if(strlen(argv[1]) > 2 && argv[1][0] == '-' && argv[1][0] == '-') {
        if(argc == 2) {
            printf(USAGE);
            exit(0);
        }
        style.nLines = argc - 2; 
        start = 2;

        // Get the passed configName, remove the first two characters
        configName = std::string(argv[1]).substr(2);
    }
    
    // create a new array of char* with only the lines
    char* lines[style.nLines];
    for(int i = start; i < argc; ++i) 
        lines[i - start] = argv[i];
    style.text = lines;

	// Load config file into style struct
    loadConfig(configName);

    // Init the window
    init();

    // Event Handler
    XEvent event;
    XFlush(dis);

    // Manage Duration
    struct timeval currentTime;
    struct timeval initTime;
    gettimeofday(&initTime, NULL);
    long microDur = style.duration * 1000, passed;

    do {
        usleep(100000); // 100 ms
        gettimeofday(&currentTime, NULL);
        passed = (long)((currentTime.tv_sec - initTime.tv_sec) * 1000000 + currentTime.tv_usec - initTime.tv_usec);
    
        while(XPending(dis))
            XNextEvent(dis, &event);

        if(event.type==ButtonPress) 
            close();
    } while(passed < microDur);
    
    return 0;
}

void init() {

    // TODO - select the right display to show the window, for now always the primary
    if ( (dis=XOpenDisplay((char *)0)) == NULL )
		fprintf(stderr, "Fatal: cannot open display");

    screen=DefaultScreen(dis);

	visual = DefaultVisual(dis, screen);
	colormap = DefaultColormap(dis, screen);
	
	setFont();
	
	setWindowAttributes();

    calcWindowDimension(font);
    
    calcCornerPosition();

	win = XCreateWindow(dis, RootWindow(dis, screen), corner.x, corner.y, style.winWidth, style.winHeight, style.border, DefaultDepth(dis, screen),
						   CopyFromParent, visual, CWOverrideRedirect | CWBackPixel | CWBorderPixel, &windowAttributes);

	XSetStandardProperties(dis, win, "gsfuff", "", None, NULL, 0, NULL);
    XSelectInput(dis, win, ExposureMask | ButtonPressMask | KeyPressMask);

    XChangeWindowAttributes(dis, win, CWOverrideRedirect, &windowAttributes);
    
    // Map the window on screen
    XMapWindow(dis, win);
    
    // Display the window on top of any other
    XClearWindow(dis, win);
    XMapRaised(dis, win);

	XftDraw *draw = XftDrawCreate(dis, win, visual, colormap);

    // Print the lines
    for(int i = 0; i < style.nLines; ++i) {
		XftDrawStringUtf8(draw, &color, font, style.paddingInside, style.paddingInside + font -> ascent + (i * (font -> descent + style.interlineSpace + font -> ascent)),
								  (FcChar8 *)style.text[i], strlen(style.text[i]));
    }
}

/* 
 * Set up the text font
 * */
void setFont()
{
	const std::string fontAndSize = style.fontName + std::string(":size=") + std::to_string(style.fontSize);

	font = XftFontOpenName(dis, screen, fontAndSize.c_str());
    if (! font) {
		// TODO: set default font
		fprintf(stderr, "Fatal: cannot load font");
		exit(1);
    }
}

/* 
 * Sets the window attributes
 * */
void setWindowAttributes(){
    // Non managed window
	windowAttributes.override_redirect = True;

	// Allocate colors
	XftColorAllocName(dis, visual, colormap, style.background, &color);
	windowAttributes.background_pixel = color.pixel;

	XftColorAllocName(dis, visual, colormap, style.borderColor, &color);
	windowAttributes.border_pixel = color.pixel;

	XftColorAllocName(dis, visual, colormap, style.textColor, &color);
}

/*
 * Dependig on the text and the font size calc the dimension of the window
 * Always the minimum possible
 * */
void calcWindowDimension(XftFont* font) {

	int linePixelLenght = 0;
	XGlyphInfo info;

    // Check each line to calc the max width
    for(int i = 0; i < style.nLines; ++i) {
		XftTextExtentsUtf8(dis, font, (FcChar8 *)style.text[i], strlen(style.text[i]), &info);
        linePixelLenght = linePixelLenght < info.width ? info.width : linePixelLenght;
	}

	style.winWidth = linePixelLenght;
	style.winWidth += 2*style.paddingInside;

    // ascent -> pixel up base line
    // descent -> pixel down base line
    int heightCharaceter = font -> ascent + font -> descent;
    style.winHeight =  style.nLines * (heightCharaceter + style.interlineSpace) + (2 * style.paddingInside);
}

/*
 * Based on the position specified and the window dimension calculate the position 
 * of the corner to display the window in the right place
 * */
void calcCornerPosition() {

    // calc top left corner position
    int borderDim = style.border * 2;

    // get the dim of the primary display
    XRRScreenResources *screens = XRRGetScreenResources(dis, DefaultRootWindow(dis));
    XRRCrtcInfo *crtc_info = NULL;
	XRROutputInfo *out_info = NULL;
	const RROutput primary = XRRGetOutputPrimary(dis, DefaultRootWindow(dis));

    int width, height, n_screens=0;

    for(int i=0; i<screens->noutput; i++){

		out_info = XRRGetOutputInfo(dis, screens, screens->outputs[i]);

		if(out_info->connection == RR_Connected){
			
			crtc_info = XRRGetCrtcInfo(dis, screens, screens->crtcs[n_screens++]);

			if(primary == screens->outputs[i]){
			    width = crtc_info->width;
			    height = crtc_info->height;
			}

			XRRFreeCrtcInfo(crtc_info);
		}
	}
    XRRFreeScreenResources(screens);

    corner.x = (width * style.padding) / 100; // x : width = padding : 100
    corner.y = (height * style.padding) / 100;

    switch(style.position) {
        case Pos::TOP_LEFT: 
			// default
        break;
        case Pos::TOP_RIGHT:
            corner.x = (width) - style.winWidth - borderDim - corner.x;
        break;
        case Pos::BOTTOM_LEFT:
            corner.y = (height) - style.winHeight - borderDim - corner.y;
        break;
        case Pos::BOTTOM_RIGHT:
            corner.x = (width) - style.winWidth - borderDim - corner.x;
            corner.y = (height) - style.winHeight - borderDim - corner.y;
        break;
        case Pos::CENTER:
            corner.x = ((width) / 2) - ((style.winWidth - borderDim) / 2) ;
            corner.y = ((height) / 2)  - ((style.winHeight - borderDim) / 2);
        break;
    }
}

/* 
 * Close the window
 * */
void close() {
    XDestroyWindow(dis, win);
	XftFontClose(dis, font);
    XCloseDisplay(dis);
    exit(0);
}


/* 
 * Get the correct slice that represents the key in
 * the config file's line.
 * The separator is the charachter ':'.
 * It sets keySlice.end to 0 if it does not find a valid key
 * */
void getConfigKeySlice(const char* config_line, StringSlice* keySlice){
	
	int index=0;

	// Find first valid charachter of line
	for(; index<strlen(config_line); index++){
		if(config_line[index] != ' '){
			keySlice->start = index;
			break;
		}
	}	

	// Scan the rest of the line to find the separator
	for(; index<strlen(config_line); index++){
		if(config_line[index] == ':'){
			keySlice->end = index;
			return;
		}
	}

	// Ureachable if there is all we want
	keySlice->end = 0;
}

/* 
 * Get the correct slice that represents the value in
 * the config file's line.
 * The separator is the charachter ':'.
 * It sets keySlice.end to 0 if it does not find a valid key
 * */
void getConfigValueSlice(const char* config_line, StringSlice* valSlice, int startIndex){
	
	int index=startIndex;

	// Find first valid charachter of line
	for(; index<strlen(config_line); index++){
		if(config_line[index] != ' ' && config_line[index] != ':' && config_line[index] != '	'){
			valSlice->start = index;
			break;
		}
	}	

	// Scan the rest of the line to find the end
	for(; index<=strlen(config_line); index++){
		if(config_line[index] == ' ' || config_line[index] == '\n' || config_line[index] == '\r' || config_line[index] == 0x0){
			valSlice->end = index;
			return;
		}
	}

	// Ureachable if there is all we want
	valSlice->end 	= 0;
}

/* 
 * Cheks if the "to_check" string is equal to the slice of the "line" string
 * that starts in slice.start and ends in slice.end (end not inclusive)
 * */
bool strSlice_equal(const char* line, const char* to_check, StringSlice* slice){

	if(slice->end - slice->start != strlen(to_check))
		return false;

	int i_s = 0;
	for(int i_line=slice->start; i_line<slice->end; i_line++){
		if(line[i_line] != to_check[i_s++])
			return false;
	}
	return true;
}

/* 
 * Cheks the status of the "conversion to int" (done by "strtol") operation and if there are some errors
 * sets the "prop" integer to be the default value for that property
 * */
void print_strol_errors(const char* propName, const char* startPtr, const char* endPtr){
	if(startPtr == endPtr || errno != 0)
		debug_print("Invalid option for %s, loading default\n", propName);
}

/* 
 * Loads the config placed in $HOME/.config/<configname>.conf
 * Returns true if succeded
 * */
bool loadConfig(const std::string& configName){

	std::string configDir_path = getpwuid(getuid())->pw_dir + std::string("/.config/gstuff/");
	std::string configFile_path = configDir_path + configName + ".conf";

	FILE *config_file;
	config_file = fopen(configFile_path.c_str(), "r");

	// Error opening the file
	if(config_file == NULL){
		debug_print("Error opening config file, loading default");
		return false;
	}

	// Read file lines one by one and parse them
	char* line = NULL;
	size_t len = 0;
	ssize_t linelenght;
	int lineindex = 0;
	
	while( (linelenght = getline(&line, &len, config_file)) != -1){

		lineindex++;

		// Skip void lines
		if(strcmp(line, "\n") == 0 || strlen(line) == 0)
			continue;

		// Get the slices for key and value
		StringSlice keySlice;
		getConfigKeySlice(line, &keySlice);
		StringSlice valSlice;
		getConfigValueSlice(line, &valSlice, keySlice.end);

		// Error getting key or value
		if(keySlice.end == 0 || valSlice.end == 0){
			debug_print("Line %d has an invalid syntax, skipping\n", lineindex);
			continue;
		}

		// These variables are used for error handling
		char* endptr = NULL;
		errno = 0;

		// Parse the line and set the values accordingly
		if(strSlice_equal(line, "position", &keySlice)){

			if(strSlice_equal(line, "TOP_LEFT", &valSlice))
				style.position = TOP_LEFT;
			
			else if(strSlice_equal(line, "TOP_RIGHT", &valSlice))
				style.position = TOP_RIGHT;
			
			else if(strSlice_equal(line, "BOTTOM_LEFT", &valSlice))
				style.position = BOTTOM_LEFT;
			
			else if(strSlice_equal(line, "BOTTOM_RIGHT", &valSlice))
				style.position = BOTTOM_RIGHT;

			else if(strSlice_equal(line, "CENTER", &valSlice))
				style.position = CENTER;

			else{
				debug_print("Invalid option for position, skipping\n");
				style.position = Pos::TOP_LEFT;
			}
		}
		else if(strSlice_equal(line, "duration", &keySlice)){
			style.duration = strtol(line + valSlice.start, &endptr, 0);
			print_strol_errors("duration", line + valSlice.start, endptr);
		}
		else if(strSlice_equal(line, "padding", &keySlice)){
			style.padding = strtol(line + valSlice.start, &endptr, 0);
			print_strol_errors("padding", line + valSlice.start, endptr);
		}
		else if(strSlice_equal(line, "paddingInside", &keySlice)){
			style.paddingInside = strtol(line + valSlice.start, &endptr, 0);
			print_strol_errors("paddingInside", line + valSlice.start, endptr);
		}
		else if(strSlice_equal(line, "border", &keySlice)){
			style.border = strtol(line + valSlice.start, &endptr, 0);
			print_strol_errors("border", line + valSlice.start, endptr);
		}
		else if(strSlice_equal(line, "interlineSpace", &keySlice)){
			style.interlineSpace = strtol(line + valSlice.start, &endptr, 0);
			print_strol_errors("interlineSpace", line + valSlice.start, endptr);
		}
		else if(strSlice_equal(line, "fontSize", &keySlice)){
			style.fontSize = strtol(line + valSlice.start, &endptr, 0);
			print_strol_errors("fontSize", line + valSlice.start, endptr);
		}
		else if(strSlice_equal(line, "fontName", &keySlice)){
			std::string parsed{line + valSlice.start};
			if(parsed[parsed.length()-1] == '\n')
				parsed.pop_back();
			strcpy(style.fontName, parsed.c_str());
		}
		else if(strSlice_equal(line, "background", &keySlice)){
			std::string parsed{line + valSlice.start};
			if(parsed[parsed.length()-1] == '\n')
				parsed.pop_back();
			strcpy(style.background, parsed.c_str());
		}
		else if(strSlice_equal(line, "borderColor", &keySlice)){
			std::string parsed{line + valSlice.start};
			if(parsed[parsed.length()-1] == '\n')
				parsed.pop_back();
			strcpy(style.borderColor, parsed.c_str());
		}
		else if(strSlice_equal(line, "textColor", &keySlice)){
			std::string parsed{line + valSlice.start};
			if(parsed[parsed.length()-1] == '\n')
				parsed.pop_back();
			strcpy(style.textColor, parsed.c_str());
		}
		else{
			debug_print("Option at line %d not recognised, skipping\n", lineindex);
		}
	}

	fclose(config_file);

	return true;
}
