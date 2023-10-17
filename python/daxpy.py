from cpu import CPU
# import pdb
def daxpy(block_sz: int, assoc: int, cache_sz: int, ram_sz: int, policy: str, arr_len: int): 
    # Initialize a Cpu object with whatever args you need
    cpu = CPU(block_sz, assoc, cache_sz, ram_sz, policy)

    # Assume 8 bytes per double
    sz = 8
    n = arr_len
    # Construct arrays of Addresses for length = 100 problem
    a = list(range( 0, n*sz, sz))
    b = list(range( n*sz, 2*n*sz, sz))
    c = list(range(2*n*sz, 3*n*sz, sz))

    # Initialize some dummy values
    for i in range(n):
        # breakpoint()
        cpu.storeDouble(address=a[i], value=i)
        # breakpoint()
        cpu.storeDouble(address=b[i], value=2*i)
        # breakpoint()
        cpu.storeDouble(address=c[i], value=0)

    # Put a random 'D' value into a register
    register0 = 3

    # Run the daxpy. Registers are just local variables.
    for i in range(n):
        register1 = cpu.loadDouble(a[i])
        register2 = cpu.multiDouble(register0, register1)
        register3 = cpu.loadDouble(b[i])
        register4 = cpu.addDouble(register2, register3)
        cpu.storeDouble(c[i], register4)

    for i in range(n):
       val = cpu.loadDouble(c[i])
       print(val)
