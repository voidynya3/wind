/*
  modifiers:
    Mod1Mask     - Alt key
    Mod4Mask     - Win key
    ControlMask  - Ctrl Key
    ShiftMask    - Shift key
  
  functions:
    wind_execute      - execute a program from arguments
    wind_kill_window  - kills focused window
    wind_quit         - kills wind
*/
static const WindKey keymap[] = {
//  key     modifier(s)            function          argument(s)
  { "d",    Mod1Mask,              wind_execute,     "dmenu_run" },
  { "Enter" Mod1Mask | ShiftMask,  wind_execute,     "st" },
  { "q",    Mod1Mask,              wind_kill_window, NULL },
  { "q",    Mod1Mask | ShiftMask,  wind_quit,        NULL },
  { NULL } // DO NOT REMOVE THIS!!!
};