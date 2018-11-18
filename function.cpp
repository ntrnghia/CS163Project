#include"header.h"

inline char trie::changecase(char a)const {
	return (a > 64 && a < 91) ? a + 32 : a - 32;
}
inline bool trie::is_word(char a)const {
	return (a >= 65 && a <= 90) || (a >= 97 && a <= 122) ? true : false;
}
bool trie::is_same_hehe(const string &word_in_file, const string &word)const {
	if (word_in_file.size() < word.size()) return false;
	if (word_in_file.size() == word.size()) {
		for (int i = 0; i < word.size(); ++i)
			if (word_in_file[i] != word[i] && word_in_file[i] != changecase(word[i])) return false;
		return true;
	}
	for (int i = 0, k = 0; i < word_in_file.size(); ++i) {
		if (!is_word(word_in_file[i])) continue;
		if (word_in_file[i] != word[k] && word_in_file[i] != changecase(word[k])) return false;
		if (k == word.size() - 1 && (word_in_file.size() == i + 1 || !is_word(word_in_file[i + 1]))) return true;
		++k;
	}
	return false;
}
inline int count_cha(string a) {
	int count = 0;
	istringstream b(a);
	string word;
	while (b >> word) ++count;
	return count;
}
bool less_vectors(const vector<size_t>&a, const vector<size_t>&b) {
	return a.size() < b.size();
}

void trie::indexing(const string &folder) {
	WIN32_FIND_DATAA fd;
	HANDLE hFind = FindFirstFileA((FOLDER + "/*").c_str(), &fd);
	do {
		string filename(fd.cFileName);
		ifstream fin(FOLDER + "/" + filename, ifstream::binary);
		if (fin.is_open()) {
			data tmp;
			getline(fin, tmp.title);
			istringstream sin(tmp.title);
			string word;
			while (sin >> word) tmp.title_vector.push_back(word);
			while (fin >> word) tmp.content_vector.push_back(word);
			string content;
			int i = 0;
			for (; i < PREVIEW; ++i) content += tmp.content_vector[i] + ' ';
			for (string word : tmp.title_vector)
				insertWord(word, filedata.size(), tmp.title + '\n' + content + "...", true);
			for (i = 0; i < PREVIEW / 2; ++i)
				insertWord(tmp.content_vector[i], filedata.size(), tmp.title + '\n' + content + "...", false);
			for (; i < tmp.content_vector.size() - PREVIEW / 2; ++i) {
				content.erase(0, tmp.content_vector[i - PREVIEW / 2].size() + 1);
				content += tmp.content_vector[i + PREVIEW / 2] + ' ';
				insertWord(tmp.content_vector[i], filedata.size(), tmp.title + '\n' + "..." + content + "...", false);
			}
			for (; i < tmp.content_vector.size(); ++i)
				insertWord(tmp.content_vector[i], filedata.size(), tmp.title + '\n' + "..." + content, false);
			filedata.push_back(tmp);
		}
		fin.close();
	} while (FindNextFileA(hFind, &fd));
	FindClose(hFind);
}
void trie::search_intersection_whole(const vector<string>&query_initial, vector<string>&cache) {
	vector<string>query;
	vector<size_t>fileid;
	vector<vector<size_t>>collected;
	for (int i = 0; i < query_initial.size();) {
		vector<string>get_query, tmp_query;
		for (int j = i; j < query_initial.size(); ++j) {
			get_query.push_back(query_initial[j]);
			if (get_query.size() == 1 || is_cache(get_query)) tmp_query = get_query, i = j + 1;
		}
		vector<size_t>tmp_fileid;
		if (query.size() == 0 && i == query_initial.size()) {
			search(tmp_query, cache);
			save_search_history(query_initial);
			return;
		}
		else search(tmp_query, tmp_fileid);
		query.insert(query.end(), tmp_query.begin(), tmp_query.end());
		collected.push_back(tmp_fileid);
		get_query.clear();
	}
	sort(collected.begin(), collected.end(), [&](const vector<size_t>&a, const vector<size_t>&b) {return a.size() < b.size();});
	if (collected.size() > 0) fileid = collected.front();
	for (int i = 1; i < collected.size(); ++i) {
		vector<size_t>intersection_fileid;
		set_intersection(fileid.begin(), fileid.end(), collected[i].begin(), collected[i].end(), back_inserter(intersection_fileid));
		fileid = intersection_fileid;
	}
	vector<int>max_length;
	for (int i = 0, tmp_max; i < fileid.size(); ++i) {
		string tmp = get_content(query, fileid[i], tmp_max);
		int index = 0;
		for (; index < cache.size(); ++index) if (tmp_max > max_length[index]) break;
		cache.insert(cache.begin() + index, tmp);
		max_length.insert(max_length.begin() + index, tmp_max);
	}
	if (!is_cache(query_initial)) make_cache(query, fileid, cache);
	save_search_history(query_initial);
}

trie::trie() {
	make_node(root);
}
trie::~trie() {
	remove(root);
}
void trie::make_node(node*&tmp) {
	tmp = new node;
	for (node*&i : tmp->child) i = NULL;
}
void trie::remove(node*&head) {
	if (head == NULL) return;
	for (node*tmp : head->child) remove(tmp);
	delete head;
}

void trie::insertWord(const string &word, const size_t &fileid, const string &cache, const bool &is_title) {
	node*tmp = root;
	for (char i : word) {
		if (!is_word(i)) continue;
		int index = CHAR_TO_INDEX(i);
		if (!tmp->child[index]) make_node(tmp->child[index]);
		tmp = tmp->child[index];
	}
	if (tmp->fileid.empty() || fileid != tmp->fileid.back()) {
		tmp->fileid.push_back(fileid);
		tmp->cache.push_back(cache);
		if (is_title) tmp->is_title.push_back(true);
	}
}

bool trie::is_cache(const vector<string>&query)const {
	node*tmp = root;
	for (int i = 0; i < query.size(); ++i) {
		if (i != 0) {
			if (!tmp->child[ALPHABET - 1]) return false;
			tmp = tmp->child[ALPHABET - 1];
		}
		for (char j : query[i]) {
			if (!is_word(j)) continue;
			int index = CHAR_TO_INDEX(j);
			if (!tmp->child[index]) return false;
			tmp = tmp->child[index];
		}
	}
	if (tmp->cache.size() == 0) return false;
	return true;
}
void trie::search(const vector<string>&query, vector<size_t>&fileid)const {
	node*tmp = root;
	for (int i = 0; i < query.size(); ++i) {
		if (i != 0) {
			if (!tmp->child[ALPHABET - 1]) return;
			tmp = tmp->child[ALPHABET - 1];
		}
		for (char j : query[i]) {
			if (!is_word(j)) continue;
			int index = CHAR_TO_INDEX(j);
			if (!tmp->child[index]) return;
			tmp = tmp->child[index];
		}
	}
	fileid = tmp->fileid;
}
void trie::search(const vector<string>&query, vector<string>&cache)const {
	node*tmp = root;
	for (int i = 0; i < query.size(); ++i) {
		if (i != 0) {
			if (!tmp->child[ALPHABET - 1]) return;
			tmp = tmp->child[ALPHABET - 1];
		}
		for (char j : query[i]) {
			if (!is_word(j)) continue;
			int index = CHAR_TO_INDEX(j);
			if (!tmp->child[index]) return;
			tmp = tmp->child[index];
		}
	}
	cache = tmp->cache;
}
string trie::get_content(const vector<string>&query, const size_t &fileid, int &max)const {
	string inra;
	vector<string>all;
	all = filedata[fileid].title_vector;
	all.insert(all.end(), filedata[fileid].content_vector.begin(), filedata[fileid].content_vector.end());
	string content;
	for (int i = 0; i < PREVIEW; ++i) content += filedata[fileid].content_vector[i] + ' ';
	max = 0;
	for (int i = 0, pos_cur = 0, max_tmp = 0; pos_cur < all.size() && max < query.size(); ++pos_cur) {
		if (pos_cur >= filedata[fileid].title_vector.size() + PREVIEW / 2 && pos_cur < all.size() - PREVIEW / 2) {
			content.erase(0, filedata[fileid].content_vector[pos_cur - filedata[fileid].title_vector.size() - PREVIEW / 2].size() + 1);
			content += filedata[fileid].content_vector[pos_cur - filedata[fileid].title_vector.size() + PREVIEW / 2] + ' ';
		}
		if (i == query.size()) i = 0, max_tmp = 0;
		for (; i < query.size(); ++i)
			if (is_same_hehe(all[pos_cur], query[i])) {
				++i;
				++max_tmp;
				if (max_tmp > max) {
					max = max_tmp;
					if (pos_cur < filedata[fileid].title_vector.size() + PREVIEW / 2)
						inra = filedata[fileid].title + '\n' + content + "...";
					else if (pos_cur < all.size() - PREVIEW / 2)
						inra = filedata[fileid].title + '\n' + "..." + content + "...";
					else inra = filedata[fileid].title + '\n' + "..." + content;
				}
				break;
			}
			else if (max_tmp > 0) i = 0, max_tmp = 0;
	}
	return inra;
}
void trie::make_cache(const vector<string>&query, const vector<size_t>&fileid, const vector<string>&cache) {
	node*tmp = root;
	for (int i = 0; i < query.size(); ++i) {
		if (i != 0) {
			if (!tmp->child[ALPHABET - 1]) make_node(tmp->child[ALPHABET - 1]);
			tmp = tmp->child[ALPHABET - 1];
		}
		for (char j : query[i]) {
			if (!is_word(j)) continue;
			int index = CHAR_TO_INDEX(j);
			if (!tmp->child[index]) make_node(tmp->child[index]);
			tmp = tmp->child[index];
		}
	}
	tmp->fileid = fileid;
	tmp->cache = cache;
}
void trie::save_search_history(const vector<string>&query) {
	if (query.size() == 0) return;
	ostringstream sout;
	for (string word : query) sout << word << ' ';
	for (string word : search_history) if (word == sout.str()) return;
	search_history.push_back(sout.str());
}