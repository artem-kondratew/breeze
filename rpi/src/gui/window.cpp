#include "gui/window.hpp"


void Window::signalHandler(int signal) {
    if (signal == SIGINT) {
        finish();
    }
    if (signal == SIGWINCH) {
        resizeHandler();
    }
}


void Window::initWindow() {
    initscr();
    savetty();  //  save terminal settings

    nonl();  //  deny going to the new line
    cbreak();  //  буфер передается без нажатия enter
    noecho();  //  не отображается печать символов
    timeout(0); //  неблокируюшее чтение
    //leaveok(stdscr, TRUE);  //  сдвиг курсора - нормально
    curs_set(0);  //  hide cursor
    keypad(stdscr, TRUE);

    signal(SIGINT, signalHandler);  // Ctrl + C
    signal(SIGWINCH, signalHandler);  //  resize screen

    set_escdelay(0);

    if(getLines() < MIN_HEIGHT || getColumns() < MIN_WIDTH) {
        resizeExit();
    }
}


void Window::finish() {
    clear();
    refresh();
    resetty();
    endwin();
    exit(0);
}


void Window::resizeExit() {
    clear();
    move(0, 0);
    printw("SMALL TERMINAL! MIN_W = 80, MIN_H = 24");
    refresh();
}


void Window::resizeHandler() {
    if(getLines() < MIN_HEIGHT || getColumns() < MIN_WIDTH) {
        resizeExit();
    }
    else {
        clear();
        paintBoat();
        refresh();
    }
}


int Window::getLines() {
    struct winsize window{};
    ioctl(0, TIOCGWINSZ, &window);
    return window.ws_row;
}


int Window::getColumns() {
    struct winsize window{};
    ioctl(0, TIOCGWINSZ, &window);
    return window.ws_col;
}


Point Window::getCenter() {
    Point pt;
    pt.cx = Window::getColumns() / 2;
    pt.cy = Window::getLines() / 2;
    return pt;
}


void Window::config() {
    paintBoat();
    paintVels(0, 0);
    paintSetupPpm(UNUSED_PPM_SETUP);
    paintCounter(true);
}


void Window::paintVels(int vel0, int vel1) {
    Point c = getCenter();

    move(c.cy - 1, c.cx + VEL_0_POSE_X-1);
    printw("VEL0:");
    move(c.cy, c.cx + VEL_0_POSE_X);
    if (vel0 < 0) {
        printw("%ld", (long int)vel0);
    }
    else {
        printw(" %ld", (long int)vel0);
    }
    
    move(c.cy - 1, c.cx + VEL_1_POSE_X-1);
    printw("VEL1:");
    move(c.cy, c.cx + VEL_1_POSE_X);
    if (vel1 < 0) {
        printw("%ld", (long int)vel1);
    }
    else {
        printw(" %ld", (long int)vel1);
    }

    refresh();
}


void Window::clearVels() {
    Point c = getCenter();

    move(c.cy, c.cx + VEL_0_POSE_X);
    printw("    ");
    move(c.cy, c.cx + VEL_1_POSE_X);
    printw("    ");
    refresh();
}


void Window::paintSetupPpm(int value) {
    Point c = getCenter();

    move(c.cy + PPM_POSE_Y, c.cx + PPM_0_POSE_X);
    printw("PPM0:");
    move(c.cy + PPM_POSE_Y, c.cx + PPM_1_POSE_X);
    printw("PPM1:");

    std::string s;
    if (value == MIN_PPM_SETUP) {
        s = "LLLLL";
    }
    if (value == REVERSE_PPM_SETUP) {
        s = "RRRRR";
    }
    if (value == MAX_PPM_SETUP) {
        s = "HHHHH";
    }
    if (value == UNUSED_PPM_SETUP) {
        s = "-----";
    }

    move(c.cy + PPM_POSE_Y + 1, c.cx + PPM_0_POSE_X);
    printw("%s", s.c_str());
    move(c.cy + PPM_POSE_Y + 1, c.cx + PPM_1_POSE_X);
    printw("%s", s.c_str());

    refresh();
}


void Window::paintCounter(bool init, uint8_t counter) {
    Point c = getCenter();

    move(c.cy - 9, c.cx - 11);
    if (init) {
        printw("-");
    }
    else {
        printw("%d", counter);
    }
    refresh();
}


void Window::paintBoat() {
    int w = Window::getColumns();
    int h = Window::getLines();
    int cx = w / 2;
    int cy = h / 2;

    int x = cx - 7 - 9;
    int y = cy - 5;
    for (int i = 0; i < 11; i++) {
        move(y, x);
        printw("|        |             |        |");
        y++;
    }

    move(cy - 3, cx - 6);
    printw("-------------");
    move(cy, cx - 6);
    printw("-------------");
    move(cy + 3, cx - 6);
    printw("-------------");

    y = cy - 10;
    move(y, x); y++;
    printw("    /\\                     /\\");
    move(y, x); y++;
    printw("   /  \\                   /  \\");
    move(y, x); y++;
    printw("  /    \\                 /    \\");
    move(y, x); y++;
    printw(" /      \\               /      \\");
    move(y, x); y++;
    printw("/        \\             /        \\");

    y = cy + 6;
    move(y, x); y++;
    printw("\\        /             \\        /");
    move(y, x); y++;
    printw(" \\      /               \\      /");
    move(y, x); y++;
    printw("  \\    /                 \\    /");
    move(y, x); y++;
    printw("   \\  /                   \\  /");
    move(y, x); y++;
    printw("    \\/                     \\/");

    refresh();
}
