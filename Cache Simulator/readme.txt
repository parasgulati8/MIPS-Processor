Instructions on how to compile code

1) save the cachesimulator.cpp file, cacheconfig.txt file, trace_small.txt file and trace.txt file in same folder 
2) Open command prompt in windows
3) change the working directory to the location where the files are stored
4) run the below script to create .exe file 
	> g++ cachesimulator.cpp -o simulator
5) execute the simulator.exe file created using :
	> simulator.exe cacheconfig.txt trace.txt
