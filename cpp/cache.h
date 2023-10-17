#include <vector>
#include "datablock.h"
#include "address.h"
#include <string>
#include <random>
#include <deque>
#include <algorithm>
using namespace std;

class Cache {
    private:
        long long num_blocks;
        long long block_size;
        long long ram_size;
        long long num_sets;
        string policy;
        vector<vector<DataBlock*>> blocks;
        vector<deque<long long>*> fifo_queues;
        vector<deque<long long>*> lru_queues;
        long long read_hits;
        long long write_hits;
        long long read_misses;
        long long write_misses;
        
    public: 
        Cache(long long num_sets, long long num_blocks, long long block_size, string policy) {
            this->num_sets = num_sets;
            this->num_blocks = num_blocks;
            this->block_size = block_size;
            this->policy = policy;
            this->read_hits = 0;
            this->write_hits = 0;
            this->read_misses = 0;
            this->write_misses = 0;
            if (this->policy == "fifo") {
                vector<deque<long long>*> fifo_queues;
                for(int i = 0; i < num_sets; ++i) {
                    deque<long long>* dq = new std::deque<long long>();
                    fifo_queues.push_back(dq);
                }
                this->fifo_queues = fifo_queues;
            } else if(this->policy == "LRU") {
                vector<deque<long long>*> lru_queues;
                for(int i = 0; i < num_sets; ++i) {
                    deque<long long>* v = new std::deque<long long>();
                    lru_queues.push_back(v);
                }
                this->lru_queues = lru_queues;
            }
            // this->blocks = vector<vector<DataBlock*>>(num_sets, vector<DataBlock*>(num_blocks / num_sets, new DataBlock(block_size)));
            this->blocks = vector<vector<DataBlock*>>(num_sets);

            for (int i = 0; i < num_sets; i++) {
                this->blocks[i] = vector<DataBlock*>(num_blocks / num_sets);
                for (int j = 0; j < num_blocks / num_sets; j++) {
                    this->blocks[i][j] = new DataBlock(block_size);
                }
            }       
        }

        double getDouble(RAM ram, Address address) {
            DataBlock* block = this->getBlock(ram, address, 0);
            long long offset = address.getOffset(this->block_size);
            return block->getData(offset);
        }

        void setDouble(RAM ram, Address address, double value) {
            DataBlock* block = this->getBlock(ram, address, 1);
            long long offset = address.getOffset(this->block_size);
            // block->data[offset] = value;
            block->setData(offset, value);
            this->setBlock(ram, address, block);
        }

        DataBlock* getBlock(RAM ram, Address address, int parentFunc) {
            long long index = address.getIndex(this->block_size, this->num_sets);
            long long tag = address.getTag(this->block_size, this->num_sets);

            vector<DataBlock*> datablock = this->blocks[index];
            long long empty_spot = -1;
            for(int i = 0; i < this->num_blocks/ this->num_sets; ++i) {
                if (tag == datablock[i]->getTag()) {
                    // cout << "Cache hit\n";
                    if (this->policy == "LRU") {
                        deque<long long>* v = this->lru_queues[index];
                        auto it = std::find(v->begin(), v->end(), (long long)(i));
                        if (it == v->end()) {
                            assert(false);
                        }
                        v->erase(it);
                        v->push_back(i);
                    }
                    if(parentFunc) this->incrementWriteHits();
                    else this->incrementReadHits();
                    return this->blocks[index][i];
                }
                if (datablock[i]->getTag() == -1) {
                    // empty spot found
                    empty_spot = i;
                    break;
                }
            }
            if(parentFunc) this->incrementWriteMisses();
            else this->incrementReadMisses();

            DataBlock* block = ram.getBlock(address);
            block->setTag(tag);

            if (empty_spot != -1) {
                // cout << "HERE " << index << " " << empty_spot << endl;
                *(this->blocks[index][empty_spot]) = *(block);
                if (this->policy == "fifo") {
                    deque<long long>* dq = this->fifo_queues[index];
                    dq->push_back(empty_spot);
                } else if(this->policy == "LRU") {
                    deque<long long>* v = this->lru_queues[index];
                    v->push_back(empty_spot);
                }
                return this->blocks[index][empty_spot];
            } else {
                if (this->policy == "random") {
                    std::random_device seed;
                    std::mt19937 gen{seed()}; // seed the generator

                    std::uniform_int_distribution<> dist{0, (int)(this->num_blocks / this->num_sets - 1)}; // set min and max
                    long long idx = dist(gen); // generate number
                    // cout << "Randomly putting in" <<  idx << endl;
                    *(this->blocks[index][idx]) = *(block);
                    return  this->blocks[index][idx];
                } else if(this->policy == "fifo") {
                    deque<long long>* q = this->fifo_queues[index];
                    assert(q->size() ==  this->num_blocks / this->num_sets);
                    long long idx = q->front();
                    q->pop_front();
                    q->push_back(idx);
                    // cout << "FIFO putting in" << idx << endl; 
                    *(this->blocks[index][idx]) = *(block);
                    return this->blocks[index][idx];
                } else if(this->policy == "LRU") {
                    deque<long long>* v = this->lru_queues[index];
                    long long idx = v->front();
                    v->pop_front();
                    v->push_back(idx);
                    // cout << "LRU putting in" << idx << endl;
                    
                    *(this->blocks[index][idx]) = *(block);
                    return this->blocks[index][idx];
                }
            }
            // *(this->blocks[index][0]) = *(block);
            // return this->blocks[index][0];
        }

        void setBlock(RAM ram, Address address, DataBlock* block) {
            long long index = address.getIndex(this->block_size, this->num_sets);
            long long tag = address.getTag(this->block_size, this->num_sets);
            vector<DataBlock*> datablock = this->blocks[index];
            long long empty_spot = -1;
            for(int i = 0; i < this->num_blocks/ this->num_sets; ++i) {
                if (tag == datablock[i]->getTag()) {
                    // cout << "Cache hit\n";
                    *(datablock[i]) = *block;
                    ram.setBlock(address, block);
                    return;
                }
                if (datablock[i]->getTag() == -1) {
                    empty_spot = i;
                    break;
                }
            }
            // *(this->blocks[index][0]) = *(block);
            // ram.setBlock(address, block);
            if (empty_spot != -1) {
                *(this->blocks[index][empty_spot]) = *(block);
                if (this->policy == "fifo") {
                    this->fifo_queues[index]->push_back(empty_spot);
                } else if(this->policy == "LRU") {
                    this->lru_queues[index]->push_back(empty_spot);
                }
                ram.setBlock(address, block);
            } else {
                if (this->policy == "random") {
                    std::random_device seed;
                    std::mt19937 gen{seed()}; // seed the generator
                    std::uniform_int_distribution<> dist{0, (int)(this->num_blocks / this->num_sets - 1)}; // set min and max
                    long long idx = dist(gen); // generate number
                    // cout << "Randomly putting in" <<  idx << endl;
                    *(this->blocks[index][idx]) = *(block);
                    ram.setBlock(address, block);
                } else if(this->policy == "fifo") {
                    deque<long long>* q = this->fifo_queues[index];
                    assert(q->size() ==  this->num_blocks / this->num_sets);
                    long long idx = q->front();
                    q->pop_front();
                    q->push_back(idx);
                    // cout << "FIFO putting in" << idx << endl;
                    *(this->blocks[index][idx]) = *(block);
                    ram.setBlock(address, block);
                } else if(this->policy == "LRU") {
                    deque<long long>* q = this->lru_queues[index];
                    long long idx = q->front();
                    q->pop_front();
                    q->push_back(idx);
                    // cout << "LRU putting in" << idx << endl;
                    *(this->blocks[index][idx]) = *(block);
                    ram.setBlock(address, block);
                }
            }
        }

        void printCache() {
            cout << "PRINTING CACHE\n";
            for(int i = 0; i < this->num_sets; ++i) {
                for(int j = 0; j < this->num_blocks / this->num_sets; ++j) {
                   cout << "Set " << i << ", Block " << j << ":" << endl;
                    for (int k = 0; k < block_size / sizeof(double); k++) {
                        cout << this->blocks[i][j]->getData(k) << " ";
                    }
                    cout << endl;
                }
            }
        }

        void incrementReadMisses() {
            this->read_misses += 1;
        }
        void incrementReadHits() {
            this->read_hits +=1 ;
        }
        void incrementWriteMisses() {
            this->write_misses += 1;
        }
        void incrementWriteHits() {
            this->write_hits += 1;
        }
        long long getReadHits() {
            return this->read_hits;
        }
        long long getReadMisses() {
            return this->read_misses;
        }
        long long getWriteHits() {
            return this->write_hits;
        }
        long long getWriteMisses() {
            return this->write_misses;
        }
};