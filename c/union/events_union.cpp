#include <stdio.h>
#include <stdlib.h>

const unsigned int GAME_EVENT_TYPE_KEYBOARD = 1;
const unsigned int GAME_EVENT_KEYBOARD_STATE_KEY_UP = 0;
const unsigned int GAME_EVENT_KEYBOARD_STATE_KEY_DOWN = 1;

const unsigned int GAME_EVENT_TYPE_MOUSE = 2;
const unsigned int GAME_EVENT_MOUSE_BUTTON_STATE_UP = 0;
const unsigned int GAME_EVENT_MOUSE_BUTTON_STATE_DOWN = 1;
const unsigned int GAME_EVENT_MOUSE_LEFT_BUTTON = 1;
const unsigned int GAME_EVENT_MOUSE_RIGHT_BUTTON = 2;
const unsigned int GAME_EVENT_MOUSE_MIDDEL_BUTTON = 3;

typedef struct {
    unsigned int type;
    unsigned int state;
    unsigned int keyCode;
} KeyboardEvent;

typedef struct {
    unsigned int type;
    unsigned int state;
    unsigned int button;
    float x;
    float y;
} MouseEvent;

typedef union {
    KeyboardEvent kbEvent;
    MouseEvent mouseEvent;
} GameEvent;

void printMouseEventDetails(GameEvent *event);
void printKeyboardEventDetails(GameEvent *event);

int main(void)
{
    printf("Size of keyboard event: %zu\n", sizeof(KeyboardEvent));
    printf("Size of mouse event: %zu\n", sizeof(MouseEvent));
    printf("Size of game event: %zu\n\n", sizeof(GameEvent));

    GameEvent gameEvent;

    KeyboardEvent kbEvent;
    kbEvent.type = GAME_EVENT_TYPE_KEYBOARD;
    kbEvent.state = GAME_EVENT_KEYBOARD_STATE_KEY_DOWN;
    kbEvent.keyCode = 'C';

    gameEvent.kbEvent = kbEvent;

    printKeyboardEventDetails(&gameEvent);

    MouseEvent mouseEvent;
    mouseEvent.type = GAME_EVENT_TYPE_MOUSE;
    mouseEvent.state = GAME_EVENT_MOUSE_BUTTON_STATE_UP;
    mouseEvent.button = GAME_EVENT_MOUSE_LEFT_BUTTON;
    mouseEvent.x = 500;
    mouseEvent.y = 150;

    gameEvent.mouseEvent = mouseEvent;

    printMouseEventDetails(&gameEvent);

    printf("\n---------- Setting Keyboard Event but using it as Mouse Event ----------\n");
    gameEvent.kbEvent = kbEvent;

    printMouseEventDetails(&gameEvent);

    return 0;
}

void printKeyboardEventDetails(GameEvent *event)
{
    printf("-------- Game Event Details: Keyboard Event --------\n");
    printf("Event Type: %d\n", event->kbEvent.type);
    printf("State: %d\n", event->kbEvent.state);
    printf("KeyCode: %d\n", event->kbEvent.keyCode);
    printf("----------------------------------------------------\n");
}

void printMouseEventDetails(GameEvent *event)
{
    printf("\n-------- Game Event Details: Mouse Event --------\n");
    printf("Event Type: %d\n", event->mouseEvent.type);
    printf("State: %d\n", event->mouseEvent.state);
    printf("Button: %d\n", event->mouseEvent.button);
    printf("X: %f\n", event->mouseEvent.x);
    printf("Y: %f\n", event->mouseEvent.y);
    printf("----------------------------------------------------\n");
}
