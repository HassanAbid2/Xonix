#pragma once
// MinHeap.h
#pragma once
#include <cstring> // For memcpy

// At the top of MinHeap.h
struct PlayerEntry {
    char nickname[50];
    int score;
};


class MinHeap {
private:
    PlayerEntry entries[10]; // Store both nickname and score
    int size;

    int parent(int index) { return (index - 1) / 2; }
    int leftChild(int index) { return 2 * index + 1; }
    int rightChild(int index) { return 2 * index + 2; }

    void heapifyUp(int index) {
        while (index > 0 && entries[parent(index)].score > entries[index].score) {
            PlayerEntry temp = entries[index];
            entries[index] = entries[parent(index)];
            entries[parent(index)] = temp;
            index = parent(index);
        }
    }

    void heapifyDown(PlayerEntry* arr, int heapSize, int index) {
        int smallest = index;
        int left = 2 * index + 1;
        int right = 2 * index + 2;

        if (left < heapSize && arr[left].score < arr[smallest].score)
            smallest = left;
        if (right < heapSize && arr[right].score < arr[smallest].score)
            smallest = right;

        if (smallest != index) {
            PlayerEntry temp = arr[index];
            arr[index] = arr[smallest];
            arr[smallest] = temp;

            heapifyDown(arr, heapSize, smallest);
        }
    }



public:
    MinHeap() : size(0) {}

    void insert(const char* nickname, int score) {
        if (size < 10) {
            strncpy_s(entries[size].nickname, nickname, sizeof(entries[size].nickname));
            entries[size].nickname[sizeof(entries[size].nickname) - 1] = '\0';
            entries[size].score = score;
            size++;
            heapifyUp(size - 1);
        }
        else if (score > entries[0].score) {
            strncpy_s(entries[0].nickname, nickname, sizeof(entries[0].nickname));
            entries[0].nickname[sizeof(entries[0].nickname) - 1] = '\0';
            entries[0].score = score;
            heapifyDown(entries, size, 0);
        }
    }

    void getTopEntries(PlayerEntry* topEntries, int& count) {
        // Step 1: Copy current heap entries
        for (int i = 0; i < size; ++i) {
            topEntries[i] = entries[i];
        }
        count = size;

        // Step 2: Sort copied entries in descending order using bubble sort
        for (int i = 0; i < count - 1; ++i) {
            for (int j = 0; j < count - i - 1; ++j) {
                if (topEntries[j].score < topEntries[j + 1].score) {
                    // Swap
                    PlayerEntry temp = topEntries[j];
                    topEntries[j] = topEntries[j + 1];
                    topEntries[j + 1] = temp;
                }
            }
        }
    }
};