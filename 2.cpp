#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <string>
using namespace std;

int main()
{
    ifstream ic("output.txt");
    ifstream sym("symboltab.txt");
    ifstream lit("literaltab.txt");
    ofstream out("final_output.txt");

    if (!ic.is_open() || !sym.is_open() || !lit.is_open()) {
        cout << "Error: One or more input files not found (output.txt, symboltab.txt, literaltab.txt)\n";
        return 1;
    }

    map<int, int> SYMTAB;
    map<int, int> LITTAB;

    string line;

    // ---------------- Load Symbol Table ----------------
    while (getline(sym, line)) {
        if (line.empty() || line.find("Index") != string::npos) continue;
        stringstream ss(line);
        int index, addr;
        string symbol;
        ss >> index >> symbol >> addr;
        SYMTAB[index] = addr;
    }

    // ---------------- Load Literal Table ----------------
    int litIndex = 1;
    while (getline(lit, line)) {
        if (line.empty() || line.find("Literal") != string::npos) continue;
        stringstream ss(line);
        string value;
        int addr;
        ss >> value >> addr;
        LITTAB[litIndex++] = addr;
    }

    // ---------------- Generate Machine Code ----------------
    while (getline(ic, line)) {
        if (line.empty()) continue;

        // Ignore Assembler Directives
        if (line.find("(AD") != string::npos) continue;

        // Handle Declarative Statements (DL)
        if (line.find("(DL") != string::npos) {
            size_t cpos = line.find("(C,");
            if (cpos != string::npos) {
                string val = line.substr(cpos + 3, line.size() - cpos - 4);
                out << "00 0 " << val << "\n"; // Constants have no opcode
            }
            continue;
        }

        // Example line: 200 (IS,04) (1) (S,1)
        stringstream ss(line);
        string lc, opcodePart, regPart, memPart;
        ss >> lc >> opcodePart >> regPart >> memPart;

        string opcode = "00";
        if (opcodePart.find("(IS,") != string::npos)
            opcode = opcodePart.substr(4, 2);

        string reg = "0";
        if (regPart.find("(") != string::npos)
            reg = regPart.substr(1, 1);

        string mem = "000";
        if (memPart.find("(S,") != string::npos) {
            int idx = stoi(memPart.substr(3, memPart.size() - 4));
            mem = to_string(SYMTAB[idx]);
        } else if (memPart.find("(L,") != string::npos) {
            int idx = stoi(memPart.substr(3, memPart.size() - 4));
            mem = to_string(LITTAB[idx]);
        } else if (memPart.find("(C,") != string::npos) {
            mem = memPart.substr(3, memPart.size() - 4);
        }

        if (!opcodePart.empty() && opcodePart.find("(IS") != string::npos)
            out << lc << "  " << opcode << "  " << reg << "  " << mem << "\n";
    }

    cout << "✅ Pass-2 completed successfully.\n";
    cout << "Generated file: final_output.txt\n";
    return 0;
}