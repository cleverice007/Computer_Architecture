#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>  // For log2 function
#include <string>

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

    CacheConfig(int addressBit=0, int blockSize=0, int cacheSets=0, int associativity=0) {
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




int main(int argc, char* argv[]){

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

    // print the cache configuration
    cout << "Address bits: " << cacheconfig.addressBits << endl;
    cout << "Block size: " << cacheconfig.blockSize << endl;
    cout << "Cache sets: " << cacheconfig.cacheSets << endl;
    cout << "Associativity: " << cacheconfig.associativity << endl;
    cout << "Tag bits: " << cacheconfig.tagBits << endl;
    cout << "Index bits: " << cacheconfig.indexBits << endl;
    cout << "Offset bits: " << cacheconfig.offsetBits << endl;
    

}