#include <iostream>
#include <string>
#include <memory>
#include <vector>
#include <deque>
using namespace std;

class Entity {
protected:
    string name;
    int health;
public:
    Entity(const string& n, int h) : name(n), health(h) {}
    virtual void displayInfo() const {
        cout << "Name: " << name << ", HP: " << health << endl;
    }
    virtual ~Entity() = default; 
};

class Player : public Entity {
    int experience;
public:
    Player(const string& n, int h, int exp) : Entity(n, h), experience(exp) {}
    void displayInfo() const override {
        Entity::displayInfo();
        cout << "Experience: " << experience << endl;
    }
};

class Enemy : public Entity {
    string type;
public:
    Enemy(const string& n, int h, const string& t) : Entity(n, h), type(t) {}
    void displayInfo() const override {
        Entity::displayInfo();
        cout << "Type: " << type << endl;
    }
};

class Boss : public Enemy {
    string specialAbility;
public:
    Boss(const string& n, int h, const string& t, const string& a)
        : Enemy(n, h, t), specialAbility(a) {
    }
    void displayInfo() const override {
        Enemy::displayInfo();
        cout << "Special Ability: " << specialAbility << endl;
    }
};

template <typename T>
class GameManager {
    vector<shared_ptr<T>> entities; 
public:
    void addEntity(shared_ptr<T> entity) {
        entities.push_back(entity);
    }
    void displayAll() const {
        for (const auto& entity : entities) {
            entity->displayInfo(); 
        }
    }
};

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
            cout << "ќчередь пуста!" << endl;
        }
        else {
            cout << "”дален элемент: " << elements.front() << endl;
            elements.pop_front();
        }
    }
    bool empty() const { return elements.empty(); }
};

int main(){
    setlocale(LC_ALL, "RU");
    GameManager<Entity> manager;
    manager.addEntity(make_shared<Player>("Hero", 100, 0));
    manager.addEntity(make_shared<Enemy>("Goblin", 50, "Goblin"));
    manager.addEntity(make_shared<Boss>("Dragon", 200, "Dragon", "Fire Breath"));
    manager.displayAll();

    Queue<int> intQueue;
    intQueue.push(321);
    intQueue.push(123);
    intQueue.pop();
    intQueue.pop();
    intQueue.pop();

    Queue<string> strQueue;
    strQueue.push("One");
    strQueue.push("Two");
    strQueue.pop();
    strQueue.pop();
    strQueue.pop();

    return 0;
}
