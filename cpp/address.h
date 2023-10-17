#ifndef ADDRESS_H
#define ADDRESS_H



#include "constants.h"


class Address {
    private: 
        long long address;
    
    public:
        Address(long long address) {
            this->address = address;
        }

        long long getTag(long long block_size, long long num_sets) {
            return address / (block_size * num_sets);
        }

        long long getIndex(long long block_size, long long num_sets) {
            return (address/block_size) % num_sets;
        }

        long long getOffset(long long block_size) {
            return (address % block_size) / DOUBLE_SIZE;
        }

        long long getAddress() {
            return this->address;
        }

};


#endif /* ADDRESS_H */