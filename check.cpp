#include "const.hpp"


int main() {

	vector<string> values;
	size_t			pos;
	string			file_name;
	values.push_back("form-data;");
	values.push_back("name=\"file\"");
	values.push_back("filename=\"input.cvs\"");

	for (int i = 0; i < values.size(); i++)
	{
		pos = values[i].find("filename=");
		if (pos != string::npos) {
			file_name = values[i].substr(pos + 9);
			if (file_name == "\"\"") {
				cout << "error\n";
				break;
			}
			file_name = file_name.substr(1, file_name.length() - 2);
		}
		cout << file_name << "\n";
		// cout << values[i] << "\n";
	}
}