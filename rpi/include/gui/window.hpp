#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <csignal>
#include <cstdlib>
#include <fstream>
#include <ncurses.h>
#include <string>
#include <vector>
#include <sys/ioctl.h>


#define KEY_RETURN 13
#define KEY_ESC    27


typedef struct {
    int cx;
    int cy;
} Point;


class Window {
public:
    static const auto MIN_PPM_SETUP = 0;
    static const auto REVERSE_PPM_SETUP = 1;
    static const auto MAX_PPM_SETUP = 2;
    static const auto UNUSED_PPM_SETUP = 3;

    static const auto MIN_WIDTH = 80;
    static const auto MIN_HEIGHT = 24;

    static const auto VEL_0_POSE_X = -13;
    static const auto VEL_1_POSE_X = +10;

    static const auto PPM_0_POSE_X = -14;
    static const auto PPM_1_POSE_X =9;
    static const auto PPM_POSE_Y = 2;

private:
    static void signalHandler(int signal);

public:
    static void initWindow();
    static void finish();
    static void resizeExit();
    static void resizeHandler();
    static int getLines();
    static int getColumns();
    static Point getCenter();
    static void config();
    static void paintVels(int vel0, int vel1);
    static void clearVels();
    static void paintSetupPpm(int value);
    static void paintCounter(bool init, uint8_t counter=0);
    static void paintBoat();
};


#endif // WINDOW_HPP


//      /\                       /\ 
//     /  \                     /  \ 
//    /    \                   /    \ 
//   /      \                 /      \ 
//  /        \               /        \ 
// |          |             |          |
// |          |             |          |
// |          |-------------|          |
// |          |             |          |
// |          |             |          |
// |          |-------------|          |
// |          |             |          |
// |          |             |          |
// |          |-------------|          |
// |          |             |          |
// |          |             |          |
//  \        /               \        /
//   \      /                 \      /
//    \    /                   \    /
//     \  /                     \  /
//      \/                       \/
