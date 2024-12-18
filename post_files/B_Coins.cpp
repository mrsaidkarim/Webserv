#include <algorithm>
#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <iterator>
#include <set>
#include <vector>
#include <numeric>
#include <cmath>
using namespace std;

void    fill(vector<int> &v, string &s) {
    if (s[1] == '>') {
        v[s[0] - 'A']++;
    } else {
        v[s[2] - 'A']++;
    }
}

int find(vector<int> &v, int val) {
    for (int i = 0; i < v.size(); i++) {
        if (v[i] == val)
            return (i);
    }
    return (-1);
} 

int main()
{
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    string x, y, z;
    cin >> x >> y >> z;
    vector<int> v(3, 0);
    fill(v, x); fill(v, y); fill(v, z);
    bool zero = false, one = false, two = false;
    for (int i = 0; i < 3; i++) {
        if (v[i] == 0)
            zero = true;
        else if (v[i] == 1) 
            one = true;
        else if (v[i] == 2)
            two = true;
    }
    if (!zero || !one || !two)
        cout << "Impossible\n";
    else {
        cout << (char)(find(v, 0) + 'A');
        cout << (char)(find(v, 1) + 'A');
        cout << (char)(find(v, 2) + 'A');
        cout << "\n";
    }
}
