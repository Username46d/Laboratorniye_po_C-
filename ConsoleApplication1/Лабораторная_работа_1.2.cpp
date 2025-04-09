#include <iostream>
#include <string>

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

int main() {
    setlocale(LC_ALL, "ru");
    Player hero("Hero", 100, 20);
    Enemy monster("Monster", 50, "Goblin");
    Boss boss("Boss", 50, "Goblin", "Fireball");

    hero.displayInfo();
    monster.displayInfo();
    boss.displayInfo();
}