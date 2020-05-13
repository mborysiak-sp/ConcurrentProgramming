#include <stdio.h>
#include <stdlib.h>
#include <X11/Xlib.h>

typedef struct Car {
    unsigned int xPosition;
    unsigned int yPosition;
} Car;

void drawCar(Display *display, Window window, GC gc, int screen, Car car) {

        // Window childWindow = XCreateSimpleWindow(display, window,
        //     car.xPosition, car.yPosition, 200, 100, 1, WhitePixel(display, screen), 
        //     WhitePixel(display, screen));
        
        // XSelectInput(display, childWindow, ExposureMask);

        // XMapWindow(display, childWindow);
        
        int x = car.xPosition;
        int y = car.yPosition;

        // XDrawRectangle(display, window, gc, 30, 70, 120, 30);

        // XDrawRectangle(display, window, gc, 50, 50, 50, 20);

        // XDrawArc(display, window, gc, 50-(30/2), 100-(30/2), 30, 30, 0, 360*64);

        // XDrawArc(display, window, gc, 120-(30/2), 100-(30/2), 30, 30, 0, 360*64);

        XDrawRectangle(display, window, gc, x, y, 120, 30);
        XDrawRectangle(display, window, gc, x + 20, y - 20, 50, 20);
        XDrawArc(display, window, gc, (x + 20) - (30 / 2), (y + 30) - (30 / 2), 30, 30, 0, 360*64);
        XDrawArc(display, window, gc, (x + 90) - (30 / 2), (y + 30) - (30 / 2), 30, 30, 0, 360*64);
    }

int main (int argc, char *argv[]) {

    Display *display;

    Window window, childWindow;

    GC gc;

    display = XOpenDisplay(NULL);

    if (display == NULL) {

        fprintf(stderr, "Cannot open display\n");

        exit(1);
    }

    int screen,
        x,
        y, 
        width = 2,
        height = 2;

    screen = DefaultScreen(display);

    window = XCreateSimpleWindow(display, RootWindow(display, screen),
        100, 100, 500, 500, 1, BlackPixel(display, screen), 
        WhitePixel(display, screen));

    XSelectInput(display, window, ExposureMask | ButtonPressMask);

    XMapWindow(display, window);

    XEvent event;

    XGCValues values;

    values.foreground = BlackPixel(display, screen);
    values.background = WhitePixel(display, screen);

    unsigned long valuemask = GCCapStyle | GCJoinStyle;

    gc = XCreateGC(display, window, valuemask, &values);

    if (gc < 0) {
        fprintf(stderr, "XCreateGC: \n");
    }

    unsigned int borderWidth,
        depth;

    Car car = {100, 100};

    XSetFillStyle(display, gc, FillSolid);

    XSetLineAttributes(display, gc, 2, LineSolid, CapRound, JoinRound);
    
    while (1) {
        //drawCar(display, window, gc, screen, car);

        XNextEvent(display, &event);
        

        // XDrawRectangle(display, window, gc, 30, 70, 120, 30);

        // XDrawRectangle(display, window, gc, 50, 50, 50, 20);

        // XDrawArc(display, window, gc, 50-(30/2), 100-(30/2), 30, 30, 0, 360*64);

        // XDrawArc(display, window, gc, 120-(30/2), 100-(30/2), 30, 30, 0, 360*64);
        // if (event.xany.window == childWindow && event.type == Expose) {

        // }

         if (event.type == ButtonPress && event.xbutton.button == 1) {
            XClearWindow(display, window);
            car.xPosition++;
            drawCar(display, window, gc, screen, car);
        }

         if (event.type == ButtonRelease && event.xbutton.button == 1) {

        }
    }

    return 0;
}


