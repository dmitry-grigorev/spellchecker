// spellchecker.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <string>
#include <iostream>
#include <fstream>
#include <queue>
#include <set>
#include <math.h>

using namespace std;

const string sharp("#");
const char sharpc = '#';
enum OPTIONS {SKIP, SKIPWA, REPLACE};

bool onechardiff(const string &s1,const string &s2)
{
	if (s1.compare(s2) == 0) return true;
	if (s1.size() > s2.size() + 1 || s2.size() > s1.size() + 1) return false;

	set<string> vars;

	string::const_iterator s1it = s1.cbegin(), s2it = s2.cbegin(), s1end = s1.cend(), s2end = s2.cend();
	while (*(s1it++) == *(s2it++) && s1it != s1end && s2it != s2end);

	int s1diffpos = distance(s1.begin(), s1it);
	int s2diffpos = distance(s2.begin(), s2it);

	if ((s1it == s1end || s2it == s2end) && s1.at(s1diffpos - 1) == s2.at(s2diffpos - 1)) return true;

	vars.insert(string(s1).replace(s1diffpos-1, 1, sharp));
	vars.insert(string(s2).replace(s2diffpos-1, 1, sharp));

	vars.insert(string(s1).insert(s1diffpos - 1, sharp));
	vars.insert(string(s2).insert(s2diffpos - 1, sharp));

	vars.insert(string(s1).insert(s1diffpos, sharp));
	vars.insert(string(s2).insert(s2diffpos, sharp));

	return vars.size() < 6 ? true : false;
}

void read_text(const char* filename, deque<string> &q) {
	ifstream file(filename);
	string word;
	while (file >> word) 
	{ 
		if (ispunct(word.back()))
		{
			string::iterator chr;
			for (chr = word.begin(); !ispunct(*chr); ++chr);
			q.push_back(string(word.begin(), chr));
			q.push_back(string(chr, word.end()));
		}
		else q.push_back(word);
	}
	file.close();
}

void read_dict(const char* filename, set<string> &d) {
	ifstream file(filename);
	string word;
	while (file >> word)
	{
		d.insert(word);
	}
	file.close();
}

bool indictionary(const string &str, const set<string> &dict)
{
	if (isupper(str[0])) 
	{
		string lower_str(str); lower_str[0] = tolower(lower_str[0]);
		return dict.find(lower_str) != dict.end();
	}
	return dict.find(str) != dict.end();
}

deque<string> provide_similar(const string &str, const set<string> &dict)
{
	deque<string> similars;
	string lower_str(str);
	bool isfirst_cap = isupper(lower_str[0]);
	if (isfirst_cap)
		lower_str[0] = tolower(lower_str[0]);
	for (auto dword = dict.begin(), dend = dict.end(); dword != dend; ++dword)
	{
		if (onechardiff(*dword, lower_str))
		{
			similars.push_back(*dword);
		}
	}
	if (isfirst_cap)
		for (auto sword = similars.begin(), send = similars.end(); sword != send; ++sword)
			(*sword)[0] = toupper((*sword)[0]);
	return similars;
}

void spellcheck(deque<string> &text, set<string> &dict)
{
	OPTIONS opt = SKIP;
	int iopt = 0;
	for (auto word = text.begin(), textend = text.end(); word != textend; ++word)
	{
		if (!indictionary(*word, dict) && !ispunct((*word)[0]))
		{
			cout << "The word '" << *word << "' is unknown." << endl << "Type 0 to skip,\nType 1 to skip with the addition,\nType 2 to replace with the dictionary word." << endl;

			cin >> iopt; opt = static_cast<OPTIONS>(iopt);

			switch (opt)
			{
			case SKIPWA: dict.insert(*word); break;
			case REPLACE: { 
				deque<string> similars = provide_similar(*word, dict);
				cout << "Choose appropriate dictionary word by its number:" << endl;
				unsigned int i = 0;
				for (auto sword = similars.begin(), send = similars.end(); sword != send; ++sword, ++i)
					cout << i << ": " << *sword << endl;
				cin >> i;
				*word = *(similars.begin() + i);
			}; break;
			case SKIP: {};
			default: {};
			}

		}
	}
}

void print_text(const deque<string> text)
{
	for (auto word = text.begin(), textend = text.end(); word != textend; ++word)
	{
		if (!ispunct((*word)[0]))
			cout << " ";
		cout << *word;
	}
	cout << endl;
}

void print_text_to_file(const char *filename, const deque<string> text)
{
	ofstream os(filename);
	for (auto word = text.begin(), textend = text.end(); word != textend; ++word)
	{
		if (!ispunct((*word)[0]))
			os << " ";
		os << *word;
	}
	os.close();
}

int main()
{
	deque<string> text;
	set<string> dict;
	read_text("text.txt", text);
	read_dict("dictionary.txt", dict);

	cout << "Before spellcheck:" << endl;
	print_text(text);

	spellcheck(text, dict);

	cout << "After spellcheck:" << endl;
	print_text(text);
	print_text_to_file("output.txt", text);
	return 0;
}

