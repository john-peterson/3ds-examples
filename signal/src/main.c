#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <3ds.h>

void task_init();
FILE *f;

#include <stdarg.h>
void logw(const char *format, ...) {
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    vfprintf(f, format, args);
    va_end(args);
		fflush(f);
}

// If you define this function, you can monitor/debug APT events
// #ifdef LIBCTRU_APT_DEBUG
static bool debug = false;
void _aptDebug(int a, int b) {
	if (!debug) return;
	if (a==222) return;
	logw("_aptDebug(%d,%x)\n", a, b);
}


void click () {
			logw("pressed a\n");

	bool tmp;
	NS_APPID i = 0x300;
	APT_IsRegistered(i, &tmp);
	logw("registered=%d\n", tmp);
	logw("should close=%d\n", aptShouldClose());
}

int main(int argc, char* argv[]) {

	gfxInitDefault();
	consoleInit(GFX_TOP, NULL);
	/* fprintf(f, "aabc\n"); */
	/* fflush(f); */

	f = fopen("log", "w");
	/* perror(0); */
	logw("log start\n");

	debug = true;

	task_init();

	printf("Hello, world!\n");

	// Main loop
	while (aptMainLoop()) {
		gspWaitForVBlank();
		gfxSwapBuffers();
		hidScanInput();

		// Your code goes here
		u32 kDown = hidKeysDown();
		if (kDown & KEY_START)
			break; // break in order to return to hbmenu
		if (kDown & KEY_A) click();
		/* if (kDown & KEY_HOME) logw("pressed home\n"); */
	}

	gfxExit();
	fclose(f);
	return 0;
}
