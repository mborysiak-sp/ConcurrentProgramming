#include <stdio.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <stdbool.h>

#define SHARED_KEY 2137

typedef struct Position
{
    unsigned int x;
    unsigned int y;
} Position;

typedef struct Car
{
    Position position;
} Car;

//GLOBAL VARIABLES
Display *display;

Window window;

GC gc;

int screen;

XEvent xEvent;

XGCValues values;

Car car;


Position positionDifference(Position a, Position b)
{

    Position result;

    result.x = a.x - b.x;
    result.y = a.y - b.y;

    return result;
}

bool comparePositions(Position a, Position b)
{
    if (a.x != b.x && a.y != b.y) {
        return true;
    }
    return false;
}

Position getMousePosition(XEvent xEvent)
{

    Position position;

    position.x = xEvent.xmotion.x;
    position.y = xEvent.xmotion.y;

    return position;
}

void moveCar(Display *display, Window window, GC gc, int screen, Car car, Position position)
{

    while (comparePositions(car.position, position))
    {
        
        Position difference;
        
        difference = positionDifference(car.position, position);

        if (difference.x > 0)
        {
            car.position.x++;
        } 
        else
        {
            car.position.x--;
        }

        if (difference.y > 0)
        {
            car.position.y++;
        } 
        else
        {
            car.position.y--;
        }
    }
}

void drawCar(Car car)
{
        int x = car.position.x;
       
        int y = car.position.y;

        
        XDrawRectangle(display, window, gc, x, y, 120, 30);
        
        XDrawRectangle(display, window, gc, x + 20, y - 20, 50, 20);
        
        XDrawArc(display, window, gc, (x + 20) - (30 / 2), (y + 30) - (30 / 2), 30, 30, 0, 360*64);
        
        XDrawArc(display, window, gc, (x + 90) - (30 / 2), (y + 30) - (30 / 2), 30, 30, 0, 360*64);
}

void init_display()
{
    display = XOpenDisplay(NULL);

    if (display == NULL) {

        fprintf(stderr, "Cannot open display\n");

        exit(1);
    }

    screen = DefaultScreen(display);

    window = XCreateSimpleWindow(display, RootWindow(display, screen),
        100, 100, 500, 500, 1, BlackPixel(display, screen), 
        WhitePixel(display, screen));

    XSelectInput(display, window, ExposureMask | PointerMotionMask | ButtonPressMask);

    XMapWindow(display, window);

    values.foreground = BlackPixel(display, screen);
    values.background = WhitePixel(display, screen);

    unsigned long valuemask = GCCapStyle | GCJoinStyle;

    gc = XCreateGC(display, window, valuemask, &values);

    if (gc < 0)
    {
        fprintf(stderr, "XCreateGC: \n");
    }

    XSetFillStyle(display, gc, FillSolid);

    XSetLineAttributes(display, gc, 2, LineSolid, CapRound, JoinRound);

}

void eventLoop()
{
    Position position;

    while (true)
    {
        XNextEvent(display, &xEvent);

        XClearWindow(display, window);

        switch (xEvent.type)
        {
            case Expose:
                break;

            case MotionNotify:
                position = getMousePosition(xEvent);

                car.position = position;

                drawCar(car);

                break;
            default:
                break;
        }
    }
}

int main (int argc, char *argv[])
{
    init_display();

    eventLoop();

    return 0;
}


