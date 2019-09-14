#ifndef NCE_COMMON_VECTOR
#define NCE_COMMON_VECTOR

namespace NCE::Common
{
    //Abstract?
    class NCEVector
    {
        private:

        public:

    };

    class Vector2 : NCEVector
    {
        private:
            double _x, _y;

        public:
            Vector2(double t_x = 0, double t_y = 0) : _x(t_x), _y(t_y) {}

            //Static?
            static Vector2 Zero()  { return Vector2( 0,  0);  }
            static Vector2 Up()    { return Vector2( 0,  1);  }
            static Vector2 Down()  { return Vector2( 0, -1);  }
            static Vector2 Right() { return Vector2( 1,  0);  }
            static Vector2 Left()  { return Vector2(-1,  0);  }

            double GetX() const { return _x; }
            double GetY() const { return _y; }

            void Set(int t_x, int t_y);
    };

    class Vector3 : NCEVector
    {
        private:
            double _x, _y, _z;

        public:
            Vector3(double t_x = 0, double t_y = 0, double t_z = 0) : _x(t_x), _y(t_y), _z(t_z) {}

            //Static?
            static Vector3 Zero()  { return Vector3( 0,  0,  0); }
            static Vector3 Up()    { return Vector3( 0,  1,  0); }
            static Vector3 Down()  { return Vector3( 0, -1,  0); }
            static Vector3 Right() { return Vector3( 1,  0,  0); }
            static Vector3 Left()  { return Vector3(-1,  0,  0); }
            static Vector3 Front() { return Vector3( 0,  0,  1); }
            static Vector3 Back()  { return Vector3( 0,  0, -1); }

            double GetX() const { return _x; }
            double GetY() const { return _y; }
            double GetZ() const { return _z; }

    };

    class Vector4 : NCEVector
    {
        private:
            double _a, _b, _c, _d;

        public:
            Vector4(double t_a = 0, double t_b = 0, double t_c = 0, double t_d = 0) : _a(t_a), _b(t_b), _c(t_c), _d(t_d) {}

            //Static?
            static Vector4 Zero()  { return Vector4( 0, 0, 0, 0); }

            double GetA() const { return _a; }
            double GetB() const { return _b; }
            double GetC() const { return _c; }
            double GetD() const { return _d; }

            void Set(double t_a, double t_b, double t_c, double t_d);

    };

    struct Rect
    {
        int x, y, w, h;

        Rect() : x(0), y(0), w(0), h(0) {};
        Rect(int t_x, int t_y, int t_w, int t_h) : x(t_x), y(t_y), w(t_w), h(t_h) {};
    };
}

#endif