
#include <csignal>

#include "model.h"
#include "view.h"

Model *m = NULL;
View *v = NULL;

void my_handler (int param)
{
      printf("csignal sigint\n");
      if(m)
          delete m;
      if(v)
          delete v;
}

int main() {
    signal (SIGINT, my_handler);
    m = new Model();
    v = new View(m);
    
    v->run();
    m->run();
}
