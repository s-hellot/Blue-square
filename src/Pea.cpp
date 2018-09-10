#include "Pea.hpp"


Pea::Pea (int x, int y, int z) :
    m_x (x), m_y (y), m_z (z) {}

int Pea::getX()  {
    return m_x ;
}
int Pea::getY() {
    return m_y ;
}
int Pea::getZ() {
    return m_z ;
}
void Pea::setX (int x) {
    m_x = x ;
}
void Pea::setY (int y) {
    m_y = y ;
}
void Pea::setZ (int z) {
    m_z = z ;
}
