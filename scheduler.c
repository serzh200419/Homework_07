#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int pid;
    int arrival_time;
    int burst_time;
    int start_time;
    int completion_time;
    int turnaround_time;
    int waiting_time;
    int response_time;
    int done;
} Process;

int cmp_arrival_pid(const void *a, const void *b) {
    Process *p = (Process*)a;
    Process *q = (Process*)b;
    if (p->arrival_time != q->arrival_time) return p->arrival_time - q->arrival_time;
    return p->pid - q->pid;
}

void print_table(Process procs[], int n) {
    printf("PID\tAT\tBT\tWT\tTAT\tRT\n");
    for (int i = 0; i < n; ++i) {
        printf("P%d\t%d\t%d\t%d\t%d\t%d\n",
               procs[i].pid,
               procs[i].arrival_time,
               procs[i].burst_time,
               procs[i].waiting_time,
               procs[i].turnaround_time,
               procs[i].response_time);
    }
}

void print_avg(Process procs[], int n) {
    double sum_wt = 0;
    double sum_tat = 0; 
    double sum_rt = 0;
    
    for (int i = 0; i < n; ++i) {
        sum_wt += procs[i].waiting_time;
        sum_tat += procs[i].turnaround_time;
        sum_rt += procs[i].response_time;
    }
    printf("Average Waite Time: %.2f\n", sum_wt / n);
    printf("Average Turnaround Time: %.2f\n", sum_tat / n);
    printf("Average Response Time: %.2f\n", sum_rt / n);
}

void print_gantt(int order[], int start[], int end[], int m) {
    printf("Gantt Chart:\n|");
    for (int i = 0; i < m; ++i) {
        printf("P%d|", order[i]);
    }
    printf("\n");
    for (int i = 0; i < m; ++i) {
        if (i == 0) printf("%d", start[i]);
        printf("%d", end[i]);
    }
    printf("\n");
}

void fcfs(Process procs[], int n) {
    printf("First Come First Served\n");
   
    qsort(procs, n, sizeof(Process), cmp_arrival_pid);

    int current_time = 0;
    int order[200], s[200], e[200], m = 0;

    for (int i = 0; i < n; ++i) {
        if (current_time < procs[i].arrival_time) {
            current_time = procs[i].arrival_time;
        }
        procs[i].start_time = current_time;
        procs[i].response_time = procs[i].start_time - procs[i].arrival_time;
        procs[i].waiting_time = procs[i].start_time - procs[i].arrival_time;
        procs[i].completion_time = procs[i].start_time + procs[i].burst_time;
        procs[i].turnaround_time = procs[i].completion_time - procs[i].arrival_time;
        current_time = procs[i].completion_time;

        order[m] = procs[i].pid;
        s[m] = procs[i].start_time;
        e[m] = procs[i].completion_time;
        m++;
    }

    print_gantt(order, s, e, m);
    print_table(procs, n);
    print_avg(procs, n);
    printf("\n");
}

void sjf(Process procs[], int n) {
    printf("Shortest Job First (non-preemptive)\n");
    
    for (int i = 0; i < n; ++i) {
        procs[i].done = 0;
        procs[i].start_time = -1;
    }
    int completed = 0;
    int current_time = 0;
    int order[200], s[200], e[200], m = 0;

    int min_arr = procs[0].arrival_time;
    for (int i = 1; i < n; ++i) if (procs[i].arrival_time < min_arr) min_arr = procs[i].arrival_time;
    current_time = min_arr;

    while (completed < n) {
        int idx = -1;
        int min_burst = 1<<30;
        for (int i = 0; i < n; ++i) {
            if (!procs[i].done && procs[i].arrival_time <= current_time) {
                if (procs[i].burst_time < min_burst) {
                    min_burst = procs[i].burst_time;
                    idx = i;
                } else if (procs[i].burst_time == min_burst) {
                    if (procs[i].arrival_time < procs[idx].arrival_time ||
                        (procs[i].arrival_time == procs[idx].arrival_time && procs[i].pid < procs[idx].pid)) {
                        idx = i;
                    }
                }
            }
        }
        if (idx == -1) {
            int next_arr = 1<<30;
            for (int i = 0; i < n; ++i)
                if (!procs[i].done && procs[i].arrival_time < next_arr)
                    next_arr = procs[i].arrival_time;
            current_time = next_arr;
            continue;
        }

        procs[idx].start_time = current_time;
        procs[idx].response_time = procs[idx].start_time - procs[idx].arrival_time;
        procs[idx].waiting_time = procs[idx].start_time - procs[idx].arrival_time;
        procs[idx].completion_time = procs[idx].start_time + procs[idx].burst_time;
        procs[idx].turnaround_time = procs[idx].completion_time - procs[idx].arrival_time;

        current_time = procs[idx].completion_time;
        procs[idx].done = 1;
        completed++;

        order[m] = procs[idx].pid;
        s[m] = procs[idx].start_time;
        e[m] = procs[idx].completion_time;
        m++;
    }

    print_gantt(order, s, e, m);
    print_table(procs, n);
    print_avg(procs, n);
    printf("\n");
}

int main() {
    int n;
    printf("Enter the number of processes: ");
    if (scanf("%d", &n) != 1 || n <= 0) {
        printf("Invalid number of processes.\n");
        return 1;
    }

    Process *arr = malloc(sizeof(Process) * n);
    Process *arr_copy = malloc(sizeof(Process) * n);

    for (int i = 0; i < n; ++i) {
        arr[i].pid = i + 1;
        arr[i].turnaround_time = arr[i].waiting_time = arr[i].response_time = 0;
        arr[i].start_time = arr[i].completion_time = -1;
        arr[i].done = 0;
        printf("Enter the arrival time and burst time for process %d: ", i+1);
        if (scanf("%d %d", &arr[i].arrival_time, &arr[i].burst_time) != 2) {
            printf("Invalid input.\n");
            free(arr);
            free(arr_copy);
            return 1;
        }
    }

    memcpy(arr_copy, arr, sizeof(Process) * n);

    Process *fcfs_procs = malloc(sizeof(Process) * n);
    Process *sjf_procs = malloc(sizeof(Process) * n);
    memcpy(fcfs_procs, arr, sizeof(Process) * n);
    memcpy(sjf_procs, arr, sizeof(Process) * n);

    fcfs(fcfs_procs, n);

    sjf(sjf_procs, n);

    free(arr);
    free(arr_copy);
    free(fcfs_procs);
    free(sjf_procs);
    return 0;
}

