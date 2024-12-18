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
		int n, m; cin >> n >> m;
		vector<string> grid(n);

		for (int i = 0; i < n; i++) {
			cin >> grid[i];
		}

		bool print;

		for (int r = 0; r < n; r++) {
			for (int c = 0; c < m; c++) {
				print = true;
				for (int i = 0; i < n; i++) {
					if (i != r && grid[r][c] == grid[i][c]) {
						print = false;
						break;
					}
				}

				if (!print)
					continue;

				for (int i = 0; i < m; i++) {
					if (i != c && grid[r][c] == grid[r][i]) {
						print = false;
						break;
					}
				}

				if (!print)
					continue;

				cout << grid[r][c];
			}		
		}

		cout << endl;


    }

};


int main()
{
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    Solution().solve();
    return (0);
}
