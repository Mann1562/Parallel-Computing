#include <string>
#include <sstream>
#include <vector>
#include <mutex>
#include <iostream>

std::vector<std::string> tokenize(const std::string& input, char delimiter) {
    std::vector<std::string> tokens;
    std::istringstream iss(input);
    std::string token;

    while (std::getline(iss, token, delimiter)) {
        tokens.push_back(token);
    }

    return tokens;
}

int main() {
    std::string input = "I M BATMAN.";
    char delimiter = ' ';

    std::vector<std::string> tokens = tokenize(input, delimiter);

    for (const auto& token : tokens) {
        std::cout << token << std::endl;
    }

    return 0;
}
