#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <memory>
#include <stdexcept>
#include <ctime>
#include <map>
#include <clocale>

// ���������� ��� ������������� �������� ��������
class HealthException : public std::runtime_error {
public:
    HealthException(const std::string& msg) : std::runtime_error(msg) {}
};

// ��������� ����� Logger
template<typename T>
class Logger {
private:
    std::ofstream logFile;
public:
    Logger(const std::string& filename) {
        logFile.open(filename, std::ios::app);
        if (!logFile.is_open()) {
            throw std::runtime_error("�� ������� ������� ���� �����");
        }
    }

    ~Logger() {
        if (logFile.is_open()) {
            logFile.close();
        }
    }

    void log(const T& message) {
        time_t now = time(0);
        char* dt = ctime(&now);
        dt[24] = '\0';
        logFile << "[" << dt << "] " << message << std::endl;
        std::cout << message << std::endl;
    }
};

// ����� Inventory
class Inventory {
private:
    std::map<std::string, int> items;
public:
    void addItem(const std::string& item, int count = 1) {
        items[item] += count;
    }

    void removeItem(const std::string& item, int count = 1) {
        if (items.find(item) != items.end()) {
            items[item] -= count;
            if (items[item] <= 0) {
                items.erase(item);
            }
        }
    }

    void display() const {
        if (items.empty()) {
            std::cout << "��������� ����.\n";
            return;
        }
        std::cout << "���������:\n";
        for (const auto& item : items) {
            std::cout << "- " << item.first << ": " << item.second << "\n";
        }
    }

    bool hasItem(const std::string& item) const {
        return items.find(item) != items.end();
    }

    void save(std::ofstream& file) const {
        size_t size = items.size();
        file.write(reinterpret_cast<const char*>(&size), sizeof(size_t));
        for (const auto& item : items) {
            size_t nameSize = item.first.size();
            file.write(reinterpret_cast<const char*>(&nameSize), sizeof(size_t));
            file.write(item.first.c_str(), nameSize);
            file.write(reinterpret_cast<const char*>(&item.second), sizeof(int));
        }
    }

    void load(std::ifstream& file) {
        items.clear();
        size_t size;
        file.read(reinterpret_cast<char*>(&size), sizeof(size_t));
        for (size_t i = 0; i < size; ++i) {
            size_t nameSize;
            file.read(reinterpret_cast<char*>(&nameSize), sizeof(size_t));
            std::string name(nameSize, '\0');
            file.read(&name[0], nameSize);
            int count;
            file.read(reinterpret_cast<char*>(&count), sizeof(int));
            items[name] = count;
        }
    }
};

// ��������������� ���������� ������ Character
class Character;

// ������� ����� Monster
class Monster {
protected:
    std::string name;
    int health;
    int maxHealth;
    int attack;
    int defense;
    int rewardExp;
public:
    Monster(const std::string& n, int h, int a, int d, int exp)
        : name(n), health(h), maxHealth(h), attack(a), defense(d), rewardExp(exp) {
    }

    virtual ~Monster() = default;

    void takeDamage(int damage) {
        if (damage > 0) {
            health -= damage;
            if (health <= 0) {
                health = 0;
                throw HealthException(name + " ��������!");
            }
        }
    }

    virtual void attackTarget(Character& target) = 0;

    virtual void displayInfo() const {
        std::cout << "������: " << name << ", HP: " << health << "/" << maxHealth
            << ", �����: " << attack << ", ������: " << defense << std::endl;
    }

    bool isAlive() const { return health > 0; }
    std::string getName() const { return name; }
    int getHealth() const { return health; }
    int getMaxHealth() const { return maxHealth; }
    int getAttack() const { return attack; }
    int getDefense() const { return defense; }
    int getRewardExp() const { return rewardExp; }
};

// ����� Character
class Character {
private:
    std::string name;
    int health;
    int maxHealth;
    int attack;
    int defense;
    int level;
    int experience;
    Inventory inventory;
public:
    Character(const std::string& n, int h, int a, int d)
        : name(n), health(h), maxHealth(h), attack(a), defense(d), level(1), experience(0) {
    }

    void attackEnemy(Monster& enemy) {
        int damage = attack - enemy.getDefense();
        if (damage > 0) {
            enemy.takeDamage(damage);
            std::cout << name << " ������� " << enemy.getName() << " � ������� " << damage << " �����!" << std::endl;
        }
        else {
            std::cout << name << " ������� " << enemy.getName() << ", �� ��� �� ����� �������!" << std::endl;
        }
    }

    void takeDamage(int damage) {
        if (damage > 0) {
            health -= damage;
            if (health <= 0) {
                health = 0;
                throw HealthException(name + " ��������!");
            }
        }
    }

    void heal(int amount) {
        health += amount;
        if (health > maxHealth) health = maxHealth;
        std::cout << name << " ��������������� " << amount << " HP!" << std::endl;
    }

    void useHealthPotion() {
        if (inventory.hasItem("����� ��������")) {
            heal(30);
            inventory.removeItem("����� ��������");
            std::cout << "������������ 1 ����� ��������. ��������: "
                << (inventory.hasItem("����� ��������") ? "����" : "0") << std::endl;
        }
        else {
            std::cout << "� ��������� ��� ����� ��������!" << std::endl;
        }
    }

    void gainExperience(int exp) {
        experience += exp;
        if (experience >= 100) {
            level++;
            experience -= 100;
            maxHealth += 10;
            health = maxHealth;
            attack += 2;
            defense += 2;
            std::cout << name << " ������� ������� �� " << level << "!" << std::endl;
            std::cout << "�������������� ��������: HP +10, ����� +2, ������ +2" << std::endl;
        }
    }

    void displayInfo() const {
        std::cout << "���: " << name << ", HP: " << health << "/" << maxHealth
            << ", �����: " << attack << ", ������: " << defense
            << ", �������: " << level << ", ����: " << experience << "/100" << std::endl;
    }

    Inventory& getInventory() { return inventory; }
    const std::string& getName() const { return name; }
    int getHealth() const { return health; }
    int getMaxHealth() const { return maxHealth; }
    int getAttack() const { return attack; }
    int getDefense() const { return defense; }
    int getLevel() const { return level; }
    int getExperience() const { return experience; }

    void save(const std::string& filename) {
        std::ofstream file(filename, std::ios::binary);
        if (!file.is_open()) {
            throw std::runtime_error("�� ������� ������� ���� ����������");
        }

        size_t nameSize = name.size();
        file.write(reinterpret_cast<const char*>(&nameSize), sizeof(size_t));
        file.write(name.c_str(), nameSize);

        file.write(reinterpret_cast<const char*>(&health), sizeof(int));
        file.write(reinterpret_cast<const char*>(&maxHealth), sizeof(int));
        file.write(reinterpret_cast<const char*>(&attack), sizeof(int));
        file.write(reinterpret_cast<const char*>(&defense), sizeof(int));
        file.write(reinterpret_cast<const char*>(&level), sizeof(int));
        file.write(reinterpret_cast<const char*>(&experience), sizeof(int));

        inventory.save(file);
        file.close();
    }

    void load(const std::string& filename) {
        std::ifstream file(filename, std::ios::binary);
        if (!file.is_open()) {
            throw std::runtime_error("�� ������� ������� ���� ����������");
        }

        size_t nameSize;
        file.read(reinterpret_cast<char*>(&nameSize), sizeof(size_t));
        name.resize(nameSize);
        file.read(&name[0], nameSize);

        file.read(reinterpret_cast<char*>(&health), sizeof(int));
        file.read(reinterpret_cast<char*>(&maxHealth), sizeof(int));
        file.read(reinterpret_cast<char*>(&attack), sizeof(int));
        file.read(reinterpret_cast<char*>(&defense), sizeof(int));
        file.read(reinterpret_cast<char*>(&level), sizeof(int));
        file.read(reinterpret_cast<char*>(&experience), sizeof(int));

        inventory.load(file);
        file.close();
    }
};

// ����������� ������ ��������
class Goblin : public Monster {
public:
    Goblin() : Monster("������", 30, 8, 3, 20) {}

    void attackTarget(Character& target) override {
        int damage = attack - target.getDefense();
        if (damage > 0) {
            target.takeDamage(damage);
            std::cout << name << " ������� " << target.getName() << " � ������� " << damage << " �����!" << std::endl;
        }
        else {
            std::cout << name << " ������� " << target.getName() << ", �� ��� �� ����� �������!" << std::endl;
        }
    }
};

class Dragon : public Monster {
public:
    Dragon() : Monster("������", 100, 20, 10, 50) {}

    void attackTarget(Character& target) override {
        int damage = attack - target.getDefense();
        if (damage > 0) {
            target.takeDamage(damage);
            std::cout << name << " ����� ����� �� " << target.getName() << " � ������� " << damage << " �����!" << std::endl;
        }
        else {
            std::cout << name << " ������� " << target.getName() << ", �� ��� �� ����� �������!" << std::endl;
        }
    }
};

class Skeleton : public Monster {
public:
    Skeleton() : Monster("������", 40, 10, 5, 30) {}

    void attackTarget(Character& target) override {
        int damage = attack - target.getDefense();
        if (damage > 0) {
            target.takeDamage(damage);
            std::cout << name << " ���� " << target.getName() << " ������ � ������� " << damage << " �����!" << std::endl;
        }
        else {
            std::cout << name << " ������� " << target.getName() << ", �� ��� �� ����� �������!" << std::endl;
        }
    }
};

// ����� Game
class Game {
private:
    Character player;
    Logger<std::string> logger;
    std::vector<std::unique_ptr<Monster>> monsters;

    void showMainMenu() {
        std::cout << "\n=== ������� ���� ===\n";
        std::cout << "1. ���������� �����������\n";
        std::cout << "2. ����������� ��������������\n";
        std::cout << "3. ������� ���������\n";
        std::cout << "4. ��������� ����\n";
        std::cout << "5. �����\n";
        std::cout << "�������� �������: ";
    }

public:
    Game(const std::string& playerName)
        : player(playerName, 100, 10, 5), logger("game_log.txt") {
        logger.log("���� ������. �����: " + playerName);
    }

    void generateMonsters() {
        monsters.clear();
        monsters.push_back(std::make_unique<Goblin>());
        monsters.push_back(std::make_unique<Skeleton>());
        monsters.push_back(std::make_unique<Dragon>());
    }

    void fight(Monster& monster) {
        logger.log("����� �������� ����� " + player.getName() + " � " + monster.getName());

        while (player.getHealth() > 0 && monster.isAlive()) {
            // ��� ������
            std::cout << "\n=== ��� ��� ===\n";
            std::cout << "1. ���������\n";
            std::cout << "2. ������������ ����� ��������\n";
            std::cout << "3. ����������� ��������������\n";
            std::cout << "�������� ��������: ";

            int choice;
            std::cin >> choice;

            try {
                switch (choice) {
                case 1:
                    player.attackEnemy(monster);
                    break;
                case 2:
                    player.useHealthPotion();
                    break;
                case 3:
                    player.displayInfo();
                    monster.displayInfo();
                    continue;
                default:
                    std::cout << "�������� �����! ���������� ���." << std::endl;
                    continue;
                }
            }
            catch (const HealthException& e) {
                logger.log(e.what());
                break;
            }

            // ��� �������
            if (monster.isAlive()) {
                try {
                    monster.attackTarget(player);
                }
                catch (const HealthException& e) {
                    logger.log(e.what());
                    break;
                }
            }

            // ����������� ���������
            player.displayInfo();
            monster.displayInfo();
        }

        if (player.getHealth() > 0 && !monster.isAlive()) {
            logger.log(player.getName() + " ������� " + monster.getName());
            player.gainExperience(monster.getRewardExp());
            if (rand() % 2 == 0) {
                player.getInventory().addItem("����� ��������");
                logger.log("������� ����� ��������!");
                std::cout << "�� ����� ����� ��������!\n";
            }
        }
    }

    void start() {
        generateMonsters();
        bool gameRunning = true;

        while (gameRunning && player.getHealth() > 0 && !monsters.empty()) {
            showMainMenu();
            int choice;
            std::cin >> choice;
            std::cin.ignore(); // ������� ����� �����

            switch (choice) {
            case 1: {
                auto& currentMonster = *monsters.back();
                std::cout << "\n�������� ����� " << currentMonster.getName() << "!\n";
                fight(currentMonster);

                if (!currentMonster.isAlive()) {
                    monsters.pop_back();
                }

                if (player.getHealth() <= 0) {
                    logger.log("���� ��������! " + player.getName() + " ��� ��������.");
                }
                break;
            }
            case 2:
                player.displayInfo();
                break;
            case 3:
                player.getInventory().display();
                break;
            case 4:
                saveGame();
                logger.log("���� ���������");
                std::cout << "���� ������� ���������.\n";
                break;
            case 5:
                gameRunning = false;
                break;
            default:
                std::cout << "�������� �����!\n";
            }
        }

        if (player.getHealth() > 0 && monsters.empty()) {
            logger.log("�����������! " + player.getName() + " ������� ���� ��������!");
            std::cout << "\n�����������! �� �������� ���� ��������!\n";
        }
    }

    void saveGame() {
        player.save("savegame.dat");
    }

    void loadGame() {
        player.load("savegame.dat");
        logger.log("���� ��������� ��� " + player.getName());
        generateMonsters(); // ������������ �������� ����� ��������
    }
};

int main() {
    setlocale(LC_ALL, "Russian"); // ������������� ������� ������

    try {
        srand(static_cast<unsigned>(time(0)));
        std::cout << "=== ��������� RPG ���� ===\n";
        std::cout << "1. ����� ����\n";
        std::cout << "2. ��������� ����\n";
        std::cout << "�������� �������: ";

        int choice;
        std::cin >> choice;
        std::cin.ignore(); // ������� ����� �����

        if (choice == 1) {
            std::cout << "������� ��� ���������: ";
            std::string name;
            std::getline(std::cin, name);

            Game game(name);
            game.start();
        }
        else if (choice == 2) {
            Game game("���������");
            try {
                game.loadGame();
                game.start();
            }
            catch (const std::exception& e) {
                std::cerr << "������ ��������: " << e.what() << "\n";
                std::cerr << "�������� ����� ����.\n";

                std::cout << "������� ��� ���������: ";
                std::string name;
                std::getline(std::cin, name);

                Game newGame(name);
                newGame.start();
            }
        }
        else {
            std::cout << "�������� �����!\n";
        }
    }
    catch (const std::exception& e) {
        std::cerr << "����������� ������: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}

