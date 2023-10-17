#include <vector>
#include "datablock.h"
#include "address.h"
#include "assert.h"

class RAM {
    private:
        long long num_blocks;
        long long block_size;
        long long ram_size;
        vector<DataBlock*> data;
        
    public: 
        RAM(long long ram_size, long long block_size) {
            this->num_blocks = ram_size / block_size + 1 ;
            this->block_size = block_size;
            this->ram_size = ram_size;
            // this->data = vector<DataBlock*>(num_blocks, new DataBlock(block_size));
            this->data = vector<DataBlock*>(num_blocks);
            for (int i = 0; i < num_blocks; i++) {
                this->data[i] = new DataBlock(block_size);
            }
        }

        DataBlock* getBlock(Address address) {
            long long index = (address.getAddress() / this->block_size);
            return this->data[index];
        }

        void setBlock(Address address, DataBlock* block){ 
            long long index = (address.getAddress() / this->block_size);
            *(this->data[index]) = *block;
        }


        void printRam() {
        cout << "PRINTING RAM\n";
        for(int i = 0; i < num_blocks; ++i) {
            for(int j = 0; j < this->data[i]->getSize(); ++j) {
                cout << this->data[i]->getData(j) << " " ;
            } 
            cout << endl;
        }
        cout << "FINISHING RAM\n";
    }
};