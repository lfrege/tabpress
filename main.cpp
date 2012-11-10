#include <iostream>
#include <sstream>


using namespace std;

string getFullStream(istream& is)
{
	stringstream ss;
	char ch;

	while (!is.bad() && !is.eof())
	{
		is.get(ch);

		ss << ch;
	}

	return ss.str();
}

int main (int argc, char ** argv)
{
	cout << getFullStream(cin);

	return 0;
}