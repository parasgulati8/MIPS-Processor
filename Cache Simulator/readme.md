<h1> Cache Simulator </h1>

<h2> Description </h2>
This project is an implementation of a two-level (L1 and L2) cache simulator which takes parameters describing the cache (cache size, block size and associativity) and a memory access file for an input program. The simulator is designed using C++.

<h2> Installation </h2>
Instructions on how to compile code
1) save the cachesimulator.cpp file, cacheconfig.txt file, trace_small.txt file and trace.txt file in same folder 
2) Open command prompt in windows
3) change the working directory to the location where the files are stored
4) run the below script to create .exe file 
	> g++ cachesimulator.cpp -o simulator
5) execute the simulator.exe file created using :
	> simulator.exe cacheconfig.txt trace.txt
	
<h2> Simulator Output </h2>
For each cache access, the simulator outputs whether the access caused a read or write hit or miss in the L1 and L2 caches. Each event is coded with a number, as shown below. 
 
 <ul>
<li>0: No Access</li>
<li>1: Read Hit </li>
<li>2: Read Miss </li>
<li>3: Write Hit </li>
<li>4: Write Miss </li>
</ul>
For example, if a read access Misses in the L1 cache but hits in the L2 cache, your simulator would output: 2 1 
