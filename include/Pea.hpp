#ifndef DEF_PEA
#define DEF_PEA


#include <Exception.hpp>

using namespace std ;

class Pea {
public :
    Pea (int x = 0, int y = 0 , int z = 0) ;
    int getX() ;
    int getY() ;
    int getZ() ;
    void setX (int x) ;
    void setY (int y) ;
    void setZ (int z) ;
private :
    int m_x ;
    int m_y ;
    int m_z ;
};


#endif // DEF_VIEWPORT
