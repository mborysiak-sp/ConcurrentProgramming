#include <stdio.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <stdbool.h>
#include <string.h>
#include <signal.h>
#include <sys/shm.h>


#define SHARED_KEY 2137

typedef struct Position
{
    unsigned int x;
    unsigned int y;
} Position;

typedef struct Player
{
    unsigned int id;
    Position position;
} Player;

//GLOBAL VARIABLES
Display *display;

Window window;

GC gc;

int screen;

XEvent xEvent;

XGCValues values;

Position car;


//SHARED GLOBAL VARIABLES
Player* players;

int playersID;

int playerCount;


//FUNCTIONS
void initPlayers()
{
    playerCount = 0;

    addPlayer();
}

void addPlayer()
{
    Player* temp = malloc((playerCount + 1) * sizeof(Player));

    memcpy(temp, players, playerCount * sizeof(Player));

    Player player = {playerCount, {0, 0}};
    
    temp[playerCount] = player;

    players = temp;

    playerCount++;
}

void removePlayer(int id)
{
    Player* temp = malloc((playerCount - 1) * sizeof(Player));

    if (id != 0)
    {
        memcpy(temp, players, id * sizeof(Player));
    }

    if (id != (playerCount - 1))
    {
        memcpy(temp + id, players + id + 1, (playerCount - id - 1) * sizeof(Player));
    }

    players = temp;

    playerCount--;
}

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

void moveCar(Position position)
{

    while (comparePositions(car, position))
    {
        
        Position difference;
        
        difference = positionDifference(car, position);

        if (difference.x > 0)
        {
            car.x++;
        } 
        else
        {
            car.x--;
        }

        if (difference.y > 0)
        {
            car.y++;
        } 
        else
        {
            car.y--;
        }
    }
}

void drawCar(Position car)
{
        int x = car.x;
       
        int y = car.y;

        
        XDrawRectangle(display, window, gc, x, y, 120, 30);
        
        XDrawRectangle(display, window, gc, x + 20, y - 20, 50, 20);
        
        XDrawArc(display, window, gc, (x + 20) - (30 / 2), (y + 30) - (30 / 2), 30, 30, 0, 360*64);
        
        XDrawArc(display, window, gc, (x + 90) - (30 / 2), (y + 30) - (30 / 2), 30, 30, 0, 360*64);
}

void initDisplay()
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

void initSharedMemory()
{
    if((playersID = shmget(SHARED_KEY, 1024, 0666 | IPC_CREAT | IPC_EXCL)) != -1)
    {
        initPlayers();
        printf("KUPA");
    }
    else
    {
        addPlayer();
        printf("DUPA");
        playersID = shmget(SHARED_KEY, 1024, 0666 | IPC_CREAT);
    }

    players = shmat(playersID, 0, 0);
}

void clearSharedMemory()
{
    shmctl(playersID, IPC_RMID, 0);

    exit(0);
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

                car = position;

                drawCar(car);

                break;
            default:
                break;
        }
    }
}

int main (int argc, char *argv[])
{
    signal(SIGINT, clearSharedMemory);

    initSharedMemory();
    
    initDisplay();

    eventLoop();

    clearSharedMemory();
    
    return 0;
}


