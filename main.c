#include <stdio.h>
#include <stdlib.h>
#include <X11/Xlib.h>

typedef struct Position {
    unsigned int x;
    unsigned int y;
} Position;

typedef struct Car {
    unsigned int x;
    unsigned int y;
} Car;

Position getMousePosition(XEvent xEvent) {

    Position position;

    position.x = xEvent.xmotion.x;
    position.y = xEvent.xmotion.y;

    return position;
}
//to powinien byc raczej wspolbiezny proces
void moveCar(Display *display, Window window, GC gc, int screen, Car car, Position position) {

    int xDifference, yDifference;

    while (car.x != position.x && car.y != position.y) {

        xDifference = position.x - car.x;

        yDifference = position.y - car.y;

        if (xDifference > 0) {

            car.x++;
        } else {

            car.x--;
        }

        if (yDifference > 0) {

            car.y++;
        } else {

            car.y--;
        }
    }
}

void drawCar(Display *display, Window window, GC gc, int screen, Car car) {

        // Window childWindow = XCreateSimpleWindow(display, window,
        //     car.xPosition, car.yPosition, 200, 100, 1, WhitePixel(display, screen), 
        //     WhitePixel(display, screen));
        
        // XSelectInput(display, childWindow, ExposureMask);

        // XMapWindow(display, childWindow);

        int x = car.x;
        int y = car.y;

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

    XSelectInput(display, window, ExposureMask | PointerMotionMask | ButtonPressMask);

    XMapWindow(display, window);

    XEvent xEvent;

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
    
    Position position;

    // XGrabPointer(display, window, False, PointerMotionMask, GrabModeAsync,
    //     GrabModeAsync, None, None, CurrentTime);

    while (1) {
        //drawCar(display, window, gc, screen, car);

        XNextEvent(display, &xEvent);

        XClearWindow(display, window);

        switch (xEvent.type) {
            case Expose:
                //drawCar(display, window, gc, screen, car);
                break;
            case MotionNotify:

                position = getMousePosition(xEvent);

                car.x = position.x;
                car.y = position.y;

                drawCar(display, window, gc, screen, car);
                break;
            default:
                break;
        }
        // XDrawRectangle(display, window, gc, 30, 70, 120, 30);

        // XDrawRectangle(display, window, gc, 50, 50, 50, 20);

        // XDrawArc(display, window, gc, 50-(30/2), 100-(30/2), 30, 30, 0, 360*64);

        // XDrawArc(display, window, gc, 120-(30/2), 100-(30/2), 30, 30, 0, 360*64);
        // if (event.xany.window == childWindow && event.type == Expose) {

        // }

        //  if (xEvent.type == ButtonPress && event.xbutton.button == 1) {
        //     XClearWindow(display, window);
        //     car.x++;
        //     drawCar(display, window, gc, screen, car);
        // }
    }

    return 0;
}


