
#include "view.h"

View::View(Model* m):
    model_(m) {
}


void
View::run() {
    pthread_create(&thread_, NULL,
                   &View::visualize, model_);
}


void*
View::visualize(void* ctx) {
    while (1) {
        printf("view");
        usleep(200000);
    }
    return NULL;
}

