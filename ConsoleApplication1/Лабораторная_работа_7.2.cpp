#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <chrono>
#include <cstdlib>

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
    int getAttack() const { return attacke; }
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
        cout << "Character: " << name << ", HP: " << health
            << ", Attack: " << attacke << ", Defense: " << defense << endl;
    }
};

class Monster : public Entity {
public:
    Monster(const string& n, int h, int a, int d)
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

vector<Monster> monsters;
mutex monstersMutex;
mutex fightMutex;

void generateMonsters() {
    while (true) {
        this_thread::sleep_for(chrono::seconds(3));
        lock_guard<mutex> lock(monstersMutex);
        monsters.push_back(Monster("Goblin", 50, 15, 5));
        cout << "New monster generated!\n";
    }
}

void fightMonsters(Character& hero) {
    while (true) {
        this_thread::sleep_for(chrono::seconds(2));

        lock_guard<mutex> monsterLock(monstersMutex);
        lock_guard<mutex> fightLock(fightMutex);

        if (!monsters.empty()) {
            Monster& monster = monsters.back();
            cout << "Битва между " << hero.getName() << " и " << monster.getName() << endl;

            int damage = max(0, hero.getAttack() - monster.getDefense());
            monster.setHealth(monster.getHealth() - damage);
            cout << hero.getName() << " нанес " << monster.getName() << " " << damage << " урона" << endl;

            if (monster.getHealth() > 0) {
                int damageMonster = max(0, monster.getAttack() - hero.getDefense());
                hero.setHealth(hero.getHealth() - damageMonster);
                cout << monster.getName() << " нанес " << hero.getName() << " " << damageMonster << " урона" << endl;
            }

            if (monster.getHealth() <= 0) {
                cout << monster.getName() << " убит!" << endl;
                monsters.pop_back();
            }

            if (hero.getHealth() <= 0) {
                cout << hero.getName() << " убит!" << endl;
                exit(0);
            }
        }
    }
}

int main() {
    setlocale(LC_ALL, "RU");
    srand(time(nullptr));

    thread monsterGenerator(generateMonsters);
    monsterGenerator.detach();

    Character hero("Hero", 100, 20, 10);

    thread fightThread(fightMonsters, ref(hero));
    fightThread.detach();

    while (true) {
        this_thread::sleep_for(chrono::seconds(1));

        cout << "\nТекущий статус: " << endl;
        hero.displayInfo();

        lock_guard<mutex> lock(monstersMutex);
        cout << "Количество монстров: " << monsters.size() << endl;
        for (auto& monster : monsters) {
            monster.displayInfo();
        }
    }

    return 0;
}