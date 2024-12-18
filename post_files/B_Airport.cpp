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
		int a;
		priority_queue<int> max_heap;
		priority_queue<int, vector<int>, greater<int> > min_heap;

		for (int i = 0; i < m; i++) {
			cin >> a;
			max_heap.push(a);
			min_heap.push(a);
		}

		int maxi = 0;
		for (int i = 0; i < n; i++) {
			a = max_heap.top();
			max_heap.pop();
			maxi += a;
			a--;
			if (a > 0)
				max_heap.push(a);
		}
			
		int mini = 0;
		for (int i = 0; i < n; i++) {
			a = min_heap.top();
			min_heap.pop();
			mini += a;
			a--;
			if (a > 0)
				min_heap.push(a);
		}

		cout << maxi << " " << mini << "\n";
    }

};


int main()
{
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    Solution().solve();
    return (0);
}
