#include <iostream>
#include <vector>
#include <cstdlib>
#include <string>
#include <fstream>
using namespace std;

// num_entries total number of entries
// associativity is the number of ways
// num of entries per way is num_entries / associativity
// 

class Cache {
    // a cache is really just a list of entries
    // or rather, multiple lists of entries
    private:
        class Entry {
            // index, tag, address
        private:
            int index;
            int tag;
            int address;
            int age; // incremented each time an action is taken
            bool initialized;
        public:
            Entry() {
                address = 0;
                age = 0;
                initialized = false;
            }
            Entry(int index, int tag, int address) : index(index), tag(tag), address(address) {
                age = 0;
                initialized = true;
            }
            int getAddress() {
                return address;
            }
            int getAge() {
                return age;
            }
            void updateAge() {
                age++;
            }
            bool getInitialized() {
                return initialized;
            }
        };

        bool cacheEmpty() {
            for(int i = 0; i < ways.size(); i++) {
                for(int j = 0; j < ways.at(i).size(); j++) {
                    if(ways.at(i).at(j).getInitialized()) {
                        // if initialized, then the entry was placed there
                        return false;
                    }
                }
            }
            return true;
        }

        vector<vector<Entry>> ways; // contains number of ways
        int entryPerWay; // entryPerWay, mod number
        int assoc;
        int num_entries;
    public:
        Cache(int num_entries, int assoc) : num_entries(num_entries), assoc(assoc) {
            entryPerWay = num_entries / assoc;
            //cout << entryPerWay << " -- mod num\n";
            for(int i = 0; i < assoc; i++) {
                vector<Entry> way(entryPerWay);
                ways.push_back(way);
            }
        }

        // have the class determine hits and misses
        string entryLookup(int target) {
            //cout << "ENTRY LOOKUP!\n";
            // returns a string with a hit/miss
            string miss;
            if(cacheEmpty()) { // cache should only be empty on first iteration
                miss = addEntry(target);
                return "MISS - " + miss + "\n";
            }

            int index = target % entryPerWay;
            updateEntryAges();

            for(int i = 0; i < ways.size(); i++) {
                // for each way
                //cout << "BREAK HERE 1?\n";
                if(ways.at(i).at(index).getAddress() == target) {
                    return "HIT\n";
                }
            }
            miss = addEntry(target); // add it, then report a miss
            //printCache();
            return "MISS - " + miss + "\n";

        }
        // string miss reason
        string addEntry(int addr) {
            string missReason;
            // only add entry if it is not found in the table
            // find the index, tag, address
            //cout << "ADD ENTRY: " << addr << endl;
            updateEntryAges();
            int index = addr % entryPerWay;
            int tag = addr << 2;

            Entry entry(index, tag, addr);
            vector<Entry> entryList;
            // check for an empty space to add entry
            for(int i = 0; i < ways.size(); i++) {
                if(!ways.at(i).at(index).getInitialized()) {
                    //ways.at(i).emplace(ways.at(i).begin() + index, entry);
                    ways.at(i).at(index) = entry;
                    return "COMPULSORY"; // no need to continue
                }
                else {
                    entryList.push_back(ways.at(i).at(index));
                }
            }
            // entry was not added. No empty space was found.
            // determine the oldest entry in the entryList, then replace that entry
            int oldestIndex = 0;
            int oldest = 0;
            //cout << "\nENTRY LIST SIZE: " << entryList.size() << endl;
            for(int i = 0; i < entryList.size(); i++) {
                // entryList will only be as long as there are ways, so the index in the entryList corresponds to the correct way
                //oldestIndex = entryList.at(i).getAge() > oldestIndex ? i : oldestIndex;
                if(entryList.at(i).getAge() > oldest) {
                    oldest = entryList.at(i).getAge();
                    oldestIndex = i;
                }
            }
            //cout << "\nOLDEST INDEX: " << oldestIndex << endl;
            //ways.at(oldestIndex).emplace(ways.at(oldestIndex).begin() + index, entry);
            ways.at(oldestIndex).at(index) = entry;
            return "NOT SURE";
        }

        void updateEntryAges() {
            for(int i = 0; i < ways.size(); i++) {
                for(int j = 0; j < ways.at(i).size(); j++) {
                    if(ways.at(i).at(j).getInitialized()) {
                        ways.at(i).at(j).updateAge();
                    }
                }
            }
        }

        void printCache() {
            for(int i = 0; i < ways.size(); i++) {
                cout << "\nWAYS " << i << endl;
                for(int j = 0; j < ways.at(i).size(); j++) {
                    cout << "index: " <<j << " addr:" << ways.at(i).at(j).getAddress() << " age:" << ways.at(i).at(j).getAge();
                    cout << endl;
                }
            }
        }
    // for each integer in file
    //  - check if it's in the cache
    //  - if it's not in the cache
    //      - MISS (identify the reason, compulsory, capacity, conflict
    //      - load the integer into the cache in the correct place
    //  - if it's in the cache
    //      - HIT


};
/////////////////////////////////////////////////////
void parseFile(string filename, vector<int> &list) {
    ifstream input(filename);

    int num;
    while(input >> num) {
        list.push_back(num);
    }
}

void printList(vector<int> &list) {
    for(int i = 0; i < list.size(); i++) {
        cout << list.at(i) << " ";
    }
    cout << "\n";
}

int main(int argc, char* argv[]) {
    if(argc < 4) {
        cout << "Usage: " << endl;
        cout << "./cache_sim num_entries associativity filename" << endl;
        return 0; // terminate the program
    }

    unsigned entries = atoi(argv[1]);
    unsigned assoc = atoi(argv[2]);
    string filename = argv[3];

    vector<int> addressList;


    //cout << entries << " " << assoc << " " << filename << "\n";
    parseFile(filename, addressList);
    //printList(addressList);


    Cache L1(entries, assoc);

    for(int i = 0; i < addressList.size(); i++) {
        // need to create a new file, but that's ok for now
        cout << addressList.at(i) << ": " << L1.entryLookup(addressList.at(i));   
    }
}

