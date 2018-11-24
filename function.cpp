#include"header.h"

inline bool Trie::isalpha(char a)const {
	return (a >= 'A' && a <= 'Z') || (a >= 'a' && a <= 'z') ? true : false;
}

void Trie::indexing(const string &folder) {
	WIN32_FIND_DATAA fd;
	HANDLE hFind = FindFirstFileA((FOLDER + "/*").c_str(), &fd);
	do {
		string filename(fd.cFileName);
		ifstream fin(FOLDER + "/" + filename, ifstream::binary);
		if (fin.is_open()) {
			Data tmp;
			string title;
			int title_size=0;
			getline(fin, title);
			istringstream sin(title);
			string word;
			while (sin >> word) tmp.content.push_back(word), ++title_size;
			tmp.title_size=title_size;
			while (fin >> word) tmp.content.push_back(word);
			data.push_back(tmp);
			for (int i=0; i<tmp.content.size(); ++i) insertWord(tmp.content[i], data.size()-1, i);
		}
		fin.close();
	} while (FindNextFileA(hFind, &fd));
	FindClose(hFind);
}

vector<Trie::Cache>Trie::IntersectVectorCache(const vector<Cache>&left, const vector<Cache>&right){
    vector<Cache>result;
    auto il = left.begin(), ir = right.begin();
    while (il != left.end() && ir != right.end()){
        if (il->data_pos < ir->data_pos) ++il;
        else if (ir->data_pos < il->data_pos) ++ir;
        else{
			int l=0, r=0;
			Cache tmp(il->data_pos);
			while (l<il->pos.size() && r<ir->pos.size()){
				if (il->pos[l]<ir->pos[r]) tmp.pos.push_back(il->pos[l++]);
				else tmp.pos.push_back(ir->pos[r++]);
			}
			while (l<il->pos.size()) tmp.pos.push_back(il->pos[l++]);
			while (r<ir->pos.size()) tmp.pos.push_back(ir->pos[r++]);
			tmp.priority=max(il->priority, ir->priority);
            result.push_back(tmp);
            ++il;
            ++ir;
        }
    }
    return result;
}

void Trie::search_intersection_whole(const vector<string>&query_initial, vector<string>&output) {
	vector<string>query;
	vector<vector<Cache>>collected;
	for (int i = 0; i < query_initial.size();) {
		vector<string>get_query, tmp_query;
		for (int j = i; j < query_initial.size(); ++j) {
			get_query.push_back(query_initial[j]);
			if (get_query.size() == 1 || is_cache(get_query)) tmp_query = get_query, i = j + 1;
		}
		if (query.size() == 0 && i == query_initial.size()) {
			if (i==1) output=get_content(search(tmp_query));
			else output=get_content(search_smart_cache(tmp_query));
			save_search_history(query_initial);
			return;
		}
		else{
			vector<Cache>tmp_cache=search(tmp_query);
			query.insert(query.end(), tmp_query.begin(), tmp_query.end());
			collected.push_back(tmp_cache);
			get_query.clear();
		}
	}
	sort(collected.begin(), collected.end(), [&](const vector<Cache>&a, const vector<Cache>&b) {return a.size() < b.size();});
	vector<Cache>cache;
	if (collected.size() > 0) cache = collected.front();
	for (int i = 1; i < collected.size(); ++i) cache = IntersectVectorCache(cache, collected[i]);
	make_cache(query, cache);
	smart_cache(cache, query.size());
	make_smart_cache(query, cache);
	output=get_content(cache);
	save_search_history(query_initial);
}

void Trie::smart_cache(vector<Cache>&cache, const int &size){
	//size always > 1
	for (auto &a:cache){
		vector<vector<int>>tmp(size-1); //[0] - 2 characters; [size-2] - size characters
		for (int i=1, dis_cur=1; i<a.pos.size(); ++i){
			if (a.pos[i]-a.pos[i-1]==1){
				if (dis_cur<size) ++dis_cur;
				tmp[dis_cur-2].push_back(a.pos[i]);
			}
			else dis_cur=1;
		}
		for (int i=size-2; i>=0; --i){
			if (tmp[i].size()>0){
				a.pos=tmp[i];
				a.priority=i+2;
				break;
			}
		}
	}
	sort(cache.begin(), cache.end(), [&](const Cache &a, const Cache &b) {return a.priority > b.priority;});
}

Trie::Trie() {
	root=new Node();
}

Trie::~Trie() {
	remove(root);
}

void Trie::remove(Node*root){
	if (!root) return;
	for (auto a:root->child) remove(a);
	delete root;
}

void Trie::insertWord(const string &word, int d, const int&pos) {
	Node*cur = root;
	for (const auto &i : word) {
		if (!isalpha(i)) continue;
		int index = CHAR_TO_INDEX(i);
		if (!cur->child[index]) cur->child[index]=new Node();
		cur = cur->child[index];
	}
	if (cur->cache.size()==0 || cur->cache.back().data_pos!=d) cur->cache.push_back(Cache(d, pos));
	else cur->cache.back().pos.push_back(pos);
}

bool Trie::is_cache(const vector<string>&query)const {
	Node*tmp = root;
	for (int i = 0; i < query.size(); ++i) {
		if (i != 0) {
			if (!tmp->child[ALPHABET - 1]) return false;
			tmp = tmp->child[ALPHABET - 1];
		}
		for (char j : query[i]) {
			if (!isalpha(j)) continue;
			int index = CHAR_TO_INDEX(j);
			if (!tmp->child[index]) return false;
			tmp = tmp->child[index];
		}
	}
	if (tmp->cache.size() == 0) return false;
	return true;
}

vector<Trie::Cache>Trie::search(const vector<string>&query)const {
	Node*tmp = root;
	for (int i = 0; i < query.size(); ++i) {
		if (i != 0) {
			if (!tmp->child[ALPHABET - 1]) return {};
			tmp = tmp->child[ALPHABET - 1];
		}
		for (char j : query[i]) {
			if (!isalpha(j)) continue;
			int index = CHAR_TO_INDEX(j);
			if (!tmp->child[index]) return {};
			tmp = tmp->child[index];
		}
	}
	return tmp->cache;
}

vector<Trie::Cache>Trie::search_smart_cache(const vector<string>&query)const {
	Node*tmp = root;
	for (int i = 0; i < query.size(); ++i) {
		if (i != 0) {
			if (!tmp->child[ALPHABET - 1]) return {};
			tmp = tmp->child[ALPHABET - 1];
		}
		for (char j : query[i]) {
			if (!isalpha(j)) continue;
			int index = CHAR_TO_INDEX(j);
			if (!tmp->child[index]) return {};
			tmp = tmp->child[index];
		}
	}
	return tmp->smart_cache;
}

vector<string>Trie::get_content(const vector<Cache>&cache){
	vector<string>res;
	for (const auto &a:cache){
		int cs=data[a.data_pos].content.size(), ts=data[a.data_pos].title_size;
		string content;
		for (int i=0; i<ts; ++i) content+=data[a.data_pos].content[i]+' ';
		content+='\n';
		if (a.pos[0]<ts+PREVIEW/2){
			for (int i=ts; i < ts+PREVIEW; ++i) content += data[a.data_pos].content[i] + ' ';
			content+="...";
		}
		else if (a.pos[0]>cs-PREVIEW/2){
			content+="...";
			for (int i=cs-PREVIEW; i < cs; ++i) content += data[a.data_pos].content[i] + ' ';
		}
		else{
			content+="...";
			for (int i=a.pos[0]-PREVIEW/2; i < a.pos[0]+PREVIEW/2; ++i) content += data[a.data_pos].content[i] + ' ';
			content+="...";
		}
		res.push_back(content);
	}
	return res;
}

void Trie::make_cache(const vector<string>&query, const vector<Cache>&cache) {
	Node*tmp = root;
	for (int i = 0; i < query.size(); ++i) {
		if (i != 0) {
			if (!tmp->child[ALPHABET - 1]) tmp->child[ALPHABET - 1]=new Node();
			tmp = tmp->child[ALPHABET - 1];
		}
		for (char j : query[i]) {
			if (!isalpha(j)) continue;
			int index = CHAR_TO_INDEX(j);
			if (!tmp->child[index]) tmp->child[index]=new Node();
			tmp = tmp->child[index];
		}
	}
	tmp->cache = cache;
}

void Trie::make_smart_cache(const vector<string>&query, const vector<Cache>&cache) {
	Node*tmp = root;
	for (int i = 0; i < query.size(); ++i) {
		if (i != 0) {
			if (!tmp->child[ALPHABET - 1]) tmp->child[ALPHABET - 1]=new Node();
			tmp = tmp->child[ALPHABET - 1];
		}
		for (char j : query[i]) {
			if (!isalpha(j)) continue;
			int index = CHAR_TO_INDEX(j);
			if (!tmp->child[index]) tmp->child[index]=new Node();
			tmp = tmp->child[index];
		}
	}
	tmp->smart_cache = cache;
}

void Trie::save_search_history(const vector<string>&query) {
	if (query.size() == 0) return;
	ostringstream sout;
	for (string word : query) sout << word << ' ';
	for (string word : search_history) if (word == sout.str()) return;
	search_history.push_back(sout.str());
}