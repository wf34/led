
#include "view.h"

View::View(Model* m):
    model_(m),
    thread_(-1) {
}


View::~View() {
    if (-1 != thread_)
        pthread_join(thread_, NULL);
}


void
View::run() {
    pthread_create(&thread_, NULL,
                   &View::visualize, model_);
}


void*
View::visualize(void* ctx) {
    Model * model = static_cast<Model*>(ctx);

    bool flipFlop = false;
    initscr();
    noecho();
    curs_set(FALSE);

    if(has_colors() == FALSE) {
        endwin();
        printf("Your terminal does not support color\n");
        return NULL;
    }

    const int BLACK = 4;
    const int WHITE = 5;
    start_color();
    init_pair(RED, COLOR_RED, COLOR_BLACK);
    init_pair(GREEN, COLOR_GREEN, COLOR_BLACK);
    init_pair(BLUE, COLOR_BLUE, COLOR_BLACK);
    init_pair(BLACK, COLOR_BLACK, COLOR_BLACK);
    init_pair(WHITE, COLOR_WHITE, COLOR_BLACK);
    
    
    int currentColor = 0;
    int rate = 0;
    int period = 0;
    bool state = false;
    LedColor col = UNDEF;
    while(1) {

        // getting Camera data
        model->getState(state);
        col = model->getColor();
        model->getFreq(rate);
        if(col == UNDEF)
            break;

        currentColor = col;
        
        period = (rate != 0 ? 1000000/rate : 100000);
        usleep(period);

        // Calculate object measures
        int max_y, max_x;
        getmaxyx(stdscr, max_y, max_x);
        int span = std::min<int>(max_y,max_x)/6;
        int width = max_x-span*6;
        int height = max_y-span*3;
        
        WINDOW *win = newwin(height,width,span,span);
        WINDOW *triWin = newwin(height,span*4,span,span+width);
        
        int triangBaseX = 1; 
        int triangBaseY = height/2;


        wattron(win, COLOR_PAIR(WHITE));
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
        wattroff(win, COLOR_PAIR(WHITE));

        // finally, led lamp 
        int drawColor = UNDEF;
        if(!state)
            drawColor = BLACK;
        else if(state && rate == 0)
            drawColor = currentColor;
        else if(state && rate != 0) {
            drawColor = flipFlop ? currentColor : BLACK;
            flipFlop = !flipFlop;
        }

        wattron(win, COLOR_PAIR(drawColor));
        mvwaddch(win, ledBaseY+1, ledBaseX+1, ACS_BLOCK);
        wattroff(win, COLOR_PAIR(drawColor));
        
        // some clarifications
        attron(COLOR_PAIR(GREEN)); 
        mvprintw(1,1,"Ya am one hell of a nice little camera;");
        attroff(COLOR_PAIR(GREEN));
        
        refresh();
        wrefresh(win);
        wrefresh(triWin);
        clear();
        delwin(win);
        delwin(triWin);
    }
    endwin();
    pthread_exit(NULL); 
}

