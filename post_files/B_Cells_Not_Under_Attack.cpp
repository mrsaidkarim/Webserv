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
		ll n, m; cin >> n >> m;
		ll r, c;
		ll s1 = 0;
		ll s2 = 0;
		vector<bool> rows(n + 1, false), cols(n + 1, false);
		for (ll i = 0; i < m; i++) {
			cin >> r >> c;
			if (rows[r] == false) {
				rows[r] = true;
				s1++;
			}
			if (cols[c] == false) {
				cols[c] = true;
				s2++;
			}
			
			ll ans = (n - s1) * (n - s2);
			cout << ans << " ";	
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
