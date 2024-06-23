#include <any>
#include <iostream>
#include <string>
#include <variant>
#include <vector>

bool getVariant(std::variant<int, float>& v, std::any e) {
    if (e.type() == typeid(int)) {
        v = std::any_cast<int>(e);
        return true;
    } else if (e.type() == typeid(float)) {
        v = std::any_cast<float>(e);
        return true;
    }
    return false;
}

std::ostream& operator<<(std::ostream& os, const std::any& e) {
    if (e.type() == typeid(int)) {
        os << std::any_cast<int>(e);
    } else if (e.type() == typeid(float)) {
        os << std::any_cast<float>(e);
    } else if (e.type() == typeid(std::string)) {
        os << '"' << std::any_cast<std::string>(e) << '"';
    } else {
        os << '?';
    }
    return os;
}

int main() {
    std::vector<std::any> vec;
    vec.push_back(4);
    vec.push_back(3.2f);
    vec.push_back(std::string("something"));
    vec.push_back('c');

    std::variant<int, float> v;
    for (const auto& el : vec) {
        if (getVariant(v, el)) {
            std::cout << el << " assigned to variant\n";
        } else {
            std::cout << "couldn't assign " << el << " to variant\n";
        }
    }
}

