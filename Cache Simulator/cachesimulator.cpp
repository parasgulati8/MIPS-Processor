/*
Cache Simulator
Level one L1 and level two L2 cache parameters are read from file (block size, line per set and set per cache).
The 32 bit address is divided into tag bits (t), set index bits (s) and block offset bits (b)
s = log2(#sets)   b = log2(block size)  t=32-s-b
*/
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <iomanip>
#include <stdlib.h>
#include <cmath>
#include <bitset>

using namespace std;
//access state:
#define NA 0 // no action
#define RH 1 // read hit
#define RM 2 // read miss
#define WH 3 // Write hit
#define WM 4 // write miss




struct config{
       int L1blocksize;
       int L1setsize;
       int L1size;
       int L2blocksize;
       int L2setsize;
       int L2size;
       };
//Defining the cache class to be used in main()
class cache
{
public:

	cache(int blockSIze, int setsize, int cacheSize)
	{
			if(setsize == 0)
			{
				rows = 1;
				cols = 32 - log2(blockSIze);
				noOfIndexbits = 0;
			}
			else
			{
				rows = (cacheSize * 1024) / blockSIze;
				rows = rows / setsize;
				cols = setsize;
				noOfIndexbits = log2(rows);
			}
			cacheMemory.resize(rows, vector<unsigned long>(cols));
			validBit.resize(rows, vector<int>(cols, 0));
			mru.resize(rows, vector<int>(cols, 0));
			noOfOffsetBits = log2(blockSIze);
			noOfTagbits = 32 - noOfIndexbits - noOfOffsetBits;
	}

	int getIndex(bitset<32> addr)
	{
		string sestring = addr.to_string<char,std::string::traits_type,std::string::allocator_type>();
		string ind = sestring.substr(noOfTagbits, noOfIndexbits);
		return bitset<32>(ind).to_ulong() ;
	}

	unsigned long getTagid(bitset<32> addr)
	{
		string sestring = addr.to_string<char,std::string::traits_type,std::string::allocator_type>();
		string ind = sestring.substr(0, noOfTagbits);
		return bitset<32>(ind).to_ulong() ;
	}

	int getOffset(bitset<32> addr)
	{
			string sestring = addr.to_string<char,std::string::traits_type,std::string::allocator_type>();
			string ind = sestring.substr((noOfTagbits + noOfIndexbits), noOfOffsetBits);
			return bitset<32>(ind).to_ulong() ;
	}

	int searchTag(int index, unsigned int tagID, int offset = 0)
	{
		int found = -1;
		int WayNumber;
	    for (int i=0; i < cols; i++)
	    {
	    	if(cacheMemory[index][i] == tagID)
	    	{
	    		if(validBit[index][i] == 1 )
	    		{
	    			found =1;
	    			WayNumber = i;
	    			break;
	    		}
	    	}
	    }
	    if(found < 0)
	    	return found;
	    else
	    	return WayNumber;
	}

    int insertTag(unsigned long index, unsigned long tagID)
    {
    	int wayNumber = -1;
    	// Finding the empty way number
    	cout<<"\n Valid Bits :";
    	for(int i = 0; i < cols; i++)
    	{
    		cout<<"\t"<<validBit[index][i];
    		if(validBit[index][i] != 1)
    		{
    			wayNumber = i;
    			break;
    		}

    	}
    	if(wayNumber < 0)
    	{
    		wayNumber = mru[index][cols - 1];
    	}
    	//inserting the data at empty way
        cacheMemory[index][wayNumber]= tagID ;
        //updating valid bit
        validBit[index][wayNumber] = 1;
        return wayNumber;
    }

    void updateMru(int index, int way)
    {
    	int pos;
    	if(mru[index][0] != way)
    	{
    		//search the position of way in the mru array i.e. 0, 1, 2 or 3
    		for(int i = 1; i < cols; i++)
    		{
    			if(mru[index][i]== way)
    			pos = i;
    		}
    		//shifting the values down the array
    		for(; pos>0; pos--)
    			mru[index][pos] = mru[index][pos -1];
    		//updating the 0th position with the most recent way
    		mru[index][0] = way;
    	}

    }

    void displayDimension()
    {
    	cout<<"\nNumber of Rows : "<<rows;
    	cout<<"\nNumber of Cols : "<<cols<<"\n";
    }

    void displayCache()
        {
        	for (int i = 0; i< rows; i++)
        	{
        		cout<<"\n";
        		for (int j=0; j < cols; j++)
        		{
        			cout<<"\t"<<cacheMemory[i][j];
        		}
        	}
        }

    void displayValidBit()
        {
        	for (int i = 0; i< rows; i++)
        	{
        		cout<<"\n";
        		for (int j=0; j < cols; j++)
        		{
        			cout<<"\t"<<validBit[i][j];
        		}
        	}
        }

    void displayMRU()
    {
    	for (int i = 0; i< rows; i++)
    	{
    		cout<<"\n";
    		for (int j=0; j < cols; j++)
    		{
    			cout<<"\t"<<mru[i][j];
    		}
    	}
    }

private:
	vector< vector<unsigned long> > cacheMemory;
	vector< vector<int> > validBit;
	vector< vector<int> > mru;
	int rows, cols, noOfTagbits, noOfIndexbits, noOfOffsetBits;
};

int main(int argc, char* argv[]){
    config cacheconfig;
    ifstream cache_params;
    string dummyLine;
    cache_params.open(argv[1]);
    while(!cache_params.eof())  // read config file
    {
      cache_params>>dummyLine;
      cache_params>>cacheconfig.L1blocksize;
      cache_params>>cacheconfig.L1setsize;              
      cache_params>>cacheconfig.L1size;
      cache_params>>dummyLine;              
      cache_params>>cacheconfig.L2blocksize;           
      cache_params>>cacheconfig.L2setsize;        
      cache_params>>cacheconfig.L2size;
      }

   // initialize the hierarchy cache system with those configs
   // probably you may define a Cache class for L1 and L2, or any data structure you like
   
    	//creating two objects of cache for L1 and L2 cache
    	cache L1(cacheconfig.L1blocksize, cacheconfig.L1setsize, cacheconfig.L1size);
    	cache L2(cacheconfig.L2blocksize, cacheconfig.L2setsize, cacheconfig.L2size);

    	int insetedAt;

  int L1AcceState =0; // L1 access state variable, can be one of NA, RH, RM, WH, WM;
  int L2AcceState =0; // L2 access state variable, can be one of NA, RH, RM, WH, WM;
   
   
    ifstream traces;
    ofstream tracesout;
    string outname;
    outname = string(argv[2]) + ".out";
    
    traces.open(argv[2]);
    tracesout.open(outname.c_str());
    
    string line;
    string accesstype;  // the Read/Write access type from the memory trace;
    string xaddr;       // the address from the memory trace store in hex;
    unsigned int addr;  // the address from the memory trace store in unsigned int;        
    bitset<32> accessaddr; // the address from the memory trace store in the bitset;
    
    if (traces.is_open()&&tracesout.is_open()){    
        while (getline (traces,line))
        {   // read mem access file and access Cache
            
            istringstream iss(line); // copies the content of line to iss
            if (!(iss >> accesstype >> xaddr)) {break;} //iterating through iss string and moves the content upto first white space to accesstype and upto second white space to xaddr
            stringstream saddr(xaddr); //copies the content of xaddr to saddr
            saddr >> std::hex >> addr; // since xaddr is hex, converting it to int
            accessaddr = bitset<32> (addr);
           
           // access the L1 and L2 Cache according to the trace;
              if (accesstype.compare("R")==0)
             {    

                 // read access to the L1 Cache, 
                 //  and then L2 (if required), 
                 //  update the L1 and L2 access state variable;

            	 int L1found, L2found;

            	 L1found = L1.searchTag(L1.getIndex(accessaddr), L1.getTagid(accessaddr));
                 if(L1found >= 0) //  found in L1 cache
                 {
                	 L1.updateMru(L1.getIndex(accessaddr), L1found);
                	 L1AcceState = RH;
                	 L2AcceState = NA;
                 }
                 else		// not found in L1, search in L2
                 {
                	 L2found = L2.searchTag(L2.getIndex(accessaddr), L2.getTagid(accessaddr));
                	 if (L2found < 0) // not found in L2 cache as well
                	 {
                		 L2AcceState = RM;
                		 L1AcceState = RM;
                		 insetedAt = L2.insertTag(L2.getIndex(accessaddr), L2.getTagid(accessaddr));
                		 L2.updateMru(L2.getIndex(accessaddr),  insetedAt);
                		 insetedAt = L1.insertTag(L1.getIndex(accessaddr), L1.getTagid(accessaddr));
                		 L1.updateMru(L1.getIndex(accessaddr),  insetedAt);
                	 }
                	 else
                	 {
                		 L2.updateMru(L2.getIndex(accessaddr), L2found);
                		 L2AcceState = RH;
                		 L1AcceState = RM;
                		 insetedAt= L1.insertTag(L1.getIndex(accessaddr), L1.getTagid(accessaddr));
                		 L1.updateMru(L1.getIndex(accessaddr),  insetedAt);
                	 }
                 }
                 
             }
             else //conditions for write
             {    
                  // write access to the L1 Cache, 
                  //and then L2 (if required), 
                  //update the L1 and L2 access state variable;
            	 int L1found, L2found;
             	 L1found = L1.searchTag(L1.getIndex(accessaddr), L1.getTagid(accessaddr));
             	 if(L1found >= 0) //  found in L1 cache
             	 {
             		L1.updateMru(L1.getIndex(accessaddr), L1found);
             		L1AcceState = WH;
             		L2AcceState = WH;
             		//Writing data to lower level L2
             		L2found = L2.searchTag(L2.getIndex(accessaddr), L2.getTagid(accessaddr));
             		if(L2found >= 0)
             		L2.updateMru(L2.getIndex(accessaddr), L2found);
             	 }
             	 else
             	 {
             		L2found = L2.searchTag(L2.getIndex(accessaddr), L2.getTagid(accessaddr));
             		if (L2found < 0) // not found in L2 cache as well
             		{
             			L2AcceState = WM;
             			L1AcceState = WM;
             		}
             		else 	// found in L2 cache
             		{
             			L2.updateMru(L2.getIndex(accessaddr), L2found);
             			L2AcceState = WH;
             			L1AcceState = WM;
             		}
             	 }
              }
              
            tracesout<< L1AcceState << " " << L2AcceState << endl;  // Output hit/miss results for L1 and L2 to the output file;
        }
        traces.close();
        tracesout.close(); 
    }
    else cout<< "Unable to open trace or traceout file ";

 //   L1.displayCache();
    //cout<<"\n\n\n\n";
 //   L2.displayCache();
    return 0;
}
