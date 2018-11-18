#include"header.h"
int main() {
	//indexing
	cout << "Initalizing, this may take several seconds...";

	trie Tree;
	high_resolution_clock::time_point t1 = high_resolution_clock::now();
	Tree.indexing(FOLDER);
	high_resolution_clock::time_point t2 = high_resolution_clock::now();

	//query
	string word;
	while (true) {

		system("CLS");
		if (Tree.search_history.size() != 0) cout << "Search history:\n";
		for (string word : Tree.search_history) cout << word << endl;
		cout << "--------------------------------------------------------" << endl;

		cout << "Insert words which want to search: ";
		string input;
		getline(cin, input);
		if (input == "exit") break;

		vector<string>query_initial;
		istringstream sin(input);
		while (sin >> word) query_initial.push_back(word);

		high_resolution_clock::time_point t3 = high_resolution_clock::now();
		vector<string>cache;
		Tree.search_intersection_whole(query_initial, cache);
		high_resolution_clock::time_point t4 = high_resolution_clock::now();

		high_resolution_clock::time_point t5 = high_resolution_clock::now();
		for (string print : cache) cout << print << endl << endl;
		high_resolution_clock::time_point t6 = high_resolution_clock::now();

		cout << "Indexing time: " << duration<double>(t2 - t1).count() << " seconds" << endl;
		cout << cache.size() << " results (" << duration<double>(t4 - t3).count() << " seconds)" << endl;
		cout << "Printing time: " << duration<double>(t6 - t5).count() << " seconds" << endl;

		cout << "Press Enter to search the next keyword!";
		cin.ignore(__INT_MAX__, '\n');
	};
	system("CLS");
	cout << "Shuting down...\n";
}