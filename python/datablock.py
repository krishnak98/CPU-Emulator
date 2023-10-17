class DataBlock:
    def __init__(self, block_sz:int):
        
        # Tag to compare datablock in cache
        self.tag = -1

        # validity bit
        self.valid = False
        
        self.data = [0.0] * (block_sz // 8)
        self.size = block_sz // 8

    def getSize(self):
        return self.size
    
    def getData(self):
        return self.data
    
    def getTag(self):
        return self.tag
    
    def isValid(self):
        return self.valid