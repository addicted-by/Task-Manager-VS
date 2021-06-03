#pragma once
#include <vector>
#include <string>
#include "Sotrudnik.h"

using namespace std;

double norm(vector<double> FirstGuy, vector<double> SecondGuy);
vector<vector<double>> Finding_Centr(double R, vector<Sotrudnik> employees, vector<double> centr);
double find_r(vector<Sotrudnik> employees);
vector<double> normalize(vector<double> abilities, double xMin, double xMax);
vector<double> UnNormalize(vector<double> abilities, double xMin, double xMax);
vector<vector<Sotrudnik>> forel(vector<Sotrudnik>& employees, double xMin, double xMax);
int find_closer_centr(vector<double> task);
Sotrudnik* find_best(vector<double> task, vector<vector<Sotrudnik>>& space);
void Fill_True();
Sotrudnik give_task(vector<double> task, vector<vector<Sotrudnik>>& space);
vector<double> min_max(vector<Sotrudnik> space);
string make_magic(int rowsSkills, int skillsCount, int rowsTasks, string data);