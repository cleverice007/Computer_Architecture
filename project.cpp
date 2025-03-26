#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>  // For log2 function
#include <string>
#include <bitset>

using namespace std;

// Define a struct to store the cache configuration
// store the address bit, block size, cache set, and associativity
// constructor : calculate the number of bits for the tag, index, and offset
struct CacheConfig {
    int addressBits;
    int blockSize;
    int cacheSets;
    int associativity;
    int tagBits;
    int indexBits;
    int offsetBits;

    CacheConfig(int addressBits=0, int blockSize=0, int cacheSets=0, int associativity=0) {
        this->addressBits = addressBits;
        this->blockSize = blockSize;
        this->cacheSets = cacheSets;
        this->associativity = associativity;

        // Calculate the number of bits for the tag, index, and offset
        this->offsetBits = log2(blockSize);
        this->indexBits = log2(cacheSets);
        this->tagBits = addressBits - indexBits - offsetBits;
    }
};

// parse the cache configuration from the file
bool parseCacheConfig(string filename, CacheConfig &cacheConfig) {
    // check if the file can be opened
    ifstream file(filename);
    if(!file.is_open()) {
        cout << "Error: cannot open file " << filename << endl;
        return false;
    }

    string header;
    int value;
    vector<int> cacheConfigValues;
    // read the file and store the cache configuration to the vector
    while(file >> header >> value) {
        cacheConfigValues.push_back(value);
    }
    file.close();

    // check if the number of values is correct
    if(cacheConfigValues.size() != 4) {
        cout << "Error: invalid cache configuration file" << endl;
        return false;
    }

    cacheConfig = CacheConfig(cacheConfigValues[0], cacheConfigValues[1], cacheConfigValues[2], cacheConfigValues[3]);
    return true;
}

// read the reference file and store the address to the vector
// type of variable is int to store the 8-bit address
bool parseReferenceFile(string filename, vector<int> &addresses) {
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "Error: cannot open file " << filename << endl;
        return false;
    }

    string line;
    while (file >> line) {
        // ignore the benchmark and end
        if (line == ".benchmark" || line == ".end" || line == "testcase1") {
            continue;
        }

        // convert the binary string to unsigned int
        int address = stoi(line, nullptr, 2);  // stoi: string to integer
        addresses.push_back(address);
    }

    file.close();
    return true;
}

// find the tag, index by using mask and shift
// get the number of index, tag bit by cache configuration
vector<int> getTagIndexOffset( int address, const CacheConfig &cacheConfig) {
    vector<int> result;

    int offsetMask = (1 << cacheConfig.offsetBits) - 1;
    int offset = address & offsetMask;

    int indexMask = (1 << cacheConfig.indexBits) - 1;
    int index = (address >> cacheConfig.offsetBits) & indexMask;

    int tag = address >> (cacheConfig.offsetBits + cacheConfig.indexBits);

    result.push_back(tag);
    result.push_back(index);
    result.push_back(offset);

    return result;
}

int main(int argc, char* argv[]) {

    // check the number of arguments
    if (argc != 4) {
        cout << "Usage: " << argv[0] << " <cache1.org> <reference1.lst> <index.rpt>" << endl;
        return 1;
    }

    // load the file and store the cache configuration to the struct
    CacheConfig cacheconfig;
    if(!parseCacheConfig(argv[1], cacheconfig)) {
        return 1;
    }

    // initialize the cache and NRU bits
    vector<vector<int>> cache(cacheconfig.cacheSets, vector<int>(cacheconfig.associativity, -1));
    vector<vector<bool>> nru(cacheconfig.cacheSets, vector<bool>(cacheconfig.associativity, true));

    // read the reference file and store the address to the vector
    vector<int> addresses;
    bool readreference = parseReferenceFile(argv[2], addresses);

    vector<bool> hitOrMissList;
    int missCount = 0;

    // iterate the address vector, to find the tag, index, and offset
    for (int address : addresses) {
        vector<int> tagIndexOffset = getTagIndexOffset(address, cacheconfig);
        int tag = tagIndexOffset[0];
        int index = tagIndexOffset[1];
        int offset = tagIndexOffset[2];

        bool hit = false;
        for (int i = 0; i < cacheconfig.associativity; i++) {
            if (cache[index][i] == tag) {
                hit = true;
                nru[index][i] = false;
                break;
            }
        }

        if (hit) {
            hitOrMissList.push_back(true);
        } else {
            hitOrMissList.push_back(false);
            missCount++;
            bool emptyBlock = false;
            for (int i = 0; i < cacheconfig.associativity; i++) {
                if (cache[index][i] == -1) {
                    cache[index][i] = tag;
                    nru[index][i] = false;
                    emptyBlock = true;
                    break;
                }
            }

            if (!emptyBlock) {
                bool allZero = true;
                for (int i = 0; i < cacheconfig.associativity; i++) {
                    if (nru[index][i]) {
                        allZero = false;
                        cache[index][i] = tag;
                        nru[index][i] = false;
                        break;
                    }
                }

                if (allZero) {
                    for (int i = 0; i < cacheconfig.associativity; i++) {
                        nru[index][i] = true;
                    }
                    cache[index][0] = tag;
                    nru[index][0] = false;
                }
            }
        }
    }

    // write output report to index.rpt
    ofstream file(argv[3]);
    if (!file.is_open()) {
        cout << "Error: cannot open output file." << endl;
        return 1;
    }

    file << "Address bits: " << cacheconfig.addressBits << endl;
    file << "Block size: " << cacheconfig.blockSize << endl;
    file << "Cache sets: " << cacheconfig.cacheSets << endl;
    file << "Associativity: " << cacheconfig.associativity << endl;
    file << endl;
    file << "Offset bit count: " << cacheconfig.offsetBits << endl;
    file << "Indexing bit count: " << cacheconfig.indexBits << endl;
    file << "Indexing bits:";
   for (int i = 0; i < cacheconfig.indexBits; ++i) {
    int fromLeft = cacheconfig.addressBits - 1 - (cacheconfig.offsetBits + i);
    file << " " << fromLeft;
    }
    file << endl << endl;

    file << ".benchmark testcase1" << endl;
    for (size_t i = 0; i < addresses.size(); i++) {
        file << bitset<8>(addresses[i]).to_string() << " ";
        file << (hitOrMissList[i] ? "hit" : "miss") << endl;
    }
    file << ".end" << endl << endl;

    file << "Total cache miss count: " << missCount << endl;
    file.close();

    return 0;
}
