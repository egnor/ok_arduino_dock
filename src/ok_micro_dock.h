// Access to screen and buttons on "OK Micro Dock" hardware variants

#pragma once

enum OkMicroDockType {
  OK_NO_DOCK,
  OK_FEATHER_DOCK_V8,
};

class OkLittleLayout;
extern OkLittleLayout* ok_dock_screen;

bool ok_dock_init(OkMicroDockType dock_type);
bool ok_dock_button(int which);
