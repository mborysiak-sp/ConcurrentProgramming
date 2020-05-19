#include <stdio.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <stdbool.h>
#include <string.h>
#include <signal.h>
#include <sys/shm.h>

//WAZNE!!!
//CO DO WYLICZANIA KOORDYNATOW: SUMUJESZ WSZYSTKIE X I Y
//I DZIELISZ JE PRZEZ ICH ILOSC WYLICZAJAC SREDNI PUNKT
//MOZE NIE DZIALAC, ALE SPROBUJ


#define SHARED_KEY1 2137
#define SHARED_KEY2 2138
#define SHARED_KEY3 2139


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

typedef struct AppData
{
    Player* players;
    unsigned int playerCount;
    unsigned int nextPlayerID;
} AppData;

// typedef struct ApplicationState
// {
//     Player* players;
//     int playerCount;
//     int nextPlayerID;
// } ApplicationState;

//GLOBAL VARIABLES
Display *display;

Window window;

GC gc;

int screen;

XEvent xEvent;

XGCValues values;

Position car;

int currentPlayerID;

//SHARED GLOBAL VARIABLES
 AppData* appData;
 int appDataID;

//  unsigned int* playerCount;
//  int playerCountID;

//  unsigned int* playerIterator;
//  int playerIteratorID;


//FUNCTIONS
void initPlayers()
{
    appData->playerCount = 0;
    appData->nextPlayerID = 0;
}

int assignID()
{
    int id = appData->nextPlayerID;
    appData->nextPlayerID++;
    return id;
}

void addPlayer()
{
    Player* temp = malloc(((appData->playerCount) + 1) * sizeof(Player));
    
    memcpy(temp, appData->players, appData->playerCount * sizeof(Player));

    // Player player = {0, {0, 0}};

    // player.id = assignID();

    // currentPlayerID = player.id;
    
    // printf("%d", currentPlayerID);
    
    // temp[appData->playerCount] = player;

    // appData->players = temp;

    //free(temp);

    appData->playerCount++;
}

void removePlayer(int id)
{
    Player* temp = malloc((appData->playerCount - 1) * sizeof(Player));

    if (id != 0)
    {
        memcpy(temp, appData->players, id * sizeof(Player));
    }

    if (id != (appData->playerCount - 1))
    {
        memcpy(temp + id, appData->players + id + 1, (appData->playerCount - id - 1) * sizeof(Player));
    }

    appData->players = temp;

    free(temp);

    appData->playerCount--;
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

Position calculateDestination()
{
    Position destination;
    
    int sumX, sumY;

    for (int i = 0; i < appData->playerCount; i++)
    {
        Position playerPosition = appData->players[i].position;
        sumX += playerPosition.x;
        sumY += playerPosition.y;
    }

    sumX /= appData->playerCount;
    sumY /= appData->playerCount;

    destination.x = sumX;
    destination.y = sumY;
    
    return destination;
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
    if((appDataID = shmget(SHARED_KEY1, sizeof(AppData), 0666 | IPC_CREAT | IPC_EXCL)) != -1)
    {
        //playerCountID = shmget(SHARED_KEY2, sizeof(unsigned int), 0666 | IPC_CREAT | IPC_EXCL);
        //playerIteratorID = shmget(SHARED_KEY3, sizeof(unsigned int), 0666 | IPC_CREAT | IPC_EXCL);
        //playerCount = shmat(playerCountID, 0, 0);
        //playerIterator = shmat(playerIteratorID, 0, 0);
        appData = (AppData*) shmat(appDataID, 0, 0);
        initPlayers();
        printf("pierwszy");
    }
    else
    {
        printf("kolejny");
        appDataID = shmget(SHARED_KEY1, sizeof(AppData), 0666 | IPC_CREAT);
        appData = (AppData*) shmat(appDataID, 0, 0);
        //playerCountID = shmget(SHARED_KEY2, sizeof(unsigned int), 0666 | IPC_CREAT);
        //playerIteratorID = shmget(SHARED_KEY3, sizeof(unsigned int), 0666 | IPC_CREAT);
        //playerCount = shmat(playerCountID, 0, 0);
        //playerIterator = shmat(playerIteratorID, 0, 0);
    }

    addPlayer();
}

void clearSharedMemory()
{
    shmctl(appDataID, IPC_RMID, 0);
    //shmctl(playerCountID, IPC_RMID, 0);
    //shmctl(playerIteratorID, IPC_RMID, 0);
    exit(0);
}

void eventLoop()
{
    while (true)
    {
        XNextEvent(display, &xEvent);

        XClearWindow(display, window);

        switch (xEvent.type)
        {
            case Expose:
                break;

            case MotionNotify:
                //appData->players[currentPlayerID].position = getMousePosition(xEvent);

                //car = calculateDestination();

                //drawCar(car);

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


