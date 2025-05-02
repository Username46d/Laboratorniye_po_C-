#include <iostream>
#include <string>
#include <memory>

using namespace std;

class Entity {
protected:
    string name;
    int health;
public:
    Entity(const string& n, int h) :
        name(n), health(h) {
    }
    virtual void displayInfo() const {
        cout << "Name: " << name << ", HP: " << health << endl;
    };
    virtual ~Entity() {}
};

class Player : public Entity {
private:
    int experience;
public:
    Player(const string& n, int h, int exp) :
        Entity(n, h), experience(exp) {
    }

    int getHealth() const { return health; }
    void displayInfo() const override {
        Entity::displayInfo();
        cout << "Experiance: " << experience << endl;
    }
};

class Enemy : public Entity {
private:
    string type;
public:
    Enemy(const string& n, int h, const string& t) :
        Entity(n, h), type(t) {
    }

    int getHealth() const { return health; }
    void displayInfo() const override {
        Entity::displayInfo();
        cout << "Type: " << type << endl;
    }
};
class Boss : public Enemy {
private:
    string specialAbility;
public:
    Boss(const string& n, int h, const string& t, const string& a) :
        Enemy(n, h, t), specialAbility(a) {
    }

    int getHealth() const { return health; }
    void displayInfo() const override {
        Enemy::displayInfo();
        cout << "specialAbility: " << specialAbility << endl;
    }
};

class Inventory {
private:
    int len;
    int max_len;
    unique_ptr<string[]> items;
    void newSize() {
        max_len = max_len * 2;
        auto newItems = make_unique<string[]>(max_len);

        for (int i = 0; i < len;i++) {
            newItems[i] = items[i];
        }

        items = move(newItems);
    }
public:
    Inventory() : len(0), max_len(1), items(make_unique<string[]>(max_len)) {}

    void addItem(const string& item) {
        if (len >= max_len) {
            newSize();
        }
        items[len++] = item;
    }
    void displayInventory() const {
        cout << "В инвентаре " << len << " вещей " << endl;
        for (int i = 0; i < len; i++) {
            cout << items[i] << endl;
        }
    }
};


int main() {
    setlocale(LC_ALL, "ru");
    // Массив умных указателей на базовый класс
    unique_ptr<Entity> entities[] = {
        make_unique<Player>("Hero", 100, 0),
        make_unique<Enemy>("Goblin", 50, "Goblin")
    };

    // Полиморфизм и автоматическое управление памятью
    for (const auto& entity : entities) {
        entity->displayInfo();
    }

    Inventory inventory_0;
    inventory_0.addItem("Меч");
    inventory_0.addItem("Щит");
    inventory_0.addItem("Лук");
    inventory_0.displayInventory();

    return 0;
}
