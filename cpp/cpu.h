#include "address.h"
#include "ram.h"
#include "cache.h"

class CPU {
    long long block_sz;
    long long assoc;
    long long cache_sz;
    long long ram_sz;
    std::string policy;
    RAM ram;
    long long num_blocks;
    long long num_sets;
    Cache cache;
    long long instruction_count;
    

public:
    CPU(long long block_sz, long long assoc, long long cache_sz, long long ram_sz, std::string policy) 
        : block_sz(block_sz), assoc(assoc), cache_sz(cache_sz), ram_sz(ram_sz), policy(policy),
          ram(ram_sz, block_sz), num_blocks(cache_sz / block_sz), num_sets(cache_sz / (assoc * block_sz)), 
          cache(num_sets, num_blocks, block_sz, policy),instruction_count(0) 
    {}

    double loadDouble(long long address) {
        this->instruction_count += 1;
        return cache.getDouble(ram, Address(address));
    }

    void storeDouble(long long address, double value) {
        this->instruction_count += 1;
        cache.setDouble(ram, Address(address), value);
    }

    double addDouble(double value1, double value2) {
        this->instruction_count += 1;
        return value1 + value2;
    }

    double multiDouble(double value1, double value2) {
        this->instruction_count += 1;
        return value1 * value2;
    }
    void printRam() {
        this->ram.printRam();
    }
    void printCache() {
        this->cache.printCache();
    }
    long long countInstructions(){
        return this->instruction_count;
    }


    long long getReadHits() {
        return this->cache.getReadHits();
    }
    long long getReadMisses() {
        return this->cache.getReadMisses();
    }
    long long getWriteHits() {
        return this->cache.getWriteHits();
    }
    long long getWriteMisses() {
        return this->cache.getWriteMisses();
    }
};
