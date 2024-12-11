#include <iostream>
#include <vector>



// Split function
std::vector<std::string> split(const std::string& str, char delimiter) {
	std::vector<std::string> tokens;
	std::string token;
	for (int i = 0; i < str.size(); i++) {
		if (str[i] == delimiter) {
			if (!token.empty()) {
				tokens.push_back(token);
			}
			token.clear();
		} else {
			token += str[i];
		}
	}
	// Add the last token if not empty
	if (!token.empty()) {
		tokens.push_back(token);
	}
	return tokens;
}

std::string trimString(const std::string& str) {

	int start = 0;
	int end = str.size() - 1;

	while (start <= end && std::isspace(str[start]))
		start++;
	while (end >= 0 && std::isspace(str[end]))
		end--;
	return (str.substr(start, end - start));
}

// Example usage
int main() {
	std::string input = "C++ is a powerful programming language";
	std::string str = "";
	char delimiter = ' ';

	std::vector<std::string> words = split(input, delimiter);

	std::cout << "Split Result:\n";
	for (const auto& word : words) {
		std::cout << word << "\n";
	}
	std::cout << "the result is : " + trimString(str);
	return 0;
}