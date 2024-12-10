#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>
#include <sys/types.h>

typedef struct Process { 
    int pid; // Process ID 
    int bt;  // Burst Time 
    int art; // Arrival Time 
    int wt;  // Waiting time
    int tat; // Turnaround time
    int pri; // Priority
} ProcessType;

#define MAX_PROCESS 100

// Function prototypes
void findWaitingTimeFCFS(ProcessType plist[], int n);
void findTurnAroundTime(ProcessType plist[], int n);
void findWaitingTimeSJF(ProcessType plist[], int n);
void findWaitingTimeRR(ProcessType plist[], int n, int quantum);
void findavgTimePriority(ProcessType plist[], int n);
void printMetrics(ProcessType plist[], int n);
ProcessType* initProc(char *filename, int *n);

// Function to find waiting time for all processes using FCFS
void findWaitingTimeFCFS(ProcessType plist[], int n) { 
    plist[0].wt = 0; // Waiting time for first process is 0

    for (int i = 1; i < n; i++) {
        plist[i].wt = plist[i - 1].bt + plist[i - 1].wt;
    }
}

// Function to calculate turnaround time
void findTurnAroundTime(ProcessType plist[], int n) { 
    for (int i = 0; i < n; i++) {
        plist[i].tat = plist[i].bt + plist[i].wt;
    }
}

// Function to find waiting time for all processes using SJF
void findWaitingTimeSJF(ProcessType plist[], int n) { 
    int rem_bt[n];
    for (int i = 0; i < n; i++)
        rem_bt[i] = plist[i].bt;

    int t = 0;
    int completed = 0;

    while (completed != n) {
        int minm = INT_MAX;
        int shortest = -1;

        for (int i = 0; i < n; i++) {
            if (plist[i].art <= t && rem_bt[i] < minm && rem_bt[i] > 0) {
                minm = rem_bt[i];
                shortest = i;
            }
        }

        if (shortest == -1) {
            t++; // No process is ready, increment time
            continue;
        }

        rem_bt[shortest]--;

        if (rem_bt[shortest] == 0) {
            completed++;
            plist[shortest].wt = t + 1 - plist[shortest].bt - plist[shortest].art;
            t++;
        } else {
            t++;
        }
    }
}

// Function to find waiting time for all processes using Round Robin
void findWaitingTimeRR(ProcessType plist[], int n, int quantum) { 
    int rem_bt[n];
    for (int i = 0; i < n; i++)
        rem_bt[i] = plist[i].bt;

    int t = 0;
    while (1) {
        int done = 1;
        for (int i = 0; i < n; i++) {
            if (rem_bt[i] > 0) {
                done = 0; // There is a pending process

                if (rem_bt[i] > quantum) {
                    t += quantum;
                    rem_bt[i] -= quantum;
                } else {
                    t += rem_bt[i];
                    plist[i].wt = t - plist[i].bt - plist[i].art;
                    rem_bt[i] = 0;
                }
            }
        }
        if (done == 1) break; // All processes are done
    }
}

// Function to sort the processes according to priority
int my_comparer(const void *this, const void *that) { 
    const ProcessType *p1 = (const ProcessType *)this;
    const ProcessType *p2 = (const ProcessType *)that;

    return p1->pri - p2->pri;
}

// Function to calculate average time and print metrics for Priority Scheduling
void findavgTimePriority(ProcessType plist[], int n) { 
    qsort(plist, n, sizeof(ProcessType), my_comparer); // Sort by priority

    findWaitingTimeFCFS(plist, n); // Apply FCFS after sorting by priority
    findTurnAroundTime(plist, n);
  
    printf("\n*********\nPriority\n");
    printMetrics(plist, n);
}

// Function to print metrics
void printMetrics(ProcessType plist[], int n) {
    printf("\tProcesses\tBurst time\tWaiting time\tTurn around time\n"); 

    for (int i=0; i<n; i++) { 
        printf("\t%d\t\t%d\t\t%d\t\t%d\n", 
               plist[i].pid, 
               plist[i].bt, 
               plist[i].wt, 
               plist[i].tat); 
    } 

    float awt = ((float)(plist[0].wt)) / n;
    float att = ((float)(plist[0].tat)) / n;

    printf("\nAverage waiting time = %.2f", awt); 
    printf("\nAverage turn around time = %.2f\n", att); 
}

ProcessType* initProc(char *filename, int *n) {
  	FILE *input_file = fopen(filename, "r");
	if (!input_file) {
		fprintf(stderr, "Error: Invalid filepath\n");
		exit(1);
	}

	ProcessType *plist = malloc(MAX_PROCESS * sizeof(ProcessType));
	*n = 0;

	while(fscanf(input_file, "%d %d %d %d", &plist[*n].pid,
		&plist[*n].bt,
		&plist[*n].art,
		&plist[*n].pri) != EOF && *n < MAX_PROCESS) {
		(*n)++;
	}

	fclose(input_file);
	return plist;
}

// Driver code 
int main(int argc, char *argv[]) { 
	int n; 
	int quantum = 2;

	if (argc < 2) {
		fprintf(stderr, "Usage: ./schedsim <input-file-path>\n");
		return 1;
	}
    
	// FCFS
	n = 0;
	ProcessType *proc_list = initProc(argv[1], &n);
	findWaitingTimeFCFS(proc_list, n);
	findTurnAroundTime(proc_list, n);
	printMetrics(proc_list, n);

	// SJF
	n = 0;
	proc_list = initProc(argv[1], &n);
	findWaitingTimeSJF(proc_list, n); 
	findTurnAroundTime(proc_list, n);
	printMetrics(proc_list, n);

	// Priority
	n = 0; 
	proc_list = initProc(argv[1], &n);
	findavgTimePriority(proc_list, n); 

	// RR
	n = 0;
	proc_list = initProc(argv[1], &n);
	findWaitingTimeRR(proc_list, n, quantum); 
	findTurnAroundTime(proc_list, n); 
	printMetrics(proc_list, n);

	free(proc_list); // Free allocated memory
	return 0; 
}