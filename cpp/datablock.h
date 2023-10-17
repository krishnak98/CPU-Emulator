#ifndef DATABLOCK_H
#define DATABLOCK_H




#include "constants.h"
#include <vector>
using namespace std;

class DataBlock {
    private:
        long long tag;
        vector<double> data;
        long long size;

    public:
        DataBlock(long long block_size) {
            this->size = block_size / 8;
            for(int i = 0; i < this->size; ++i) {
                this->data.push_back(0.0);
            }
            this->tag = -1;
        }

        long long getTag() {
            return this->tag;
        }

        void setTag(long long tag) {
            this->tag = tag;
        }

        vector<double> getData() { 
            return this->data;
        }
        double getData(int idx) {
            return this->data[idx];
        }
        
        long long getSize() {
            return this->size;
        }

        void setData(int idx ,double val) {
            this->data[idx] = val;
        }
};



#endif /* DATABLOCK_H */