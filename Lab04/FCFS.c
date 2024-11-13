#include <stdio.h>
#include <stdlib.h>

#define SORT_BY_ARRIVAL 0
#define SORT_BY_PID 1
#define SORT_BY_BURST 2
#define SORT_BY_START 3

typedef struct {
    int iPID;
    int iArrival, iBurst;
    int iStart, iFinish, iWaiting, iResponse, iTaT;
} PCB;

int getInformation(PCB Q, int iCriteria) {
    switch(iCriteria) {
        case SORT_BY_ARRIVAL: return Q.iArrival;
        case SORT_BY_PID: return Q.iPID;
        case SORT_BY_BURST: return Q.iBurst;
        case SORT_BY_START: return Q.iStart;
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

int main() {
    PCB Input[100], ReadyQueue[100], TerminatedArray[100];
    int iNumberOfProcess;

    printf("Please input number of Process: ");
    scanf("%d", &iNumberOfProcess);

    inputProcess(iNumberOfProcess, Input);
    quickSort(Input, 0, iNumberOfProcess - 1, SORT_BY_ARRIVAL);

    ReadyQueue[0] = Input[0];
    ReadyQueue[0].iStart = ReadyQueue[0].iArrival;
    ReadyQueue[0].iFinish = ReadyQueue[0].iStart + ReadyQueue[0].iBurst;
    ReadyQueue[0].iResponse = ReadyQueue[0].iStart - ReadyQueue[0].iArrival;
    ReadyQueue[0].iWaiting = ReadyQueue[0].iResponse;
    ReadyQueue[0].iTaT = ReadyQueue[0].iFinish - ReadyQueue[0].iArrival;

    int iTerminated = 1, iReady = 1;

    for (int i = 1; i < iNumberOfProcess; i++) {
        if (Input[i].iArrival <= ReadyQueue[iReady - 1].iFinish) {
            ReadyQueue[iReady++] = Input[i];
        }
    }

    exportGanttChart(iReady, ReadyQueue);
    quickSort(ReadyQueue, 0, iReady - 1, SORT_BY_PID);

    printProcess(iReady, ReadyQueue);

    calculateAverage(iReady, ReadyQueue, "Response time");
    calculateAverage(iReady, ReadyQueue, "Waiting time");
    calculateAverage(iReady, ReadyQueue, "Turnaround time");

    return 0;
}
