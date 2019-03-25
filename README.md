# Page Replacement Simulation

A C program that simulates the performance of the First-In-First-Out (FIFO), Least Recently Used (LRU), and Second Chance (SC) 
page replacement algorithms. The program represents pages in an array called 'table', corresponding page reference bits in an
array called 'ref_table', and corresponding page time-since-last-use in an array called 'priority_table'. Each algorithm reads
a page request and determines if the requested page exists in memory. If the page does not exist in memory, the request is 
printed and the corresponding replacement algorithm is run based on the algorithm and the number of pages in memory. Included 
is 'in.txt' file with 10000 random page requests to be used as input to the program, as well as '<algorithm>.txt' files 
containing the expected output (page faults) for the respective algorithm for a table of size 100. To run the code, make the 
'paging_algorithms.c' file executable in the command line. Then, enter 

    >> cat in.txt | ./paging_algorithms <algorithm> <table size> > out.txt. 

The options for <algorithm> are: FIFO, LRU, or SC. The table size can be any of your choosing, but to test the program against
the output files, enter 100 for this value. The program will parse the arguments and run the chosen algorithm on the input
file. After doing this, enter >> diff out.txt out_&lt;algorithm>.txt to compare the output of the execution to the respective 
expected output file. If nothing is printed, then the output is identical and the program works perfectly.
