#ifndef DEF_VIEWPORT
#define DEF_VIEWPORT

typedef enum View { sagitalPlane, coronalPlane, volumeRendering } ;
class Viewport {

public :
    Viewport (int a, int b, int h, int w, View v) ;
    setUpCamera () ;
    display () ;
    ~Viewport () ;

private :
    int x ;
    int y ; // position of the bottom left corner of the viewport
    int height ;
    int width ; // height and width of the viewport
    View view ;

};
#endif // DEF_VIEWPORT
