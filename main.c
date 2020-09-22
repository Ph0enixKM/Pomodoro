#include <libnotify/notify.h>
#include <X11/Xlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define INTERVAL 50
int minutes = 0;

void say(char* text) {
	notify_init ("Pomodoro Session");
	NotifyNotification *notif = notify_notification_new ("Timer Session", text, "/ware/phx/Pomodoro/icon.png");
	notify_notification_show (notif, NULL);
	g_object_unref(G_OBJECT(notif));
	notify_uninit();
}

void drawLogo(Display* display, int screen, Window window, XColor col) {
	XSetForeground(display, DefaultGC(display, screen), WhitePixel(display, screen));
	XFillRectangle(display, window, DefaultGC(display, screen), 0, 0, 128, 64);
	XSetForeground(display, DefaultGC(display, screen), col.pixel);
	XFillRectangle(display, window, DefaultGC(display, screen), 53, 20, 12, 12);
	XSetForeground(display, DefaultGC(display, screen), WhitePixel(display, screen));
	XFillRectangle(display, window, DefaultGC(display, screen), 56, 23, 6, 6);
}

int main() {
	XEvent event;
	char msg[128] = "0 Minutes left";

	Display *display = XOpenDisplay(NULL);
	if (display == NULL) {
		printf("Please use X11 environment\n");
		exit(1);
	}

	int screen = DefaultScreen(display);
	Window window = XCreateSimpleWindow(
		display, RootWindow(display, screen),
		10, 10,
		128, 64,
		1,
		BlackPixel(display, screen),
		WhitePixel(display, screen)
	);

	long int events = ExposureMask;
	XSelectInput(display, window, events);
	XMapWindow(display, window);

	char title[64] = "Pomodoro Timer";
	XStoreName(display, window, title);

	// Introdution
	char string[128];
	sprintf(string, "%d minutes on counting. Happy working!", INTERVAL);
	say (string);

	// Orange color
	XColor col;
	col.red = 65000; col.green = 32000; col.blue = 0;
	col.flags = DoRed | DoGreen | DoBlue;
	XAllocColor(display, DefaultColormap(display, screen), &col);

	// Load up the window
	XNextEvent(display, &event);
	XNextEvent(display, &event);

	while (minutes < INTERVAL) {
		while (XPending(display) > 0) {
			XNextEvent(display, &event);
		}
		int delta = INTERVAL - minutes;
		// If it's half way through
		if (minutes == (INTERVAL/2)) {
			sprintf(string, "%d minutes left. Half way through.", INTERVAL/2);
			say(string);
		}
		// If there is 5 minutes left
		if (minutes == (INTERVAL-5)) {
			say("5 minutes left. Save your work "
				"and close the computer. Remember"
				" to take a 20 minute break");
		}
		// If there is 60 seconds left
		if (minutes == (INTERVAL-1)) {
			say("60 seconds left. Save your work! "
				" Computer is about to shut down!");
		}
		sprintf(msg, "%d minutes left", INTERVAL - minutes);
		drawLogo(display, screen, window, col);
		// Display remaining time
		XSetForeground(display, DefaultGC(display, screen), col.pixel);
		XDrawString(display, window, DefaultGC(display, screen), 16, 50, msg, strlen(msg));
		XFlush(display);
		minutes++;
		sleep(60);
	}


	XCloseDisplay(display);
	system("poweroff");
}
