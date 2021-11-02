#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>

using std::cout; using std::endl;

// Helper functions
void input(std::map<int, std::vector<int> >&, 
			std::ifstream&, 
			std::vector<int>&,
			int&);
void combos(int, int, std::multimap<int, std::vector<int> >&);
double support(int, std::vector<int>, std::map<int, std::vector<int> >);

int main (int argc, char* argv[]) {

	// Usage is ./a.out [text input file] [integer support percentage]

	std::ifstream in(argv[1]);
	if (!in.is_open()) {
		std::cout << "Unable to open input file" << std::endl;
		return 1;
	}

	// Confidence percent stored as double
	std::string conf(argv[2]);
	double confPercent = stod(conf) / 100;
	
	// Declaring storage containers
	std::map<int, std::vector<int> > transactions;
	std::vector<int> uniques;
	int trans_id = 0;

	// Taking in all transactions 
	input(transactions, in, uniques, trans_id);

	// Storing all possible combinations in a multimap
	std::multimap<int, std::vector<int> > allCombos;
	for (int i=uniques.size(); i>0; --i) {
		combos(uniques.size(), i, allCombos);
	}

	// Creating the support counts
	std::map<std::vector<int>, double> suppCounts;
	for (int i=0; i<uniques.size(); ++i) {
		auto itLow = allCombos.lower_bound(i+1);
		auto itHigh = allCombos.upper_bound(i+1);
		for (auto it=itLow; it!=itHigh; ++it) {
			suppCounts[(*it).second] = support(trans_id, (*it).second, transactions);
		}
	}	

	// Outputting all itemsets, the itemset sizes
	// and the support count if they satisfy the given
	// constraint
	for (int i=1; i<=uniques.size(); ++i) {
		cout << "Itemsets of size " << i << endl;
		bool contains = false;
		for (const auto& e : suppCounts) {
			if (e.first.size() == i && e.second >= confPercent) {
				contains = true;
				cout << "[";
				for (int i=0; i < e.first.size(); ++i) {
					cout << e.first[i];
					if (i != e.first.size()-1) cout << " ";
				}
				cout << "] -> Conf Percent: " << e.second << endl;
			}
		}
		if (!contains) cout << "N/A" << endl;
	}
	
	return 0;
}

double support(int totalTrans, std::vector<int> combo, 
		std::map<int, std::vector<int> > transactions) {
	double support = 0;
	for (const auto& e : transactions) {
		bool supported = true;
		auto it = combo.begin();
		do {
			auto finder = std::find(e.second.begin(), e.second.end(), *it);
			if (finder == e.second.end()) supported = false;
			++it;
		} while (supported && it != combo.end());

		if (supported) support += 1;
	}
	return (support / double(totalTrans));
}

void combos(int n, int r, 
		std::multimap<int, std::vector<int> >& allCombos) {
	// Adapted from 
	// https://stackoverflow.com/questions/9430568/generating-combinations-in-c
	
	std::vector<bool> v(n);
    std::fill(v.end() - r, v.end(), true);

    do {
		std::vector<int> tmp;
        for (int i = 0; i < n; ++i) {
            if (v[i]) {
                tmp.push_back(i + 1);
            }
        }
		allCombos.insert(std::pair<int, std::vector<int>>
			(tmp.size(), tmp));
    } while (std::next_permutation(v.begin(), v.end()));
}

void input(std::map<int, std::vector<int> >& transactions,
			std::ifstream& in,
			std::vector<int>& uniques,
			int& trans_id) {

	std::string line;
	
	while(getline(in, line)) {
		++trans_id;
		std::vector<int> tmp;
		for (const auto& s : line) {
			if (s != ' ') {
				const char* str0 = &s;
				int num = atoi(str0);
				tmp.push_back(num);

				auto found = std::find(uniques.begin(), uniques.end(), num);
				if (found == uniques.end()) uniques.push_back(num);
			}
		}
		transactions[trans_id] = tmp;
	}
}