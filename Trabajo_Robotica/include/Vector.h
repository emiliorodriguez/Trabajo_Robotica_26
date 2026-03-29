#include <iostream>

class Vector{
    public:
        double x, y, z;

        Vector( double x_ = 0, double y_ = 0, double z_ = 0): x(x_), y(y_), z(z_) {}

        void setX(float _x) { x = _x; }
        void setY(float _y) { y = _y; }
        void setZ(float _z) { z = _z; }

        // Métodos para obtener valores (Getters)
        float getX() { return x; }
        float getY() { return y; }
        float getZ() { return z; }
};
