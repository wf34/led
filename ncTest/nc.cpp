#include <ncurses.h>

int main(int argc, char *argv[])
{
    initscr();

    WINDOW *win = newwin(10,10,1,1);

    box(win, '*', '*');
    touchwin(win);
    wrefresh(win);

    getchar();

    endwin();
    return 0;
}
