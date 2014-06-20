
#include "model.h"
#include "view.h"

int main() {
    Model m;
    View v(&m);
    
    v.run();
    m.run();
}
