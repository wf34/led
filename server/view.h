#ifndef VIEW_H__
#define VIEW_H__

#include <cstdio>

#include <unistd.h>
#include <pthread.h>
#include <ncurses.h>

#include "model.h"

class View {
    public:
        View(Model* m);
        ~View();
        void run();
     
    private:
        static void* visualize(void* ctx);
        Model * model_;
        pthread_t thread_;
};

#endif /* VIEW_H__ */
