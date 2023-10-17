from cpu import CPU


def mxm_tiled(block_sz: int, assoc: int, cache_sz: int, ram_sz: int, policy: str, arr_len: int, tile: int): 
    # Initialize a Cpu object with whatever args you need
    cpu = CPU(block_sz, assoc, cache_sz, ram_sz, policy)

    # Assume 8 bytes per double
    sz = 8
    n = arr_len
    # Construct arrays of Addresses for length = 100 problem
    a = list(range( 0, n*n*sz, sz))
    b = list(range( n*n*sz, 2*n*n*sz, sz))
    c = list(range(2*n*n*sz, 3*n*n*sz, sz))

    # Initialize some dummy values
    x = 1
    for i in range(n):
        # print(i)
        for j in range(n):
            cpu.storeDouble(address=a[i*n+j], value=x)
            cpu.storeDouble(address=b[i*n+j], value=2*x)
            cpu.storeDouble(address=c[i*n+j], value=0)
            x += 1

    # Run the mxm_tiled. Registers are just local variables.
    # for i in range(n):
    #     # print(i)
    #     for j in range(n):
    #         print(j)
    #         # store the sum
    #         register0 = 0 
    #         for k in range(n):
    #             register1 = cpu.loadDouble(a[i*n+k])
    #             register2 = cpu.loadDouble(b[k*n+j])
    #             register3 = cpu.multiDouble(register1, register2)
    #             register0 = cpu.addDouble(register0, register3)
            
    #         cpu.storeDouble(c[i*n+j], register0)


    for jj in range(0, n, tile):
        for kk in range(0, n, tile):
            for i in range(0,n):
                for j in range(jj,min(jj + tile, n)):
                    register0 = 0 
                    for k in range(kk, min(kk+tile, n)):
                        register1 = cpu.loadDouble(a[i*n+k])
                        register2 = cpu.loadDouble(b[k*n+j])
                        register3 = cpu.multiDouble(register1, register2)
                        register0 = cpu.addDouble(register0, register3)
                    register4 = cpu.loadDouble(c[i*n+j])
                    register4 = cpu.addDouble(register4, register0)
                    cpu.storeDouble(c[i*n+j], register4)



    for i in range(n):
        for j in range(n):
            val = cpu.loadDouble(c[i*n+j])
            print(val)
