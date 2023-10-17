class Address:
    def __init__(self, address: int):
        self.address = address
    
    def getTag(self, block_sz: int, num_sets: int):
        return self.address // (block_sz * num_sets)

    def getIndex(self, block_sz: int, num_sets: int):
        return (self.address // block_sz) % num_sets
    
    def getOffset(self, block_sz:int):
        return (self.address % (block_sz)) // 8
