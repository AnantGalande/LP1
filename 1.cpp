#include <iostream>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>
using namespace std;

class Pass1
{
    // ------------------ Data Members ------------------
    int litindex = 0;
    int LC = -1;  // Location Counter

    map<string, pair<int, int>> SymTab;     // Symbol Table
    map<string, pair<string, string>> opcode; // Opcode Table
    vector<pair<string, int>> LitTab;       // Literal Table
    vector<string> pooltab;                 // Pool Table

    ifstream fin;                           // Input File
    ofstream fout, sym, lit, pool;          // Output Files

public:
    // ------------------ Functions ------------------
    void initialize()
    {
        opcode = {
            // ---------- Imperative Statements ----------
            {"STOP",  {"IS", "00"}},
            {"ADD",   {"IS", "01"}},
            {"SUB",   {"IS", "02"}},
            {"MULT",  {"IS", "03"}},
            {"MOVER", {"IS", "04"}},
            {"MOVEM", {"IS", "05"}},
            {"COMP",  {"IS", "06"}},
            {"BC",    {"IS", "07"}},
            {"DIV",   {"IS", "08"}},
            {"READ",  {"IS", "09"}},
            {"PRINT", {"IS", "10"}},

            // ---------- Assembler Directives ----------
            {"START",  {"AD", "01"}},
            {"END",    {"AD", "02"}},
            {"ORIGIN", {"AD", "03"}},
            {"EQU",    {"AD", "04"}},
            {"LTORG",  {"AD", "05"}},

            // ---------- Declarative Statements ----------
            {"DC", {"DL", "01"}},
            {"DS", {"DL", "02"}},

            // ---------- Registers ----------
            {"AREG", {"1", ""}},
            {"BREG", {"2", ""}},
            {"CREG", {"3", ""}},
            {"DREG", {"4", ""}},

            // ---------- Condition Codes ----------
            {"LT",  {"1", ""}},
            {"LE",  {"2", ""}},
            {"EQ",  {"3", ""}},
            {"GT",  {"4", ""}},
            {"GE",  {"5", ""}},
            {"ANY", {"6", ""}}
        };
    }

    // ------------------ Tokenize and Process Input ------------------
    void Tokenize()
    {
        fout.open("output.txt");
        fin.open("input.txt");

        string line, word, label;

        while (getline(fin, line))
        {
            stringstream ss(line);
            ss >> word;

            // -------- Label Handling --------
            if (opcode.count(word) == 0)
            {
                if (SymTab.count(word) == 0)
                    SymTab[word] = {LC, SymTab.size() + 1};
                else
                    SymTab[word].first = LC;

                label = word;
                ss >> word;
            }

            string operation = word;

            // -------- START --------
            if (operation == "START")
            {
                ss >> word;
                LC = stoi(word);
                fout << "    (" << opcode[operation].first << "," << opcode[operation].second << ")";
            }

            // -------- END --------
            else if (operation == "END")
            {
                fout << "\n    (" << opcode[operation].first << "," << opcode[operation].second << ")";
                pooltab.push_back("#" + to_string(litindex + 1));

                // Assign literals
                while (litindex < LitTab.size())
                {
                    LitTab[litindex].second = LC;
                    fout << "\n" << LC << " (DL,01) (C," << LitTab[litindex].first.substr(2, 1) << ")";
                    litindex++;
                    LC++;
                }
                return;
            }

            // -------- DC --------
            else if (operation == "DC")
            {
                ss >> word;
                fout << "\n" << LC << " (" << opcode[operation].first << "," << opcode[operation].second
                     << ") (C," << word << ")";
            }

            // -------- DS --------
            else if (operation == "DS")
            {
                ss >> word;
                fout << "\n" << LC << " (" << opcode[operation].first << "," << opcode[operation].second
                     << ") (C," << word << ")";
                LC += stoi(word);
            }

            // -------- ORIGIN --------
            else if (operation == "ORIGIN")
            {
                ss >> word;
                fout << endl << "    (" << opcode[operation].first << "," << opcode[operation].second << ") " << word;

                int ind = -1;
                char op;
                for (int i = 0; i < word.length(); i++)
                {
                    if (word[i] == '+' || word[i] == '-')
                    {
                        op = word[i];
                        ind = i;
                        break;
                    }
                }

                string beforeind = word.substr(0, ind);
                string afterind = ind != -1 ? word.substr(ind + 1) : "";

                LC = SymTab[beforeind].first;
                if (afterind != "")
                    LC += (op == '+' ? stoi(afterind) : -stoi(afterind));
            }

            // -------- EQU --------
            else if (operation == "EQU")
            {
                ss >> word;
                int ind = -1;
                char op;

                for (int i = 0; i < word.length(); i++)
                {
                    if (word[i] == '+' || word[i] == '-')
                    {
                        op = word[i];
                        ind = i;
                        break;
                    }
                }

                string beforeind = word.substr(0, ind);
                string afterind = ind != -1 ? word.substr(ind + 1) : "";
                int temp = SymTab[beforeind].first;

                if (afterind != "")
                    temp += (op == '+' ? stoi(afterind) : -stoi(afterind));

                SymTab[label].first = temp;
            }

            // -------- LTORG --------
            else if (operation == "LTORG")
            {
                pooltab.push_back("#" + to_string(litindex + 1));

                while (litindex < LitTab.size())
                {
                    LitTab[litindex].second = LC;
                    fout << "\n" << LC << " (DL,01) (C," << LitTab[litindex].first.substr(2, 1) << ")";
                    LC++;
                    litindex++;
                }
            }

            // -------- Imperative Statements --------
            else
            {
                if (opcode.count(word) > 0)
                {
                    fout << endl << LC << " (" << opcode[word].first << "," << opcode[word].second << ")";

                    while (ss >> word)
                    {
                        // Register / Condition Code
                        if (opcode.count(word) > 0)
                        {
                            fout << " (" << opcode[word].first << "," << opcode[word].second << ")";
                        }

                        // Literal
                        else if (word.find("=") != string::npos)
                        {
                            LitTab.push_back({word, LC});
                            fout << " (L," << LitTab.size() << ")";
                        }

                        // Symbol
                        else
                        {
                            if (SymTab.count(word) == 0)
                                SymTab[word] = {LC, SymTab.size() + 1};

                            fout << " (S," << SymTab[word].second << ")";
                        }
                    }
                }
                LC++;
            }
        }
    }

    // ------------------ Symbol Table ------------------
    void readSymTab()
    {
        sym.open("symboltab.txt");
        for (auto it : SymTab)
            sym << "\n" << it.second.second << " " << it.first << " " << it.second.first;
    }

    // ------------------ Literal Table ------------------
    void displayLitTab()
    {
        lit.open("literaltab.txt");
        for (auto it : LitTab)
            lit << "\n" << it.first << " " << it.second;
    }

    // ------------------ Pool Table ------------------
    void displayPoolTab()
    {
        pool.open("pooltab.txt");
        for (int i = 0; i < pooltab.size(); i++)
            pool << "\n" << pooltab[i];
    }
};

// ------------------ Main Function ------------------
int main()
{
    Pass1 p1;

    p1.initialize();
    p1.Tokenize();
    p1.readSymTab();
    p1.displayLitTab();
    p1.displayPoolTab();

    cout << "Pass 1 of two-pass assembler done successfully!";

    return 0;
}
