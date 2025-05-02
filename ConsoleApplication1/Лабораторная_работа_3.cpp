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
    Character(const string& n, int h, int a, int d)
        : name(n), health(h), attack(a), defense(d) {
    }
    bool operator==(const Character& other) const {
        return (name == other.name) && (health == other.health);
    }

    friend ostream& operator<<(ostream& os, const Character& character) {
        os << "Character: " << character.name << ", HP: " << character.health
            << ", Attack: " << character.attack << ", Defense: " << character.defense;
        return os;
    }
};

class Weapon {
private:
    string name;
    int damage;
public:
    Weapon(const string& n, int d) :
        name(n), damage(d) {
        cout << "Weapon " << name << " created!" << endl;
    }
    void displayInfo() const {
        cout << "Name: " << name << ", Damage: " << damage << endl;
    }
    Weapon operator+(const Weapon& other) {
        string newName = name + "-" + other.name;
        return Weapon(newName, damage + other.damage);
    }
    bool operator>(const Weapon& other) const {
        return damage > other.damage;
    }
    friend ostream& operator<<(ostream& os, const Weapon& weapon) {
        os << "Weapon: " << weapon.name << ", Damage: " << weapon.damage << endl;
        return os;
    }
    ~Weapon() {
        cout << "Weapon " << name << " destroed!" << endl;
    }
};

int main() {
    setlocale(LC_ALL, "RU");
    Character hero1("Hero", 100, 20, 10);
    Character hero2("Hero", 100, 20, 10);
    Character hero3("Warrior", 150, 25, 15);

    if (hero1 == hero2) {
        cout << "Hero1 and Hero2 are the same!\n";
    }
    if (!(hero1 == hero3)) {
        cout << "Hero1 and Hero3 are different!\n";
    }

    cout << hero1 << endl; 

    Weapon sword("Sword", 100);
    Weapon bow("Bow", 50);
    cout << sword + bow;
    if (sword > bow) {
        cout << "Оружие 1 сильнее оружия 2" << endl;
    }
    else {
        cout << "Оружие 2 сильнее оружия 1" << endl;
    }
    return 0;
}
