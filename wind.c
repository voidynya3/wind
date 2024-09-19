
#ifndef WIND_VERSION
#  define WIND_VERSION "undefined"
#endif

#ifndef WIND_GIT_SOURCE
#  define WIND_GIT_SOURCE "undefined"
#endif


#include <stdio.h>
#include <stdlib.h>
#include <X11/cursorfont.h>
#include <X11/X.h>
#include <X11/Xlib.h>


#define WIND_DEADCODE { printf("warning: called dead code.\n"); }
#define WIND_PRINT_HELLO printf("wind %s | %s\n", WIND_VERSION, WIND_GIT_SOURCE);
#define WIND_MAX(a, b) ((a) > (b) ? (a) : (b))


typedef struct _wind {
  Display *display;
  Window root;
  void (**handlers)(XEvent *);
} WindWM;

typedef struct _wind_key {
  const char *key;
  unsigned int modifier;
  void (*function)(void *);
  void *argument;
} WindKey;

typedef struct _wind_button {
  unsigned int button;
  unsigned int modifier;
  void (*function)(void *);
  void *argument;
} WindButton;


static void wind_info(const char *); 
static void wind_warn(const char *); 
static void wind_panic(const char *);
static int wind_error_handler(Display *, XErrorEvent *);
static void wind_init();
static KeyCode wind_string_to_keycode(const char *);
static void wind_grab_key(const char *, unsigned int);
static void wind_grab_keys();
static void wind_grab_button(unsigned int, unsigned int);
static void wind_grab_buttons();
static void wind_setup();
static void wind_mainloop();
static void wind_destroy();
static void wind_execute(void *);
static void wind_kill_window(void *);
static void wind_quit(void *);
static void wind_on_key_press(XEvent *);
static void wind_on_mapping_notify(XEvent *);
static void wind_on_button_press(XEvent *);
static void wind_on_motion_notify(XEvent *);
static void wind_on_button_release(XEvent *);


#include "config.h"


static XWindowAttributes window_attributes;
static XButtonEvent pointer_drag_start;
static WindWM *wm;
static void (*event_array[])(XEvent *) = {
  [KeyPress] = wind_on_key_press,
  [ButtonPress] = wind_on_button_press,
  [ButtonRelease] = wind_on_button_release,
  [MotionNotify] = wind_on_motion_notify,
  [MappingNotify] = wind_on_mapping_notify,
};


static void
wind_info(
    const char *msg
) {
  printf("wind: %s\n", msg);
}

static void
wind_warn(
    const char *msg
) {
  printf("warn: %s\n", msg);
}

static void
wind_panic(
    const char *msg
) {
  printf("panic: %s\n", msg);
  exit(EXIT_FAILURE);
}

static int
wind_error_handler(
    Display *display,
    XErrorEvent *event
) {
  wind_warn("unhandled error occured!");

  char buffer[512];
  XGetErrorText(display, event->error_code, buffer, 512);

  wind_warn(buffer);

  return 0;
}

static void
wind_init() {
  if (!(wm = (WindWM *)malloc(sizeof(WindWM))))
    wind_panic("failed to allocate memory for window manager.");
  
  if (!(wm->display = XOpenDisplay(NULL)))
    wind_panic("failed to open x display.");

  wm->root = XDefaultRootWindow(wm->display);

  XSetErrorHandler(wind_error_handler);
}

static KeyCode
wind_string_to_keycode(
    const char *key
) {
  KeySym symbol = XStringToKeysym(key);
  KeyCode code = XKeysymToKeycode(wm->display, symbol);
  return code;
}

static void
wind_grab_key(
    const char *key,
    unsigned int modifier
) {
  XGrabKey(wm->display, wind_string_to_keycode(key), modifier, wm->root, True,
    GrabModeAsync, GrabModeAsync);
  XSync(wm->display, False);
}

static void
wind_grab_keys() {
	XUngrabKey(wm->display, AnyKey, AnyModifier, wm->root);

  int current_key = 0;
  WindKey key = keymap[current_key++];

  if (key.key == NULL)
    return;

  while (key.key != NULL) {
    wind_grab_key(key.key, key.modifier);
    key = keymap[current_key++];
  }
}

static void
wind_grab_button(
    unsigned int button,
    unsigned int modifier
) {
  XGrabButton(wm->display, button, modifier, wm->root, False,
    ButtonPressMask, GrabModeAsync, GrabModeAsync, 0, 0);
}

static void
wind_grab_buttons() {
  XGrabButton(wm->display, Button1, None, wm->root, False,
    ButtonPressMask, GrabModeSync, GrabModeAsync, 0, 0);
  wind_grab_button(Button1, Mod1Mask);
  wind_grab_button(Button3, Mod1Mask);
}

static void
wind_setup() {
  //XSelectInput(wm->display, wm->root, SubstructureRedirectMask | SubstructureNotifyMask);

  Cursor cursor = XCreateFontCursor(wm->display, XC_arrow);
  XDefineCursor(wm->display, wm->root, cursor);
  XSync(wm->display, False);

  wind_grab_buttons();
  wind_grab_keys();
}

static void
wind_mainloop() {
  XEvent event;
  for (;;) {
    XNextEvent(wm->display, &event);
    if (wm->handlers[event.type] != NULL) {
      wm->handlers[event.type](&event);
    } else {
      wind_warn("undefine event.");
      char id_str[3];
      sprintf(id_str, "%d", event.type);
      wind_warn(id_str);
    }
    XAllowEvents(wm->display, ReplayPointer, CurrentTime);
    XSync(wm->display, False);
  }
}

static void
wind_destroy() {
  XCloseDisplay(wm->display);
  free(wm);
}

static void
wind_execute(
    void *argument
) {
  system((char *)argument);
}

static void
wind_kill_window(
    void *argument
) {
  unsigned int child_amount;
  Window root_window, parent_window, *child_windows;
  XQueryTree(wm->display, wm->root, &root_window, &parent_window, &child_windows, &child_amount);

  if (child_amount < 1)
    return;

	XGrabServer(wm->display);
	XSetCloseDownMode(wm->display, DestroyAll);
	XKillClient(wm->display, child_windows[child_amount - 1]);
	XSync(wm->display, False);
	XUngrabServer(wm->display);
}

static void
wind_quit(
    void *argument
) {
  wind_destroy();
}

static void
wind_on_key_press(
    XEvent *event
) {
  printf("key pressed! (%d;%d)\n", event->xkey.keycode, event->xkey.state);
  int current_key = 0;
  WindKey key = keymap[current_key++];

  while (key.key != NULL) {
    printf("comparing keycode %d...\n", wind_string_to_keycode(key.key));
    if (event->xkey.keycode == wind_string_to_keycode(key.key) && 
        event->xkey.state & key.modifier) {
      key.function(key.argument);
      return;
    }

    key = keymap[current_key++];
  }
}

static void
wind_on_mapping_notify(
    XEvent *event
) {
	XMappingEvent *mapping_event = &event->xmapping;
  XRefreshKeyboardMapping(mapping_event);
  if (mapping_event->request == MappingKeyboard)
    wind_grab_keys();
}

static void
wind_on_button_press(
    XEvent *event
) {
  wind_info("click detected.");
  if (event->xbutton.subwindow == None) {
    wind_warn("window is root.");
    return;
  }

  XRaiseWindow(wm->display, event->xbutton.subwindow);
  XSetInputFocus(wm->display, event->xbutton.subwindow, RevertToPointerRoot, CurrentTime);
  
  if (event->xbutton.state & Mod1Mask) {
    XGrabPointer(wm->display, event->xbutton.subwindow, False,
      Button1MotionMask | Button3MotionMask | ButtonReleaseMask,
      GrabModeAsync, GrabModeAsync,
      None, None, CurrentTime);
    XGetWindowAttributes(wm->display, event->xbutton.subwindow, &window_attributes);
    pointer_drag_start = event->xbutton;
  } else {
    XAllowEvents(wm->display, ReplayPointer, CurrentTime);
    XSync(wm->display, 0);
  }

}

static void
wind_on_motion_notify(
    XEvent *event
) {
  int x_difference, y_difference;

  while (XCheckTypedEvent(wm->display, MotionNotify, event));

  x_difference = event->xbutton.x_root - pointer_drag_start.x_root;
  y_difference = event->xbutton.y_root - pointer_drag_start.y_root;

  XMoveResizeWindow(wm->display, event->xmotion.window,
    window_attributes.x + (pointer_drag_start.button == 1 ? x_difference : 0),
    window_attributes.y + (pointer_drag_start.button == 1 ? y_difference : 0),
    WIND_MAX(64, window_attributes.width + (pointer_drag_start.button == 3 ? x_difference : 0)),
    WIND_MAX(64, window_attributes.height + (pointer_drag_start.button == 3 ? y_difference : 0))
  );
}

static void
wind_on_button_release(
    XEvent *event
) {
  XUngrabPointer(wm->display, CurrentTime);
}


int
main(void) {
  WIND_PRINT_HELLO

  wind_init();
  wind_setup();
  wm->handlers = event_array;

  wind_mainloop();
  
  wind_destroy();
  return 0;
}