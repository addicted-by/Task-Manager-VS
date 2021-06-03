#include "Sotrudnik.h"

Sotrudnik::Sotrudnik(vector<double> abilities, int norm, int busy, bool entry, string name, string surname) {
    this->abilities = abilities;
    this->norm = norm;
    this->busy = busy;
    this->entry = entry;
    this->name = name;
    this->surname = surname;
    this->id = NULL;
}
Sotrudnik::Sotrudnik(int id, vector<double> abilities, int norm, int busy, bool entry) {
    this->abilities = abilities;
    this->norm = norm;
    this->busy = busy;
    this->entry = entry;
    this->name = " ";
    this->surname = " ";
    this->id = id;
}
double Sotrudnik::coefficient() {
    /*if (this->busy >= this->norm)
        return 2.0;*/
    return 1.0 + exp(static_cast<double>(this->busy) / static_cast<double>(this->norm) - 1);
}