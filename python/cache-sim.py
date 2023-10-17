import argparse
from daxpy import daxpy
from mxm import mxm
from mxm_tiled import mxm_tiled
import time

def get_ram_size(dim, alg):
    if alg == "mxm_blocked" or alg == "mxm":
        return dim * dim * 24
    else:
        return dim * 24

def main():
    parser = argparse.ArgumentParser(description='CPU Emulator')
    parser.add_argument('-c', '--cache', help='Size of cache')
    parser.add_argument('-b', '--block', help='Size of block')
    parser.add_argument('-n', '--associativity', help='associativity of the cache')
    parser.add_argument('-r', '--replacement', help='replacement policy of the cache')
    parser.add_argument('-a', '--algorithm', help='algorithm to simulate')
    parser.add_argument('-d', '--dimension', help='dimension of matrix/vector')
    parser.add_argument('-p', '--print', help='Print', action='store_true') 
    parser.add_argument('-f', '--blocking', help='Blocking factor') 
    
    args = parser.parse_args()

    cache = 65536
    block_size = 64
    n = 1024
    r = "LRU"
    a = "mxm_blocked"
    d = 320
    f = 32
    p = False


    if args.cache:
        cache = int(args.cache)
    if args.block:
        block_size = int(args.block)
    if args.associativity:
        n = int(args.associativity)
    if args.replacement:
        r = args.replacement
    if args.algorithm:
        a = args.algorithm
    if args.dimension:
        d = int(args.dimension)
    if args.print:
        p = args.print
    if args.blocking:
        f = int(args.blocking)


    ram = get_ram_size(dim=d, alg=a)
    cache_blocks = cache//block_size
    sets = cache_blocks // n

    
    print("INPUTS====================================")
    print("Ram Size =               ", ram , "bytes")
    print("Cache Size =             ", cache , "bytes")
    print("Block size =             ", block_size, "bytes")
    print("Total blocks in cache =  ", cache_blocks)
    print("Associativity =          ", n)
    print("Number of sets =         ", sets)
    print("Replacement policy =     ", r)
    print("Algorithm =              ", a)
    print("MXM Blocking Factor =    ", f)
    print("Matrix/Vector dimension =", d)

    start = time.time()
    # daxpy(block_size, n, cache, ram, r, d)
    # mxm(block_size, n, cache, ram, r, d)
    mxm_tiled(block_size, n, cache, ram, r, d, 1)

    print(f"Elapsed time: {time.time() - start:.6f} seconds")
if __name__ == "__main__":
    main()