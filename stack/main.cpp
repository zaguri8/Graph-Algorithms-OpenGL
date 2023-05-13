
#include <iostream>
#include <stdlib.h>
template <typename T>
struct Stack {

    T pop() {
        return data.erase(data.begin(), data.begin() + 1)[0];
    }
    void push(T d) {
        data.push_back(d);
    }
    bool empty() {
        return data.size() == 0;
    }
    std::vector<T> data;
};

int main() {
 
    while(!s.empty()) {
        std::cout << s.pop() << "," << std::endl;
    }

    return 0;
}