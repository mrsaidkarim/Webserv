#include <iostream>

using namespace std;

int main() {
    size_t i = 0;

    while (i < 1000000000) {
        cout << i << endl;
        if (i == 1000000000)
            i = 0;
        i++;
    }
}