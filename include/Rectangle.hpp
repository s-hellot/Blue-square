#ifndef DEF_RECTANGLE
#define DEF_RECTANGLE

#include <iostream>
#include <fstream>
#include <Exception.hpp>
#include <map>

using namespace std ;

class Rectangle {
public :
    Rectangle (int width = 0, int height = 0, double x_top_left = 0, double y_top_left = 0, int slice_number = 0, int viewport_number = 2) ;

    int getWidth () const  ;
    int getHeight () const ;
    double getXTopLeft () const ;
    double getYTopLeft () const ;
    int getSliceNumber () const ;
    Rectangle mean (Rectangle rect2, int slice_new_rect) ;
    void saveInFile (string file_name) ;


private :
    int m_width ;
    int m_height ;
    double m_x_top_left_coord ;
    double m_y_top_left_coord ;
    // x and y are between -1 and 1 in the viewport coord (basically the openGL coord)
    int m_slice_number ;
    // might not be usefull with Hashmap ?
    int m_viewport_number ;
    /* Index of g_p_viewport :   0     1
                                2     3 */


};



#endif // DEF_VIEWPORT
