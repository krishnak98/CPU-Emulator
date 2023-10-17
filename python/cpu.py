from python.address import Address
from python.ram import RAM
from cache import Cache
class CPU:
    def __init__(self, block_sz: int, assoc: int, cache_sz: int, ram_sz:int, policy: str):
        self.cache_sz = cache_sz
        self.ram_sz = ram_sz
        self.assoc = assoc
        self.block_sz = block_sz
        self.policy = policy
        self.ram = RAM(ram_sz, block_sz)
        self.num_blocks = (cache_sz) // (block_sz)
        self.num_sets = (cache_sz) // (assoc * block_sz)
        self.cache = Cache(self.num_sets, self.num_blocks, self.block_sz, self.policy)

    def loadDouble(self, address:int) -> float:
        return Cache.getDouble(self.cache, self.ram, Address(address))

    def storeDouble(self, address:int, value:float):
        Cache.setDouble(self.cache, self.ram, Address(address), value=value)

    def addDouble(self, value1: float, value2: float) -> float:
        return value1 + value2

    def multiDouble(self, value1: float, value2: float) -> float:
        return value1 * value2
