/*
Kenya Foster
CS 101 Project 4
project4.cpp
*/

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <ctime> //to get time
#include <algorithm> //to sort
#include <climits> //to disable white space in user input
#pragma warning(disable:4996) //for visual studio warning
//using namespace std;

class StringMap
{
private:
	struct Cell
	{
		std::string key; //this should be screen_name
		std::vector<std::string> value; //vector of strings that holds tweets per user
		Cell *link;
	};

	static const int INITIAL_COUNT = 9000;
	Cell **table;
	int nTable;

public:
	StringMap() //constructor
	{
		nTable = INITIAL_COUNT;
		table = new Cell *[nTable];
		for (int i = 0; i < nTable; i++)
			table[i] = NULL;
	}

	~StringMap() //deconstructor
	{
		for (int i = 0; i < nTable; i++)
		{
			Cell *c = table[i];
			while (c != NULL)
			{
				Cell *oldCell = c;
				c = c->link;
				delete oldCell;
			}
		}
	}

	int hash(const std::string & str) //hash function
	{
		const int HASH_SEED = 5381;
		const int HASH_MULTIPLIER = 33;
		const int HASH_MASK = unsigned(-1) >> 1;

		unsigned hash = HASH_SEED;
		int nchars = str.length();
		for (int i = 0; i < nchars; i++) 
		{
			hash = HASH_MULTIPLIER * hash + str[i];
		}
		return (hash & HASH_MASK);
	}

	std::vector<std::string> get(std::string & key)
	{
		std::vector<std::string> empty;
		empty.push_back("No tweets or screen name was found");
		int cell = hash(key) % nTable;
		Cell *c = find(cell, key);
		if (c == NULL)
			return empty;
		else 
			return c->value;
	}

	void put(std::string & key, std::string & tweet, std::string & date)
	{
		tweet.insert(0," ");
		tweet.insert(1, date);
		//std::cout << tweet << std::endl;
		int cell = hash(key) % nTable;
		Cell *c = find(cell, key);
		if (c == NULL)
		{
			c = new Cell;
			c->key = key;
			c->link = table[cell];
			table[cell] = c;
		}
		c->value.push_back(tweet);

		std::sort(c->value.begin(), c->value.end()); //sorts the tweets in ascending order
	}

	Cell *find(int cell, std::string  & key)
	{
		Cell *c = table[cell];
		while (c != NULL && key != c->key)
			c = c->link;
		return c;
	}

	bool contains(std::string  & key)
	{
		bool contain;
		int cell = hash(key) % nTable;
		Cell *c = find(cell, key);
		if (c == NULL)
			contain = false;
		else
			contain = true;

		return contain;
	}

	int count(int cell, std::string & key)
	{
		Cell *c = table[cell];
		int count = 0;
		while (c != NULL && key != c->key)
		{
			c = c->link;
			count++;
		}
		return count;
	}

	void remove(std::string & key)
	{
		int cell = hash(key) % nTable;
		Cell *c = find(cell, key);
		if (c == NULL)
			std::cout << "No tweets or screen name was found" << std::endl;
		else
			c->value.clear();
	}

	void print(std::vector<std::string> & v)
	{
		for (unsigned i = 0; i < v.size(); i++)
			std::cout << v[i] << std::endl;

		if (v.size() == 0)
			std::cout << "There are no tweets for this username" << std::endl;
	}

};

int main(int argc, char*argv[])
{
	std::ifstream input(argv[1]);
	StringMap table = StringMap();
	std::string line; //tweet
	std::string line2; //time
	std::string line3; //key

	while (getline(input,line)) //reads in file to hashtable
	{
		if (line == "{")
		{
			getline(input, line2);
			line2.erase(0,14);
			line2.erase(line2.end() - 2, line2.end() - 0);

			getline(input, line);
			line.erase(0, 8);
			line.erase(line.end() - 2, line.end() - 0);

			getline(input, line3);
			line3.erase(0, 15);
			line3.erase(line3.end() - 3, line3.end() - 0);

			for (size_t i = 0; i < line.size(); ++i)
				if (line[int(i)] < 0 || line[int(i)] > 126)
					line.erase(i);
			for (size_t i = 0; i < line2.size(); ++i)
				if (line2[int(i)] < 0 || line2[int(i)] > 126)
					line2.erase(i);
			for (size_t i = 0; i < line3.size(); ++i)
				if (line3[int(i)] < 0 || line3[int(i)] > 126)
					line3.erase(i);

			table.put(line3, line, line2);
		}
	}

	int user = 0; //users option
	std::string userString; //user's tweet, etc.
	std::string userKey;
	std::vector<std::string> tweets;

	/*These lines get the current time*/
	time_t rawtime;
	char buffer[80];
	struct tm *newtime;
	time (&rawtime);
	newtime = localtime (&rawtime);
	strftime(buffer, 80, "%FTO%TZ", newtime); //converts it to a readable string
	std::string str(buffer);
	/**/

	while (user != 4)
	{
		std::cout << "1. Enter new Tweet" << std::endl;
		std::cout << "2. Delete all tweets for a given screen_name" << std::endl;
		std::cout << "3. View Tweets" << std::endl;
		std::cout << "4. Quit" << std::endl;
		std::cin >> user;

		if (user == 1)
		{
			std::cout << "Enter a new tweet" << std::endl;
			std::cin.clear();
			if (std::cin.peek() == '\n')
				std::cin.ignore(INT_MAX, '\n');
			std::getline(std::cin, userString);
			std::cout << "Enter the username for that tweet (usernames contain no spaces)" << std::endl;
			std::cin >> userKey;
			table.put(userKey, userString, str);
		}

		if (user == 2)
		{
			std::cout << "Enter the screen name for the tweet you want to delete" << std::endl;
			std::cin >> userString;
			table.remove(userString);
		}

		if (user == 3)
		{
			std::cout << "Enter the screen name for the tweets you want to display" << std::endl;
			std::cin >> userString;
			tweets = table.get(userString);
			table.print(tweets);
		}

		if (user == 4)
			std::exit(0);
	}

}