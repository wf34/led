
#include <algorithm>
#include <unistd.h>

#include <ncurses.h>


int main(int argc, char *argv[])
{
    bool flipFlop = false;
    initscr();
    noecho();
    curs_set(FALSE);
    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);

    if(has_colors() == FALSE) {
        endwin();
        printf("Your terminal does not support color\n");
        return -1;
    }
    start_color();
    init_pair(1, COLOR_RED, COLOR_BLACK);
    init_pair(2, COLOR_GREEN, COLOR_BLACK);
    init_pair(3, COLOR_BLUE, COLOR_BLACK);
    init_pair(4, COLOR_BLACK, COLOR_BLACK);
    init_pair(5, COLOR_WHITE, COLOR_BLACK);
    
    int span = std::min<int>(max_y,max_x)/6;
    int width = max_x-span*6;
    int height = max_y-span*3;
    
    WINDOW *win = newwin(height,width,span,span);
    WINDOW *triWin = newwin(height,span*4,span,span+width);
    
    int triangBaseX = 1; 
    int triangBaseY = height/2;

    while(1) {
        usleep(200000);
        wattron(win, COLOR_PAIR(5));
        box(win, '*', '*');

        for(int i=0; i<8;++i) {
            mvwaddch(triWin, triangBaseY+i, triangBaseX+i, ACS_BLOCK);
            mvwaddch(triWin, triangBaseY-i, triangBaseX+i, ACS_BLOCK);
        }
        for(int i=0; i<15;++i) {
            mvwaddch(triWin, triangBaseY-7+i, triangBaseX+8, ACS_BLOCK);
        }

        int ledBaseX = 4;
        int ledBaseY = 2;
        mvwaddch(win, ledBaseY, ledBaseX, ACS_BLOCK);
        mvwaddch(win, ledBaseY, ledBaseX+1, ACS_BLOCK);
        mvwaddch(win, ledBaseY, ledBaseX+2, ACS_BLOCK);
        mvwaddch(win, ledBaseY+2, ledBaseX, ACS_BLOCK);
        mvwaddch(win, ledBaseY+2, ledBaseX+1, ACS_BLOCK);
        mvwaddch(win, ledBaseY+2, ledBaseX+2, ACS_BLOCK);
        mvwaddch(win, ledBaseY+1, ledBaseX, ACS_BLOCK);
        mvwaddch(win, ledBaseY+1, ledBaseX+2, ACS_BLOCK);
        wattroff(win, COLOR_PAIR(5));

        // finally, led lamp 
        int currentColor = flipFlop ? 1:4;
        flipFlop = !flipFlop;
        wattron(win, COLOR_PAIR(currentColor));
        mvwaddch(win, ledBaseY+1, ledBaseX+1, ACS_BLOCK);
        wattroff(win, COLOR_PAIR(currentColor));
        
        // some clarifications
        attron(COLOR_PAIR(2)); 
        mvprintw(span*4,span+2,"Ya am one hell of a nice little camera;");
        attroff(COLOR_PAIR(2));
        wrefresh(win);
        wrefresh(triWin);
        refresh();
    }


    endwin();
    return 0;
}
