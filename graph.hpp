#ifndef GRAPH_H
#define GRAPH_H
#define GL_SILENCE_DEPRECATION
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <OpenGL/gl3.h>
#include <GLFW/glfw3.h>
#include <vector>
#include "graphics.hpp"


using namespace Graphics;
using namespace std;
using namespace glm;


template <typename T>
class GraphVertex;

template <typename T>
class Edge;


template <typename T>
using EdgePtr = shared_ptr<Edge<T>>;

template <typename T>
using VertexPtr = shared_ptr<GraphVertex<T>>;

template <typename T>
using WeakEdge = weak_ptr<Edge<T>>;

class Arrow {
public:
    Arrow(
        shared_ptr<ShaderProgram> sp,
        shared_ptr<ShaderProgram> spPoints,
        vec3 startPoint, vec3 direction, float magnitute) :
        direction(direction),
        startPoint(startPoint),
        magnitute(magnitute),
        sp(sp),
        spPoints(spPoints) {

        glGenVertexArrays(1, &VAO);
        glGenVertexArrays(1, &PVAO);

        glGenBuffers(1, &VBO);
        glGenBuffers(1, &CBO);
        glGenBuffers(1, &EBO);
        glGenBuffers(1, &PVBO);
        glGenBuffers(1, &PCBO);
        glBindVertexArray(VAO);
        if (!sp->isLoaded())
            sp->load("shaders/vertex_shader.glsl", "shaders/fragment_shader.glsl");
        sp->use();
        direction = normalize(direction);
        vec3 tempStart = startPoint;
        startPoint = vec3(0, 0, 0);
        // Calculate the end point of the arrow
        vec3 endPoint = startPoint + magnitute * direction;
        // get a normal to that vector
        vec3 arrowNormal = normalize(vec3(-endPoint.y, endPoint.x, 0.0f));

        // calculate the arrow length
        float arrowLength = length(endPoint - startPoint);
        // Compute the arrowhead size as a fraction of the arrow's length
        float arrowheadSize = arrowLength * 0.1f;  // Adjust this factor as needed
        // calculate the base vector
        vec3 aControl = 0.05f * arrowNormal;
        vec3 bControl = 0.05f * arrowNormal;

        quat q1 = angleAxis(radians(45.0f), vec3(0, 0, 1));
        quat q2 = angleAxis(radians(45.0f), vec3(0, 0, 1));

        aControl = vec3(toMat3(q1) * aControl);
        bControl = reflect(aControl, arrowNormal);

        startPoint = tempStart;
        endPoint = startPoint + (magnitute)*direction;
        aControl = endPoint + aControl;
        bControl = endPoint + bControl;
        aControl = endPoint + 0.75f * (aControl - endPoint);
        bControl = endPoint + 0.75f * (bControl - endPoint);


        vector<vec3> points({ startPoint, endPoint, aControl,bControl });
        vector<vec3> colors(points.size(), vec3(0, 0, 1));

        colors[2] = vec3(1, 0, 0);
        colors[3] = vec3(1, 0, 0);


        vector<unsigned int> indices({ 0,1,   1,2,  1,3 });


        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(vec3), points.data(), GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, CBO);
        glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(vec3), colors.data(), GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_DYNAMIC_DRAW);


        // position
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, (void*)0);
        glEnableVertexAttribArray(0);

        // color
        glBindBuffer(GL_ARRAY_BUFFER, CBO);
        glVertexAttribPointer(1, 3, GL_FLOAT, false, 0, (void*)0);
        glEnableVertexAttribArray(1);
        this->indices = indices;

        glBindVertexArray(PVAO);
        if (!spPoints->isLoaded())
            spPoints->load("shaders/vertex_shader.glsl", "shaders/fragment_shader.glsl");
        spPoints->use();
        vector<vec3> pPoints({ startPoint, endPoint });
        vector<vec3> pColors(pPoints.size(), vec3(0, 1, 0));
        glBindBuffer(GL_ARRAY_BUFFER, PVBO);
        glBufferData(GL_ARRAY_BUFFER, pPoints.size() * sizeof(vec3), pPoints.data(), GL_DYNAMIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, PCBO);
        glBufferData(GL_ARRAY_BUFFER, pColors.size() * sizeof(vec3), pColors.data(), GL_DYNAMIC_DRAW);


        glBindBuffer(GL_ARRAY_BUFFER, PVBO);
        glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, (void*)0);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, PCBO);
        glVertexAttribPointer(1, 3, GL_FLOAT, false, 0, (void*)0);
        glEnableVertexAttribArray(1);

        this->pPoints = pPoints;

    }

    void render(Camera2D& camera) {
        glBindVertexArray(VAO);
        if (sp) {
            sp->use();
            sp->setUniform("projection", camera.getProjectionMatrix());
            sp->setUniform("view", camera.getViewMatrix());
            sp->setUniform("model", modelMatrix);
            sp->setUniform("cSize", 1.0f);
        }
        glDrawElements(GL_LINES, indices.size(), GL_UNSIGNED_INT, 0);

        glBindVertexArray(PVAO);
        if (spPoints) {
            spPoints->use();
            spPoints->setUniform("projection", camera.getProjectionMatrix());
            spPoints->setUniform("view", camera.getViewMatrix());
            spPoints->setUniform("model", modelMatrix);
            spPoints->setUniform("cSize", 7.0f);
        }
        glDrawArrays(GL_POINTS, 0, pPoints.size());
    }



    ~Arrow() {
        glDeleteVertexArrays(1, &VAO);
        glDeleteVertexArrays(1, &PVAO);
        glDeleteBuffers(1, &CBO);
        glDeleteBuffers(1, &EBO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &PVBO);
        glDeleteBuffers(1, &PCBO);
    }


    void setColor(vec3 color) {
        glBindVertexArray(VAO);
        vector<vec3> colors(pPoints.size(), color);
        glBindBuffer(GL_ARRAY_BUFFER, CBO);
        glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(vec3), colors.data(), GL_DYNAMIC_DRAW);
    }


    void setFirstPointColor(vec3 color) {
        glBindVertexArray(PVAO);
        vector<vec3> colors(2, color);
        glBindBuffer(GL_ARRAY_BUFFER, PCBO);
        glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(vec3), colors.data(), GL_DYNAMIC_DRAW);
    }

    void setSecondPointColor(vec3 color) {
        glBindVertexArray(PVAO);
        vector<vec3> colors(2, color);
        glBindBuffer(GL_ARRAY_BUFFER, PCBO);
        glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(vec3), colors.data(), GL_DYNAMIC_DRAW);
    }


    vec2 direction;
    vec2 startPoint;
    float magnitute;
    mat4 modelMatrix = mat4(1.0f);

    GLuint VAO, VBO, CBO, EBO;
    GLuint PVAO, PVBO, PCBO;
    shared_ptr<ShaderProgram> sp, spPoints;
    vector<unsigned int> indices;
    vector<vec3> pPoints;
};

template <typename T>
class Edge {


public:
    Edge(shared_ptr<ShaderProgram> sp,
        shared_ptr<ShaderProgram> spPoints) : sp(sp), spPoints(spPoints) {

    }

    void create(
        shared_ptr<ShaderProgram> sp,
        shared_ptr<ShaderProgram> pointsSp) {
        vec3 startPoint = start->position;
        vec3 endPoint = end->position;
        vec3 direction = endPoint - startPoint;
        float magnitute = length(direction);
        if (arrow == nullptr)
            arrow = unique_ptr<Arrow>(new Arrow(sp, pointsSp, startPoint, direction, magnitute));
    }

    Edge(const Edge<T>& other) : start(other.start), end(other.end) {
        create(other->sp, other->spPoints);

    }
    Edge(Edge<T>&& other) : start(other.start), end(other.end) {
        create(other->sp, other->spPoints);
    }

    Edge(
        shared_ptr<ShaderProgram> sp,
        shared_ptr<ShaderProgram> pointsSp,
        GraphVertex<T>* start, GraphVertex<T>* end) : start(start), end(end) {
        create();
    }

    bool operator==(const Edge<T>& other) const {
        return (start == other.start && end == other.end);
    }

    void render(Camera2D& camera) {
        arrow->render(camera);
    }

    void setColor(vec3 color) {
        arrow->setColor(color);
    }
    void setStartVertexColor(vec3 color) {
        start->color = color;
        arrow->setFirstPointColor(start->color);
    }
    void setEndVertexColor(vec3 color) {
        end->color = color;
        arrow->setSecondPointColor(end->color);
    }

public:
    VertexPtr<T> start;
    VertexPtr<T> end;
    unique_ptr<Arrow> arrow;
    shared_ptr<ShaderProgram> sp, spPoints;
};

template <typename T>
class GraphVertex {


public:
    GraphVertex(T data) : data(data) {

    }

    bool operator==(const GraphVertex<T>& other) const {
        return (data == other.data);
    }

    void addEdge(GraphVertex<T>& vertex) {
        WeakEdge<T> edge = WeakEdge<T>();
        edge->start = this;
        edge->end = &vertex;
        edge->create();
        edgesOut.push_back(edge);
        vertex.edgesIn.push_back(edge);
    }


    void setColorBufferIds(GLuint colorVAO, GLuint colorCBO) {
        this->vColorVAO = colorVAO;
        this->vColorCBO = colorCBO;
    }

    void setColor(vec3 color) {
        if (vColorVAO == 100 || vColorCBO == 100) return;
        glBindVertexArray(vColorVAO);
        vector<vec3> colors(2, color);
        glBindBuffer(GL_ARRAY_BUFFER, vColorCBO);
        glBufferData(GL_ARRAY_BUFFER, 2 * sizeof(vec3), colors.data(), GL_DYNAMIC_DRAW);
    }

public:
    vec3 position;
    vec3 color = vec3(1, 0, 0);
    vector<WeakEdge<T>> edgesOut;
    vector<WeakEdge<T>> edgesIn;
    GLuint vColorCBO = 100;
    GLuint vColorVAO = 100;
    T data;
};


template <typename T>
class Stack {

public:
    Stack() {

    }

    void push(T d) {
        data.push_back(d);
    }

    T pop() {
        T d = data.back();
        data.pop_back();
        return d;
    }

    T peek() {
        return data.back();
    }

    bool isEmpty() {
        return data.empty();
    }

private:
    vector<T> data;
};



class HaltonSequence {
public:
    int d, n, base;

    HaltonSequence(int base) : base(base), n(0), d(1) {}


    double next() {
        int x = d - n;

        if (x == 1) {
            n = 1;
            d *= base;
        }
        else {
            int y = d / base;
            while (x <= y) {
                y /= base;
            }
            n = (base + 1) * y - x;
        }
        return static_cast<double>(n) / d;
    }
};


template <typename T>
class Graph {

public:
    vec3 position;

    Graph() {
        sp = make_shared<ShaderProgram>();
        spPoints = make_shared<ShaderProgram>();
    }

    Graph(T data) {
        addVertex(data);
        sp = make_shared<ShaderProgram>();
        spPoints = make_shared<ShaderProgram>();

    }


    void addVertex(T data, vec3 position = vec3(0, 0, 0)) {
        VertexPtr<T>  vertex = make_shared<GraphVertex<T>>(data);
        vertex->position = position;
        vertices.push_back(vertex);
    }



    void render(Camera2D& camera) {
        for (auto& edge : edges) {
            edge->render(camera);
        }
    }

    void addEdge(VertexPtr<T>& start, VertexPtr<T>& end) {
        EdgePtr<T> edge = make_shared<Edge<T>>(sp, spPoints);
        edge->start = VertexPtr<T>(start);
        edge->end = VertexPtr<T>(end);
        edge->create(sp, spPoints);
        edge->start->setColorBufferIds(edge->arrow->PVAO, edge->arrow->PCBO);
        edge->end->setColorBufferIds(edge->arrow->PVAO, edge->arrow->PCBO);
        edges.push_back(edge);
        start->edgesOut.push_back(edge);
        end->edgesIn.push_back(edge);
    }



    void removeVertex(VertexPtr<T>& vertex) {
        for (auto& edge : vertex->edgesIn) {
            removeEdge(edge);
        }
        for (auto& edge : vertex->edgesOut) {
            removeEdge(edge);
        }
        vertices.erase(std::remove(vertices.begin(), vertices.end(), vertex), vertices.end());

    }

    void dfs(VertexPtr<T> vertex, std::function<void(VertexPtr<T>)> callback) {
        Stack<VertexPtr<T>> stack;
        stack.push(vertex);

        unordered_map<VertexPtr<T>, bool> seen;
        seen[vertex] = true;

        while (!stack.isEmpty()) {
            VertexPtr<T> current = stack.pop();
            callback(current);
            for (auto& edge : current->edgesOut) {
                if (EdgePtr<T> e = edge.lock()) {
                    if (seen.find(e->end) == seen.end()) {
                        stack.push(e->end);
                        seen[e->end] = true;
                    }
                }
            }
        }
    }



    void scale(vec3 by) {
        for (auto& edge : edges) {
            edge->arrow->modelMatrix = glm::scale(edge->arrow->modelMatrix, by);
        }
    }

    void translate(vec3 by) {
        for (auto& edge : edges) {
            edge->arrow->modelMatrix = glm::translate(edge->arrow->modelMatrix, by);
        }
    }


    /*
        rotate - angle is given in degrees
    */
    void rotate(float angle, vec3 axis) {
        mat4 q = toMat4(angleAxis(radians(angle), axis));
        for (auto& edge : edges) {
            edge.arrow->modelMatrix *= q;
        }
    }

    void removeEdge(EdgePtr<T>& edge) {
        edge->start->edgesOut.erase(std::remove(edge->start->edgesOut.begin(), edge->start->edgesOut.end(), edge), edge->start->edgesOut.end());
        edge->end->edgesIn.erase(std::remove(edge->end->edgesIn.begin(), edge->end->edgesIn.end(), edge), edge->end->edgesIn.end());
        edges.erase(std::remove(edges.begin(), edges.end(), edge), edges.end());
    }

public:
    vector<EdgePtr<T>> edges;
    vector<VertexPtr<T>> vertices;
    shared_ptr<ShaderProgram>  sp, spPoints;
};
#endif
