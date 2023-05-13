
#include "main.hpp"
#include <random>
#include <thread>
#include <chrono>

// CONSTANTS
const Color WHITE(1, 1, 1);
const Color BLACK(0, 0, 0);
const Color RED(1, 0, 0);
const Color GREEN(0, 1, 0);
const Color BLUE(0, 0, 1);

int W_WIDTH = 1250;
int W_HEIGHT = 820;


Camera2D camera(W_WIDTH, W_HEIGHT);

void framechanged(GLFWwindow* window, int width, int height) {
    Utilities::framebuffer_size_callback(window, &W_WIDTH, &W_HEIGHT, width, height, &camera);
}

void setup(GLFWwindow* win) {

    BLACK.clear();
    glfwSetFramebufferSizeCallback(win, framechanged);
}


int main() {


    GLFWwindow* win = Renderer::createWindow(W_WIDTH, W_HEIGHT, "OpenGL Template 3D", true);
    if (!win) {
        return 1;
    }

    setup(win);
    Graph<int> graph;

    // Add some random vertices
    random_device rd;
    uniform_real_distribution<float> dist(-5, 5);
    mt19937 rng(rd());

    int numVertices = 32;
    for (int i = 0; i < numVertices; i++) {
        graph.addVertex(i);
    }



    /**
     * @brief Halton sequence for space partitioning
     * this algorithm provides a good distribution
     * of positions around 2D spaces
     * we use the Calpot Halton sequence to decide
     * positions for graph vertices
     */
    HaltonSequence seq(2);
    HaltonSequence seq2(3);

    for (int i = 0; i < numVertices; i++) {
        vec3 next_position_in_dist(seq.next(), seq2.next(), 0.0f);
        graph.vertices[i]->position = next_position_in_dist;
    }

    /**
     * @brief Create random graph with random edges
     * We make sure the graph is fully connected
     * by using 2 unordered maps for seen start points and end points
     * 
     */

    uniform_int_distribution<int> dist2(0, numVertices - 1);
    unordered_map<int, bool> s;
    unordered_map<int, bool> e;

    for (int i = 0; i < numVertices; i++) {

        int start = dist2(rng);
        int end = dist2(rng);

        int tries = 0;
        while (s.find(start) != s.end()) {
            start = dist2(rng);
            tries++;
            if (tries > 100)
                break;
        }
        tries = 0;
        while (e.find(start) != e.end()) {
            end = dist2(rng);
            tries++;
            if (tries > 100)
                break;
        }

        s.insert(make_pair(start, true));
        e.insert(make_pair(end, true));

        graph.addEdge(graph.vertices[start], graph.vertices[end]);
    }

    // move the graph to center
    graph.translate(vec3(-0.5f, -0.5f, 0));

    std::mutex bufferMutex;
  

    /**  
       @brief Depth first search thread
        this thread manages the depth first search simulation 
        on a directed graph, updating vertex position colors
        for each explored vertex on the graph
    **/
    std::thread dfs_thread([&]() {
        glfwMakeContextCurrent(win);
        graph.dfs(graph.vertices[0], [&bufferMutex](VertexPtr<int> p) {
            // create a delay - used for animate the algorithm's steps
            this_thread::sleep_for(chrono::milliseconds(500));


            /*  Critical section, the thread needs to modify OpenGL array buffers  */
            unique_lock<mutex> lock(bufferMutex);
            p->setColor(vec3(1, 0, 0));
            lock.unlock();
        });

        glfwMakeContextCurrent(nullptr);
    });

    dfs_thread.detach();


    while (!glfwWindowShouldClose(win)) {
        glClear(GL_COLOR_BUFFER_BIT);
        glfwPollEvents();
        unique_lock<mutex> lock(bufferMutex);
        graph.render(camera);
        lock.unlock();
        glfwSwapBuffers(win);
    }


    return 0;
}