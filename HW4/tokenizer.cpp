#include <iostream>
#include <string>
#include <sstream>

// Custom strtok_r_keep function
std::string strtok_r_keep(std::string& str, const std::string& delim, size_t& pos) {
    size_t start = str.find_first_not_of(delim, pos);
    if (start == std::string::npos) {
        pos = str.length();
        return "";
    }

    size_t end = str.find_first_of(delim, start);
    if (end == std::string::npos) {
        pos = str.length();
        return str.substr(start);
    }

    pos = end;
    return str.substr(start, end - start);
}

int main() {
    std::string str = "I M BATMAN";
    std::string delim = " ";
    size_t pos = 0;

    while (true) {
        std::string token = strtok_r_keep(str, delim, pos);
        if (token.empty()) {
            break;
        }
        std::cout << token << "\n";
    }

    return 0;
}
