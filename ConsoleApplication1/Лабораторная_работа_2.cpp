#include <iostream>
#include <string>

using namespace std;
class Character {
private:
    string name;
    int health;
    int attack;
    int defense;

public:
    // Конструктор
    Character(const string& n, int h, int a, int d)
        : name(n), health(h), attack(a), defense(d) {
        cout << "Character " << name << " created!" << endl;
    }

    // Деструктор
    ~Character() {
        cout << "Character " << name << " destroyed!" << endl;
    }

    void displayInfo() const {
        cout << "Name: " << name << ", HP: " << health
            << ", Attack: " << attack << ", Defense: " << defense << endl;
    }
};

class Monster {
private:
    string name;
    int health;
    int attack;
    int defense;

public:
    Monster(const string& n, int h, int a, int d)
        : name(n), health(h), attack(a), defense(d) {
        cout << "Monster " << name << " created!" << endl;
    }

    ~Monster() {
        cout << "Monster " << name << " destroyed!" << endl;
    }

    void displayInfo() const {
        cout << "Name: " << name << ", HP: " << health
            << ", Attack: " << attack << ", Defense: " << defense << endl;
    }
};
class Weapon {
private:
    string name;
    int damage;
    int mass;
public:
    Weapon(const string& n, int d, int m) :
        name(n), damage(d), mass(m) {
        cout << "Weapon " << name << " created!" << endl;
    }
    void displayInfo() const {
        cout << "Name: " << name << ", Damage: " << damage
            << ", Mass: " << mass << endl;
    }
    ~Weapon() {
        cout << "Weapon " << name << " destroed!" << endl;
    }
};
int main() {
    Weapon sword("Sword", 30, 1500);
    Weapon catana("Catana", 20, 1000);
    Weapon arrow("Arrow", 15, 700);

    sword.displayInfo();
    catana.displayInfo();
    arrow.displayInfo();
}