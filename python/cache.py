from datablock import DataBlock
from python.address import Address
from python.ram import RAM
import numpy as np  
from random import randint
from collections import deque, OrderedDict


class Cache:

    def __init__(self, num_sets: int, num_blocks: int, block_sz: int, policy: str):
        self.num_sets = num_sets
        self.num_blocks = num_blocks
        self.block_sz = block_sz
        self.blocks = [[DataBlock(block_sz) for j in range(num_blocks // num_sets)] for i in range(num_sets)]
        self.policy = policy
        if policy == "fifo":
            queue_list = []
            for i in range(num_sets):
                q = deque()
                queue_list.append(q)
            self.queues = queue_list

        if policy == "LRU":
            dict_list = []
            for i in range(num_sets):
                d = OrderedDict()
                dict_list.append(d)
            self.dicts = dict_list
        # self.blocks = np.full((num_sets, num_blocks // num_sets), fill_value=None, dtype=object)
        # for i in range(num_sets):
        #     for j in range(num_blocks // num_sets):
        #         self.blocks[i, j] = DataBlock(block_sz)

    def getDouble(self, ram: RAM, address:Address):
        block = self.getBlock(ram, address=address)
        offset = address.getOffset(self.block_sz)
        return block.data[offset]

    def setDouble(self, ram:RAM, address:Address, value:float):
        block = self.getBlock(ram, address)
        offset = address.getOffset(self.block_sz)
        block.data[offset] = value
        self.setBlock(ram, address, block)

    def getBlock(self, ram:RAM, address:Address):
        index = address.getIndex(self.block_sz, self.num_sets)
        tag = address.getTag(self.block_sz, self.num_sets)

        # try to see if block exists in cache
        
        datablock = self.blocks[index]
        empty_spot = -1
        for i in range(0, self.num_blocks//self.num_sets):
            if tag == datablock[i].getTag():
                # cache exists and is valid
                # print("cache hit in getBlock")

                if self.policy == "LRU":
                    # cache hit, move to the end
                    del self.dicts[index][i]
                    self.dicts[index][i] = None

                return self.blocks[index][i]

            if datablock[i].getTag() == -1: 
                empty_spot = i
                break

        # print("cache miss in getBlock")
        # get block from ram
        new_block = ram.getBlock(address=address)
        
        new_block.tag = tag
        
        # add cache eviction policy

        if empty_spot != -1: 
            # print("HERE", index, empty_spot)
            self.blocks[index][empty_spot] = new_block
            if self.policy == "fifo":
                self.queues[index].append(empty_spot)
            if self.policy == "LRU":
                self.dicts[index][empty_spot] = None
            return new_block
        else:
            # print("NO PLACE", index)
            if self.policy == "random":
                assert(len((self.blocks[index])) == self.num_blocks // self.num_sets)
                idx = randint(0, self.num_blocks // self.num_sets - 1)
                # print("Randomly putting in", idx)
                self.blocks[index][idx] = new_block
                return new_block
            elif self.policy == "fifo":
                assert(len(self.queues[index]) == self.num_blocks // self.num_sets)
                # get queue for correct set
                q = self.queues[index]
                idx = q.popleft()
                q.append(idx)
                self.queues[index] = q
                assert(idx <= self.num_blocks // self.num_sets)
                # print("FIFO putting in", idx)
                self.blocks[index][idx] = new_block
                return new_block

            elif self.policy == "LRU":
                # print(self.dicts[index])
                idx = self.dicts[index].popitem(last=False)
                self.dicts[index][idx[0]] = None
                # print("LRU putting in", idx[0])
                self.blocks[index][idx[0]] = new_block
                return new_block

                
                


    def setBlock(self, ram:RAM, address:Address, block:DataBlock):
        index = address.getIndex(self.block_sz, self.num_sets)
        tag = address.getTag(self.block_sz, self.num_sets)
        datablock = self.blocks[index]

        empty_spot = -1

        for i in range(0, self.num_blocks//self.num_sets):
            # if tag == datablock[i].getTag() and datablock[i].is_valid():
            if tag == datablock[i].getTag():
                # cache exists and is valid
                datablock[i] = block
                # print("cache hit in setBLock")
                # ram_block = ram.getBlock(address=address)
                # ram[ram_block] = block
                ram.setBlock(address=address, value=block)
                return 
            
            if datablock[i].getTag() == -1: 
                empty_spot = i
                break

        # print("cache miss in setBlock")

        if empty_spot != -1: 
            # empty spot found
            self.blocks[index][empty_spot] = block
            if self.policy == "LRU":
                self.dicts[index][empty_spot] = None
            if self.policy == "FIFO":
                self.queues[index].append(idx)
            # get block from ram
            ram.setBlock(address=address, value=block)
        else:
            if self.policy == "random":
                idx = randint(0, self.num_blocks // self.num_sets - 1)
                self.blocks[index][idx] = block
                ram.setBlock(address=address, value=block)
            elif self.policy == "fifo":
                assert(len(self.queues[index]) == self.num_blocks // self.num_sets)
                # get queue for correct set
                q = self.queues[index]
                idx = q.popleft()
                q.append(idx)
                # print("FIFO putting in", idx)
                self.queues[index] = q
                assert(idx <= self.num_blocks // self.num_sets)
                self.blocks[index][idx] = block
                ram.setBlock(address=address, value=block)

            elif self.policy == "LRU":
                idx = self.dicts[index].popitem(last=False)
                self.dicts[index][idx[0]] = None
                # print("LRU putting in", idx)
                self.blocks[index][idx[0]] = block
                ram.setBlock(address=address, value=block)
        