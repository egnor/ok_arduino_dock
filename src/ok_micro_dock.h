// Access to screen and buttons on "OK Micro Dock" hardware variants

#pragma once

class OkLittleLayout;
class U8G2;

extern U8G2* ok_dock_screen;
extern OkLittleLayout* ok_dock_layout;

bool ok_dock_init_feather_v8();
bool ok_dock_button(int which);
