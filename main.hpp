#ifndef MAIN_H
#define MAIN_H
#include "graph.hpp"
using namespace Graphics;
using namespace std;
using namespace glm;
template <typename T, int N>
struct Tuple {

    Tuple(): data(vector<T>(N)) {}
    T& operator[](int where) {
        static_assert(where < N, "Index must be smaller than tuple-N");
        return data[where];
    }

    vector<T> data;
};

struct Color: public Tuple<GLfloat, 3> {
    Color(GLfloat r, GLfloat g, GLfloat b) {
        this->data[0] = r;
        this->data[1] = g;
        this->data[2] = b;
    }
    Color(Color& c) {
        data = vector<GLfloat>(c.data);
    }

    Color(Color&& c) {
        data = vector<GLfloat>(c.data);
        c.data = vector<GLfloat>(3);
    }

    void clear() const {
        glClearColor(this->data[0], this->data[1], this->data[2], 1.0);
    }

    Color operator+(Color& c) {
        return Color(this->data[0] + c.data[0], this->data[1] + c.data[1], this->data[2] + c.data[2]);
    }

    vec3 toVec() const {
        return vec3(this->data[0], this->data[1], this->data[2]);
    }
};
#endif


