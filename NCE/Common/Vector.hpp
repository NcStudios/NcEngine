#ifndef NCE_COMMON_VECTOR
#define NCE_COMMON_VECTOR

#include <cmath>
#include <limits>

namespace NCE::Common
{
    //double check what this is doing, there may be a better way
    const double EPSILON = std::numeric_limits<double>::epsilon(); 
   
    //may not want any inheritance
    class NCEVector
    {
        private:

        public:

    };

    class Vector2 : NCEVector
    {
        private:
            double m_x, m_y;

        public:
            Vector2(double x_ = 0, double y_ = 0) : m_x(x_), m_y(y_) {}

            static Vector2 Zero()  { return Vector2( 0,  0);  }
            static Vector2 One()   { return Vector2( 1,  1);  }
            static Vector2 Up()    { return Vector2( 0,  1);  }
            static Vector2 Down()  { return Vector2( 0, -1);  }
            static Vector2 Right() { return Vector2( 1,  0);  }
            static Vector2 Left()  { return Vector2(-1,  0);  }

            double GetX() const { return m_x; }
            double GetY() const { return m_y; }
            void Set(double x_, double y_);

            double Magnitude();
            double SquareMagnitude();
            static double SquareMagnitude(const Vector2 &v_);

            void Normalize();
            Vector2 GetNormalized();

            //implement [] operator?
            friend Vector2 operator +(const Vector2 & first_, const Vector2& second_);
            friend Vector2 operator -(const Vector2 & first_, const Vector2& second_);
            friend Vector2 operator *(const Vector2 &v_, const double s_);
            friend Vector2 operator *(const double s_, const Vector2 &v_);
            friend Vector2 operator /(const Vector2 &v_, const double s_);
            friend Vector2 operator /(const double s_, const Vector2 &v_);
            friend bool operator ==(const Vector2 &first_, const Vector2 &second_);
            friend bool operator !=(const Vector2 &first_, const Vector2 &second_);
    };

    class Vector3 : NCEVector
    {
        private:
            double m_x, m_y, m_z;

        public:
            Vector3(double x_ = 0, double y_ = 0, double z_ = 0) : m_x(x_), m_y(y_), m_z(z_) {}

            static Vector3 Zero()  { return Vector3( 0,  0,  0); }
            static Vector3 Up()    { return Vector3( 0,  1,  0); }
            static Vector3 Down()  { return Vector3( 0, -1,  0); }
            static Vector3 Right() { return Vector3( 1,  0,  0); }
            static Vector3 Left()  { return Vector3(-1,  0,  0); }
            static Vector3 Front() { return Vector3( 0,  0,  1); }
            static Vector3 Back()  { return Vector3( 0,  0, -1); }

            double GetX() const { return m_x; }
            double GetY() const { return m_y; }
            double GetZ() const { return m_z; }
    };

    class Vector4 : NCEVector
    {
        private:
            double m_x, m_y, m_z, m_w;

        public:
            Vector4(double x_ = 0, double y_ = 0, double z_ = 0, double w_ = 0) : m_x(x_), m_y(y_), m_z(z_), m_w(w_) {}

            //Static?
            static Vector4 Zero()  { return Vector4( 0, 0, 0, 0); }

            double GetX() const { return m_x; }
            double GetY() const { return m_y; }
            double GetZ() const { return m_z; }
            double GetW() const { return m_w; }

            void Set(double x_, double y_, double z_, double w_);

    };

    //does not belong here
    struct Rect
    {
        int x, y, w, h;

        Rect() : x(0), y(0), w(0), h(0) {};
        Rect(int x_, int y_, int w_, int h_) : x(x_), y(y_), w(w_), h(h_) {};
    };
}

#endif