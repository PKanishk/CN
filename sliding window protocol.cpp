#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

#define WINDOW_SIZE 4
#define FRAME_COUNT 10

typedef struct {
    int seqNum;
    bool ackReceived;
    char data[50];
} Frame;

void sendData(Frame *frames, int startIdx, int endIdx) {
    for (int i = startIdx; i <= endIdx; i++) {
        printf("Sending frame %d: %s\n", frames[i].seqNum, frames[i].data);
        frames[i].ackReceived = false;
    }
}

void receiveAck(Frame *frames, int ackNum) {
    frames[ackNum].ackReceived = true;
    printf("Received ACK for frame %d\n", ackNum);
}

int main() {
    Frame frames[FRAME_COUNT];
    for (int i = 0; i < FRAME_COUNT; i++) {
        frames[i].seqNum = i;
        frames[i].ackReceived = false;
        sprintf(frames[i].data, "Frame data %d", i);
    }

    int base = 0;
    int nextSeqNum = 0;

    while (base < FRAME_COUNT) {
        while (nextSeqNum < FRAME_COUNT && nextSeqNum < base + WINDOW_SIZE) {
            sendData(frames, nextSeqNum, base + WINDOW_SIZE - 1);
            nextSeqNum++;
        }

        // Simulate network delay
        usleep(2000);

        for (int i = base; i < nextSeqNum; i++) {
            if (frames[i].ackReceived) {
                base = i + 1;
            }
        }
    }

    printf("All frames sent and acknowledged.\n");

    return 0;
}

