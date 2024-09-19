static const WindKey keymap[] = {
  { "d", Mod1Mask, wind_execute, "dmenu_run" },
  { "q", Mod1Mask, wind_kill_window, NULL },
  { "q", Mod1Mask | ShiftMask, wind_quit, NULL },
  { NULL }
};