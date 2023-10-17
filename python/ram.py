from datablock import DataBlock
from python.address import Address
import numpy as np

class RAM:
    def __init__(self, ram_size: int, block_sz: int) -> None:
        self.num_blocks = ram_size // block_sz + 1
        self.block_sz = block_sz
        self.ram_size = ram_size
        self.data = [DataBlock(block_sz) for i in range(self.num_blocks)]
        # self.data = np.full((self.num_blocks,), fill_value=None, dtype=object)
        # for i in range(self.num_blocks):
        #     self.data[i] = DataBlock(block_sz)

    def getBlock(self, address:Address) -> DataBlock:
        return self.data[(address.address) // (self.block_sz)] 

    def setBlock(self, address:Address, value:DataBlock):
        self.data[(address.address) // (self.block_sz) ] = value

    # def getBlock(self, address:Address) -> DataBlock:
    #     return self.data[(address.address) // (self.block_sz)] 
    
    # def setBlock(self, address:Address, value: DataBlock):
    #     self.data[(address.address) // (self.block_sz)] = value