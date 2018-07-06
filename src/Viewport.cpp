#include "Viewport.hpp"


Viewport::Viewport (int a, int b, int h, int w, View v) :
    x(a), y (b), height (h), width (w)
{
    view = v ;
}

Viewport::~Viewport () {
}

Viewport::display () {
}


