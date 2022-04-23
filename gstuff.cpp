#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <string.h>

#include <unistd.h>
#include <pwd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <errno.h>
#include <dirent.h>


#define DEBUG


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

struct stringSlice{
	int start;	// Inclusive
	int end;	// Not inclusive
};

void init();
void close();
unsigned long RGB(int r, int g, int b);
XFontStruct* getFont();
bool loadConfig(std::string configName);
void loadDefaultConfig();

int main(int argc, char* argv[]) {

    if(argc == 1) {
        std::cout << "Not specified text" <<std::endl;
        exit(0);
    }

    //Now I have to divide the first argument and all the others
	std::string configName = "config_template";
    
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

        //configName = argv[1].substr(2, strlen(argv[1]) - 2);
        //memcpy(&configName[0], &argv[1][2], strlen(argv[1]) - 2);
        configName = std::string(argv[1]).substr(2);
    }
    
    // create a new array of char* with only the lines
    char* lines[style.nLines];
    for(int i = start; i < argc; ++i) 
        lines[i - start] = argv[i];

    style.text = lines;

    // Load Config
    std::cout  << "Config Name File: " << configName <<std::endl;
    if(!loadConfig(configName)){
        loadDefaultConfig();
    }

    init();
    XEvent event;

    XFlush(dis);

    struct timeval current;
    struct timeval init;
    gettimeofday(&current, NULL);
    gettimeofday(&init, NULL);
    long microDur = style.duration * 1000, passed;

    do {
        usleep(100000); // 100 ms
        gettimeofday(&current, NULL);
        passed = (long)((current.tv_sec - init.tv_sec) * 1000000 + current.tv_usec - init.tv_usec);

        std::cout << passed <<std::endl;
    
        while(XPending(dis))
            XNextEvent(dis, &event);

        if(event.type==ButtonPress) 
            close();
    } while(passed < microDur);
    
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

    int borderDim = style.border * 2;

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
            corner.x = (s -> width) - style.winWidth - borderDim - corner.x;
            break;
        case Pos::BOTTOM_LEFT:
            corner.y = (s -> height) - style.winHeight - borderDim - corner.y;
            break;
        case Pos::BOTTOM_RIGHT:
            corner.x = (s -> width) - style.winWidth - borderDim - corner.x;
            corner.y = (s -> height) - style.winHeight - borderDim - corner.y;
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

void loadDefaultConfig(){
	style.background = RGB(0, 0, 0);
    style.borderColor = RGB(255, 0, 0);
    style.textColor = RGB(255, 255, 255);
    style.duration = 1000;
    style.padding = 4;
    style.border = 5;
    style.position = Pos::TOP_LEFT;
    style.paddingInside = 30;
    style.fontSize = 20;
    style.fontName = "arial black";
    style.interlineSpace = 3;
}


void getConfigKeySlice(const char* s, stringSlice* keySlice){
	
	int index=0;

	// Find first valid charachter of line
	for(; index<strlen(s); index++){
		if(s[index] != ' '){
			keySlice->start = index;
			break;
		}
	}	

	// Scan the rest of the line to find the separator
	for(; index<strlen(s); index++){
		if(s[index] == ':'){
			keySlice->end = index;
			return;
		}
	}

	// Ureachable if there is all we want
	keySlice->end 	= 0;
}


void getConfigValueSlice(const char* s, stringSlice* valSlice, int startIndex){
	
	int index=startIndex;

	// Find first valid charachter of line
	for(; index<strlen(s); index++){
		if(s[index] != ' ' && s[index] != ':' && s[index] != '	'){
			valSlice->start = index;
			break;
		}
	}	

	// Scan the rest of the line to find the end
	for(; index<=strlen(s); index++){
		if(s[index] == ' ' || s[index] == '\n' || s[index] == '\r' || s[index] == 0x0){
			valSlice->end = index;
			return;
		}
	}

	// Ureachable if there is all we want
	valSlice->end 	= 0;
}


bool strSlice_equal(const char* line, const char* s, stringSlice* slice){

	if(slice->end - slice->start != strlen(s))
		return false;

	int i_s = 0;
	for(int i_line=slice->start; i_line<slice->end; i_line++){
		if(line[i_line] != s[i_s++])
			return false;
	}

	return true;

}


// Returns true if succeded
bool createDir(const char* base_path, const char* dirname){

	DIR *base_dir;
	if((base_dir = opendir(base_path)) == NULL){
		// directory in base_path does not exist
		#ifdef DEBUG
			printf("%s does not exist, loading default config", base_path);
		#endif
		return false;
	}

	int dfd = dirfd(base_dir);
	errno = 0;
	int ret = mkdirat(dfd, dirname, S_IRWXU);
	if (ret == -1) {
		switch (errno) {
			case EACCES :
				#ifdef DEBUG
					printf("Failed creating %s directory, loading default config", dirname);
				#endif
				return false;
			case EEXIST:
				// direcory already exists
			break;
			case ENAMETOOLONG:
				printf("Pathname is too long, loading default config");
				return false;
			default:
				#ifdef DEBUG
					perror("mkdir");
				#endif
				return false;
		}

	}
	closedir(base_dir);

	return true;
}

void checkAndSet(int* prop, const char* propName, int defaultValue, const char* startPtr, const char* endPtr){
	if(startPtr == endPtr || errno != 0){
		*prop = defaultValue;
		#ifdef DEBUG
			printf("Invalid option for %s, loading default\n", propName);
		#endif
	}
}

// Returns true if succeded
bool loadConfig(std::string configName){

	configName += ".conf";

	// Get .config/ directory path of user that is launching the script
	std::string configDir_path = getpwuid(getuid())->pw_dir +  std::string("/.config");


	if(!createDir(configDir_path.c_str(), "gstuff"))
		return false;
	
	configDir_path += "/gstuff/";

	std::string configFile_path = configDir_path + configName;

    std::cout << "Namefile with path "<< configFile_path <<std::endl;

	FILE *config_file;

	config_file = fopen(configFile_path.c_str(), "r");

	if(config_file == NULL){
		#ifdef DEBUG
			printf("Error opening config file, loading default");
		#endif
		return false;
	}

	char* line = NULL;
	size_t len = 0;
	ssize_t linelenght;
	int lineindex = 0;
	

	while( (linelenght = getline(&line, &len, config_file)) != -1){

		lineindex++;

		// Skip void lines
		if(strcmp(line, "\n") == 0 || strlen(line) == 0)
			continue;

		stringSlice keySlice;
		getConfigKeySlice(line, &keySlice);

		stringSlice valSlice;
		getConfigValueSlice(line, &valSlice, keySlice.end);

		char* endptr = NULL;
		errno = 0;

		if(keySlice.end == 0 || valSlice.end == 0){
			#ifdef DEBUG
				printf("Line %d has an invalid syntax, skipping\n", lineindex);
			#endif

			continue;
		}

		if(strSlice_equal(line, "position", &keySlice)){

			if(strSlice_equal(line, "TOP_LEFT", &valSlice))
				style.position = TOP_LEFT;
			
			else if(strSlice_equal(line, "TOP_RIGHT", &valSlice))
				style.position = TOP_RIGHT;
			
			else if(strSlice_equal(line, "BOTTOM_LEFT", &valSlice))
				style.position = BOTTOM_LEFT;
			
			else if(strSlice_equal(line, "BOTTOM_RIGHT", &valSlice))
				style.position = BOTTOM_RIGHT;

			else{
				#ifdef DEBUG
					printf("Invalid option for position, skipping\n");
				#endif
				style.position = Pos::TOP_LEFT;
			}
		}

		else if(strSlice_equal(line, "background", &keySlice)){
			style.background = strtol(line + valSlice.start + 1, &endptr, 16);
			checkAndSet(&style.background, "background", RGB(0, 0, 0), line + valSlice.start + 1, endptr);
		}
		else if(strSlice_equal(line, "borderColor", &keySlice)){
			style.borderColor = strtol(line + valSlice.start + 1, &endptr, 16);
			checkAndSet(&style.borderColor, "borderColor", RGB(255, 0, 0), line + valSlice.start + 1, endptr);
		}
		else if(strSlice_equal(line, "textColor", &keySlice)){
			style.textColor = strtol(line + valSlice.start + 1, &endptr, 16);
			checkAndSet(&style.textColor, "textColor", RGB(255, 255, 255), line + valSlice.start + 1, endptr);
		}
		else if(strSlice_equal(line, "duration", &keySlice)){
			style.duration = strtol(line + valSlice.start, &endptr, 0);
			checkAndSet(&style.duration, "duration", 1000, line + valSlice.start, endptr);
		}
		else if(strSlice_equal(line, "padding", &keySlice)){
			style.padding = strtol(line + valSlice.start, &endptr, 0);
			checkAndSet(&style.padding, "padding", 4, line + valSlice.start, endptr);
		}
		else if(strSlice_equal(line, "paddingInside", &keySlice)){
			style.paddingInside = strtol(line + valSlice.start, &endptr, 0);
			checkAndSet(&style.paddingInside, "paddingInside", 30, line + valSlice.start, endptr);
		}
		else if(strSlice_equal(line, "border", &keySlice)){
			style.border = strtol(line + valSlice.start, &endptr, 0);
			checkAndSet(&style.border, "border", 5, line + valSlice.start, endptr);
		}
		else if(strSlice_equal(line, "interlineSpace", &keySlice)){
			style.interlineSpace = strtol(line + valSlice.start, &endptr, 0);
			checkAndSet(&style.interlineSpace, "interlineSpace", 3, line + valSlice.start, endptr);
		}
		else if(strSlice_equal(line, "fontSize", &keySlice)){
			style.fontSize = strtol(line + valSlice.start, &endptr, 0);
			checkAndSet(&style.fontSize, "fontSize", 20, line + valSlice.start, endptr);
		}

		else if(strSlice_equal(line, "fontName", &keySlice)){
			style.fontName = (char*)malloc(sizeof(char) * (valSlice.end - valSlice.start + 1));
			strcpy(style.fontName, line + valSlice.start);
			if(style.fontName[strlen(style.fontName) - 1] == '\n')
				style.fontName[strlen(style.fontName) - 1] = 0x0;
		}

		else{
			#ifdef DEBUG
				printf("Option at line %d not recognised, skipping\n", lineindex);
			#endif
		}

	}

	fclose(config_file);

	return true;
}
