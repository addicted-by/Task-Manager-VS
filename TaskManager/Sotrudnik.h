#pragma once
#include <vector>
#include <string>
using namespace std;
class Sotrudnik
{
public:
    string name;
    string surname;
    int id;
    vector<double> abilities;
    Sotrudnik(vector<double> abilities, int norm, int busy, bool entry, string name, string surname);
    Sotrudnik(int id, vector<double> abilities, int norm, int busy, bool entry);
    double coefficient();
    void busyer();
    int norm;
    int busy;
    int pizdec;
    bool entry;
};