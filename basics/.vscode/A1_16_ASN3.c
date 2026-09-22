/*
***********************************************************************
* Name       : Rajdeep Das
* Roll       : 002411001063
* Email      : rajdeepdasyear2006@gmail.com
* Name       : Brihan Sahoo
* Roll       : 002411001041
* Email      : sbrihan763@gmail.com
* Team No.   : 06
* Assignment : OS Assignment - III
* Question   : Q1
* Date       : 08/09/2026
*
* Assignment Details:
* Develop a CPU Process Scheduling Simulator to implement and compare
* different CPU scheduling algorithms.
*
* The program implements the following scheduling algorithms:
*
* 1. First Come First Serve (FCFS)
* 2. Shortest Job First (SJF) - Non-Preemptive
* 3. Priority Scheduling - Non-Preemptive
* 4. Round Robin (RR)
* 5. Preemptive Shortest Remaining Time First (PSRTF)
* 6. Multi Level Feedback Queue (MLFQ)
*
* For every scheduling algorithm, the program generates:
* 1. A visual Gantt Chart.
* 2. Process-wise scheduling results.
* 3. Completion Time (CT).
* 4. Turnaround Time (TAT).
* 5. Waiting Time (WT).
* 6. Response Time (RT).
* 7. Average Waiting Time.
* 8. Average Turnaround Time.
* 9. Average Response Time.
*
* Priority Convention:
* A smaller priority number indicates a higher priority.
* Therefore, Priority 2 has a higher priority than Priority 9,
* and Priority 9 has a higher priority than Priority 10.
*
* Input Description:
* The process information is read from a CSV file named:
*
* Process_Scheduling_Data.csv
*
* The CSV file contains the following columns:
*
* PID, Arrival_Time, Burst_Time, Priority
*
* Sample Input Data:
* PID,Arrival_Time,Burst_Time,Priority
* P001,0,8,10
* P002,0,19,10
* P003,1,2,9
* P004,4,6,2
*
* Input Field Description:
*
* PID          : Unique Process ID.
* Arrival_Time : Time at which the process enters the ready queue.
* Burst_Time   : Total CPU time required by the process.
* Priority     : Priority assigned to the process. Smaller value
*                represents higher priority.
*
* Output Description:
* For each scheduling algorithm, the program displays:
* 1. A Gantt Chart representing the execution sequence.
* 2. A process result table.
* 3. Completion Time (CT).
* 4. Turnaround Time (TAT).
* 5. Waiting Time (WT).
* 6. Response Time (RT).
* 7. Average Waiting Time.
* 8. Average Turnaround Time.
* 9. Average Response Time.
*
* A separate comparison option is provided to compare the performance
* of all implemented scheduling algorithms side-by-side.
*
* Compilation Command:
* gcc -O2 -Wall -o scheduler A2_06_ASN-3.c
*
* Execution Sequence:
* ./scheduler Process_Scheduling_Data.csv
*
* CSV File:
* Process_Scheduling_Data.csv
*
***********************************************************************
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PROC_COUNT   300
#define GANTT_MAX_SIZE   20000
#define PID_MAX_LEN      10
#define FILE_CSV_DEFAULT "Process_Scheduling_Data.csv"
#define BOX_WIDTH        8
#define ITEMS_PER_LINE   6

/* ------------------------------------------------------------------ */
/*  Data structures                                                    */
/* ------------------------------------------------------------------ */

typedef struct {
    char process_id[PID_MAX_LEN];
    int  arr_time;
    int  cpu_burst;
    int  prio_val;

    int  time_left;
    int  start_ts;
    int  finish_ts;
    int  wait_duration;
    int  tat_duration;
    int  resp_duration;
} ProcData;

typedef struct {
    char process_id[PID_MAX_LEN];
    int  start_mark;
    int  end_mark;
} GanttBlock;

typedef struct {
    char algo_title[32];
    double meanWT, meanTAT, meanRT;
} PerformanceMetrics;

typedef struct {
    int buffer[MAX_PROC_COUNT + 5];
    int head, tail, size;
} CircularQueue;

static void init_queue(CircularQueue *q) {
    q->head = 0;
    q->tail = 0;
    q->size = 0;
}

static int is_queue_empty(CircularQueue *q) {
    return q->size == 0;
}

static void enqueue_back(CircularQueue *q, int val) {
    q->buffer[q->tail] = val;
    q->tail = (q->tail + 1) % (MAX_PROC_COUNT + 5);
    q->size++;
}

static void enqueue_front(CircularQueue *q, int val) {
    q->head = (q->head - 1 + (MAX_PROC_COUNT + 5)) % (MAX_PROC_COUNT + 5);
    q->buffer[q->head] = val;
    q->size++;
}

static int dequeue_front(CircularQueue *q) {
    int item = q->buffer[q->head];
    q->head = (q->head + 1) % (MAX_PROC_COUNT + 5);
    q->size--;
    return item;
}

/* ------------------------------------------------------------------ */
/*  Globals                                                            */
/* ------------------------------------------------------------------ */

ProcData global_proc_list[MAX_PROC_COUNT];
int      total_processes = 0;

GanttBlock gantt_chart[GANTT_MAX_SIZE];
int        gantt_size = 0;

PerformanceMetrics algo_metrics[8];
int                metrics_count = 0;

/* ------------------------------------------------------------------ */
/*  Utility functions                                                  */
/* ------------------------------------------------------------------ */

static void clear_gantt(void) {
    gantt_size = 0;
}

static void append_gantt(const char *pid, int start, int end) {
    if (gantt_size > 0) {
        GanttBlock *prev = &gantt_chart[gantt_size - 1];
        if (strcmp(prev->process_id, pid) == 0 && prev->end_mark == start) {
            prev->end_mark = end;
            return;
        }
    }

    if (gantt_size >= GANTT_MAX_SIZE) return;

    strncpy(gantt_chart[gantt_size].process_id, pid, PID_MAX_LEN - 1);
    gantt_chart[gantt_size].process_id[PID_MAX_LEN - 1] = '\0';
    gantt_chart[gantt_size].start_mark = start;
    gantt_chart[gantt_size].end_mark   = end;
    gantt_size++;
}

static void clone_process_data(ProcData *destination) {
    for (int idx = 0; idx < total_processes; idx++) {
        destination[idx] = global_proc_list[idx];
        destination[idx].time_left     = destination[idx].cpu_burst;
        destination[idx].start_ts      = -1;
        destination[idx].finish_ts     = 0;
        destination[idx].wait_duration = 0;
        destination[idx].tat_duration  = 0;
        destination[idx].resp_duration = 0;
    }
}

static void show_banner(const char *heading) {
    int length = (int)strlen(heading);
    int total_width = length + 8;
    printf("\n");
    for (int idx = 0; idx < total_width; idx++) printf("=");
    printf("\n====  %s  ====\n", heading);
    for (int idx = 0; idx < total_width; idx++) printf("=");
    printf("\n\n");
}

/* ------------------------------------------------------------------ */
/*  CSV Loader                                                         */
/* ------------------------------------------------------------------ */

static int parse_csv(const char *filepath) {
    FILE *file_ptr = fopen(filepath, "r");
    if (!file_ptr) {
        printf("  [ERROR] Unable to read CSV file: %s\n", filepath);
        return 0;
    }

    char buffer[512];
    int is_header = 1;
    total_processes = 0;

    while (fgets(buffer, sizeof(buffer), file_ptr)) {
        char *ptr = buffer;
        while (*ptr) {
            if (*ptr == '\r' || *ptr == '\n') {
                *ptr = '\0';
                break;
            }
            ptr++;
        }

        if (strlen(buffer) == 0) continue;

        if (is_header) {
            is_header = 0;
            if (strstr(buffer, "PID") != NULL) continue;
        }

        if (total_processes >= MAX_PROC_COUNT) break;

        char temp_pid[PID_MAX_LEN];
        int  arr, burst, prio;

        if (sscanf(buffer, "%9[^,],%d,%d,%d", temp_pid, &arr, &burst, &prio) == 4) {
            strncpy(global_proc_list[total_processes].process_id, temp_pid, PID_MAX_LEN - 1);
            global_proc_list[total_processes].process_id[PID_MAX_LEN - 1] = '\0';
            global_proc_list[total_processes].arr_time  = arr;
            global_proc_list[total_processes].cpu_burst    = burst;
            global_proc_list[total_processes].prio_val = prio;
            total_processes++;
        }
    }
    fclose(file_ptr);
    return 1;
}

/* ------------------------------------------------------------------ */
/*  Gantt Chart printer                                                */
/* ------------------------------------------------------------------ */

static void render_gantt(void) {
    printf("\n  GANTT CHART\n");
    printf("  -----------\n");

    for (int idx = 0; idx < gantt_size; idx += ITEMS_PER_LINE) {
        int bound = (idx + ITEMS_PER_LINE < gantt_size) ? idx + ITEMS_PER_LINE : gantt_size;

        printf("      ");
        for (int jdx = idx; jdx < bound; jdx++) {
            printf("+");
            for (int kdx = 0; kdx < BOX_WIDTH; kdx++) printf("-");
        }
        printf("+\n      ");

        for (int jdx = idx; jdx < bound; jdx++) {
            int str_len = (int)strlen(gantt_chart[jdx].process_id);
            int pad_left = (BOX_WIDTH - str_len) / 2;
            int pad_right = BOX_WIDTH - str_len - pad_left;

            if (pad_left < 0) pad_left = 0;
            if (pad_right < 0) pad_right = 0;

            printf("|");
            for (int kdx = 0; kdx < pad_left; kdx++) printf(" ");
            printf("%s", gantt_chart[jdx].process_id);
            for (int kdx = 0; kdx < pad_right; kdx++) printf(" ");
        }
        printf("|\n      ");

        for (int jdx = idx; jdx < bound; jdx++) {
            printf("+");
            for (int kdx = 0; kdx < BOX_WIDTH; kdx++) printf("-");
        }
        printf("+\n");

        printf("%6d", gantt_chart[idx].start_mark);
        for (int jdx = idx; jdx < bound; jdx++) {
            printf("%*d", BOX_WIDTH + 1, gantt_chart[jdx].end_mark);
        }
        printf("\n\n");
    }
}

/* ------------------------------------------------------------------ */
/*  Result table + averages                                            */
/* ------------------------------------------------------------------ */

static void render_results(ProcData *procs, int limit, const char *algo_label) {
    double sum_wt = 0, sum_tat = 0, sum_rt = 0;

    printf("  PER-PROCESS RESULT TABLE\n");
    printf("  -------------------------\n");
    printf("  +----------+-----+-----+----------+-----+-----+-----+-----+\n");
    printf("  |   PID    |  AT |  BT | Priority |  CT | TAT |  WT |  RT |\n");
    printf("  +----------+-----+-----+----------+-----+-----+-----+-----+\n");

    for (int idx = 0; idx < limit; idx++) {
        printf("  | %-8s | %3d | %3d |   %3d    | %3d | %3d | %3d | %3d |\n",
               procs[idx].process_id, procs[idx].arr_time, procs[idx].cpu_burst, procs[idx].prio_val,
               procs[idx].finish_ts, procs[idx].tat_duration, procs[idx].wait_duration, procs[idx].resp_duration);
        sum_wt  += procs[idx].wait_duration;
        sum_tat += procs[idx].tat_duration;
        sum_rt  += procs[idx].resp_duration;
    }
    printf("  +----------+-----+-----+----------+-----+-----+-----+-----+\n\n");

    double meanWT  = sum_wt / limit;
    double meanTAT = sum_tat / limit;
    double meanRT  = sum_rt / limit;

    printf("  SUMMARY  ( %s )\n", algo_label);
    printf("  -------------------------------------------------\n");
    printf("  Average Waiting Time     : %8.3f units\n", meanWT);
    printf("  Average Turnaround Time  : %8.3f units\n", meanTAT);
    printf("  Average Response Time    : %8.3f units\n", meanRT);
    printf("  -------------------------------------------------\n");

    if (metrics_count < 8) {
        strncpy(algo_metrics[metrics_count].algo_title, algo_label, 31);
        algo_metrics[metrics_count].algo_title[31] = '\0';
        algo_metrics[metrics_count].meanWT  = meanWT;
        algo_metrics[metrics_count].meanTAT = meanTAT;
        algo_metrics[metrics_count].meanRT  = meanRT;
        metrics_count++;
    }
}

/* ------------------------------------------------------------------ */
/*  Sorting helper                                                     */
/* ------------------------------------------------------------------ */

static void arrange_by_arrival(ProcData *procs, int limit) {
    for (int idx = 1; idx < limit; idx++) {
        ProcData temp = procs[idx];
        int ptr = idx - 1;
        while (ptr >= 0 && procs[ptr].arr_time > temp.arr_time) {
            procs[ptr + 1] = procs[ptr];
            ptr--;
        }
        procs[ptr + 1] = temp;
    }
}

/* ==================================================================== */
/*  I. FCFS                                                              */
/* ==================================================================== */

static void execute_fcfs(void) {
    ProcData procs[MAX_PROC_COUNT];
    clone_process_data(procs);
    arrange_by_arrival(procs, total_processes);
    clear_gantt();
    show_banner("FIRST COME FIRST SERVE (FCFS)");

    int current_time = 0;

    for (int idx = 0; idx < total_processes; idx++) {
        if (current_time < procs[idx].arr_time) {
            append_gantt("IDLE", current_time, procs[idx].arr_time);
            current_time = procs[idx].arr_time;
        }

        procs[idx].start_ts = current_time;
        procs[idx].resp_duration = procs[idx].start_ts - procs[idx].arr_time;
        procs[idx].finish_ts = current_time + procs[idx].cpu_burst;
        procs[idx].tat_duration = procs[idx].finish_ts - procs[idx].arr_time;
        procs[idx].wait_duration = procs[idx].tat_duration - procs[idx].cpu_burst;

        append_gantt(procs[idx].process_id, current_time, procs[idx].finish_ts);
        current_time = procs[idx].finish_ts;
    }

    render_gantt();
    render_results(procs, total_processes, "FCFS");
}

/* ==================================================================== */
/*  II. SJF (Non-Preemptive)                                             */
/* ==================================================================== */

static void execute_sjf(void) {
    ProcData procs[MAX_PROC_COUNT];
    clone_process_data(procs);
    clear_gantt();
    show_banner("SHORTEST JOB FIRST -- Non Preemptive (SJF)");

    int finished[MAX_PROC_COUNT] = {0};
    int procs_done = 0;
    int current_time = 0;

    while (procs_done < total_processes) {
        int selected = -1;

        for (int idx = 0; idx < total_processes; idx++) {
            if (!finished[idx] && procs[idx].arr_time <= current_time) {
                if (selected == -1 ||
                    procs[idx].cpu_burst < procs[selected].cpu_burst ||
                    (procs[idx].cpu_burst == procs[selected].cpu_burst && procs[idx].arr_time < procs[selected].arr_time)) {
                    selected = idx;
                }
            }
        }

        if (selected == -1) {
            int next_arr = -1;
            for (int idx = 0; idx < total_processes; idx++) {
                if (!finished[idx] && (next_arr == -1 || procs[idx].arr_time < next_arr)) {
                    next_arr = procs[idx].arr_time;
                }
            }
            append_gantt("IDLE", current_time, next_arr);
            current_time = next_arr;
            continue;
        }

        procs[selected].start_ts = current_time;
        procs[selected].resp_duration = procs[selected].start_ts - procs[selected].arr_time;
        procs[selected].finish_ts = current_time + procs[selected].cpu_burst;
        procs[selected].tat_duration = procs[selected].finish_ts - procs[selected].arr_time;
        procs[selected].wait_duration = procs[selected].tat_duration - procs[selected].cpu_burst;

        append_gantt(procs[selected].process_id, current_time, procs[selected].finish_ts);
        current_time = procs[selected].finish_ts;
        finished[selected] = 1;
        procs_done++;
    }

    render_gantt();
    render_results(procs, total_processes, "SJF (Non-Preemptive)");
}

/* ==================================================================== */
/*  III. Priority (Non-Preemptive)                                      */
/* ==================================================================== */

static void execute_priority(void) {
    ProcData procs[MAX_PROC_COUNT];
    clone_process_data(procs);
    clear_gantt();
    show_banner("PRIORITY SCHEDULING -- Non Preemptive");
    printf("  (Convention: SMALLER priority number = HIGHER priority)\n");

    int finished[MAX_PROC_COUNT] = {0};
    int procs_done = 0;
    int current_time = 0;

    while (procs_done < total_processes) {
        int selected = -1;
        for (int idx = 0; idx < total_processes; idx++) {
            if (!finished[idx] && procs[idx].arr_time <= current_time) {
                if (selected == -1 || procs[idx].prio_val < procs[selected].prio_val ||
                    (procs[idx].prio_val == procs[selected].prio_val && procs[idx].arr_time < procs[selected].arr_time)) {
                    selected = idx;
                }
            }
        }

        if (selected == -1) {
            int next_arr = -1;
            for (int idx = 0; idx < total_processes; idx++) {
                if (!finished[idx] && (next_arr == -1 || procs[idx].arr_time < next_arr)) {
                    next_arr = procs[idx].arr_time;
                }
            }
            append_gantt("IDLE", current_time, next_arr);
            current_time = next_arr;
            continue;
        }

        procs[selected].start_ts = current_time;
        procs[selected].resp_duration = procs[selected].start_ts - procs[selected].arr_time;
        procs[selected].finish_ts = current_time + procs[selected].cpu_burst;
        procs[selected].tat_duration = procs[selected].finish_ts - procs[selected].arr_time;
        procs[selected].wait_duration = procs[selected].tat_duration - procs[selected].cpu_burst;

        append_gantt(procs[selected].process_id, current_time, procs[selected].finish_ts);
        current_time = procs[selected].finish_ts;
        finished[selected] = 1;
        procs_done++;
    }

    render_gantt();
    render_results(procs, total_processes, "Priority (Non-Preemptive)");
}

/* ==================================================================== */
/*  IV. Round Robin                                                       */
/* ==================================================================== */

static void execute_round_robin(int quantum_val) {
    ProcData procs[MAX_PROC_COUNT];
    clone_process_data(procs);
    clear_gantt();
    show_banner("ROUND ROBIN (RR)");
    printf("  Time Quantum = %d\n", quantum_val);

    ProcData sorted_procs[MAX_PROC_COUNT];
    for (int idx = 0; idx < total_processes; idx++) {
        sorted_procs[idx] = procs[idx];
    }
    arrange_by_arrival(sorted_procs, total_processes);

    int map_idx[MAX_PROC_COUNT];
    for (int idx = 0; idx < total_processes; idx++) {
        for (int jdx = 0; jdx < total_processes; jdx++) {
            if (strcmp(sorted_procs[idx].process_id, procs[jdx].process_id) == 0) {
                map_idx[idx] = jdx;
                break;
            }
        }
    }

    CircularQueue ready_queue;
    init_queue(&ready_queue);

    int current_time = sorted_procs[0].arr_time;
    int next_in_line = 0;

    while (next_in_line < total_processes && sorted_procs[next_in_line].arr_time <= current_time) {
        enqueue_back(&ready_queue, map_idx[next_in_line]);
        next_in_line++;
    }

    int procs_done = 0;
    while (procs_done < total_processes) {
        if (is_queue_empty(&ready_queue)) {
            if (next_in_line < total_processes) {
                current_time = sorted_procs[next_in_line].arr_time;
                append_gantt("IDLE", current_time - 0, current_time);
                while (next_in_line < total_processes && sorted_procs[next_in_line].arr_time <= current_time) {
                    enqueue_back(&ready_queue, map_idx[next_in_line]);
                    next_in_line++;
                }
            } else {
                break;
            }
            continue;
        }

        int active_idx = dequeue_front(&ready_queue);
        if (procs[active_idx].start_ts == -1) {
            procs[active_idx].start_ts = current_time;
            procs[active_idx].resp_duration = procs[active_idx].start_ts - procs[active_idx].arr_time;
        }

        int run_time = (procs[active_idx].time_left < quantum_val) ? procs[active_idx].time_left : quantum_val;
        append_gantt(procs[active_idx].process_id, current_time, current_time + run_time);
        current_time += run_time;
        procs[active_idx].time_left -= run_time;

        while (next_in_line < total_processes && sorted_procs[next_in_line].arr_time <= current_time) {
            enqueue_back(&ready_queue, map_idx[next_in_line]);
            next_in_line++;
        }

        if (procs[active_idx].time_left > 0) {
            enqueue_back(&ready_queue, active_idx);
        } else {
            procs[active_idx].finish_ts = current_time;
            procs[active_idx].tat_duration = procs[active_idx].finish_ts - procs[active_idx].arr_time;
            procs[active_idx].wait_duration = procs[active_idx].tat_duration - procs[active_idx].cpu_burst;
            procs_done++;
        }
    }

    render_gantt();
    render_results(procs, total_processes, "Round Robin");
}

/* ==================================================================== */
/*  V. PSRTF                                                             */
/* ==================================================================== */

static void execute_psrtf(void) {
    ProcData procs[MAX_PROC_COUNT];
    clone_process_data(procs);
    clear_gantt();
    show_banner("PREEMPTIVE SHORTEST REMAINING TIME FIRST (PSRTF)");

    int max_arr = 0, sum_burst = 0;
    for (int idx = 0; idx < total_processes; idx++) {
        if (procs[idx].arr_time > max_arr) max_arr = procs[idx].arr_time;
        sum_burst += procs[idx].cpu_burst;
    }
    int max_time = max_arr + sum_burst + 5;
    int procs_done = 0;

    for (int t = 0; t < max_time && procs_done < total_processes; t++) {
        int selected = -1;
        for (int idx = 0; idx < total_processes; idx++) {
            if (procs[idx].arr_time <= t && procs[idx].time_left > 0) {
                if (selected == -1 || procs[idx].time_left < procs[selected].time_left ||
                    (procs[idx].time_left == procs[selected].time_left && procs[idx].arr_time < procs[selected].arr_time)) {
                    selected = idx;
                }
            }
        }

        if (selected == -1) {
            append_gantt("IDLE", t, t + 1);
            continue;
        }

        if (procs[selected].start_ts == -1) {
            procs[selected].start_ts = t;
            procs[selected].resp_duration = procs[selected].start_ts - procs[selected].arr_time;
        }

        append_gantt(procs[selected].process_id, t, t + 1);
        procs[selected].time_left--;

        if (procs[selected].time_left == 0) {
            procs[selected].finish_ts = t + 1;
            procs[selected].tat_duration = procs[selected].finish_ts - procs[selected].arr_time;
            procs[selected].wait_duration = procs[selected].tat_duration - procs[selected].cpu_burst;
            procs_done++;
        }
    }

    render_gantt();
    render_results(procs, total_processes, "PSRTF");
}

/* ==================================================================== */
/*  VI. Multi Level Feedback Queue (MLFQ)                               */
/* ==================================================================== */

static void execute_mlfq(void) {
    ProcData procs[MAX_PROC_COUNT];
    clone_process_data(procs);
    clear_gantt();
    show_banner("MULTI LEVEL FEEDBACK QUEUE (MLFQ)");
    printf("  Levels : Q0 (RR, quantum=4)  ->  Q1 (RR, quantum=8)  ->  Q2 (FCFS)\n");

    int q_limits[3] = {4, 8, 1 << 29};
    int proc_level[MAX_PROC_COUNT];
    for (int idx = 0; idx < total_processes; idx++) proc_level[idx] = 0;

    CircularQueue q0, q1, q2;
    init_queue(&q0); init_queue(&q1); init_queue(&q2);
    CircularQueue *queue_arr[3] = {&q0, &q1, &q2};

    int max_arr = 0, sum_burst = 0;
    for (int idx = 0; idx < total_processes; idx++) {
        if (procs[idx].arr_time > max_arr) max_arr = procs[idx].arr_time;
        sum_burst += procs[idx].cpu_burst;
    }
    int max_time = max_arr + sum_burst + 5;

    int in_system[MAX_PROC_COUNT] = {0};
    int active_idx = -1;
    int used_quantum = 0;
    int procs_done = 0;

    for (int t = 0; t < max_time && procs_done < total_processes; t++) {
        for (int idx = 0; idx < total_processes; idx++) {
            if (!in_system[idx] && procs[idx].arr_time <= t) {
                in_system[idx] = 1;
                proc_level[idx] = 0;
                enqueue_back(&q0, idx);
            }
        }

        if (active_idx != -1) {
            int current_q = proc_level[active_idx];
            int better_ready = -1;
            for (int lvl = 0; lvl < current_q; lvl++) {
                if (!is_queue_empty(The following refactored code modifies the data structures, variable names, function names, and bracing style to look distinctly different while preserving the original logic and keeping the initial 92-line comment block completely intact[cite: 1].

```c
/*
***********************************************************************
* Name       : Rajdeep Das
* Roll       : 002411001063
* Email      : rajdeepdasyear2006@gmail.com
* Name       : Brihan Sahoo
* Roll       : 002411001041
* Email      : sbrihanHere is the modified version of the scheduling simulator code with updated variable names, refactored data structures, and adjusted function signatures, while keeping the original 92-line block comment completely unchanged[cite: 1].

```c
/*
***********************************************************************
* Name       : Rajdeep Das
* Roll       : 002411001063
* Email      : rajdeepdasyear2006@gmail.com
* Name       : Brihan Sahoo
* Roll       : 002411001041
* Email      : sbrihan763@gmail.com
* Team No.   : 06
* Assignment : OS Assignment - III
* Question   : Q1
* Date       : 08/09/2026
*
* Assignment Details:
* Develop a CPU Process Scheduling Simulator to implement and compare
* different CPU scheduling algorithms.
*
* The program implements the following scheduling algorithms:
*
* 1. First Come First Serve (FCFS)
* 2. Shortest Job First (SJF) - Non-Preemptive
* 3. Priority Scheduling - Non-Preemptive
* 4. Round Robin (RR)
* 5. Preemptive Shortest Remaining Time First (PSRTF)
* 6. Multi Level Feedback Queue (MLFQ)
*
* For every scheduling algorithm, the program generates:
* 1. A visual Gantt Chart.
* 2. Process-wise scheduling results.
* 3. Completion Time (CT).
* 4. Turnaround Time (TAT).
* 5. Waiting Time (WT).
* 6. Response Time (RT).
* 7. Average Waiting Time.
* 8. Average Turnaround Time.
* 9. Average Response Time.
*
* Priority Convention:
* A smaller priority number indicates a higher priority.
* Therefore, Priority 2 has a higher priority than Priority 9,
* and Priority 9 has a higher priority than Priority 10.
*
* Input Description:
* The process information is read from a CSV file named:
*
* Process_Scheduling_Data.csv
*
* The CSV file contains the following columns:
*
* PID, Arrival_Time, Burst_Time, Priority
*
* Sample Input Data:
* PID,Arrival_Time,Burst_Time,Priority
* P001,0,8,10
* P002,0,19,10
* P003,1,2,9
* P004,4,6,2
*
* Input Field Description:
*
* PID          : Unique Process ID.
* Arrival_Time : Time at which the process enters the ready queue.
* Burst_Time   : Total CPU time required by the process.
* Priority     : Priority assigned to the process. Smaller value
*                represents higher priority.
*
* Output Description:
* For each scheduling algorithm, the program displays:
* 1. A Gantt Chart representing the execution sequence.
* 2. A process result table.
* 3. Completion Time (CT).
* 4. Turnaround Time (TAT).
* 5. Waiting Time (WT).
* 6. Response Time (RT).
* 7. Average Waiting Time.
* 8. Average Turnaround Time.
* 9. Average Response Time.
*
* A separate comparison option is provided to compare the performance
* of all implemented scheduling algorithms side-by-side.
*
* Compilation Command:
* gcc -O2 -Wall -o scheduler A2_06_ASN-3.c
*
* Execution Sequence:
* ./scheduler Process_Scheduling_Data.csv
*
* CSV File:
* Process_Scheduling_Data.csv
*
***********************************************************************
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PROCESSES  300
#define GANTT_MAX_SIZE 20000
#define MAX_PID_LEN    10
#define DEF_CSV_FILE   "Process_Scheduling_Data.csv"
#define BOX_W          8          
#define BOX_PER_ROW    6          

/* ------------------------------------------------------------------ */
/*  Refactored Data Structures                                        */
/* ------------------------------------------------------------------ */

typedef struct {
    char p_id[MAX_PID_LEN];
    int  arr_time;
    int  burst_time;
    int  prio;

    int  rem_time;
    int  start_time;
    int  comp_time;
    int  wait_time;
    int  turn_time;
    int  resp_time;
} ProcInfo;

typedef struct {
    char p_id[MAX_PID_LEN];
    int  start_val;
    int  end_val;
} GanttBlock;

typedef struct {
    char algo_name[32];
    double w_avg, t_avg, r_avg;
} SimResult;

typedef struct {
    int data[MAX_PROCESSES + 5];
    int head, tail, size;
} CircularQueue;

static void init_q(CircularQueue *q) {
    q->head = 0;
    q->tail = 0;
    q->size = 0;
}

static int is_q_empty(CircularQueue *q) {
    return (q->size == 0) ? 1 : 0;
}

static void enqueue_rear(CircularQueue *q, int val) {
    q->data[q->tail] = val;
    q->tail = (q->tail + 1) % (MAX_PROCESSES + 5);
    q->size++;
}

static void enqueue_front(CircularQueue *q, int val) {
    q->head = (q->head - 1 + (MAX_PROCESSES + 5)) % (MAX_PROCESSES + 5);
    q->data[q->head] = val;
    q->size++;
}

static int dequeue_front(CircularQueue *q) {
    int val = q->data[q->head];
    q->head = (q->head + 1) % (MAX_PROCESSES + 5);
    q->size--;
    return val;
}

/* ------------------------------------------------------------------ */
/*  Global Variables                                                  */
/* ------------------------------------------------------------------ */

ProcInfo original_procs[MAX_PROCESSES];
int total_procs = 0;

GanttBlock gantt_chart[GANTT_MAX_SIZE];
int gantt_count = 0;

SimResult exec_results[8];
int res_count = 0;

/* ------------------------------------------------------------------ */
/*  Utility Functions                                                 */
/* ------------------------------------------------------------------ */

static void clear_gantt_chart(void) {
    gantt_count = 0;
}

static void append_gantt_block(const char *p_id, int start, int end) {
    if (gantt_count > 0) {
        GanttBlock *prev = &gantt_chart[gantt_count - 1];
        if (strcmp(prev->p_id, p_id) == 0 && prev->end_val == start) {
            prev->end_val = end;
            return;
        }
    }

    if (gantt_count >= GANTT_MAX_SIZE) return;

    strncpy(gantt_chart[gantt_count].p_id, p_id, MAX_PID_LEN - 1);
    gantt_chart[gantt_count].p_id[MAX_PID_LEN - 1] = '\0';
    gantt_chart[gantt_count].start_val = start;
    gantt_chart[gantt_count].end_val   = end;

    gantt_count++;
}

static void clone_process_list(ProcInfo *dest) {
    for (int idx = 0; idx < total_procs; idx++) {
        dest[idx] = original_procs[idx];
        dest[idx].rem_time   = dest[idx].burst_time;
        dest[idx].start_time = -1;
        dest[idx].comp_time  = 0;
        dest[idx].wait_time  = 0;
        dest[idx].turn_time  = 0;
        dest[idx].resp_time  = 0;
    }
}

static void display_header(const char *header_text) {
    int str_len = (int)strlen(header_text);
    int total_w = str_len + 8;

    printf("\n");
    for (int k = 0; k < total_w; k++) printf("=");
    printf("\n====  %s  ====\n", header_text);
    for (int k = 0; k < total_w; k++) printf("=");
    printf("\n\n");
}

/* ------------------------------------------------------------------ */
/*  Data Loader                                                       */
/* ------------------------------------------------------------------ */

static int read_csv_file(const char *filepath) {
    FILE *file_ptr = fopen(filepath, "r");
    if (file_ptr == NULL) {
        printf("  [ERROR] Failed to open file: %s\n", filepath);
        return 0;
    }

    char buffer[512];
    int is_header = 1;
    total_procs = 0;

    while (fgets(buffer, sizeof(buffer), file_ptr)) {
        char *ptr = buffer;
        while (*ptr) {
            if (*ptr == '\r' || *ptr == '\n') {
                *ptr = '\0';
                break;
            }
            ptr++;
        }

        if (strlen(buffer) == 0) continue;

        if (is_header) {
            is_header = 0;
            if (strstr(buffer, "PID") != NULL) continue;
        }

        if (total_procs >= MAX_PROCESSES) break;

        char t_pid[MAX_PID_LEN];
        int t_arr, t_burst, t_prio;

        if (sscanf(buffer, "%9[^,],%d,%d,%d", t_pid, &t_arr, &t_burst, &t_prio) == 4) {
            strncpy(original_procs[total_procs].p_id, t_pid, MAX_PID_LEN - 1);
            original_procs[total_procs].p_id[MAX_PID_LEN - 1] = '\0';
            original_procs[total_procs].arr_time   = t_arr;
            original_procs[total_procs].burst_time = t_burst;
            original_procs[total_procs].prio       = t_prio;
            total_procs++;
        }
    }
    fclose(file_ptr);
    return 1;
}

/* ------------------------------------------------------------------ */
/*  Output Formatters                                                 */
/* ------------------------------------------------------------------ */

static void draw_gantt_chart(void) {
    printf("\n  GANTT CHART\n");
    printf("  -----------\n");

    for (int idx = 0; idx < gantt_count; idx += BOX_PER_ROW) {
        int boundary = (idx + BOX_PER_ROW < gantt_count) ? idx + BOX_PER_ROW : gantt_count;

        printf("      ");
        for (int k = idx; k < boundary; k++) {
            printf("+");
            for (int m = 0; m < BOX_W; m++) printf("-");
        }
        printf("+\n      ");

        for (int k = idx; k < boundary; k++) {
            int lbl_len = (int)strlen(gantt_chart[k].p_id);
            int l_pad = (BOX_W - lbl_len) / 2;
            int r_pad = BOX_W - lbl_len - l_pad;
            if (l_pad < 0) l_pad = 0;
            if (r_pad < 0) r_pad = 0;

            printf("|");
            for (int m = 0; m < l_pad; m++) printf(" ");
            printf("%s", gantt_chart[k].p_id);
            for (int m = 0; m < r_pad; m++) printf(" ");
        }
        printf("|\n      ");

        for (int k = idx; k < boundary; k++) {
            printf("+");
            for (int m = 0; m < BOX_W; m++) printf("-");
        }
        printf("+\n%6d", gantt_chart[idx].start_val);

        for (int k = idx; k < boundary; k++) {
            printf("%*d", BOX_W + 1, gantt_chart[k].end_val);
        }
        printf("\n\n");
    }
}

static void display_metrics_table(ProcInfo *plist, int count, const char *algo) {
    double sum_wt = 0, sum_tat = 0, sum_rt = 0;

    printf("  PER-PROCESS RESULT TABLE\n");
    printf("  -------------------------\n");
    printf("  +----------+-----+-----+----------+-----+-----+-----+-----+\n");
    printf("  |   PID    |  AT |  BT | Priority |  CT | TAT |  WT |  RT |\n");
    printf("  +----------+-----+-----+----------+-----+-----+-----+-----+\n");

    for (int idx = 0; idx < count; idx++) {
        printf("  | %-8s | %3d | %3d |   %3d    | %3d | %3d | %3d | %3d |\n",
               plist[idx].p_id, plist[idx].arr_time, plist[idx].burst_time,
               plist[idx].prio, plist[idx].comp_time, plist[idx].turn_time,
               plist[idx].wait_time, plist[idx].resp_time);

        sum_wt  += plist[idx].wait_time;
        sum_tat += plist[idx].turn_time;
        sum_rt  += plist[idx].resp_time;
    }
    printf("  +----------+-----+-----+----------+-----+-----+-----+-----+\n\n");

    double w_mean = sum_wt / count;
    double t_mean = sum_tat / count;
    double r_mean = sum_rt / count;

    printf("  SUMMARY  ( %s )\n", algo);
    printf("  -------------------------------------------------\n");
    printf("  Average Waiting Time     : %8.3f units\n", w_mean);
    printf("  Average Turnaround Time  : %8.3f units\n", t_mean);
    printf("  Average Response Time    : %8.3f units\n", r_mean);
    printf("  -------------------------------------------------\n");

    if (res_count < 8) {
        strncpy(exec_results[res_count].algo_name, algo, 31);
        exec_results[res_count].algo_name[31] = '\0';
        exec_results[res_count].w_avg = w_mean;
        exec_results[res_count].t_avg = t_mean;
        exec_results[res_count].r_avg = r_mean;
        res_count++;
    }
}

static void order_by_arrival_time(ProcInfo *plist, int count) {
    for (int step = 1; step < count; step++) {
        ProcInfo key = plist[step];
        int loc = step - 1;
        while (loc >= 0 && plist[loc].arr_time > key.arr_time) {
            plist[loc + 1] = plist[loc];
            loc--;
        }
        plist[loc + 1] = key;
    }
}

/* ==================================================================== */
/*  Scheduling Algorithms                                               */
/* ==================================================================== */

static void execute_fcfs(void) {
    ProcInfo plist[MAX_PROCESSES];
    clone_process_list(plist);
    order_by_arrival_time(plist, total_procs);
    clear_gantt_chart();

    display_header("FIRST COME FIRST SERVE (FCFS)");
    int clock_time = 0;

    for (int idx = 0; idx < total_procs; idx++) {
        if (clock_time < plist[idx].arr_time) {
            append_gantt_block("IDLE", clock_time, plist[idx].arr_time);
            clock_time = plist[idx].arr_time;
        }

        plist[idx].start_time = clock_time;
        plist[idx].resp_time  = plist[idx].start_time - plist[idx].arr_time;
        plist[idx].comp_time  = clock_time + plist[idx].burst_time;
        plist[idx].turn_time  = plist[idx].comp_time - plist[idx].arr_time;
        plist[idx].wait_time  = plist[idx].turn_time - plist[idx].burst_time;

        append_gantt_block(plist[idx].p_id, clock_time, plist[idx].comp_time);
        clock_time = plist[idx].comp_time;
    }
    draw_gantt_chart();
    display_metrics_table(plist, total_procs, "FCFS");
}

static void execute_sjf(void) {
    ProcInfo plist[MAX_PROCESSES];
    clone_process_list(plist);
    clear_gantt_chart();

    display_header("SHORTEST JOB FIRST -- Non Preemptive (SJF)");

    int is_finished[MAX_PROCESSES] = {0};
    int procs_done = 0, clock_time = 0;

    while (procs_done < total_procs) {
        int target = -1;
        for (int idx = 0; idx < total_procs; idx++) {
            if (!is_finished[idx] && plist[idx].arr_time <= clock_time) {
                if (target == -1 || 
                    plist[idx].burst_time < plist[target].burst_time ||
                    (plist[idx].burst_time == plist[target].burst_time && plist[idx].arr_time < plist[target].arr_time)) {
                    target = idx;
                }
            }
        }

        if (target == -1) {
            int next_arr = -1;
            for (int idx = 0; idx < total_procs; idx++) {
                if (!is_finished[idx] && (next_arr == -1 || plist[idx].arr_time < next_arr)) {
                    next_arr = plist[idx].arr_time;
                }
            }
            append_gantt_block("IDLE", clock_time, next_arr);
            clock_time = next_arr;
            continue;
        }

        plist[target].start_time = clock_time;
        plist[target].resp_time  = plist[target].start_time - plist[target].arr_time;
        plist[target].comp_time  = clock_time + plist[target].burst_time;
        plist[target].turn_time  = plist[target].comp_time - plist[target].arr_time;
        plist[target].wait_time  = plist[target].turn_time - plist[target].burst_time;

        append_gantt_block(plist[target].p_id, clock_time, plist[target].comp_time);
        clock_time = plist[target].comp_time;
        is_finished[target] = 1;
        procs_done++;
    }
    draw_gantt_chart();
    display_metrics_table(plist, total_procs, "SJF (Non-Preemptive)");
}

static void execute_priority(void) {
    ProcInfo plist[MAX_PROCESSES];
    clone_process_list(plist);
    clear_gantt_chart();

    display_header("PRIORITY SCHEDULING -- Non Preemptive");
    printf("  (Convention: SMALLER priority number = HIGHER priority)\n");

    int is_finished[MAX_PROCESSES] = {0};
    int procs_done = 0, clock_time = 0;

    while (procs_done < total_procs) {
        int target = -1;
        for (int idx = 0; idx < total_procs; idx++) {
            if (!is_finished[idx] && plist[idx].arr_time <= clock_time) {
                if (target == -1 || 
                    plist[idx].prio < plist[target].prio ||
                    (plist[idx].prio == plist[target].prio && plist[idx].arr_time < plist[target].arr_time)) {
                    target = idx;
                }
            }
        }

        if (target == -1) {
            int next_arr = -1;
            for (int idx = 0; idx < total_procs; idx++) {
                if (!is_finished[idx] && (next_arr == -1 || plist[idx].arr_time < next_arr)) {
                    next_arr = plist[idx].arr_time;
                }
            }
            append_gantt_block("IDLE", clock_time, next_arr);
            clock_time = next_arr;
            continue;
        }

        plist[target].start_time = clock_time;
        plist[target].resp_time  = plist[target].start_time - plist[target].arr_time;
        plist[target].comp_time  = clock_time + plist[target].burst_time;
        plist[target].turn_time  = plist[target].comp_time - plist[target].arr_time;
        plist[target].wait_time  = plist[target].turn_time - plist[target].burst_time;

        append_gantt_block(plist[target].p_id, clock_time, plist[target].comp_time);
        clock_time = plist[target].comp_time;
        is_finished[target] = 1;
        procs_done++;
    }
    draw_gantt_chart();
    display_metrics_table(plist, total_procs, "Priority (Non-Preemptive)");
}

static void execute_round_robin(int qt) {
    ProcInfo plist[MAX_PROCESSES];
    clone_process_list(plist);
    clear_gantt_chart();

    display_header("ROUND ROBIN (RR)");
    printf("  Time Quantum = %d\n", qt);

    ProcInfo seq[MAX_PROCESSES];
    for (int idx = 0; idx < total_procs; idx++) seq[idx] = plist[idx];
    order_by_arrival_time(seq, total_procs);

    int map_idx[MAX_PROCESSES];
    for (int idx = 0; idx < total_procs; idx++) {
        for (int k = 0; k < total_procs; k++) {
            if (strcmp(seq[idx].p_id, plist[k].p_id) == 0) {
                map_idx[idx] = k;
                break;
            }
        }
    }

    CircularQueue q;
    init_q(&q);

    int clock_time = seq[0].arr_time;
    int ptr = 0;

    while (ptr < total_procs && seq[ptr].arr_time <= clock_time) {
        enqueue_rear(&q, map_idx[ptr]);
        ptr++;
    }

    int procs_done = 0;
    while (procs_done < total_procs) {
        if (is_q_empty(&q)) {
            if (ptr < total_procs) {
                clock_time = seq[ptr].arr_time;
                append_gantt_block("IDLE", clock_time - 0, clock_time);
                while (ptr < total_procs && seq[ptr].arr_time <= clock_time) {
                    enqueue_rear(&q, map_idx[ptr]);
                    ptr++;
                }
            } else break;
            continue;
        }

        int active = dequeue_front(&q);
        if (plist[active].start_time == -1) {
            plist[active].start_time = clock_time;
            plist[active].resp_time = plist[active].start_time - plist[active].arr_time;
        }

        int runtime = (plist[active].rem_time < qt) ? plist[active].rem_time : qt;
        append_gantt_block(plist[active].p_id, clock_time, clock_time + runtime);
        clock_time += runtime;
        plist[active].rem_time -= runtime;

        while (ptr < total_procs && seq[ptr].arr_time <= clock_time) {
            enqueue_rear(&q, map_idx[ptr]);
            ptr++;
        }

        if (plist[active].rem_time > 0) {
            enqueue_rear(&q, active);
        } else {
            plist[active].comp_time = clock_time;
            plist[active].turn_time = plist[active].comp_time - plist[active].arr_time;
            plist[active].wait_time = plist[active].turn_time - plist[active].burst_time;
            procs_done++;
        }
    }
    draw_gantt_chart();
    display_metrics_table(plist, total_procs, "Round Robin");
}

static void execute_psrtf(void) {
    ProcInfo plist[MAX_PROCESSES];
    clone_process_list(plist);
    clear_gantt_chart();

    display_header("PREEMPTIVE SHORTEST REMAINING TIME FIRST (PSRTF)");

    int cap_time = 0, sum_b = 0;
    for (int idx = 0; idx < total_procs; idx++) {
        if (plist[idx].arr_time > cap_time) cap_time = plist[idx].arr_time;
        sum_b += plist[idx].burst_time;
    }
    int sim_limit = cap_time + sum_b + 5;
    int procs_done = 0;

    for (int tick = 0; tick < sim_limit && procs_done < total_procs; tick++) {
        int target = -1;
        for (int idx = 0; idx < total_procs; idx++) {
            if (plist[idx].arr_time <= tick && plist[idx].rem_time > 0) {
                if (target == -1 || 
                    plist[idx].rem_time < plist[target].rem_time ||
                    (plist[idx].rem_time == plist[target].rem_time && plist[idx].arr_time < plist[target].arr_time)) {
                    target = idx;
                }
            }
        }

        if (target == -1) {
            append_gantt_block("IDLE", tick, tick + 1);
            continue;
        }

        if (plist[target].start_time == -1) {
            plist[target].start_time = tick;
            plist[target].resp_time = plist[target].start_time - plist[target].arr_time;
        }

        append_gantt_block(plist[target].p_id, tick, tick + 1);
        plist[target].rem_time--;

        if (plist[target].rem_time == 0) {
            plist[target].comp_time = tick + 1;
            plist[target].turn_time = plist[target].comp_time - plist[target].arr_time;
            plist[target].wait_time = plist[target].turn_time - plist[target].burst_time;
            procs_done++;
        }
    }
    draw_gantt_chart();
    display_metrics_table(plist, total_procs, "PSRTF");
}

static void execute_mlfq(void) {
    ProcInfo plist[MAX_PROCESSES];
    clone_process_list(plist);
    clear_gantt_chart();

    display_header("MULTI LEVEL FEEDBACK QUEUE (MLFQ)");
    printf("  Levels : Q0 (RR, quantum=4)  ->  Q1 (RR, quantum=8)  ->  Q2 (FCFS)\n");

    int q_times[3] = {4, 8, 1 << 29};
    int p_lvl[MAX_PROCESSES] = {0};

    CircularQueue q0, q1, q2;
    init_q(&q0); init_q(&q1); init_q(&q2);
    CircularQueue *q_ptrs[3] = {&q0, &q1, &q2};

    int cap_time = 0, sum_b = 0;
    for (int idx = 0; idx < total_procs; idx++) {
        if (plist[idx].arr_time > cap_time) cap_time = plist[idx].arr_time;
        sum_b += plist[idx].burst_time;
    }
    int sim_limit = cap_time + sum_b + 5;

    int is_enqueued[MAX_PROCESSES] = {0};
    int running_proc = -1;
    int time_slice_used = 0, procs_done = 0;

    for (int tick = 0; tick < sim_limit && procs_done < total_procs; tick++) {
        for (int idx = 0; idx < total_procs; idx++) {
            if (!is_enqueued[idx] && plist[idx].arr_time <= tick) {
                is_enqueued[idx] = 1;
                p_lvl[idx] = 0;
                enqueue_rear(&q0, idx);
            }
        }

        if (running_proc != -1) {
            int current_lvl = p_lvl[running_proc];
            int override_lvl = -1;

            for (int lvl = 0; lvl < current_lvl; lvl++) {
                if (!is_q_empty(q_ptrs[lvl])) {
                    override_lvl = lvl;
                    break;
                }
            }

            if (override_lvl != -1) {
                enqueue_front(q_ptrs[current_lvl], running_proc);
                running_proc = -1;
                time_slice_used = 0;
            }
        }

        if (running_proc == -1) {
            for (int lvl = 0; lvl < 3; lvl++) {
                if (!is_q_empty(q_ptrs[lvl])) {
                    running_proc = dequeue_front(q_ptrs[lvl]);
                    break;
                }
            }
            time_slice_used = 0;
        }

        if (running_proc == -1) {
            append_gantt_block("IDLE", tick, tick + 1);
            continue;
        }

        if (plist[running_proc].start_time == -1) {
            plist[running_proc].start_time = tick;
            plist[running_proc].resp_time = plist[running_proc].start_time - plist[running_proc].arr_time;
        }

        append_gantt_block(plist[running_proc].p_id, tick, tick + 1);
        plist[running_proc].rem_time--;
        time_slice_used++;

        if (plist[running_proc].rem_time == 0) {
            plist[running_proc].comp_time = tick + 1;
            plist[running_proc].turn_time = plist[running_proc].comp_time - plist[running_proc].arr_time;
            plist[running_proc].wait_time = plist[running_proc].turn_time - plist[running_proc].burst_time;
            procs_done++;
            running_proc = -1;
            time_slice_used = 0;
        }
        else if (p_lvl[running_proc] < 2 && time_slice_used >= q_times[p_lvl[running_proc]]) {
            p_lvl[running_proc]++;
            enqueue_rear(q_ptrs[p_lvl[running_proc]], running_proc);
            running_proc = -1;
            time_slice_used = 0;
        }
    }
    draw_gantt_chart();
    display_metrics_table(plist, total_procs, "MLFQ");
}

/* ==================================================================== */
/*  Analysis                                                            */
/* ==================================================================== */

static void render_comparisons(void) {
    if (res_count == 0) {
        printf("\n  No algorithms have been run yet. Choose option 7 to run all of them.\n");
        return;
    }

    display_header("COMPARISON OF ALL SCHEDULING ALGORITHMS");
    printf("  +----------------------------+----------------+----------------+----------------+\n");
    printf("  | %-26s | %-14s | %-14s | %-14s |\n", "Algorithm", "Avg Waiting", "Avg Turnaround", "Avg Response");
    printf("  +----------------------------+----------------+----------------+----------------+\n");

    for (int idx = 0; idx < res_count; idx++) {
        printf("  | %-26s | %14.3f | %14.3f | %14.3f |\n", exec_results[idx].algo_name, exec_results[idx].w_avg, exec_results[idx].t_avg, exec_results[idx].r_avg);
    }
    printf("  +----------------------------+----------------+----------------+----------------+\n");

    int max_perf_idx = 0;
    for (int idx = 1; idx < res_count; idx++) {
        if (exec_results[idx].w_avg < exec_results[max_perf_idx].w_avg) max_perf_idx = idx;
    }

    printf("\n  >> Lowest Average Waiting Time : %s  (%.3f units)\n", exec_results[max_perf_idx].algo_name, exec_results[max_perf_idx].w_avg);
}

/* ==================================================================== */
/*  Main Execution                                                      */
/* ==================================================================== */

static void render_menu(void) {
    printf("\n  +---------------------------------------------------------+\n");
    printf("  |            CPU  PROCESS  SCHEDULING  SIMULATOR          |\n");
    printf("  +---------------------------------------------------------+\n");
    printf("  |  1. FCFS                                                |\n");
    printf("  |  2. SJF (Non-Preemptive)                                |\n");
    printf("  |  3. Priority Scheduling (Non-Preemptive)                |\n");
    printf("  |  4. Round Robin                                         |\n");
    printf("  |  5. PSRTF (Preemptive SJF)                              |\n");
    printf("  |  6. Multi Level Feedback Queue (MLFQ)                   |\n");
    printf("  |  7. Run ALL Algorithms + Comparison Table               |\n");
    printf("  |  8. Show Comparison Table (from algorithms already run) |\n");
    printf("  |  9. Reload CSV Data                                     |\n");
    printf("  |  0. Exit                                                |\n");
    printf("  +---------------------------------------------------------+\n");
    printf("  Loaded Processes : %d\n", total_procs);
    printf("  Enter your choice : ");
}

int main(int argc, char *argv[]) {
    const char *csv_src = (argc > 1) ? argv[1] : DEF_CSV_FILE;
    printf("Loading process data from: %s\n", csv_src);

    if (!read_csv_file(csv_src)) {
        printf("Program cannot continue without input data. Exiting.\n");
        return 1;
    }

    printf("Successfully loaded %d processes.\n", total_procs);
    int user_input, qt_val;

    while (1) {
        render_menu();
        if (scanf("%d", &user_input) != 1) {
            printf("Invalid input. Exiting.\n");
            break;
        }

        switch (user_input) {
            case 1: 
                execute_fcfs(); 
                break;
            case 2: 
                execute_sjf(); 
                break;
            case 3: 
                execute_priority(); 
                break;
            case 4:
                printf("  Enter Time Quantum for Round Robin : ");
                scanf("%d", &qt_val);
                if (qt_val <= 0) qt_val = 2;
                execute_round_robin(qt_val);
                break;
            case 5: 
                execute_psrtf(); 
                break;
            case 6: 
                execute_mlfq(); 
                break;
            case 7:
                res_count = 0;
                printf("  Enter Time Quantum to use for Round Robin : ");
                scanf("%d", &qt_val);
                if (qt_val <= 0) qt_val = 2;

                execute_fcfs();
                execute_sjf();
                execute_priority();
                execute_round_robin(qt_val);
                execute_psrtf();
                execute_mlfq();
                render_comparisons();
                break;
            case 8:
                render_comparisons();
                break;
            case 9:
                if (read_csv_file(csv_src)) {
                    printf("  Reloaded %d processes from %s\n", total_procs, csv_src);
                }
                break;
            case 0:
                printf("\n  Exiting Scheduler. Goodbye!\n\n");
                return 0;
            default:
                printf("  Invalid choice. Please try again.\n");
        }
    }
    return 0;
}