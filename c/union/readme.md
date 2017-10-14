Usage of Union
==============

This program describes the use of ```union```. ```union``` is a date type which can hold variables of different types but only one at given time. The programmer should know which type of data is stored and which to access from the ```union```.

The ```GameEvent``` is a ```union``` used to notify the different types of events in a game. ```GameEvent``` can describe two types of events, ```KeyboardEvent``` or ```MouseEvent```.

```
typedef union {
    unsigned int type;
    KeyboardEvent kbEvent;
    MouseEvent mouseEvent;
} GameEvent;
```

The ```KeyboardEvent``` provides details about any key press on the keyboard. While the ```MouseEvent``` provides details of any mouse button click and its coordinates.

```
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
    unsigned int x;
    unsigned int y;
} MouseEvent;
```

The program first sets the ```GameEvent``` to notify a ```KeyboardEvent``` and prints its details. The it notify about ```MouseEvent``` and prints its details. At the last it shows how wrong information is shown in case ```KeyboardEvent``` is set but ```MouseEvent``` details are assumed.

Output:
```
Size of keyboard event: 12
Size of mouse event: 20
Size of game event: 20

-------- Game Event Details: Keyboard Event --------
Event Type: 1
State: 1
KeyCode: 67
----------------------------------------------------

-------- Game Event Details: Mouse Event --------
Event Type: 2
State: 0
Button: 1
X: 500.000000
Y: 150.000000
----------------------------------------------------

---------- Setting Keyboard Event but using it as Mouse Event ----------

-------- Game Event Details: Mouse Event --------
Event Type: 1
State: 1
Button: 67
X: 500.000000
Y: 150.000000
----------------------------------------------------
```
