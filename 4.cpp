#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <vector>
#include <string>
using namespace std;

struct MNTEntry {
    string name;
    int mdtIndex, pp, kp, kpdtIndex;
};

struct KPDTEntry {
    string param, value;
};

int main() {
    ifstream mntf("mnt.txt"), mdtf("mdt.txt"), kpdtf("kpdt.txt"), inter("intermediate.txt");
    ofstream outf("output1.txt");

    if (!mntf || !mdtf || !kpdtf || !inter) {
        cout << " Error: One or more input files missing!\n";
        return 1;
    }

    vector<MNTEntry> MNT;
    vector<string> MDT;
    vector<KPDTEntry> KPDTAB;

    // -------- Load MNT --------
    MNTEntry m;
    while (mntf >> m.name >> m.mdtIndex >> m.pp >> m.kp >> m.kpdtIndex)
        MNT.push_back(m);

    // -------- Load MDT --------
    string line;
    while (getline(mdtf, line)) {
        if (!line.empty()) MDT.push_back(line);
    }

    // -------- Load KPDTAB --------
    KPDTEntry k;
    while (kpdtf >> k.param >> k.value)
        KPDTAB.push_back(k);

    // -------- Process Intermediate Code --------
    while (getline(inter, line)) {
        if (line.empty()) continue;
        stringstream ss(line);
        string word;
        ss >> word;

        bool isMacro = false;
        int macroIndex = -1;

        // Check if macro is being called
        for (int i = 0; i < MNT.size(); i++) {
            if (word == MNT[i].name) {
                isMacro = true;
                macroIndex = i;
                break;
            }
        }

        if (!isMacro) {
            outf << line << "\n";
            continue;
        }

        // ---------------- MACRO EXPANSION ----------------
        MNTEntry curr = MNT[macroIndex];
        vector<string> actualParams;

        // Split actual parameters
        string param;
        while (getline(ss, param, ',')) {
            param.erase(0, param.find_first_not_of(" \t"));
            param.erase(param.find_last_not_of(" \t") + 1);
            if (!param.empty()) actualParams.push_back(param);
        }

        // Create Actual Parameter Table (APTAB)
        map<int, string> APTAB;
        int kpIndex = curr.kpdtIndex - 1;

        // Fill keyword params with defaults
        for (int i = 0; i < curr.kp; i++) {
            APTAB[curr.pp + i + 1] = KPDTAB[kpIndex + i].value;
        }

        // Fill positional parameters
        for (int i = 0; i < actualParams.size(); i++) {
            APTAB[i + 1] = actualParams[i];
        }

        // Expand macro from MDT
        int mdti = curr.mdtIndex;
        while (mdti < MDT.size() && MDT[mdti] != "MEND") {
            string expanded = MDT[mdti];

            // Replace (P,n) with actual value
            size_t pos = expanded.find("(P,");
            while (pos != string::npos) {
                size_t start = pos + 3;
                size_t end = expanded.find(')', start);
                int num = stoi(expanded.substr(start, end - start));
                string value = APTAB[num];
                expanded.replace(pos, end - pos + 1, value);
                pos = expanded.find("(P,", pos + 1);
            }

            outf << expanded << "\n";
            mdti++;
        }
    }

    cout << " Pass-II Macro Expansion Completed.\nOutput generated in output1.txt\n";
    return 0;
}
