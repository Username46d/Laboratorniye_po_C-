#include <iostream>
#include <string>

using namespace std;

class Character {
private:
    string name;
    int health;
    int level;
    int attack;
    int defense;
public:
    Character(const string& n, int h, int a, int d) :
        name(n), health(h), attack(a), defense(d) {}
    int getHealth() const { return health; }
    void displayInfo() const {
        cout << "Name: " << name << ", HP: " << health << ", Attack: " << attack << ", Defense: " << defense << endl;
    }
    void attackEnemy(Character& enemy) {
        int damage = attack - enemy.defense;
        if (damage > 0) {
            enemy.health -= damage;
            cout << name << " attacks " << enemy.name << " for " << damage << " damage!" << endl;}
        else {
            cout << name << " attacks " << enemy.name << " , but it has no effect!" << endl;
        }
    }
    void heal(int amount) {
        if (health + amount > 100) {
            health = 100;
        }
        else {
            health += amount;
        }
    }
    void takeDamage(int amount) {
        if (health - amount < 0) {
            health = 0;
        }
        else {
            health -= amount;
        }
    }
};

int main()
{
    setlocale(LC_ALL, "ru");
    Character hero("Hero", 100, 20, 10);
    Character monster("Monster", 50, 15, 5);

    hero.displayInfo();
    monster.displayInfo();

    hero.attackEnemy(monster);
    monster.displayInfo();

    hero.takeDamage(120);
    hero.displayInfo();

    hero.heal(150);
    hero.displayInfo();
}