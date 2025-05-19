#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <fstream>
#include <algorithm>
#include <map>
#include <ctime>
#include <cstdlib>
#include <stdexcept>
#include <sstream>
#include <chrono>
#include <thread>
#include <random>
#include <limits>

using namespace std;

class GameException : public std::runtime_error {
public:
    GameException(const std::string& msg) : std::runtime_error(msg) {}
};

class HealthException : public GameException {
public:
    HealthException(const std::string& msg) : GameException(msg) {}
};

class FileException : public GameException {
public:
    FileException(const std::string& msg) : GameException(msg) {}
};

class InputException : public GameException {
public:
    InputException(const std::string& msg) : GameException(msg) {}
};

// Логгер
class Logger {
private:
    std::ofstream logFile;

public:
    Logger(const std::string& filename) {
        logFile.open(filename, std::ios::app);
        if (!logFile.is_open()) {
            throw std::runtime_error("Не удалось открыть файл логов");
        }
    }

    ~Logger() {
        if (logFile.is_open()) {
            logFile.close();
        }
    }

    template <typename T>
    void log(const T& message) {
        logFile << message << std::endl; 
        std::cout << message << std::endl; 
    }
};

// Инвентарь
class Inventory {
private:
    map<string, int> items;
public:
    void addItem(const string& item, int count = 1) {
        if (count <= 0) throw GameException("Нельзя добавить отрицательное количество предметов");
        items[item] += count;
    }

    void removeItem(const string& item, int count = 1) {
        if (count <= 0) throw GameException("Нельзя удалить отрицательное количество предметов");
        if (!hasItem(item)) throw GameException("Предмет " + item + " не найден в инвентаре");
        if (items[item] < count) throw GameException("Недостаточно предметов " + item);

        items[item] -= count;
        if (items[item] <= 0) {
            items.erase(item);
        }
    }

    void display() const {
        if (items.empty()) {
            std::cout << "Инвентарь пуст.\n";
            return;
        }
        std::cout << "Инвентарь:\n";
        for (const auto& item : items) {
            cout << "- " << item.first << ": " << item.second << "\n";
        }
    }

    bool hasItem(const string& item) const {
        return items.find(item) != items.end();
    }

    void save(ofstream& file) const {
        size_t size = items.size();
        file.write(reinterpret_cast<const char*>(&size), sizeof(size_t));
        for (const auto& item : items) {
            size_t nameSize = item.first.size();
            file.write(reinterpret_cast<const char*>(&nameSize), sizeof(size_t));
            file.write(item.first.c_str(), nameSize);
            file.write(reinterpret_cast<const char*>(&item.second), sizeof(int));
        }
    }

    void load(ifstream& file) {
        items.clear();
        size_t size;
        file.read(reinterpret_cast<char*>(&size), sizeof(size_t));

        for (size_t i = 0; i < size; ++i) {
            size_t nameSize;
            file.read(reinterpret_cast<char*>(&nameSize), sizeof(size_t));
            if (nameSize > 100) throw FileException("Некорректный размер имени предмета");

            string name(nameSize, '\0');
            file.read(&name[0], nameSize);

            int count;
            file.read(reinterpret_cast<char*>(&count), sizeof(int));
            if (count < 0 || count > 1000) throw FileException("Некорректное количество предметов");

            items[name] = count;
        }
    }
};

// Монстры
class Character; // Предварительное объявление

class Monster {
protected:
    string name;
    int healthpoint;
    int maxHealth;
    int attack;
    int defense;
    int rewardExperience;
    std::mt19937 gen;

public:
    Monster(string n, int h, int a, int d, int exp) :
        name(n), healthpoint(h), maxHealth(h), attack(a), defense(d), rewardExperience(exp),
        gen(std::random_device{}()) {
        if (n.empty()) throw GameException("Имя монстра не может быть пустым");
        if (a < 0) throw GameException("Атака монстра не может быть отрицательной");
        if (h < 0) throw GameException("Здоровье монстра не может быть отрицательным");
        if (d < 0) throw GameException("Защита монстра не может быть отрицательной");
    }

    virtual ~Monster() = default;

    string getName() const { return name; }
    int getAttack() const { return attack; }
    int getDefense() const { return defense; }
    int getHealthpoint() const { return healthpoint; }
    int getRewardExp() const { return rewardExperience; }

    void takeDamage(int damage) {
        if (damage <= 0) return;

        healthpoint -= damage;
        if (healthpoint <= 0) {
            healthpoint = 0;
            throw HealthException(name + " был побежден!");
        }
    }

    virtual void attackCharacter(Character& player) = 0;

    virtual void info() const {
        cout << "Имя: " << name << " Здоровье: " << healthpoint << "/" << maxHealth
            << " Атака: " << attack << " Защита: " << defense << endl;
    }
};

class Character {
private:
    string name;
    int attack;
    int healthpoint;
    int maxHealth;
    int defense;
    int level;
    int experience;
    Inventory inventory;
    std::shared_ptr<Logger> logger;

public:
    Character(string n, int a, int h, int d, std::shared_ptr<Logger> log) :
        name(n), attack(a), healthpoint(h), maxHealth(h), defense(d),
        level(1), experience(0), logger(log) {
        if (n.empty()) throw GameException("Имя персонажа не может быть пустым");
        if (a < 0) throw GameException("Атака персонажа не может быть отрицательной");
        if (h < 0) throw GameException("Здоровье персонажа не может быть отрицательным");
        if (d < 0) throw GameException("Защита персонажа не может быть отрицательной");

        logger->log("Создан персонаж: " + name);
    }

    void attackMonster(Monster& monster) {
        int damage = std::max(1, attack - monster.getDefense());
        logger->log(name + " атакует " + monster.getName() + " и наносит " + to_string(damage) + " урона");
        monster.takeDamage(damage);
    }

    void useHealthPotion() {
        if (!inventory.hasItem("Зелье здоровья")) {
            logger->log("Попытка использовать зелье здоровья: нет в инвентаре");
            throw GameException("В инвентаре нет зелий здоровья!");
        }

        heal(30);
        inventory.removeItem("Зелье здоровья");
        logger->log(name + " использовал зелье здоровья. Текущее здоровье: " + to_string(healthpoint));
    }

    void heal(int healAmount) {
        if (healAmount <= 0) return;

        healthpoint = min(healthpoint + healAmount, maxHealth);
        logger->log(name + " восстановил " + to_string(healAmount) + " здоровья");
    }

    void takeDamage(int damage) {
        if (damage <= 0) return;

        healthpoint -= damage;
        logger->log(name + " получил " + to_string(damage) + " урона");

        if (healthpoint <= 0) {
            healthpoint = 0;
            throw HealthException(name + " был побежден!");
        }
    }

    void addExperience(int exp) {
        if (exp <= 0) return;

        experience += exp;
        logger->log(name + " получил " + to_string(exp) + " опыта");

        while (experience >= 100) {
            levelUp();
        }
    }

    void levelUp() {
        level++;
        experience -= 100;
        maxHealth += 10;
        healthpoint = maxHealth;
        attack += 2;
        defense += 2;

        string msg = name + " достиг уровня " + to_string(level) + "!";
        msg += "\nУлучшены характеристики: +10 к макс. здоровью, +2 к атаке, +2 к защите";
        logger->log(msg);
    }

    void addItem(const string& item, int count = 1) {
        inventory.addItem(item, count);
        logger->log("Добавлен предмет: " + item + " (x" + to_string(count) + ")");
    }

    void displayInventory() const {
        inventory.display();
    }

    void info() const {
        cout << "Имя: " << name << "\nЗдоровье: " << healthpoint << "/" << maxHealth
            << "\nАтака: " << attack << "\nЗащита: " << defense
            << "\nУровень: " << level << "\nОпыт: " << experience << "/100" << endl;
    }

    string getName() const { return name; }
    int getAttack() const { return attack; }
    int getDefense() const { return defense; }
    int getMaxHealth() const { return maxHealth; }
    int getHealthpoint() const { return healthpoint; }
    int getLevel() const { return level; }
    int getExperience() const { return experience; }
    const Inventory& getInventory() const { return inventory; }

    void save(const string& filename) const {
        ofstream file(filename, ios::binary);
        if (!file.is_open()) {
            throw FileException("Не удалось открыть файл сохранения: " + filename);
        }

        // Сигнатура и версия
        const char* signature = "RPGSAVE";
        file.write(signature, 7);
        const uint16_t version = 1;
        file.write(reinterpret_cast<const char*>(&version), sizeof(version));

        size_t nameSize = name.size();
        file.write(reinterpret_cast<const char*>(&nameSize), sizeof(size_t));
        file.write(name.c_str(), nameSize);

        file.write(reinterpret_cast<const char*>(&healthpoint), sizeof(int));
        file.write(reinterpret_cast<const char*>(&maxHealth), sizeof(int));
        file.write(reinterpret_cast<const char*>(&attack), sizeof(int));
        file.write(reinterpret_cast<const char*>(&defense), sizeof(int));
        file.write(reinterpret_cast<const char*>(&level), sizeof(int));
        file.write(reinterpret_cast<const char*>(&experience), sizeof(int));

        inventory.save(file);

        if (!file.good()) {
            throw FileException("Ошибка записи в файл сохранения");
        }

        logger->log("Игра сохранена в файл: " + filename);
    }

    void load(const string& filename) {
        ifstream file(filename, ios::binary);
        if (!file.is_open()) {
            throw FileException("Не удалось открыть файл сохранения: " + filename);
        }

        // Проверка сигнатуры и версии
        char signature[8] = { 0 };
        file.read(signature, 7);
        if (string(signature) != "RPGSAVE") {
            throw FileException("Неверный формат файла сохранения");
        }

        uint16_t version;
        file.read(reinterpret_cast<char*>(&version), sizeof(version));
        if (version != 1) {
            throw FileException("Неподдерживаемая версия файла сохранения");
        }

        size_t nameSize;
        file.read(reinterpret_cast<char*>(&nameSize), sizeof(size_t));
        if (nameSize > 100) {
            throw FileException("Некорректное имя персонажа в сохранении");
        }

        name.resize(nameSize);
        file.read(&name[0], nameSize);

        file.read(reinterpret_cast<char*>(&healthpoint), sizeof(int));
        file.read(reinterpret_cast<char*>(&maxHealth), sizeof(int));
        file.read(reinterpret_cast<char*>(&attack), sizeof(int));
        file.read(reinterpret_cast<char*>(&defense), sizeof(int));
        file.read(reinterpret_cast<char*>(&level), sizeof(int));
        file.read(reinterpret_cast<char*>(&experience), sizeof(int));

        // Проверка корректности загруженных значений
        if (healthpoint < 0 || maxHealth < 1 || attack < 0 || defense < 0 || level < 1 || experience < 0) {
            throw FileException("Некорректные данные в сохранении");
        }

        inventory.load(file);

        if (!file.good() && !file.eof()) {
            throw FileException("Ошибка чтения файла сохранения");
        }

        logger->log("Игра загружена из файла: " + filename);
    }
};

// Конкретные монстры
class Goblin : public Monster {
public:
    Goblin() : Monster("Гоблин", 50, 15, 5, 20) {}

    void attackCharacter(Character& player) override {
        std::uniform_int_distribution<> dis(0, 9);
        int damage = std::max(1, attack - player.getDefense());

        // 40% chance to bypass some defense
        if (dis(gen) < 4) {
            damage += (player.getDefense() / 2);
            cout << "Гоблин " << name << " пробивает защиту игрока " << player.getName() << endl;
        }

        cout << "Гоблин " << name << " наносит " << damage << " урона игроку " << player.getName() << endl;
        player.takeDamage(damage);
    }
};

class Dragon : public Monster {
public:
    Dragon() : Monster("Дракон", 150, 25, 15, 50) {}

    void attackCharacter(Character& player) override {
        std::uniform_int_distribution<> dis(0, 9);
        int damage = std::max(1, attack - player.getDefense());

        // 30% chance for fire breath (extra damage)
        if (dis(gen) < 3) {
            damage *= 2;
            cout << "Дракон " << name << " использует огненное дыхание на " << player.getName() << endl;
        }

        cout << "Дракон " << name << " наносит " << damage << " урона игроку " << player.getName() << endl;
        player.takeDamage(damage);
    }
};

class Skeleton : public Monster {
public:
    Skeleton() : Monster("Скелет", 70, 10, 8, 30) {}

    void attackCharacter(Character& player) override {
        std::uniform_int_distribution<> dis(0, 99);
        int damage = std::max(1, attack - player.getDefense());

        // 25% chance to heal itself
        if (dis(gen) < 25) {
            int heal = 10;
            healthpoint = min(healthpoint + heal, maxHealth);
            cout << "Скелет " << name << " восстанавливает " << heal << " здоровья" << endl;
        }

        cout << "Скелет " << name << " наносит " << damage << " урона игроку " << player.getName() << endl;
        player.takeDamage(damage);
    }
};

// Игра
class Game {
private:
    std::shared_ptr<Character> player;
    vector<unique_ptr<Monster>> monsters;
    bool gameActive;
    std::shared_ptr<Logger> logger;
    std::mt19937 gen;

    void clearInput() {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }

    int getChoice(int min, int max) {
        int choice;
        while (true) {
            if (cin >> choice) {
                if (choice >= min && choice <= max) {
                    clearInput();
                    return choice;
                }
            }
            clearInput();
            cout << "Неверный ввод. Пожалуйста, введите число от " << min << " до " << max << ": ";
        }
    }

public:
    Game(string playerName) : gameActive(true), gen(std::random_device{}()) {
        logger = make_shared<Logger>("game_log.txt");
        player = make_shared<Character>(playerName, 10, 100, 5, logger);
    }

    void startGame() {
        cout << "Добро пожаловать в текстовую RPG игру!\n";
        cout << "Ваш персонаж:\n";
        player->info();

        // Initial monsters
        monsters.push_back(make_unique<Goblin>());
        monsters.push_back(make_unique<Skeleton>());
        monsters.push_back(make_unique<Dragon>());

        logger->log("Игра начата с 3 монстрами");
    }

    void fight(Monster& monster) {
        cout << "\n=== Бой начинается ===\n";
        cout << player->getName() << " vs " << monster.getName() << "\n\n";

        try {
            while (true) {
                // Player's turn
                cout << "--- Ваш ход ---\n";
                player->info();
                cout << "\n1. Атаковать\n2. Использовать зелье здоровья\n3. Попытаться сбежать\nВыберите действие: ";

                int choice = getChoice(1, 3);

                if (choice == 1) {
                    player->attackMonster(monster);
                }
                else if (choice == 2) {
                    try {
                        player->useHealthPotion();
                    }
                    catch (const GameException& e) {
                        cout << e.what() << endl;
                        continue;
                    }
                }
                else if (choice == 3) {
                    std::uniform_int_distribution<> dis(0, 1);
                    if (dis(gen)) {
                        cout << "Вы успешно сбежали!\n";
                        logger->log(player->getName() + " сбежал от " + monster.getName());
                        return;
                    }
                    else {
                        cout << "Попытка сбежать не удалась!\n";
                        logger->log(player->getName() + " не смог сбежать от " + monster.getName());
                    }
                }

                // Check if monster is defeated
                if (monster.getHealthpoint() <= 0) {
                    cout << monster.getName() << " побежден!\n";
                    int exp = monster.getRewardExp();
                    player->addExperience(exp);

                    // 50% chance to get health potion
                    std::uniform_int_distribution<> dis(0, 1);
                    if (dis(gen)) {
                        player->addItem("Зелье здоровья");
                        cout << "Вы получили зелье здоровья!\n";
                    }

                    break;
                }

                // Monster's turn
                cout << "\n--- Ход монстра ---\n";
                monster.attackCharacter(*player);

                // Check if player is defeated
                if (player->getHealthpoint() <= 0) {
                    cout << player->getName() << " был побежден!\n";
                    gameActive = false;
                    break;
                }

                cout << "\n--- Результаты раунда ---\n";
                cout << player->getName() << ": " << player->getHealthpoint() << "/" << player->getMaxHealth() << " HP\n";
                cout << monster.getName() << ": " << monster.getHealthpoint() << "/" << monster.getHealthpoint() << " HP\n\n";

                this_thread::sleep_for(chrono::seconds(1));
            }
        }
        catch (const HealthException& e) {
            cout << e.what() << endl;
            logger->log(e.what());
        }
    }

    void explore() {
        cout << "\nВы исследуете территорию...\n";
        logger->log(player->getName() + " исследует территорию");

        std::uniform_int_distribution<> encounter(0, 3);
        if (encounter(gen) == 0) {
            cout << "Вы ничего не нашли.\n";
            logger->log(player->getName() + " ничего не нашел");
            return;
        }

        std::uniform_int_distribution<> monsterType(0, 2);
        unique_ptr<Monster> newMonster;

        switch (monsterType(gen)) {
        case 0:
            newMonster = make_unique<Goblin>();
            break;
        case 1:
            newMonster = make_unique<Skeleton>();
            break;
        case 2:
            newMonster = make_unique<Dragon>();
            break;
        }

        cout << "Вы встретили " << newMonster->getName() << "!\n";
        logger->log(player->getName() + " встретил монстра: " + newMonster->getName());

        fight(*newMonster);
    }

    void showMenu() {
        while (gameActive) {
            cout << "\n=== Главное меню ===\n";
            cout << "1. Показать информацию о персонаже\n";
            cout << "2. Показать инвентарь\n";
            cout << "3. Исследовать территорию\n";
            cout << "4. Сохранить игру\n";
            cout << "5. Загрузить игру\n";
            cout << "6. Выйти из игры\n";
            cout << "Выберите действие: ";

            int choice = getChoice(1, 6);

            switch (choice) {
            case 1:
                player->info();
                break;
            case 2:
                player->displayInventory();
                break;
            case 3:
                explore();
                break;
            case 4:
                try {
                    player->save("savegame.dat");
                    cout << "Игра сохранена!\n";
                }
                catch (const FileException& e) {
                    cout << "Ошибка сохранения: " << e.what() << "\n";
                }
                break;
            case 5:
                try {
                    player->load("savegame.dat");
                    cout << "Игра загружена!\n";
                    player->info();
                }
                catch (const exception& e) {
                    cout << "Ошибка загрузки: " << e.what() << "\n";
                }
                break;
            case 6:
                gameActive = false;
                cout << "Выход из игры...\n";
                logger->log("Игра завершена пользователем");
                break;
            }
        }
    }
};

int main() {
    setlocale(LC_ALL, "RU");

    try {
        cout << "Введите имя вашего персонажа: ";
        string name;
        getline(cin, name);

        if (name.empty()) {
            name = "Безымянный";
        }

        Game game(name);
        game.startGame();
        game.showMenu();
    }
    catch (const exception& e) {
        cerr << "Критическая ошибка: " << e.what() << endl;
        return 1;
    }

    return 0;
}


