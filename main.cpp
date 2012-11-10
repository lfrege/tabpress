#include <iostream>
#include <sstream>
#include <vector>

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

string trim(const string& str)
{
	int first = -1;
	int last = -1;
	int i;

	for (i = 0; i < (int)str.length(); i++)
	{
		if (!(str.c_str()[i] == '\n' || str.c_str()[i] == '\r' || str.c_str()[i] == '\t' || str.c_str()[i] == ' '))
		{
			if (first == -1) { first = i; }
			last = i;
		}
	}

	if (first == -1 || first == last) { return ""; }

	return str.substr(first, last-first+1);
}

class statement
{
	protected:
	int layer;

	public:
	string str;
	string comment;
	vector<statement> subs;

	void setLayer(int l) { layer = l; }

	statement& last()
	{
		if (subs.size() == 0) { nextStmt(); }
		return subs.back();
	}

	void nextStmt()
	{
		subs.push_back(statement());
		subs.back().setLayer(layer + 1);
	}

	string toString() const
	{
		stringstream output;
		string tabs = "";
		string tempstmt = "";
		int i;

		for (i = 0; i < layer; i++) { tabs += "\t"; }

		if (trim(str) != "")
		{
			output << tabs << trim(str);
			if (subs.size() != 0) { output << "\n"; }
			else { output << ";"; }
		}

		if (comment.length() != 0)
		{
			output << tabs << "/* " << trim(comment) << " */\n";
		}

		if (subs.size() != 0)
		{
			output << tabs << "{\n";

			for (i = 0; i < (int)subs.size(); i++)
			{
				tempstmt = subs[i].toString();
				if (trim(tempstmt) != "")
				{
					output << tempstmt << endl;
				}
			}

			output << tabs << "}" << endl;
		}

		return output.str();
	}

	statement()
	{
		layer = -1;
	}
};

const int MODE_CODE = 0;
const int MODE_STRING1 = 1;
const int MODE_COMMENT1 = 2;
const int MODE_WHITESPACE = 3;
const int MODE_COMMENT2 = 4;
const int MODE_STRING2 = 5;

int main (int argc, char ** argv)
{
	int i;
	int mode = MODE_CODE;
	char ch;
	vector<statement*> stack;
	string data = getFullStream(cin);
	statement top;

	stack.push_back(&top);

	for (i = 0; i < (int)data.length(); i++)
	{
		ch = data.c_str()[i];
		if (mode == MODE_CODE)
		{
			if (ch == ' ' || ch == '\n' || ch == '\r' || ch == '\t')
			{
				stack.back()->last().str += ' ';
				mode = MODE_WHITESPACE;
			}
			else if (ch == '/')
			{
				if (data.length() - i > 2 && data.c_str()[i+1] == '*')
				{
					i++;
					mode = MODE_COMMENT1;
				}
				else if (data.length() - i > 2 && data.c_str()[i+1] == '/')
				{
					i++;
					mode = MODE_COMMENT2;
				}
				else
				{
					stack.back()->last().str += ch;
				}
			}
			else if (ch == ';')
			{
				stack.back()->nextStmt();
			}
			else if (ch == '{')
			{
				stack.push_back(&(stack.back()->last()));
			}
			else if (ch == '}')
			{
				stack.pop_back();
				stack.back()->nextStmt();
			}
			else if (ch == '\'')
			{
				stack.back()->last().str += ch;
				mode = MODE_STRING1;
			}
			else if (ch == '\"')
			{
				stack.back()->last().str += ch;
				mode = MODE_STRING2;
			}
			else
			{
				stack.back()->last().str += ch;
			}
		}
		else if (mode == MODE_WHITESPACE)
		{
			if (!(ch == ' ' || ch == '\n' || ch == '\r' || ch == '\t'))
			{
				i--;
				mode = MODE_CODE;
			}
		}
		else if (mode == MODE_STRING1)
		{
			if (ch == '\\' && data.length() - i > 2)
			{
				stack.back()->last().str += ch;
				i++;
				stack.back()->last().str += data.c_str()[i];
			}
			else if (ch == '\'')
			{
				stack.back()->last().str += ch;
				mode = MODE_CODE;
			}
			else
			{
				stack.back()->last().str += ch;
			}
		}
		else if (mode == MODE_STRING2)
		{
			if (ch == '\\' && data.length() - i > 2)
			{
				stack.back()->last().str += ch;
				i++;
				stack.back()->last().str += data.c_str()[i];
			}
			else if (ch == '\"')
			{
				stack.back()->last().str += ch;
				mode = MODE_CODE;
			}
			else
			{
				stack.back()->last().str += ch;
			}
		}
		else if (mode == MODE_COMMENT1)
		{
			if (ch == '*' && data.length() - i > 2 && data.c_str()[i+1] == '/')
			{
				i++;
				mode = MODE_CODE;
			}
			else
			{
				stack.back()->last().comment += ch;
			}
		}
		else if (mode == MODE_COMMENT2)
		{
			if (ch == '\n' || ch == '\r')
			{
				mode = MODE_CODE;
			}
			else
			{
				stack.back()->last().comment += ch;
			}
		}
	}

	for (i = 0; i < (int)top.subs.size(); i++)
	{
		cout << top.subs[i].toString() << endl;
	}

	return 0;
}

