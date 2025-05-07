#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <memory>
#include <stdexcept>
#include <ctime>
#include <map>
#include <clocale>

// Исключение для некорректного значения здоровья
class HealthException : public std::runtime_error {
public:
    HealthException(const std::string& msg) : std::runtime_error(msg) {}
};

// Шаблонный класс Logger
template<typename T>
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

    void log(const T& message) {
        time_t now = time(0);
        char* dt = ctime(&now);
        dt[24] = '\0';
        logFile << "[" << dt << "] " << message << std::endl;
        std::cout << message << std::endl;
    }
};

// Класс Inventory
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
            std::cout << "Инвентарь пуст.\n";
            return;
        }
        std::cout << "Инвентарь:\n";
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

// Предварительное объявление класса Character
class Character;

// Базовый класс Monster
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
                throw HealthException(name + " побежден!");
            }
        }
    }

    virtual void attackTarget(Character& target) = 0;

    virtual void displayInfo() const {
        std::cout << "Монстр: " << name << ", HP: " << health << "/" << maxHealth
            << ", Атака: " << attack << ", Защита: " << defense << std::endl;
    }

    bool isAlive() const { return health > 0; }
    std::string getName() const { return name; }
    int getHealth() const { return health; }
    int getMaxHealth() const { return maxHealth; }
    int getAttack() const { return attack; }
    int getDefense() const { return defense; }
    int getRewardExp() const { return rewardExp; }
};

// Класс Character
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
            std::cout << name << " атакует " << enemy.getName() << " и наносит " << damage << " урона!" << std::endl;
        }
        else {
            std::cout << name << " атакует " << enemy.getName() << ", но это не имеет эффекта!" << std::endl;
        }
    }

    void takeDamage(int damage) {
        if (damage > 0) {
            health -= damage;
            if (health <= 0) {
                health = 0;
                throw HealthException(name + " побежден!");
            }
        }
    }

    void heal(int amount) {
        health += amount;
        if (health > maxHealth) health = maxHealth;
        std::cout << name << " восстанавливает " << amount << " HP!" << std::endl;
    }

    void useHealthPotion() {
        if (inventory.hasItem("Зелье здоровья")) {
            heal(30);
            inventory.removeItem("Зелье здоровья");
            std::cout << "Использовано 1 зелье здоровья. Осталось: "
                << (inventory.hasItem("Зелье здоровья") ? "есть" : "0") << std::endl;
        }
        else {
            std::cout << "В инвентаре нет зелий здоровья!" << std::endl;
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
            std::cout << name << " повысил уровень до " << level << "!" << std::endl;
            std::cout << "Характеристики улучшены: HP +10, Атака +2, Защита +2" << std::endl;
        }
    }

    void displayInfo() const {
        std::cout << "Имя: " << name << ", HP: " << health << "/" << maxHealth
            << ", Атака: " << attack << ", Защита: " << defense
            << ", Уровень: " << level << ", Опыт: " << experience << "/100" << std::endl;
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
            throw std::runtime_error("Не удалось открыть файл сохранения");
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
            throw std::runtime_error("Не удалось открыть файл сохранения");
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

// Производные классы монстров
class Goblin : public Monster {
public:
    Goblin() : Monster("Гоблин", 30, 8, 3, 20) {}

    void attackTarget(Character& target) override {
        int damage = attack - target.getDefense();
        if (damage > 0) {
            target.takeDamage(damage);
            std::cout << name << " атакует " << target.getName() << " и наносит " << damage << " урона!" << std::endl;
        }
        else {
            std::cout << name << " атакует " << target.getName() << ", но это не имеет эффекта!" << std::endl;
        }
    }
};

class Dragon : public Monster {
public:
    Dragon() : Monster("Дракон", 100, 20, 10, 50) {}

    void attackTarget(Character& target) override {
        int damage = attack - target.getDefense();
        if (damage > 0) {
            target.takeDamage(damage);
            std::cout << name << " дышит огнем на " << target.getName() << " и наносит " << damage << " урона!" << std::endl;
        }
        else {
            std::cout << name << " атакует " << target.getName() << ", но это не имеет эффекта!" << std::endl;
        }
    }
};

class Skeleton : public Monster {
public:
    Skeleton() : Monster("Скелет", 40, 10, 5, 30) {}

    void attackTarget(Character& target) override {
        int damage = attack - target.getDefense();
        if (damage > 0) {
            target.takeDamage(damage);
            std::cout << name << " бьет " << target.getName() << " костью и наносит " << damage << " урона!" << std::endl;
        }
        else {
            std::cout << name << " атакует " << target.getName() << ", но это не имеет эффекта!" << std::endl;
        }
    }
};

// Класс Game
class Game {
private:
    Character player;
    Logger<std::string> logger;
    std::vector<std::unique_ptr<Monster>> monsters;

    void showMainMenu() {
        std::cout << "\n=== Главное меню ===\n";
        std::cout << "1. Продолжить приключение\n";
        std::cout << "2. Просмотреть характеристики\n";
        std::cout << "3. Открыть инвентарь\n";
        std::cout << "4. Сохранить игру\n";
        std::cout << "5. Выйти\n";
        std::cout << "Выберите вариант: ";
    }

public:
    Game(const std::string& playerName)
        : player(playerName, 100, 10, 5), logger("game_log.txt") {
        logger.log("Игра начата. Игрок: " + playerName);
    }

    void generateMonsters() {
        monsters.clear();
        monsters.push_back(std::make_unique<Goblin>());
        monsters.push_back(std::make_unique<Skeleton>());
        monsters.push_back(std::make_unique<Dragon>());
    }

    void fight(Monster& monster) {
        logger.log("Битва началась между " + player.getName() + " и " + monster.getName());

        while (player.getHealth() > 0 && monster.isAlive()) {
            // Ход игрока
            std::cout << "\n=== Ваш ход ===\n";
            std::cout << "1. Атаковать\n";
            std::cout << "2. Использовать зелье здоровья\n";
            std::cout << "3. Просмотреть характеристики\n";
            std::cout << "Выберите действие: ";

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
                    std::cout << "Неверный выбор! Пропускаем ход." << std::endl;
                    continue;
                }
            }
            catch (const HealthException& e) {
                logger.log(e.what());
                break;
            }

            // Ход монстра
            if (monster.isAlive()) {
                try {
                    monster.attackTarget(player);
                }
                catch (const HealthException& e) {
                    logger.log(e.what());
                    break;
                }
            }

            // Отображение состояния
            player.displayInfo();
            monster.displayInfo();
        }

        if (player.getHealth() > 0 && !monster.isAlive()) {
            logger.log(player.getName() + " победил " + monster.getName());
            player.gainExperience(monster.getRewardExp());
            if (rand() % 2 == 0) {
                player.getInventory().addItem("Зелье здоровья");
                logger.log("Найдено зелье здоровья!");
                std::cout << "Вы нашли зелье здоровья!\n";
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
            std::cin.ignore(); // Очищаем буфер ввода

            switch (choice) {
            case 1: {
                auto& currentMonster = *monsters.back();
                std::cout << "\nПоявился дикий " << currentMonster.getName() << "!\n";
                fight(currentMonster);

                if (!currentMonster.isAlive()) {
                    monsters.pop_back();
                }

                if (player.getHealth() <= 0) {
                    logger.log("Игра окончена! " + player.getName() + " был побежден.");
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
                logger.log("Игра сохранена");
                std::cout << "Игра успешно сохранена.\n";
                break;
            case 5:
                gameRunning = false;
                break;
            default:
                std::cout << "Неверный выбор!\n";
            }
        }

        if (player.getHealth() > 0 && monsters.empty()) {
            logger.log("Поздравляем! " + player.getName() + " победил всех монстров!");
            std::cout << "\nПоздравляем! Вы победили всех монстров!\n";
        }
    }

    void saveGame() {
        player.save("savegame.dat");
    }

    void loadGame() {
        player.load("savegame.dat");
        logger.log("Игра загружена для " + player.getName());
        generateMonsters(); // Регенерируем монстров после загрузки
    }
};

int main() {
    setlocale(LC_ALL, "Russian"); // Устанавливаем русскую локаль

    try {
        srand(static_cast<unsigned>(time(0)));
        std::cout << "=== Текстовая RPG игра ===\n";
        std::cout << "1. Новая игра\n";
        std::cout << "2. Загрузить игру\n";
        std::cout << "Выберите вариант: ";

        int choice;
        std::cin >> choice;
        std::cin.ignore(); // Очищаем буфер ввода

        if (choice == 1) {
            std::cout << "Введите имя персонажа: ";
            std::string name;
            std::getline(std::cin, name);

            Game game(name);
            game.start();
        }
        else if (choice == 2) {
            Game game("Временный");
            try {
                game.loadGame();
                game.start();
            }
            catch (const std::exception& e) {
                std::cerr << "Ошибка загрузки: " << e.what() << "\n";
                std::cerr << "Начинаем новую игру.\n";

                std::cout << "Введите имя персонажа: ";
                std::string name;
                std::getline(std::cin, name);

                Game newGame(name);
                newGame.start();
            }
        }
        else {
            std::cout << "Неверный выбор!\n";
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Критическая ошибка: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}

