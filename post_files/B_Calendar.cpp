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
	bool is_leap_year(int year) {
		if (year % 4 == 0) {
			if (year % 100 == 0) {
				return (year % 400 == 0);
			}
			return (true);
		}	
		return (false);
	}

	bool condition(vector<int> &date1, vector<int> &date2) {
		if (date1[0] < date2[0]) 
			return (true);
	
		if (date1[1] < date2[1])
			return (true);

		if (date1[2] < date2[2])
			return (true);
		
		return (false);
	}

	void swap_dates(vector<int> &date1, vector<int> &date2) {
		if (date1[0] > date2[0]) {
			swap(date1[0], date2[0]);
			swap(date1[1], date2[1]);
			swap(date1[2], date2[2]);
		}

		else if (date1[0] == date2[0]) {
			if (date1[1] > date2[1]) {
				swap(date1[1], date2[1]);
 		        swap(date1[2], date2[2]);
			}

			else if (date1[1] == date2[1]) {	
				if (date1[2] > date2[2])	
 		        	swap(date1[2], date2[2]);
			}
		}

	}

    void solve() {
		string d1; cin >> d1;
		string d2; cin >> d2;

		vector<int> date1;
		vector<int> date2;

		vector<int> days_in_months = {
        0, // for 1 indexed
		31, // 1
        28, // 2
        31, // 3
        30, // 4
        31, // 5
        30, // 6
        31, // 7
        31, // 8
        30, // 9
        31, // 10
        30, // 11
        31  // 12
    	};

		stringstream ss1(d1);
		string token;

		while (getline(ss1, token, ':')) {
			date1.push_back(stoi(token));
		}

		stringstream ss2(d2);
		while (getline(ss2, token, ':')) {
			date2.push_back(stoi(token));	
		}

		swap_dates(date1, date2);

		int cnt_days = 0;

		while (condition(date1, date2)) {
			if (date1[1] == 12 && date1[2] == days_in_months[12]) {
				date1[0]++;
				date1[1] = 1;
				date1[2] = 1;
			} else if (date1[1] == 2 && date1[2] == days_in_months[2] && is_leap_year(date1[0])) {
				date1[2]++;
			} else if (date1[2] >= days_in_months[date1[1]]) {
				date1[1]++;
				date1[2] = 1;
			} else {
				date1[2]++;
			}
			cnt_days++;
		}

		cout << cnt_days << "\n";

    }

};


int main()
{
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    Solution().solve();
    return (0);
}
