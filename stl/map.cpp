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
map<int, vector<Employee>> employees;

void addEmployee(int wage, Employee e) {
    employees[wage].push_back(e);

    //map<int, vector<Employee>>::iterator it;
    //if ((it = employees.find(wage)) != employees.end()) {
    //    it->second.push_back(e);
    //} else {
    //    vector<Employee> tmp;
    //    tmp.push_back(e);
    //    employees.insert({wage, tmp});
    //}
}
vector<Employee> getSortedNames() {
    vector<Employee> sorted = {};
    for (auto const& [wage, vec] : employees) {
        for (auto const& e : vec) {
            sorted.push_back(e);
        }
    }
    return sorted;
}

struct Key {
    int first;
    int second;

    bool operator<(const Key& key) const {
        if (first == key.first) {
            return second < key.second;
        }
        return first < key.first;
    }
};

template<class T, class P>
void printMap(const map<T, P>& m) {
    for (const auto& [key, value] : m) {    // c++17 syntax
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
ostream& operator<<(ostream& os, const Key& k) {
    return os << k.first << ':' << k.second;
}

int main(void) {
    map<int, string> names;

    names[0] = "kowalski";
    names.insert({3, "wisniewski"});
    printMap(names);

    names[0] = "nowak";
    if (!names.insert({3, "malczewski"}).second) {
        cout << "couldn't add" << endl;
    }
    printMap(names);

    cout << endl << "ZADANIE 1" << endl;
    addEmployee(8000, Employee("Nowak", 5));
    addEmployee(13000, Employee("Kowalski", 20));
    addEmployee(4300, Employee("Polak", 1));
    addEmployee(8000, Employee("Nowacki", 3));
    vector<Employee> sortedEmployees = getSortedNames();
    printVec(sortedEmployees);

    cout << endl << "ZADANIE 2" << endl;
    map<Key, int> keys;
    keys.insert({{2,3},1});
    keys.insert({{2,6},2});
    keys.insert({{2,4},3});
    keys.insert({{4,1},4});
    keys.insert({{2,3},5});
    printMap(keys);
}

