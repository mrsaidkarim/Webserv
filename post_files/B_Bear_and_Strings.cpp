/*
*   zech-chi
*/
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
#include <stack>
#include <iomanip>
typedef long long ll;
using namespace std;
#define abs(a) (((a) > 0) ? (a) : (-(a)))
#define max(a, b) (((a) > (b)) ? (a) : (b))
#define min(a, b) (((a) > (b)) ? (b) : (a))
 
 
void solve()
{
    string s; cin >> s;
    string b = "bear";
    int bleft = 0;
    int i, j;
    int left, right;
    int l, r;
    long long res = 0;
 
    i = 0;
    while (i < s.size()) {
        j = 0;
        l = i;
        r = i;
        while (i < s.size() && j < b.size() && s[i] == b[j]) {
            i++;
            j++;
            r++;
        }
 
        if (j == b.size()) {
            left = l - bleft + 1;
            right = s.size() - r + 1;
            res += (left * right);
            bleft = l + 1;
            i--;
        } else {
            i = l;
        }
        i++;
    }

    cout << res << "\n";
}
 
int main()
{
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);
 
    solve();
    return (0);
}