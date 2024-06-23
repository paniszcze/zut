#include <algorithm>
#include <iostream>
#include <random>
#include <string>
#include <type_traits>
#include <vector>

struct Employee {
    Employee(std::string n, int e = 0, int p = 0, std::string c = "")
        : name(n), experience(e), pay(p), company(c) {}
    std::string name;
    int experience;
    int pay;
    std::string company;
};

bool anyWorksAt(std::vector<Employee> empls, std::string company) {
    return std::any_of(empls.begin(), empls.end(),
                       [&] (const auto& e) { return e.company == company; });
}

bool anyWorksAt(std::vector<Employee> empls, std::string company, std::size_t& count) {
    count = std::count_if(empls.begin(), empls.end(),
                          [&] (const auto& e) { return e.company == company; });
    return count > 0;
}

std::size_t removeWorkingAt(std::vector<Employee>& empls, std::string company) {
    std::size_t count = 0;
    empls.erase(std::remove_if(empls.begin(), empls.end(),
                   [&] (auto e) {
                       if (e.company == company) count++;
                       return e.company == company;
                   }), empls.end());
    return count;
}

bool cmpEmpls(const Employee& a, const Employee& b) {
    if (a.experience == b.experience) {
        return a.pay < b.pay;
    }
    return a.experience < b.experience;
}

template<typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
bool cmpNums(const T& a, const T& b) {
    return a > b;
}

std::ostream& operator<<(std::ostream& os, const std::vector<int> vec) {
    for (const auto& el : vec) {
        os << el << ' ';
    }
    os << '\n';
    return os;
}

std::ostream& operator<<(std::ostream& os, const std::vector<Employee> vec) {
    for (const auto& el : vec) {
        os << el.name << ", "
           << el.company << " (" << el.experience << "), "
           << el.pay << "\n";
    }
    return os;
}


int main() {
    std::cout << "ZADANIE 1\n";
    auto vec1 = std::vector<int>(20);
    
    std::random_device rd;
    std::uniform_int_distribution<int> dist(0, 100);
    for (auto& el : vec1) el = dist(rd);

    auto vec2 = std::vector(vec1);
    auto vec3 = std::vector(vec1);
    auto vec4 = std::vector(vec1);
    auto vec5 = std::vector(vec1);

    std::sort(vec2.begin(), vec2.end());
    std::sort(vec3.begin() + 8, vec3.begin() + 15);
    std::sort(vec4.begin(), vec4.end(), &cmpNums<int>);
    std::sort(vec5.begin(), vec5.end(),
              [] (const auto& a, const auto& b) { return a > b; });

    std::cout << vec1
              << vec2
              << vec3
              << vec4
              << vec5;

    std::cout << "\nZADANIE 2\n";
    std::vector<Employee> employees{
        { "Jan Kowalski", 3, 8000, "Mobica" },
        { "Kornel Makowski", 2, 6500, "Tietoevry" },
        { "Janusz Polak", 10, 11000, "GlobalLogic" },
        { "Tomasz Nowak", 13, 11200, "Sii" },
        { "Janina Niemiec", 6, 9000, "Mobica" },
        { "Ireneusz Grodzki", 2, 7000, "Tietoevry" }
    };

    std::sort(employees.begin(), employees.end(), &cmpEmpls);
    std::cout << employees;
    
    std::cout << "\nZADANIE 3\n"
              << (anyWorksAt(employees, "Tieto") ? "someone" : "noone")
              << " works at Tieto\n"
              << (anyWorksAt(employees, "Tietoevry") ? "someone" : "noone")
              << " works at Tietoevry\n";
    
    std::size_t count;
    anyWorksAt(employees, "Mobica", count);
    std::cout << count << (count != 1 ? " work" : " works") << " at Mobica\n";

    count = removeWorkingAt(employees, "GlobalLogic");
    std::cout << count << " from GlobalLogic removed\n"
              << employees;
}

