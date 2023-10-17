#include <iostream>
#include <string.h>
#include <stdio.h>
#include "constants.h"
#include "cpu.h"
#include <iomanip>


using namespace std;



long long get_ram_size(long long dim, string alg) {
    if (alg == "daxpy") {
        return dim * 24;
    } else {
        return dim * dim * 24;
    }
}


void daxpy(int block_sz, int assoc, int cache_sz, int ram_sz, std::string policy, int arr_len, bool p) {
    // Initialize a CPU object with whatever args you need
    CPU cpu(block_sz, assoc, cache_sz, ram_sz, policy);
        
    // Assume 8 bytes per double
    int sz = 8;
    int n = arr_len;
    // Construct arrays of Addresses for length = 100 problem
    int a[n], b[n], c[n];
    for (int i = 0; i < n; i++) {
        a[i] = i * sz;
        b[i] = (n + i) * sz;
        c[i] = (2 * n + i) * sz;
    }

    // Initialize some dummy values
    for (int i = 0; i < n; i++) {
        cpu.storeDouble(a[i], i);
        cpu.storeDouble(b[i], 2*i);
        cpu.storeDouble(c[i], 0);
    }
    // Put a random 'D' value into a register
    double register0 = 3;
    
    // cpu.printRam();

    // cpu.printCache();
    // Run the daxpy. Registers are just local variables.
    for (int i = 0; i < n; i++) {
        double register1 = cpu.loadDouble(a[i]);
        double register2 = cpu.multiDouble(register0, register1);
        double register3 = cpu.loadDouble(b[i]);
        double register4 = cpu.addDouble(register2, register3);
        cpu.storeDouble(c[i], register4);
    }

    // cpu.printRam();
    // cpu.printCache();
    if (p) {
        for (int i = 0; i < n; i++) {
            double val = cpu.loadDouble(c[i]);
            cout << val << endl;
        }
    }
    cout << "Instructions Count: " << cpu.countInstructions() << endl;
    cout << "Read Hits: " << cpu.getReadHits() << endl;
    cout << "Read Misses: " << cpu.getReadMisses() << endl;
    cout << "Read miss rate: " << (double)(cpu.getReadMisses()) / (double)(cpu.getReadMisses() + cpu.getReadHits()) * 100.0 << endl;
    cout << "Write Hits: " << cpu.getWriteHits() << endl;
    cout << "Write Misses: " << cpu.getWriteMisses() << endl;
    cout << "Write miss rate: " << (double)(cpu.getWriteMisses()) / (double)(cpu.getWriteMisses() + cpu.getWriteHits()) * 100.0 << endl;
}


void mxm(int block_sz, int assoc, int cache_sz, int ram_sz, string policy, int arr_len, bool p) {
    // Initialize a Cpu object with whatever args you need
    CPU cpu(block_sz, assoc, cache_sz, ram_sz, policy);

    // Assume 8 bytes per double
    int sz = 8;
    int n = arr_len;

    // Construct arrays of Addresses for length = 100 problem
    vector<int> a,b,c;
    for (int i = 0; i < n * n * sz; i += sz) {
        a.push_back(i);
        b.push_back(i + n*n*sz);
        c.push_back(i + n*n*sz*2);
    }
    
    // Initialize some dummy values
    for (int i = 0; i < n*n; i++) {
        // cout << i << endl;
        cpu.storeDouble(a[i], i);
        cpu.storeDouble(b[i], 2*i);
        // cpu.storeDouble(a[i], 1);
        // cpu.storeDouble(b[i], 1);
        cpu.storeDouble(c[i], 0);
    }

    // cpu.printRam();
    // cpu.printCache();

    // Run the mxm. Registers are just local variables.
    for (int i = 0; i < n; i++) {
        // cout << i << endl;
        for (int j = 0; j < n; j++) {
            // store the sum
            double register0 = 0;
            for (int k = 0; k < n; k++) {
                double register1 = cpu.loadDouble(a[i*n+k]);
                double register2 = cpu.loadDouble(b[k*n+j]);
                double register3 = cpu.multiDouble(register1, register2);
                register0 = cpu.addDouble(register0, register3);
            }
            cpu.storeDouble(c[i*n+j], register0);
        }
    }
    if (p) {
        for (int i = 0; i < n; i++) {
            for(int j = 0; j < n; ++j) {
                double val = cpu.loadDouble(c[i*n+j]);
                cout << val << " ";
            }
            cout << endl;
        }
    }
    cout << "Instructions Count: " << cpu.countInstructions() << endl;
    cout << "Read Hits: " << cpu.getReadHits() << endl;
    cout << "Read Misses: " << cpu.getReadMisses() << endl;
    cout << "Read miss rate: " << (double)(cpu.getReadMisses()) / (double)(cpu.getReadMisses() + cpu.getReadHits()) * 100.0 << endl;
    cout << "Write Hits: " << cpu.getWriteHits() << endl;
    cout << "Write Misses: " << cpu.getWriteMisses() << endl;
    cout << "Write miss rate: " << (double)(cpu.getWriteMisses()) / (double)(cpu.getWriteMisses() + cpu.getWriteHits()) * 100.0 << endl;

}

void mxm_block(int block_sz, int assoc, int cache_sz, int ram_sz, string policy, int arr_len, int f, bool p) {
    // Initialize a Cpu object with whatever args you need
    CPU cpu(block_sz, assoc, cache_sz, ram_sz, policy);

    // Assume 8 bytes per double
    int sz = 8;
    int n = arr_len;
    // Construct arrays of Addresses for length = 100 problem
    vector<int> a,b,c;
    for (int i = 0; i < n * n * sz; i += sz) {
        a.push_back(i);
        b.push_back(i + n*n*sz);
        c.push_back(i + n*n*sz*2);
    }
    
    // Initialize some dummy values
    for (int i = 0; i < n*n; i++) {
        // cout << i << endl;
        cpu.storeDouble(a[i], i);
        cpu.storeDouble(b[i], 2*i);
        // cpu.storeDouble(a[i], 1);
        // cpu.storeDouble(b[i], 1);
        cpu.storeDouble(c[i], 0);
    }

    // cpu.printRam();
    // cpu.printCache();

    for (int jj=0;jj<n;jj+=f){
        // cout << jj << endl;
        for (int kk=0;kk<n;kk+=f){
            // cout << kk << endl;
            for (int i=0;i<n;++i){
	            for (int j=jj;j<min(jj+f,n);++j){
	                double register0 = 0;
	                for (int k=kk;k<min(kk+f,n);++k) {
                        double register1 = cpu.loadDouble(a[i*n+k]);
                        double register2 = cpu.loadDouble(b[k*n+j]);
                        double register3 = cpu.multiDouble(register1, register2);
                        register0 = cpu.addDouble(register0, register3);
                    }
                    double register4 = cpu.loadDouble(c[i*n+j]);
                    register4 = cpu.addDouble(register4, register0);
	                cpu.storeDouble(c[i*n+j], register4);
	            }
            }
        }
    }
    if(p) {
        for (int i = 0; i < n; i++) {
            for(int j = 0; j < n; ++j) {
                double val = cpu.loadDouble(c[i*n+j]);
                cout << val << " ";
            }
            cout << endl;
        }
    }
    cout << "Instructions Count: " << cpu.countInstructions() << endl;
    cout << "Read Hits: " << cpu.getReadHits() << endl;
    cout << "Read Misses: " << cpu.getReadMisses() << endl;
    cout << "Read miss rate: " << (double)(cpu.getReadMisses()) / (double)(cpu.getReadMisses() + cpu.getReadHits()) * 100.0 << endl;
    cout << "Write Hits: " << cpu.getWriteHits() << endl;
    cout << "Write Misses: " << cpu.getWriteMisses() << endl;
    cout << "Write miss rate: " << (double)(cpu.getWriteMisses()) / (double)(cpu.getWriteMisses() + cpu.getWriteHits()) * 100.0 << endl;
 }


int main(int argc, char* argv[]) {
    /*
    *   default values    
    */
    long long cache = DEFAULT_CACHE_SIZE; 
    long long block_size = DEFAULT_BLOCK_SIZE;
    long long n = DEFAULT_ASSOCIATIVITY;
    string r = "LRU";
    string a = "mxm_block";
    long long d = 200;
    long long f = 32;
    bool p = false;


    for(int i = 0; i < argc; ++i) {
        if (strcmp(argv[i], "-c") == 0 && i+1 < argc) {
            cache = atoi(argv[i+1]);
        } else if(strcmp(argv[i], "-b") == 0 && i+1 < argc) {
            block_size = atoi(argv[i+1]);
        } else if(strcmp(argv[i], "-n") == 0 && i+1 < argc) {
            n = atoi(argv[i+1]);
        } else if(strcmp(argv[i], "-r") == 0 && i+1 < argc) {
            r = (argv[i+1]);
        } else if(strcmp(argv[i], "-a") == 0 && i+1 < argc) {
            a = (argv[i+1]);
        } else if (strcmp(argv[i], "-d") == 0 && i+1 < argc) {
            d = atoi(argv[i+1]);
        } else if (strcmp(argv[i], "-f") == 0 && i+1 < argc) {
            f = atoi(argv[i+1]);
        }
        else if (strcmp(argv[i], "-p") == 0) {
            p = true;
        }
    }

    long long ram_size = get_ram_size(d , a);

    long long cache_blocks = cache / block_size;

    long long sets = cache_blocks / n ;
    cout << "INPUTS====================================\n";
    cout << "Ram Size =                 " << ram_size << " bytes\n";
    cout << "Cache Size =               " << cache << " bytes\n";
    cout << "Block size =               " << block_size << " bytes\n";
    cout << "Total blocks in cache =    " << cache_blocks << "\n";
    cout << "Associativity =            " << n << "\n";
    cout << "Number of sets =           " << sets << "\n";
    cout << "Replacement policy =       " << r << "\n";
    cout << "Algorithm =                " << a << "\n";
    cout << "MXM Blocking Factor =      " << f << "\n";
    cout << "Matrix/Vector dimension =  " << d << "\n";
    if (a == "daxpy") {
        daxpy(block_size, n, cache, ram_size, r, d, p);
    }
    else if(a == "mxm") {
        mxm(block_size, n, cache, ram_size, r, d, p);
    }
    else {
        mxm_block(block_size, n, cache, ram_size, r, d, f, p);
    }
}