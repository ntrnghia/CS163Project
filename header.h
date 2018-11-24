#pragma once
#include<iostream>
#include<fstream>
#include<vector>
#include<Windows.h>				//get file in folder
#include<chrono>				//high_resolution_clock
#include<sstream>
#include<algorithm>				//sort
#define ALPHABET 27				//include space
#define CHAR_TO_INDEX(a) a>='a'?int(a)-'a':a-'A'
#define FOLDER string("Data")
#define PREVIEW 30				//preview text
using namespace std;
using namespace std::chrono;	//high_resolution_clock
class Trie {
public:
	Trie();
	~Trie();
	vector<string>search_history;
	void indexing(const string &folder);
	void search_intersection_whole(const vector<string>&, vector<string>&);
private:
	struct Data {
		int title_size;
		vector<string>content_vector;
	};
	vector<Data>data;
	struct Cache {
		int data_pos;
		vector<int> pos;
		int priority;
		Cache(int x):data_pos(x){};
		Cache(int x, int y):data_pos(x), pos({y}), priority(1){};
	};
	struct Node {
		vector<Cache>cache;
		Node*child[ALPHABET];
		Node(){for (auto&a:child) a=NULL;};
	}*root;
	void remove(Node*);
	//tool
	inline bool isalpha(char a)const;
	vector<Cache>IntersectVectorCache(const vector<Cache>&left, const vector<Cache>&right);
	void smart_cache(vector<Cache>&cache, const int &size);
	//indexing
	void insertWord(const string &word, int d, const int&pos);
	//querying
	bool is_cache(const vector<string>&query)const;
	vector<Cache>search(const vector<string>&)const;
	vector<string>get_content(const vector<Cache>&);
	void make_cache(const vector<string>&query, const vector<Cache>&cache);
	void save_search_history(const vector<string>&query);
};