#pragma once
#include<iostream>
#include<fstream>
#include<string>
#include<vector>
#include<Windows.h>				//get file in folder
#include<chrono>				//high_resolution_clock
#include<sstream>
#include<algorithm>				//set_intersection
#define ALPHABET 27
#define CHAR_TO_INDEX(a) a>=97?int(a)-'a':a+32-'a'
#define FOLDER string("Data")
#define PREVIEW 30				//preview text
using namespace std;
using namespace std::chrono;	//high_resolution_clock
class trie {
public:
	trie();
	~trie();
	vector<string>search_history;
	void indexing(const string &folder);
	void search_intersection_whole(const vector<string>&query_initial, vector<string>&cache);
private:
	struct data {
		string title;
		vector<string>title_vector, content_vector;
	};
	vector<data>filedata;
	struct node {
		vector<size_t>fileid;
		vector<string>cache;
		vector<bool>is_title;
		node*child[ALPHABET];
	}*root;
	void make_node(node*&tmp);
	void remove(node*&head);

	//tool
	inline char changecase(char a)const;
	inline bool is_word(char a)const;
	bool is_same_hehe(const string &word_in_file, const string &word)const;

	//indexing
	void insertWord(const string &word, const size_t &fileid, const string &cache, const bool &is_title);

	//querying
	bool is_cache(const vector<string>&query)const;
	void search(const vector<string>&query, vector<size_t>&fileid)const;
	void search(const vector<string>&query, vector<string>&cache)const;
	string get_content(const vector<string>&query, const size_t &fileid, int &max)const;
	void make_cache(const vector<string>&query, const vector<size_t>&fileid, const vector<string>&cache);
	void save_search_history(const vector<string>&query);
};