#include <iostream>
#include <map>
#include <vector>

using namespace std;

struct Employee {
    Employee(string n, int exp) : name(n), experience(exp) {}
    bool operator<(const Employee& e) const {
        return experience < e.experience;
    }

    string  name;
    int     experience;
};

struct Factory {
    multimap<int, Employee> employees;

    void addEmployee(Employee e, int wage) {
        employees.insert({wage, e});
    }

    vector<Employee> getEmployeesAt(int wage) {
        vector<Employee> tmp;
        for (auto it = employees.lower_bound(wage); it != employees.upper_bound(wage); ++it) {
            tmp.push_back(it->second);
        }
        return tmp;
    }

    size_t countEmployeesAt(int wage) {
        return employees.count(wage);
    }

    vector<int> getUniqueKeys() {
        vector<int> tmp;
        for (auto it = employees.begin(), end = employees.end();
             it != end;
             it = employees.upper_bound(it->first)) {
            tmp.push_back(it->first);
        }
        return tmp;
    }
};

template<class T, class P>
void printMap(const map<T, P>& m) {
    for (const auto& [key, value] : m) {
        cout << '[' << key << "] = " << value << endl;
    }
}

template<class T>
void printVec(const vector<T>& v) {
    for (const auto& el : v) {
        cout << el << " ";
    }
    cout << endl;
}

ostream& operator<<(ostream& os, const Employee& e) {
    return os << e.name << '(' << e.experience << ')';
}

int main(void) {
    cout << "ZADANIE 1" << endl;
    Factory factory;
    factory.addEmployee({"Kowalski", 10}, 10000);
    factory.addEmployee({"Nowak", 7}, 8000);
    factory.addEmployee({"Misiewicz", 1}, 5000);
    factory.addEmployee({"Niziurski", 11}, 10000);
    printVec(factory.getEmployeesAt(10000));
    cout << factory.countEmployeesAt(5000) << endl;
    printVec(factory.getUniqueKeys());
}

