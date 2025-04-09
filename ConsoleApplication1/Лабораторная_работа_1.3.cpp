#include <iostream>
#include <string>

using namespace std;

class Entity {
protected:
    string name;
    int health;
    int attacke;
    int defense;

public:
    Entity(const string& n, int h, int a, int d)
        : name(n), health(h), attacke(a), defense(d) {
    }
    virtual void attack(Entity& target) {
        int damage = attacke - target.defense;
        if (damage > 0) {
            target.health -= damage;
            cout << name << " attacks " << target.name << " for " << damage << " damage!" << endl;
        }
        else {
            cout << name << " attacks " << target.name << ", but it has no effect!" << endl;
        }
    }
    virtual void heal(int amount) {}
    virtual void displayInfo() const {
        cout << "Name: " << name << ", HP: " << health
            << ", Attack: " << attacke << ", Defense: " << defense << endl;
    }
    int getDefense() const { return defense; }
    int getHealth() const { return health; }
    string getName() const { return name; }
    void setHealth(int h) { health = h; }
    virtual ~Entity() {}
};
class Character : public Entity {
public:
    Character(const string& n, int h, int a, int d)
        : Entity(n, h, a, d) {
    }

    void attack(Entity& target) override {
        int damage = attacke - target.getDefense();
        if (damage > 0) {
            if (rand() % 100 < 20) {
                damage *= 2;
                cout << "Critical hit! ";
            }
            target.setHealth(target.getHealth() - damage);
            cout << name << " attacks " << target.getName() << " for " << damage << " damage!" << endl;
        }
        else {
            cout << name << " attacks " << target.getName() << ", but it has no effect!" << endl;
        }
    }
    void heal(int amount) override {
        if (health + amount > 100) {
            health = 100;
        }
        else {
            health += amount;
        }
        cout << "Heal!" << endl;
    }
    void displayInfo() const override {
        std::cout << "Character: " << name << ", HP: " << health
            << ", Attack: " << attacke << ", Defense: " << defense << std::endl;
    }
};

class Monster : public Entity {
public:
    Monster(const std::string& n, int h, int a, int d)
        : Entity(n, h, a, d) {
    }

    void attack(Entity& target) override {
        int damage = attacke - target.getDefense();
        if (damage > 0) {
            if (rand() % 100 < 30) {
                damage += 5;
                cout << "Poisonous attack! ";
            }
            target.setHealth(target.getHealth() - damage);
            cout << name << " attacks " << target.getName() << " for " << damage << " damage!" << endl;
        }
        else {
            cout << name << " attacks " << target.getName() << ", but it has no effect!" << endl;
        }
    }
    void displayInfo() const override {
        cout << "Monster: " << name << ", HP: " << health
            << ", Attack: " << attacke << ", Defense: " << defense << endl;
    }
};

class Boss : public Monster {
public:
    Boss(const std::string& n, int h, int a, int d)
        : Monster(n, h, a, d) {
    }

    void attack(Entity& target) override {
        int damage = attacke - target.getDefense();
        if (damage > 0) {
            if (rand() % 100 > 50) {
                damage += 15;
                cout << "Fireball! ";
            }
            target.setHealth(target.getHealth() - damage);
            cout << name << " attacks " << target.getName() << " for " << damage << " damage!" << endl;
        }
        else {
            cout << name << " attacks " << target.getName() << ", but it has no effect!" << endl;
        }
    }
    void displayInfo() const override {
        cout << "Monster: " << name << ", HP: " << health
            << ", Attack: " << attacke << ", Defense: " << defense << endl;
    }
};

int main() {
    srand(static_cast<unsigned>(time(0)));
    Character hero("Hero", 100, 20, 10);
    Monster goblin("Goblin", 50, 15, 5);
    Monster dragon("Dragon", 150, 25, 20);
    Boss fire_dragon("Dragon", 150, 25, 20);


    hero.displayInfo();
    goblin.displayInfo();
    dragon.displayInfo();
    fire_dragon.displayInfo();

    hero.attack(goblin);
    goblin.attack(hero);
    dragon.attack(hero);
    fire_dragon.attack(hero);

    hero.heal(100);
    return 0;
}
