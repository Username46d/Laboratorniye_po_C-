#include <iostream>
#include <string>
#include <memory>
#include <vector>
#include <deque>
#include <fstream>
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
    vector<shared_ptr<Entity>> entities;
public:
    void addEntity(shared_ptr<T> entity) {
        entities.push_back(entity);
    }
    void displayAll() const {
        for (const auto& entity : entities) {
            entity->displayInfo();
        }
    }
    void saveToFile(const std::string& filename) {
        std::ofstream file(filename);
        if (!file) {
            throw std::runtime_error("Failed to open file for writing.");
        }
    }
    void loadFromFile(GameManager<Entity*>& manager, const std::string& filename) {
        std::ifstream file(filename);
        if (!file) {
            throw std::runtime_error("Failed to open file for reading.");
        }
    }
};

template <typename T>
class Queue {
private:
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

    GameManager<Player> manager;
    manager.addEntity(make_shared<Player>("Hero", 100, 0));
    manager.saveToFile("game_save.txt");

    GameManager<Entity*> loadedManager;
    manager.loadFromFile(loadedManager, "game_save.txt");
    loadedManager.displayAll();

    manager.displayAll();
    return 0;
}