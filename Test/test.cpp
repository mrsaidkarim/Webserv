#include <iostream>
#include <vector>
#include <fstream>

using namespace std;

int main() {

	vector<string> _vect;
	ifstream inputFile("main.c");
	_vect.push_back("hello");

	if (_vect.empty())
		printf("the vector is empty");
	for (int i = 0; i < _vect.size(); i++)
	{
		cout << _vect.at(i);
	}
	cout << "\n";
	try
	{
		cout <<  _vect.at(7);
	}
	catch(const std::exception& e)
	{
		std::cerr << "error: " + static_cast<string> (e.what()) << '\n';
	}

	if (!inputFile.is_open())
		cout << "can't open the file";
	else {
		string line;
		while (getline(inputFile, line))
		{
			if (line.empty()) {
				cout << "empty!\n";
				continue;
			}
			line += '\n';
			cout << line;
		}
		inputFile.close();
	}
}