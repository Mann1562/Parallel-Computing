#include <string>
#include <sstream>
#include <vector>
#include <iostream>
#include <omp.h>

std::vector<std::string> tokenize(const std::string& input, char delimiter) {
    std::vector<std::string> tokens;
    std::istringstream iss(input);
    std::string token;

    while (std::getline(iss, token, delimiter)) {
        tokens.push_back(token);
    }

    return tokens;
}

std::vector<std::string> parallel_tokenize(const std::string& input, char delimiter) {
    std::vector<std::string> result;
    std::istringstream iss(input);
    std::string token;
    
    #pragma omp parallel private(token) shared(result)
    {
        #pragma omp for schedule(static)
        for (size_t i = 0; i < input.size(); ++i) {
            if (input[i] == delimiter) {
                #pragma omp critical
                {
                    result.push_back(token);
                    token.clear();
                }
            } else {
                token += input[i];
            }
        }
        
        // Add the last token
        #pragma omp critical
        {
            result.push_back(token);
        }
    }

    return result;
}

int main() {
    std::string input = "I M BATMAN.";
    char delimiter = ' ';

    std::vector<std::string> tokens = parallel_tokenize(input, delimiter);

    for (const auto& token : tokens) {
        std::cout << token << std::endl;
    }

    return 0;
}
