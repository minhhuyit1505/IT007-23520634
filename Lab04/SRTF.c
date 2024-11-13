#include <stdio.h>
#include <stdlib.h>

#define SORT_BY_ARRIVAL 0
#define SORT_BY_PID 1
#define SORT_BY_BURST 2

typedef struct {
    int iPID;
    int iArrival, iBurst;
    int iStart, iFinish, iWaiting, iResponse, iTaT;
} PCB;

int getInformation(PCB Q, int iCriteria) {
    switch (iCriteria) {
        case SORT_BY_ARRIVAL: return Q.iArrival;
        case SORT_BY_PID: return Q.iPID;
        case SORT_BY_BURST: return Q.iBurst;
        default: return -1;
    }
}

void inputProcess(int n, PCB P[]) {
    for (int i = 0; i < n; i++) {
        printf("Input PID of %d-th process: ", i + 1);
        scanf("%d", &P[i].iPID);
        printf("Input Arrival time of %d process: ", P[i].iPID);
        scanf("%d", &P[i].iArrival);
        printf("Input Burst time %d process: ", P[i].iPID);
        scanf("%d", &P[i].iBurst);
    }
}

void printProcess(int n, PCB P[]) {
    for (int i = 0; i < n; i++) {
        printf("===== P%d =====\n", P[i].iPID);
        printf("Start time: %d\n", P[i].iStart);
        printf("Finish time: %d\n", P[i].iFinish);
        printf("Waiting time: %d\n", P[i].iWaiting);
        printf("Response time: %d\n", P[i].iResponse);
        printf("Turnaround time: %d\n", P[i].iTaT);
        printf("\n");
    }
}

void exportGanttChart(int n, PCB P[]) {
    printf("Gantt Chart:\n ");
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < P[i].iBurst; j++) printf("--");
        printf(" ");
    }
    printf("\n|");

    for (int i = 0; i < n; i++) {
        printf(" P%d ", P[i].iPID);
        for (int j = 1; j < P[i].iBurst; j++) printf(" ");
        printf("|");
    }
    printf("\n ");

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < P[i].iBurst; j++) printf("--");
        printf(" ");
    }
    printf("\n0");

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < P[i].iBurst; j++) printf("  ");
        if (P[i].iFinish > 9) printf("\b");
        printf("%d", P[i].iFinish);
    }
    printf("\n");
}

void pushProcess(int *n, PCB P[], PCB Q) {
    P[(*n)++] = Q;
}

void removeProcess(int *n, int index, PCB P[]) {
    for (int i = index; i + 1 < (*n); i++) P[i] = P[i + 1];
    (*n)--;
}

void quickSort(PCB P[], int low, int high, int iCriteria) {
    if (low >= high) return;
    int pivot = low + (high - low) / 2;
    int L = low, R = high;

    while (L <= R) {
        while (getInformation(P[L], iCriteria) < getInformation(P[pivot], iCriteria)) L++;
        while (getInformation(P[R], iCriteria) > getInformation(P[pivot], iCriteria)) R--;
        if (L <= R) {
            PCB temp = P[L];
            P[L] = P[R];
            P[R] = temp;
            L++;
            R--;
        }
    }
    if (low < R) quickSort(P, low, R, iCriteria);
    if (L < high) quickSort(P, L, high, iCriteria);
}

void calculateAverage(int n, PCB P[], char* timeType) {
    int total = 0;
    printf("\n%s:\t", timeType);
    for (int i = 0; i < n; i++) {
        int time = (timeType[0] == 'W') ? P[i].iWaiting : (timeType[0] == 'R') ? P[i].iResponse : P[i].iTaT;
        printf("\t%d", time);
        total += time;
    }
    printf("\nAverage %s: %.2f\n", timeType, (float)total / n);
}

int main() {
    PCB Input[100];
    PCB Gantt[200];
    int iNumberOfProcess;
    printf("Please input number of Process: ");
    scanf("%d", &iNumberOfProcess);

    inputProcess(iNumberOfProcess, Input);

    int iRemain = iNumberOfProcess;
    int iTerminated = 0, iGantt = 0, curTime = 0;

    quickSort(Input, 0, iNumberOfProcess - 1, SORT_BY_ARRIVAL);

    while (iTerminated < iNumberOfProcess) {
        // Select the process with the shortest remaining time at curTime
        int shortest = -1;
        for (int i = 0; i < iRemain; i++) {
            if (Input[i].iArrival <= curTime && (shortest == -1 || Input[i].iBurst < Input[shortest].iBurst)) {
                shortest = i;
            }
        }

        if (shortest == -1) {
            curTime++;
            continue;
        }

        if (Input[shortest].iStart == -1) {
            Input[shortest].iStart = curTime;
            Input[shortest].iResponse = curTime - Input[shortest].iArrival;
        }

        PCB exec = Input[shortest];
        exec.iBurst = 1; // Execute 1 time unit for SRTF
        exec.iStart = curTime;
        exec.iFinish = ++curTime;
        pushProcess(&iGantt, Gantt, exec);

        Input[shortest].iBurst--;

        if (Input[shortest].iBurst == 0) {
            Input[shortest].iFinish = curTime;
            Input[shortest].iTaT = Input[shortest].iFinish - Input[shortest].iArrival;
            Input[shortest].iWaiting = Input[shortest].iTaT - (exec.iBurst);
            pushProcess(&iTerminated, Input, Input[shortest]);
            removeProcess(&iRemain, shortest, Input);
        }
    }

    printf("\n===== SRTF Scheduling =====\n");
    exportGanttChart(iGantt, Gantt);
    quickSort(Input, 0, iTerminated - 1, SORT_BY_PID);

    printProcess(iTerminated, Input);

    calculateAverage(iTerminated, Input, "Response time");
    calculateAverage(iTerminated, Input, "Waiting time");
    calculateAverage(iTerminated, Input, "Turnaround time");
    printf("\n");

    return 0;
}
