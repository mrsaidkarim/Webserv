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
		int n, k; cin >> n >> k;
		vector<int> v(n);

		for (int i = 0; i < n; i++) {
			cin >> v[i];
		}

		int res = 0;

		for (int i = 1; i < n; i++) {
			if ((v[i] + v[i - 1]) < k) {
				res += k - v[i] - v[i - 1];
				v[i] += k - v[i] - v[i - 1];
			}
		}

		cout << res << "\n";
		for (int i = 0; i < n; i++) {
			cout << v[i] << " ";
		}
		cout << "\n";
    }

};


int main()
{
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    Solution().solve();
    return (0);
}
