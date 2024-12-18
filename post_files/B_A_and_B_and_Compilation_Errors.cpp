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
#include <queue>
#include <map>
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


class Solution {
private:

public:
    void solve() {
		int n; cin >> n;
		int a;
		map<ll, ll> store1;

		for (int i = 0; i < n; i++) {
			cin >> a;
			store1[a]++;
		}

		map<ll, ll> store2;
		for (int i = 0; i < n - 1; i++) {
			cin >> a;
			store1[a]--;
			store2[a]++;
		}

		for (int i = 0; i < n - 2; i++) {
			cin >> a;
			store2[a]--;
		}

		map<ll, ll>::iterator it;

		it = store1.begin();
		while (it != store1.end() && it->second == 0)
			it++;
		cout << it->first << "\n";
		it = store2.begin();
		while (it != store2.end() && it->second == 0)
			it++;
		cout << it->first << "\n";
    }

};


int main()
{
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    Solution().solve();
    return (0);
}
