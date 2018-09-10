#include "Viewport.hpp"


Viewport::Viewport (GLFWwindow* w, int x, int y, int height, int width, View v) :
    m_x(x), m_y (y), m_height (height), m_width (width)
{
    m_view = v ;
    m_window_parent = w ;
    this->setUpCamera() ;
    m_ratio_screen = 0 ;
    m_image_depth = 0 ;
    m_image_width = 0 ;
    m_image_height = 0 ;
}

int Viewport::getX () const {
    return m_x ;
}

int Viewport::getY () const {
    return m_y ;
}

int Viewport::getHeight () const {
    return m_height ;
}

int Viewport::getWidth () const {
    return m_width ;
}

View Viewport::getView () const {
    return m_view ;
}

double Viewport::getRatio() const {
    return m_ratio_screen ;
}

GLFWwindow* Viewport::getWindow () const {
    return m_window_parent ;
}

mat4 Viewport::getCamera () const {
    return m_camera ;
}
vec3 Viewport::getCameraPosition() const {
    return m_camera_position ;
}

int Viewport::getDepthCoordinate (int x, int y, int z) const {
    switch (m_view) {
    case SAGITAL_PLANE :
        return y ;
        break ;
    case CORONAL_PLANE :
        return x ;
        break ;
    case TRANSVERSE_PLANE :
        return z ;
        break ;
    }
}
void Viewport::setX (int x) {
    m_x = x ;
}

void Viewport::setY (int y) {
    m_y = y ;
}

void Viewport::addX (int x) {
    m_x += x ;
}

void Viewport::addY (int y) {
    m_y += y ;
}

void Viewport::setWidth (int width) {
    m_width = width ;
}

void Viewport::setHeight (int height) {
    m_height = height ;
}

void Viewport::setCamera (mat4 camera) {
    m_camera = camera ;
}
void Viewport::setUpCamera () {
    switch (m_view) {
    case SAGITAL_PLANE :
    // SAGITAL PLANE : separates left and right part of the body (right and left for hand) x = 0
        m_camera_position = vec3 (-10, 0, 0) ;
        break ;
    case CORONAL_PLANE :
    // CORONAL PLANE : separates ventral and dorsal part z = 0
        m_camera_position = vec3 (0.0000001, 0, -10) ;

        break ;
    case TRANSVERSE_PLANE :
    // TRANSVERSE PLANE : separates upper and lower part y = 0
        m_camera_position = vec3 (0, -10, 0) ;
        break ;
    case VOLUME_RENDERING :
        m_camera_position = vec3 (0, -10, -10) ;
        break ;
    case UNKNOWN :
        m_camera_position = vec3 (-10, -10, -10) ;
        break ;
    }
    m_camera = lookAt (m_camera_position, vec3 (0, 0, 0), vec3 (0, 0, 1)) ;
}

void Viewport::setViewportRatio (Image<float,1>* img) {
    m_image_height = img->getVoxelHeight() * img->getHeight() ;
    m_image_width = img->getVoxelWidth() * img->getWidth() ;
    m_image_depth = img->getVoxelDepth() * img->getNumberOfSlices() ;
    switch (m_view) {
    case SAGITAL_PLANE :
        m_ratio_screen = m_image_width / m_image_depth ;
        break ;
    case CORONAL_PLANE :
        m_ratio_screen =  m_image_height / m_image_depth ;
        break ;
    case TRANSVERSE_PLANE :
        m_ratio_screen = m_image_width / m_image_height ;
        break ;
    case VOLUME_RENDERING :
        m_ratio_screen = 1 ;
        break ;
    }
}
void Viewport::getXYZCoordinate (int x_viewport, int y_viewport, double slice, int* x, int* y, int* z, double zoom, double zoom_center_x, double zoom_center_y) {
// x_viewport, y_viewport are the viewport coordinate (top left is 0,0)
// x, y, z the coordinate of the 3D model (x, y 0 to 320 z 0 to 98)
    slice = (slice > 0) ? slice : (-1*slice) ;
    double zoom_distance_xy = (NB_OF_PIXEL_XY - NB_OF_PIXEL_XY/zoom)/2 ;
    // number of pixel "skipped" by the zoom
    double zoom_distance_z = (NB_OF_SLICES - NB_OF_SLICES/zoom)/2 ;
    switch (m_view) {
    case SAGITAL_PLANE :
        *x =   x_viewport * m_image_width / ((float) m_width );
        *z =   y_viewport * m_image_depth / ((float) m_height );
        *y = fmod (slice, NB_OF_PIXEL_XY) ;
        // x from distance to 320 - distance
        *x = *x * (NB_OF_PIXEL_XY - 2*zoom_distance_xy)/NB_OF_PIXEL_XY + zoom_distance_xy + zoom_center_x*NB_OF_PIXEL_XY/2 ;
        *z = *z * (NB_OF_SLICES - 2*zoom_distance_z)/NB_OF_SLICES + zoom_distance_z + zoom_center_y*NB_OF_SLICES/2 ;
        break ;
    case CORONAL_PLANE :
        *y =   x_viewport * m_image_width / ((float) m_width );
        *z =   y_viewport * m_image_depth / ((float) m_height );
        *x = fmod (slice, NB_OF_PIXEL_XY)  ;
        *y = *y * (NB_OF_PIXEL_XY - 2*zoom_distance_xy)/NB_OF_PIXEL_XY + zoom_distance_xy + zoom_center_x*NB_OF_PIXEL_XY/2 ;
        *z = *z * (NB_OF_SLICES - 2*zoom_distance_z)/NB_OF_SLICES + zoom_distance_z + zoom_center_y*NB_OF_SLICES/2 ;
        break ;
    case TRANSVERSE_PLANE :
        *x =   x_viewport * m_image_width / ((float) m_width);
        *y =  y_viewport * m_image_height / ((float) m_height );
        *x = *x * (NB_OF_PIXEL_XY - 2*zoom_distance_xy)/NB_OF_PIXEL_XY + zoom_distance_xy + zoom_center_x*NB_OF_PIXEL_XY/2;
        *y = *y * (NB_OF_PIXEL_XY - 2*zoom_distance_xy)/NB_OF_PIXEL_XY + zoom_distance_xy + zoom_center_y*NB_OF_PIXEL_XY/2;
        *z = fmod (slice, NB_OF_SLICES) ;
        break ;
    }
}
void Viewport::getViewportCoordinateFromXYZ (int x, int y, int z, int* x_viewport, int* y_viewport) {
// x_viewport, y_viewport are the viewport coordinate (top left is 0,0)
// x, y, z the coordinate of the 3D model (x, y 0 to 320 z 0 to 98)
    switch (m_view) {
    case SAGITAL_PLANE :
        *x_viewport = x * ((float) m_width) / ( m_image_width) ;
        *y_viewport = z * ((float) m_height ) / ( m_image_depth) ;
        break ;
    case CORONAL_PLANE :
        *x_viewport = y * ((float) m_width ) / ( m_image_width) ;
        *y_viewport = z * ((float) m_height ) / ( m_image_depth) ;
        break ;
    case TRANSVERSE_PLANE :
        *x_viewport =  x * ((float) m_width ) / ( m_image_width) ;
        *y_viewport = y * ((float) m_height) / ( m_image_height) ;
        break ;
    }

}
float* Viewport::getVertexCoordinateFromViewportCoord (int x, int y) {
// return the vertex coordinate of the 2 lines that meet in (x,y) where x and y are viewport coordinate (top left is 0,0)
// x and y are in the range 0 to m_width/m_height -> normalise to -1 to 1
    float* vertex_return = new float [NB_OF_POINTS] ;
    float y_vertex, x_vertex ;
    x_vertex = (2 * x / ((float) m_width)) - 1 ;
    y_vertex = (2 * y / ((float)m_height)) - 1 ;
    float vertex_coordinate [] = {
                             -1, - y_vertex, 0,
                             1, - y_vertex, 0,
                             x_vertex, -1, 0,
                             x_vertex, 1, 0

    } ;
    for (int i = 0 ; i < NB_OF_POINTS ; i++) {
        vertex_return [i] = vertex_coordinate [i] ;
    }
    return vertex_return ;

}



float* Viewport::getRectangleCoordinateFromViewportCoord (Rectangle rect) {
// return the vertex coordinate of the 4 lines that defines the rectangle
// x and y are in the range 0 to m_width/m_height -> normalise to -1 to 1
    float* vertex_return = new float [NB_OF_POINTS] ;
    double y_top, x_left, x_right, y_bottom,  slice_number, x_vertex_left, x_vertex_right, y_vertex_top, y_vertex_bottom, width_rect, height_rect ;
    int x_viewport, y_viewport, x_right_viewport, y_bottom_viewport ;
    y_top = rect.getYTopLeft() ;
    x_left = rect.getXTopLeft() ;
    slice_number = rect.getSliceNumber() ;
    width_rect = rect.getWidth() ;
    height_rect = rect.getHeight () ;
    x_right = x_left + width_rect ;
    y_bottom = y_top + height_rect ;
    this->getViewportCoordinateFromXYZ(x_left, y_top, slice_number, &x_viewport, &y_viewport) ;
// x_viewport and y_viewport are in the range 0 to m_width/m_height -> normalize to -1 to 1
    this->getViewportCoordinateFromXYZ(x_right, y_bottom, slice_number, &x_right_viewport, &y_bottom_viewport ) ;
    x_vertex_left = (2 * x_viewport / ((float) m_width)) - 1 ;
    y_vertex_top = (2 * y_viewport / ((float) m_height)) - 1 ;
    x_vertex_right = (2 * x_right_viewport / ((float) m_width)) - 1 ;
    y_vertex_bottom = (2 * y_bottom_viewport / ((float) m_height)) - 1 ;
    float vertex_coordinate [] = {
                             x_vertex_left, - y_vertex_top, 0,
                             x_vertex_left, - y_vertex_bottom, 0,
                             x_vertex_right, - y_vertex_top, 0,
                             x_vertex_right, - y_vertex_bottom, 0,
                             x_vertex_left, - y_vertex_top, 0,
                             x_vertex_right, - y_vertex_top, 0,
                             x_vertex_left, - y_vertex_bottom, 0,
                             x_vertex_right,  - y_vertex_bottom, 0,


    } ;
    for (int i = 0 ; i < 2*NB_OF_POINTS ; i++) {
        vertex_return [i] = vertex_coordinate [i] ;
    }
    return vertex_return ;

}
Viewport::~Viewport () {
}

void Viewport::useViewport () const {
    glViewport (m_x, m_y, m_width, m_height) ;
}

Viewport::Viewport(const Viewport& aViewport) {
    m_x = aViewport.getX() ;
    m_y = aViewport.getY() ;
    m_height = aViewport.getHeight() ;
    m_width = aViewport.getWidth() ;
    m_camera = aViewport.getCamera() ;
    m_view = aViewport.getView() ;
    m_window_parent = aViewport.getWindow() ;

}

void Viewport::operator=(const Viewport& aViewport) {
    m_x = aViewport.getX() ;
    m_y = aViewport.getY() ;
    m_height = aViewport.getHeight() ;
    m_width = aViewport.getWidth() ;
    m_camera = aViewport.getCamera() ;
    m_view = aViewport.getView() ;
    m_window_parent = aViewport.getWindow() ;
}
int* Viewport::worldCoordToViewportCoord (int x, int y, int height_window) {
//x and y are mouse coord in the window
//p_xy_viewport are viewport coord (top left is 0,0)
    int* p_xy_viewport = new int [2] ;
    //y = height_window - y ;
    p_xy_viewport[0] = x - m_x ;
    //p_xy[1] = y - m_y  ;
    int y_top_left_corner_viewport = height_window - (m_y + m_height) ;
    p_xy_viewport [1] = y - y_top_left_corner_viewport ;
    return p_xy_viewport ;
}
/* doesn't work
int* Viewport::viewportCoordToWorldCoord (int x, int y) {
//x and y are viewport coord (top left is 0,0) in the window
//p_xy_window are viewport coord (top left is 0,0)
    int* p_xy_window = new int [2] ;
    p_xy_window[0] = x + m_x ;
    p_xy_window[1] = y + m_y ;
    return p_xy_window ;
}*/

bool Viewport::coordInViewport (int x, int y, int height_window) {
    y = height_window - y ;
    return ((x >= m_x) && ( x <= m_x + m_width) && (y >= m_y) && (y <= m_y + m_height)) ;
}



