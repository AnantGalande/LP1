#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
using namespace std;

struct MNTEntry {
    string name;
    int mdtIndex, pp, kp, kpdtIndex;
};

int main() {
    ifstream fin("macro_input.txt");   // Input file (your screenshot code)
    ofstream mnt("mnt.txt");
    ofstream mdt("mdt.txt");
    ofstream kpdt("kpdt.txt");
    ofstream pntab("pntab.txt");

    vector<MNTEntry> MNT;
    vector<string> MDT;
    vector<pair<string, string>> KPDTAB;
    map<string, int> PNTAB;

    string line, macroName;
    bool isMacro = false;
    int mdtIndex = 1, kpdtIndex = 1;

    while (getline(fin, line)) {
        if (line == "MACRO") {
            isMacro = true;
            getline(fin, line);  // Get macro header line
            stringstream ss(line);
            ss >> macroName;

            string param;
            vector<string> params;
            int pp = 0, kp = 0;
            PNTAB.clear();

            // Split parameters by ','
            while (getline(ss, param, ',')) {
                param.erase(0, param.find_first_not_of(" \t"));
                param.erase(param.find_last_not_of(" \t") + 1);
                if (param.empty()) continue;

                if (param.find('=') != string::npos) {
                    size_t eq = param.find('=');
                    string pname = param.substr(0, eq);
                    string val = param.substr(eq + 1);
                    kp++;
                    PNTAB[pname] = PNTAB.size() + 1;
                    KPDTAB.push_back({pname, val});
                } else {
                    pp++;
                    PNTAB[param] = PNTAB.size() + 1;
                }
            }

            // Store MNT entry
            MNT.push_back({macroName, mdtIndex, pp, kp, kp == 0 ? 0 : kpdtIndex});
            kpdtIndex += kp;
            continue;
        }

        if (line == "MEND") {
            MDT.push_back("MEND");
            mdtIndex++;
            isMacro = false;
            continue;
        }

        if (isMacro) {
            // Replace parameters with (P,n)
            for (auto &p : PNTAB) {
                size_t pos = line.find(p.first);
                if (pos != string::npos)
                    line.replace(pos, p.first.size(), "(P," + to_string(p.second) + ")");
            }
            MDT.push_back(line);
            mdtIndex++;
        }
    }

    // Write MNT
    for (auto &m : MNT)
        mnt << m.name << " " << m.mdtIndex << " " << m.pp << " " << m.kp << " " << m.kpdtIndex << "\n";

    // Write MDT
    for (auto &d : MDT)
        mdt << d << "\n";

    // Write KPDTAB
    for (auto &k : KPDTAB)
        kpdt << k.first << " " << k.second << "\n";

    // Write PNTAB
    for (auto &p : PNTAB)
        pntab << p.first << " " << p.second << "\n";

    cout << "✅ Pass-I completed. Files generated: mnt.txt, mdt.txt, kpdt.txt, pntab.txt\n";
    return 0;
}
