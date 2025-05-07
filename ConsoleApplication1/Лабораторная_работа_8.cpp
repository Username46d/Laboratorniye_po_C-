#include <iostream>
#include <string>

using namespace std;

class Person {
private:
    string name;
    int age;
    string email;
    string address;
public:
    // Геттеры
    string getName() const {
        return name;
    }

    int getAge() const {
        return age;
    }

    string getEmail() const {
        return email;
    }

    void setName(const string& newName) {
        if (!newName.empty()) {
            name = newName;
        }
        else {
            cerr << "Error: Name cannot be empty!" << std::endl;
        }
    }

    void setAge(int newAge) {
        if (newAge >= 0 && newAge <= 120) {
            age = newAge;
        }
        else {
            cerr << "Error: Age must be between 0 and 120!" << std::endl;
        }
    }

    void setEmail(const string& newEmail) {
        if (newEmail.find('@') != string::npos) {
            email = newEmail;
        }
        else {
            cerr << "Error: Invalid email format!" << std::endl;
        }
    }

    string getAddress() const {
        return address;
    }

    void setAddress(const string& newAddress) {
        if (newAddress.length() > 0) {
            address = newAddress;
        }
        else {
            cerr << "Error: Invalid address format!" << std::endl;
        }
    }

    void displayInfo() const {
        std::cout << "Name: " << name << ", Age: " << age << ", Email: " << email << ", Address: " << address << std::endl;
    }
};

int main() {
    setlocale(LC_ALL, "RU");
    Person person;

    person.setName("John Doe");
    person.setAge(25);
    person.setEmail("john.doe@example.com");
    person.setAddress("Ростов-на-Дону, дом 1");

    cout << "Name: " << person.getName() << endl;
    cout << "Age: " << person.getAge() << endl;
    cout << "Email: " << person.getEmail() << endl;
    cout << "Address: " << person.getAddress() << endl;

    // Пытаемся установить некорректные значения
    person.setName(""); // Ошибка: имя не может быть пустым
    person.setAge(150); // Ошибка: возраст должен быть от 0 до 120
    person.setEmail("invalid-email"); // Ошибка: некорректный email
    person.setAddress("");

    // Выводим информацию о человеке
    person.displayInfo();

    return 0;
}
