// Access to screen and buttons on "OK Micro Dock" hardware variants

#pragma once

class OkLittleLayout;
class U8G2;

enum OkDockType {
  OK_NO_DOCK,
  OK_FEATHER_DOCK_V8,
};

extern U8G2* ok_dock_screen;
extern OkLittleLayout* ok_dock_layout;

bool ok_dock_init(OkDockType dock_type);
bool ok_dock_button(int which);
