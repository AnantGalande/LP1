#include <iostream>
#include <vector>
using namespace std;

struct Block {
    int size;
    bool allocated;
};

struct Process {
    int size;
    int block_no;
};

// ---------- FIRST FIT ----------
void firstFit(vector<Block> blocks, vector<Process> processes) {
    cout << "\n--- FIRST FIT ---\n";
    for (int i = 0; i < processes.size(); i++) {
        for (int j = 0; j < blocks.size(); j++) {
            if (!blocks[j].allocated && blocks[j].size >= processes[i].size) {
                processes[i].block_no = j + 1;
                blocks[j].allocated = true;
                break;
            }
        }
    }

    cout << "Process\tSize\tBlock\n";
    for (int i = 0; i < processes.size(); i++) {
        if (processes[i].block_no != -1)
            cout << "P" << i + 1 << "\t" << processes[i].size << "\t" << processes[i].block_no << endl;
        else
            cout << "P" << i + 1 << "\t" << processes[i].size << "\tNot Allocated\n";
    }
}

// ---------- BEST FIT ----------
void bestFit(vector<Block> blocks, vector<Process> processes) {
    cout << "\n--- BEST FIT ---\n";
    for (int i = 0; i < processes.size(); i++) {
        int best_idx = -1;
        for (int j = 0; j < blocks.size(); j++) {
            if (!blocks[j].allocated && blocks[j].size >= processes[i].size) {
                if (best_idx == -1 || blocks[j].size < blocks[best_idx].size)
                    best_idx = j;
            }
        }
        if (best_idx != -1) {
            processes[i].block_no = best_idx + 1;
            blocks[best_idx].allocated = true;
        }
    }

    cout << "Process\tSize\tBlock\n";
    for (int i = 0; i < processes.size(); i++) {
        if (processes[i].block_no != -1)
            cout << "P" << i + 1 << "\t" << processes[i].size << "\t" << processes[i].block_no << endl;
        else
            cout << "P" << i + 1 << "\t" << processes[i].size << "\tNot Allocated\n";
    }
}

// ---------- WORST FIT ----------
void worstFit(vector<Block> blocks, vector<Process> processes) {
    cout << "\n--- WORST FIT ---\n";
    for (int i = 0; i < processes.size(); i++) {
        int worst_idx = -1;
        for (int j = 0; j < blocks.size(); j++) {
            if (!blocks[j].allocated && blocks[j].size >= processes[i].size) {
                // Choose the largest block
                if (worst_idx == -1 || blocks[j].size > blocks[worst_idx].size)
                    worst_idx = j;
            }
        }
        if (worst_idx != -1) {
            processes[i].block_no = worst_idx + 1;
            blocks[worst_idx].allocated = true;
        }
    }

    cout << "Process\tSize\tBlock\n";
    for (int i = 0; i < processes.size(); i++) {
        if (processes[i].block_no != -1)
            cout << "P" << i + 1 << "\t" << processes[i].size << "\t" << processes[i].block_no << endl;
        else
            cout << "P" << i + 1 << "\t" << processes[i].size << "\tNot Allocated\n";
    }
}

// ---------- NEXT FIT ----------
void nextFit(vector<Block> blocks, vector<Process> processes) {
    cout << "\n--- NEXT FIT ---\n";
    int last_alloc = 0;
    for (int i = 0; i < processes.size(); i++) {
        int j = last_alloc;
        bool done = false;
        int count = 0;
        while (count < blocks.size()) {
            if (!blocks[j].allocated && blocks[j].size >= processes[i].size) {
                processes[i].block_no = j + 1;
                blocks[j].allocated = true;
                last_alloc = j;
                done = true;
                break;
            }
            j = (j + 1) % blocks.size();
            count++;
        }
    }

    cout << "Process\tSize\tBlock\n";
    for (int i = 0; i < processes.size(); i++) {
        if (processes[i].block_no != -1)
            cout << "P" << i + 1 << "\t" << processes[i].size << "\t" << processes[i].block_no << endl;
        else
            cout << "P" << i + 1 << "\t" << processes[i].size << "\tNot Allocated\n";
    }
}

// ---------- MAIN ----------
int main() {
    int n, m;
    cout << "Enter number of memory blocks: ";
    cin >> n;

    vector<Block> blocks(n);
    cout << "Enter sizes of blocks:\n";
    for (int i = 0; i < n; i++) {
        cin >> blocks[i].size;
        blocks[i].allocated = false;
    }

    cout << "Enter number of processes: ";
    cin >> m;

    vector<Process> processes(m);
    cout << "Enter sizes of processes:\n";
    for (int i = 0; i < m; i++) {
        cin >> processes[i].size;
        processes[i].block_no = -1;
    }

    firstFit(blocks, processes);
    bestFit(blocks, processes);
    worstFit(blocks, processes);
    nextFit(blocks, processes);

    return 0;
}
