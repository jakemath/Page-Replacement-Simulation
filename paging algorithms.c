#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int capacity;   // Table capacity
int occupied;   // # entries
int time;   // Current time
int *table; // Page table
int *ref_table; // Reference bit table
int *priority_table;    // Priority table

int find_page (int request, int *occupied) // Return index of request if it exists in memory, else -1
{
	int i;
	for (i = 0; i < *occupied; ++i)
	{
		if (table[i] == request)
			return i;
	}
	return -1;
}

void rotate_right (int alg, int *occupied)  // Move front element of table to back of array and shift elements back by one
{
	int t1 = table[0], i;
	if (alg == 1)   // 1 = FIFO --> shift only the table
	{
		for (i = 0; i < *occupied - 1; ++i)
		{
			table[i] = table[i+1];
		}
		table[*occupied-1] = t1;
	}
	else if (alg == 2)  // 2 = Second Chance --> shift table and reference table
	{
		int t2 = ref_table[0];
		for(i = 0; i < *occupied - 1 ; ++i)
		{
			table[i] = table[i+1];
			ref_table[i] = ref_table[i+1];
		}
		table[*occupied - 1] = t1;
		ref_table[*occupied - 1] = t2;
	}
	else if (alg == 3)  // 3 = LRU --> shift table and priority table
	{
		int t2 = priority_table[0];
		for(i = 0; i < *occupied - 1 ; ++i)
                {
                        table[i] = table[i+1];
                        priority_table[i] = ref_table[i+1];
                }
                table[*occupied - 1] = t1;
                priority_table[*occupied - 1] = t2;
	}
	else
		fprintf(stderr,"ERROR\n");
}

int fifo (int request, int *occupied)	// FIFO function
{
	if (*occupied == 0)
	{
		printf("%i\n",request);	// Page fault
		table[*occupied] = request;
		++*occupied;
		return 1;
	}
	else if (find_page(request, occupied) == -1)	// If request not in memory
	{
			printf("%i\n",request);	// Print page fault
			if (*occupied < capacity)	// If space for a new entry
			{	
				table[*occupied] = request;	// Insert at end of array
				++*occupied;    // Increment occupancy
				return 1;   // Return 1 page fault
			}
			else	// If no space for a new entry
			{
				rotate_right(1,occupied);	// Rotate FIFO
				table[*occupied - 1] = request;	// Replace the last entry (oldest) with request
				return 1;   // Return 1 page fault
			}
	}
	return 0;   // If page request is in memory --> return 0 page fault	
}

int lru (int request, int *occupied, int *time) // LRU function
{
	if (*occupied == 0) // If table is empty
	{
		printf("%i\n",request); // Print page fault
		table[0] = request; // Insert request at front
		priority_table[0] = *time;  // Set priority to current time
		++*time;    // Increment time and occupancy
		++*occupied;
		return 1;   // Return 1 page fault
	}
	else if (find_page(request, occupied) == -1)  // If request does not exist in memory
	{
		printf("%i\n",request);     // Print page fault
		if (*occupied < capacity)   // If there is space for a new entry
		{
			table[*occupied] = request; // Insert at next available entry
			priority_table[*occupied] = *time;  // Set time to current time
			++*time;    // Increment occupancy and time
			++*occupied;
			return 1;   // Return 1 page fault
		}
		else    // If no room for page fault
		{
			int i, min_idx = 0, min = priority_table[0];    // Find entry with lowest priority
			for(i = 1; i < *occupied; i++)
			{
				if(priority_table[i] < min)
				{
					min = priority_table[i];
					min_idx = i;
				}
			}
			table[min_idx] = request;   // Replace lowest priority page with the request
			priority_table[min_idx] = *time;  // Set time to current time
			++*time;     // Increment time
			return 1;   // Return 1 page fault
		}
	}
	else    // If request exists in memory
	{
		priority_table[find_page(request, occupied)] = *time; // Assign entry's time to current time
		++*time;    // Increment time
		return 0;   // Return 0 page faults
	}
}

int second_chance (int request, int *occupied)  // Second chance function
{
	if (*occupied == 0) // If table empty
	{
		printf("%i\n",request); // Print page fault
		table[0] = request; // Insert at front of table
		ref_table[0] = 0;   // Mark as not referenced
		++*occupied;    // Increment occupancy
		return 1;   // Return 1 page fault
	}
	else if (find_page(request, occupied) == -1)  // If request does not exist in memory
	{
		printf("%i\n", request);    // Print page fault
		if (*occupied < capacity)   // If there is room for a new entry
		{
			table[*occupied] = request; // Insert request at first available entry
			ref_table[*occupied] = 0;   // Mark as not referenced
			++*occupied;    // Increment occupancy
			return 1;   // Return 1 page fault
		}
		else    // If there is no room for a new entry
		{
			while (ref_table[0] != 0)  // Rotate through table until first entry has a 0 reference bit, clearing 1 entries to 0
			{
				ref_table[0] = 0;
				rotate_right(2,occupied);
			}
			table[0] = request;	// Replace entry with a 0 reference bit with the request
			ref_table[0] = 0;
			rotate_right(2,occupied);	// Rotate entry to the back
                	return 1;   // Return 1 page fault
		}
	}
	else    // If request exists in memory
	{
		ref_table[find_page(request, occupied)] = 1;  // Mark as referenced
		return 0;   // Return 0 page fault
	}
}

int main(int argc, char *argv[]){

	occupied = 0, time = 0; // Initialize occupancy and time
	capacity = atoi(argv[2]);   // Capacity is 3rd argument in stdin

	char *alg = argv[1];    // Algorithm is 2nd argument in stdin
	
	int request, num_requests = 0, total_faults = 0;    // Declare request variable, initialize count of requests and total page faults

	table = (int *) malloc(sizeof(int)*capacity);   // Allocate table to size of capaciry
	
	char *input = NULL;     // Pointer for input
	size_t inputAllocated = 0;
	ssize_t bytesRead;
	if (strncmp(alg,"FIFO",4) == 0)     // If FIFO specified --> no additional allocation
	{
		while((bytesRead = getline(&input, &inputAllocated, stdin)) != -1)
		{
                	request = atoi(input);  // Convert input to int
                	if(request == 0)    // Ignore 0 requests
                        	continue;
                	++num_requests; // Increment requests
			total_faults += fifo(request, &occupied);   // Add to total page faults
		}

	}
	else if (strncmp(alg,"SC",2) == 0)  // If second chance specified --> allocate reference table to size of capacity
	{
		ref_table = (int *) malloc (sizeof (int)* capacity);
		while((bytesRead = getline(&input, &inputAllocated, stdin)) != -1)
		{
			request = atoi(input);
			if(request == 0)
				continue;
			++num_requests;
			total_faults += second_chance(request, &occupied);
		}
	}
	else if (strncmp(alg,"LRU",3) == 0) // If LRU specified --> allocate priority table to size of capacity
        {
                priority_table = (int *) malloc (sizeof (int)* capacity);
                while((bytesRead = getline(&input, &inputAllocated, stdin)) != -1)
                {
                        request = atoi(input);
                        if(request == 0)
                                continue;
                        ++num_requests;
                        total_faults += lru(request, &occupied, &time);
                }
        }
	else
		fprintf(stderr,"ERROR IN MAIN\n");

	printf("Hit Rate = %f\n", (num_requests - total_faults)/(double)num_requests);  // Print the hit rate

	free(input);
	free(table);
	free(ref_table);
	free(priority_table);

	return 0;
	
}




