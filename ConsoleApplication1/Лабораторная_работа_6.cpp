#include <iostream>
#include <string>
#include <memory>
#include <vector>
#include <deque>
using namespace std;

template <typename T>
class Queue {
    deque<T> elements;
public:
    void push(const T& item) {
        elements.push_back(item);
        cout << "Ёлемент добавлен: " << item << endl;
    }
    void pop() {
        if (elements.empty()) {
            throw runtime_error("ќчередь пуста!");
        }
        cout << "”дален элемент: " << elements.front() << endl;
        elements.pop_front();
    }
    bool empty() const { return elements.empty(); }
};

int main() {
    setlocale(LC_ALL, "RU");
    try {
        Queue<int> intQueue;
        intQueue.push(321);
        intQueue.push(123);
        intQueue.pop();
        intQueue.pop();
        intQueue.pop();
    }
    catch (const runtime_error& e){
        cerr << "ќшибка очереди: " << e.what() <<  endl;
    }

    return 0;
}
