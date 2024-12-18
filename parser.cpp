#include <iostream>
#include <fstream>
#include <sstream>
#include <stack>
#include <map>
#include <vector>
#include <string>

using namespace std;

struct Block {
    map<string, vector<string>> keyValues;
    map<string, Block> subBlocks;
};

class ConfigParser {
private:
    ifstream configFile;
    Block serverBlock; // Root block

    // Utility functions remain the same...
public:
    void parse() {
        string line;
        stack<Block*> contextStack;
        contextStack.push(&serverBlock); // Start at the root

        while (getline(configFile, line)) {
            line = trim(line);
            if (line.empty() || line[0] == '#') continue; // Skip comments and empty lines

            if (line.back() == '{') {
                string blockName = trim(line.substr(0, line.size() - 1)); // Remove '{'
                contextStack.top()->subBlocks[blockName] = Block();     // Create a new block
                contextStack.push(&contextStack.top()->subBlocks[blockName]); // Push new block
            } else if (line == "}") {
                contextStack.pop(); // End the current block
            } else {
                auto [key, values] = parseKeyValue(line);
                contextStack.top()->keyValues[key] = values; // Add key-value pair to the current block
            }
        }
    }

    void printConfig(const Block& block, int depth = 0) const {
        string indent(depth * 2, ' ');

        for (const auto& [key, values] : block.keyValues) {
            cout << indent << key << ": ";
            for (const auto& value : values) {
                cout << value << " ";
            }
            cout << endl;
        }

        for (const auto& [name, subBlock] : block.subBlocks) {
            cout << indent << name << " {" << endl;
            printConfig(subBlock, depth + 1);
            cout << indent << "}" << endl;
        }
    }
};


int main(int ac, char** av) {
    try {

        // Create an instance of ConfigParser
        ConfigParser parser(av[1]);

        // Parse the configuration file
        parser.parse();

        // Print the parsed configuration for debugging
        cout << "Parsed Configuration:\n";
        parser.print();

    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }

    return 0;
}