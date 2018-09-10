#include "Rectangle.hpp"


Rectangle::Rectangle (int width, int height, double x_top_left, double y_top_left, int slice_number, int viewport_number) :
    m_width (width), m_height (height), m_x_top_left_coord (x_top_left),
    m_y_top_left_coord(y_top_left), m_slice_number (slice_number), m_viewport_number (viewport_number)
{

}
int Rectangle::getWidth () const   {
    return m_width ;
}
int Rectangle::getHeight () const  {
    return m_height ;
}

double Rectangle::getXTopLeft () const  {
    return m_x_top_left_coord ;
}
double Rectangle::getYTopLeft () const  {
    return m_y_top_left_coord ;
}
int Rectangle::getSliceNumber () const  {
    return m_slice_number ;
}

double absol (double a) {
    return (a>0?a:(-1*a)) ;
}

Rectangle Rectangle::mean (Rectangle rect2, int slice_new_rect) {
// rect1 is this, rect2 the parameter
//return the mean between them with more influence the nearest you are from the slice

    double coeff1 = 1.0f/absol(this->getSliceNumber() - slice_new_rect) ;
    double coeff2 = 1.0f/absol(rect2.getSliceNumber() - slice_new_rect) ;
    int width = (this->getWidth()*coeff1+ rect2.getWidth()*coeff2)/(coeff1+coeff2) ;
    int height = (this->getHeight()*coeff1+ rect2.getHeight()*coeff2)/(coeff1+coeff2) ;
    double x_left = (this->getXTopLeft()*coeff1+ rect2.getXTopLeft()*coeff2)/(coeff1+coeff2) ;
    double y_top = (this->getYTopLeft()*coeff1+ rect2.getYTopLeft()*coeff2)/(coeff1+coeff2) ;
    return Rectangle(width, height, x_left, y_top, slice_new_rect, 2) ;
}

void Rectangle::saveInFile (string file_name) {
    ofstream file_stream (file_name.c_str(), ios::out | ios::app) ;
    if (file_stream) {
        file_stream << m_width << " " << m_height << " " << m_x_top_left_coord << " " << m_y_top_left_coord << " " << m_slice_number << endl ;
        file_stream.close () ;
    } else
        throw Exception(__FILE__, __FUNCTION__, __LINE__, "Can't open file") ;


}
