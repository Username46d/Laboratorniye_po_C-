#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <fstream>
#include <algorithm>

using namespace std;

class User {
private:
    string name;
    int id;
    int accessLevel;
public:
    User(string n, int i, int a) : name(n), id(i), accessLevel(a) {
        if (n.empty()) {
            throw invalid_argument("��� ������������ �� ����� ���� ������");
        }
        if (i < 0) {
            throw invalid_argument("������������� ������������ �� ����� ���� �������������");
        }
        if (a < 0) {
            throw invalid_argument("��� ������� �� ����� ���� �������������");
        }
    }
    string getName() const { return name; }
    int getId() const { return id; }
    int getAccessLevel() const { return accessLevel; }

    void setName(const string& newName) {
        if (newName.empty()) {
            throw invalid_argument("��� �� ����� ���� ������");
        }
        name = newName;
    }
    void setId(int newId) {
        if (newId < 0) {
            throw invalid_argument("������������� ������������ �� ����� ���� �������������");
        }
        id = newId;
    }
    void setAccessLevel(int newAccessLevel) {
        if (newAccessLevel < 0) {
            throw invalid_argument("������� ������� �� ����� ���� �������������");
        }
        accessLevel = newAccessLevel;
    }
    virtual void displayInfo() const {
        cout << "���: " << name << " Id: " << id << " ������� �������: " << accessLevel;
    }

    virtual ~User() = default;

    virtual void SaveToFile(ofstream& out) const {
        out << name << '\n' << id << '\n' << accessLevel << endl;
    }
};

class Student : public User {
private:
    string group;
public:
    Student(string n, int i, int a, string g) : User(n, i, a), group(g) {
        if (g.empty()) {
            throw invalid_argument("�������� ������ �� ����� ���� ������");
        }
    }
    string getGroup() const { return group; }
    void setGroup(const string& newGroup) {
        if (newGroup.empty()) {
            throw invalid_argument("�������� ������ �� ����� ���� ������");
        }
        group = newGroup;
    }
    void displayInfo() const override {
        User::displayInfo();
        cout << " ������: " << group << endl;
    }
    void SaveToFile(ofstream& out) const override {
        out << "Student\n";
        User::SaveToFile(out);
        out << group << endl;
    }
};

class Teacher : public User {
private:
    string departament;
public:
    Teacher(string n, int i, int a, string d) : User(n, i, a), departament(d) {
        if (d.empty()) {
            throw invalid_argument("�������� ������� �� ����� ���� ������");
        }
    }
    string getDepartament() const { return departament; }
    void setDepartament(const string& newDepartament) {
        if (newDepartament.empty()) {
            throw invalid_argument("�������� ������� �� ����� ���� ������");
        }
        departament = newDepartament;
    }
    void displayInfo() const override {
        User::displayInfo();
        cout << " �������: " << departament << endl;
    }
    void SaveToFile(ofstream& out) const override {
        out << "Teacher\n";
        User::SaveToFile(out);
        out << departament << endl;
    }
};

class Administrator : public User {
private:
    string position;
public:
    Administrator(string n, int i, int a, string p) : User(n, i, a), position(p) {
        if (p.empty()) {
            throw invalid_argument("��������� �� ����� ���� ������");
        }
    }
    string getPosition() const { return position; }
    void setPosition(const string& newPosition) {
        if (newPosition.empty()) {
            throw invalid_argument("��������� �� ����� ���� ������");
        }
        position = newPosition;
    }
    void displayInfo() const override {
        User::displayInfo();
        cout << " ���������: " << position << endl;
    }
    void SaveToFile(ofstream& out) const override {
        out << "Administrator\n";
        User::SaveToFile(out);
        out << position << endl;
    }
};

class Resource {
private:
    string typeOfStudy;
    int securityLevel;
public:
    Resource(string t, int s) : typeOfStudy(t), securityLevel(s) {
        if (t.empty()) {
            throw invalid_argument("�������� �� ����� ���� ������");
        }
        if (s < 0) {
            throw invalid_argument("������� ������� �� ����� ���� �������������");
        }
    }
    bool checkAccess(const User& user) const {
        return user.getAccessLevel() >= securityLevel;
    }
    string getTypeOfStudy() const { return typeOfStudy; }
    void setTypeOfStudy(const string& newTypeOfStudy) {
        if (newTypeOfStudy.empty()) {
            throw invalid_argument("�������� �� ����� ���� ������");
        }
        typeOfStudy = newTypeOfStudy;
    }
    int getSecurityLevel() const { return securityLevel; }
    void setSecurityLevel(int newSecurityLevel) {
        if (newSecurityLevel < 0) {
            throw invalid_argument("������� ������� �� ����� ���� �������������");
        }
        securityLevel = newSecurityLevel;
    }
    void displayInfo() const {
        cout << "��������: " << typeOfStudy << " ������� ������������: " << securityLevel << endl;
    }
    void SaveToFile(ofstream& out) const {
        out << typeOfStudy << '\n' << securityLevel << endl;
    }
};

template <typename T>
class AccessControlSystem {
private:
    vector<unique_ptr<User>> users;
    vector<T> resources;
public:
    void addUser(unique_ptr<User> user) {
        users.push_back(move(user));
    }

    void addResource(const T& resource) {
        resources.push_back(resource);
    }

    bool checkAccess(int userId, const string& resourceName) {
        User* foundUser = nullptr;
        for (const auto& user : users) {
            if (user->getId() == userId) {
                foundUser = user.get();
                break;
            }
        }

        const T* foundResource = nullptr;
        for (const auto& resource : resources) {
            if (resource.getTypeOfStudy() == resourceName) {
                foundResource = &resource;
                break;
            }
        }

        if (!foundUser || !foundResource) {
            return false;
        }

        return foundResource->checkAccess(*foundUser);
    }

    void displayAllUsers() const {
        for (const auto& user : users) {
            user->displayInfo();
        }
    }

    void displayAllResources() const {
        for (const auto& resource : resources) {
            resource.displayInfo();
        }
    }
    void saveToFile(const string& filename) const {
        ofstream out(filename);
        if (!out) {
            throw runtime_error("�� ������� ������� ���� ��� ������");
        }
        out << users.size() << '\n';
        for (const auto& user : users) {
            user->saveToFile(out);
        }

        out << resources.size() << '\n';
        for (const auto& resource : resources) {
            resource.saveToFile(out);
        }
    }

    void loadFromFile(const string& filename) {
        ifstream in(filename);
        if (!in) {
            throw runtime_error("�� ������� ������� ���� ��� ������");
        }

        users.clear();
        resources.clear();

        int userCount;
        in >> userCount;
        in.ignore(); 

        for (int i = 0; i < userCount; ++i) {
            string type;
            getline(in, type);

            string name;
            getline(in, name);

            int id, accessLevel;
            in >> id >> accessLevel;
            in.ignore();

            if (type == "Student") {
                string group;
                getline(in, group);
                users.push_back(std::make_unique<Student>(name, id, accessLevel, group));
            }
            else if (type == "Teacher") {
                string department;
                getline(in, department);
                users.push_back(std::make_unique<Teacher>(name, id, accessLevel, department));
            }
            else if (type == "Administrator") {
                string position;
                getline(in, position);
                users.push_back(std::make_unique<Administrator>(name, id, accessLevel, position));
            }
        }
        int resourceCount;
        in >> resourceCount;
        in.ignore();

        for (int i = 0; i < resourceCount; ++i) {
            string name;
            getline(in, name);

            int requiredAccessLevel;
            in >> requiredAccessLevel;
            in.ignore();

            resources.emplace_back(name, requiredAccessLevel);
        }
    }

    void findUserByName(const std::string& name) const {
        bool found = false;
        for (const auto& user : users) {
            if (user->getName() == name) {
                user->displayInfo();
                found = true;
            }
        }
        if (!found) {
            cout << "������������ � ������ '" << name << "' �� ������.\n";
        }
    }

    void findUserById(int id) const {
        bool found = false;
        for (const auto& user : users) {
            if (user->getId() == id) {
                user->displayInfo();
                found = true;
                break;
            }
        }
        if (!found) {
            cout << "������������ � ID " << id << " �� ������.\n";
        }
    }

    void sortUsersByAccessLevel() {
        sort(users.begin(), users.end(),
            [](const auto& a, const auto& b) {
                return a->getAccessLevel() < b->getAccessLevel();
            });
    }

    void sortUsersByName() {
        sort(users.begin(), users.end(),
            [](const auto& a, const auto& b) {
                return a->getName() < b->getName();
            });
    }
};


int main() {
    setlocale(LC_ALL, "RU");
    try {
        AccessControlSystem<Resource> system;

        system.addUser(make_unique<Student>("���� ������", 1, 1, "������ 101"));
        system.addUser(make_unique<Teacher>("���� ������", 2, 2, "������� ����������"));
        system.addUser(make_unique<Administrator>("������ �������", 3, 3, "��������� ������"));

        system.addResource(Resource("��������� 101", 1));
        system.addResource(Resource("����������� ����������������", 2));
        system.addResource(Resource("����������", 1));
        system.addResource(Resource("���������", 3));

        cout << "��� ������������:\n";
        system.displayAllUsers();

        cout << "\n��� �������:\n";
        system.displayAllResources();

        cout << "\n�������� �������:\n";
        cout << "������� � ���������: "
            << (system.checkAccess(1, "��������� 101") ? "������ ��������" : "������ ��������") << "\n";
        cout << "������� � ���������: "
            << (system.checkAccess(1, "���������") ? "������ ��������" : "������ ��������") << "\n";
        cout << "������������� � ���������: "
            << (system.checkAccess(3, "���������") ? "������ ��������" : "������ ��������") << "\n";

    }
    catch (const exception& e) {
        cerr << "������: " << e.what() << "\n";
        return 1;
    }
    return 0;
}


