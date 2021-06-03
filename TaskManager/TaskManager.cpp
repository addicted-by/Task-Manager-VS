#include <iostream>
#include <string>
#include <fstream>
#include <math.h>
#include <list>
#include <vector>
#include "Sotrudnik.h"
#include <ctime>
#include <iomanip>
#include "TaskManager.h"

using namespace std;

vector<vector<double>> centrs;
vector<bool> entry_centr;
vector<double> min_distances;
vector<vector<int>> min_distances_inds = vector<vector<int>>();
vector<double> min_max_values;

//Euclidean norm
double norm(vector<double> FirstGuy, vector<double> SecondGuy) {
    /*double sum = 0;
    for (int i = 0; i < static_cast<int>(FirstGuy.size()); i++) {
        sum += (FirstGuy[i] - SecondGuy[i]) * (FirstGuy[i] - SecondGuy[i]);
    }
    return sqrt(sum);*/
    double max = abs(FirstGuy[0] - SecondGuy[0]);
    for (int i = 1; i < FirstGuy.size(); i++)
        if (max <= abs(FirstGuy[i] - SecondGuy[i]))
        {
            max = abs(FirstGuy[i] - SecondGuy[i]);
        }
    return max;
}

//finding center of mass
vector<vector<double>> Finding_Centr(double R, vector<Sotrudnik> employees, vector<double> centr) {
    vector<vector<double>> answer;
    vector<double> tmp;
    vector<double> sum = {};
    for (int i = 0; i < employees[0].abilities.size(); i++)
        sum.push_back(0);
    for (int i = 0; i < static_cast<int>(employees.size()); i++) {
        //pushing guys in sphere
        if (norm(centr, employees[i].abilities) <= R) {
            tmp.push_back(i);
            for (int j = 0; j < static_cast<int>(employees[i].abilities.size()); j++) {
                //sum of abilities
                sum[j] += employees[i].abilities[j]; //сумма по всем координатам для всех сотрудников на расстоянии до R
            }
        }
    }
    //finding centr
    for (int i = 0; i < static_cast<int>(sum.size()); i++) {
        sum[i] /= tmp.size();
        centr[i] = sum[i];
    }
    answer.push_back(centr);
    answer.push_back(tmp);
    return answer;
}

//finding minimal radius of sphere that contain all employees
double find_r(vector<Sotrudnik> employees) {
    vector<double> center_postition = employees[0].abilities;
    double max = 0;
    for (int i = 1; i < static_cast<int>(employees.size()); i++)
        for (int j = 0; j < static_cast<int>(employees[i].abilities.size()); j++)
            if (norm(center_postition, employees[i].abilities) > max)
                max = norm(center_postition, employees[i].abilities);
    return max;
}

//normalize dots
vector<double> normalize(vector<double> abilities, double xMin, double xMax) {
    for (int i = 0; i < static_cast<int>(abilities.size()); i++)
        abilities[i] = (abilities[i] - xMin) / (xMax - xMin);
    return abilities;
}

vector<double> UnNormalize(vector<double> abilities, double xMin, double xMax) {
    for (int i = 0; i < static_cast<int>(abilities.size()); i++)
        abilities[i] = abilities[i] * (xMax - xMin) + xMin;
    return abilities;
}

//Forel
vector<vector<Sotrudnik>> forel(vector<Sotrudnik> &employees, double xMin, double xMax) {
    for (int i = 0; i < static_cast<int>(employees.size()); i++)
        employees[i].abilities = normalize(employees[i].abilities, xMin, xMax);
    vector<vector<Sotrudnik>> clusters = {};
    vector<double> centr = employees[0].abilities;
    vector<double> first_centr = centr;
    double Rconst = find_r(employees);
    double R;
    while (employees.size() != 0) {
        vector<double> centr = employees[0].abilities;
        R = Rconst;
        vector<vector<double>> tmp = Finding_Centr(R, employees, centr);
        //R /= 8;
        R /= 3;
        vector<double> previous_centr = tmp[0];
        //probably shit
        bool flag = true;
        while (flag) {
            previous_centr = centr;
            tmp = Finding_Centr(R, employees, centr);
            centr = tmp[0];
            flag = false;
            for (unsigned int i = 0; i < centr.size(); i++) {
                if (centr[i] != previous_centr[i]) {
                    flag = true;
                    break;
                }  
            } 
        }
        int i = tmp[1].size()-1;
        vector<Sotrudnik> cluster;
        centrs.push_back(tmp[0]);
        while (i >= 0) {
             cluster.push_back(employees[tmp[1][i]]);
             employees.erase(employees.begin() + tmp[1][i]);
             i--;
         }
        tmp.clear();
        clusters.push_back(cluster);
        cluster.clear();
    }
    return clusters;
}

int find_closer_centr(vector<double> task) {
    double min = 3*sqrt(2);
    int ind = 0;
    for (int i = 1; i < centrs.size(); ++i) {
        if (norm(task, centrs[i]) < min && entry_centr[i]) {
            min = norm(task, centrs[i]);
            ind = i;
        }
    }
    return ind;
}

//didnt overwrite
Sotrudnik *find_best(vector<double> task, vector<vector<Sotrudnik>> &space) {
    double min;
    int ind_centr = find_closer_centr(task);
    entry_centr[ind_centr] = space[ind_centr][0].entry;
    Sotrudnik *answer = &space[ind_centr][0];
    int ind = 0;
    if (!space.empty()) {
        min = norm(space[ind_centr][0].abilities, task) + space[ind_centr][0].coefficient();
        int tmp;
        for (int i = 1; i < space[ind_centr].size(); ++i) {
            if (norm(space[ind_centr][i].abilities, task) + space[ind_centr][i].coefficient() < min &&
                space[ind_centr][i].entry) {
                min = norm(space[ind_centr][i].abilities, task) + space[ind_centr][i].coefficient();
                answer = &space[ind_centr][i];
                ind = i;
                entry_centr[ind_centr] = true;
            }
        }
        
        if (min > sqrt(2) / 16 && (min_distances.size() < space.size())) {
            min_distances.push_back(min);
            min_distances_inds.push_back({ ind_centr, ind });
            entry_centr[ind_centr] = false;
        }
        if (min_distances.size() == space.size())
            for (int i = 0; i < min_distances.size(); i++)
                if (min_distances[i] < min) {
                    min = min_distances[i];
                    entry_centr[ind_centr] = true;
                    answer = &space[min_distances_inds[i][0]][min_distances_inds[i][1]];
                }
        if (!entry_centr[ind_centr])
            answer = find_best(task, space);
    }
    return answer;
}

void Fill_True() {
    entry_centr.clear();
    for (int i = 0; i < centrs.size(); ++i)
        entry_centr.push_back(true);
}

int itask = 0;

Sotrudnik give_task(vector<double> task, vector<vector<Sotrudnik>> &space)
{
    min_distances.clear();
    min_distances_inds.clear();
    Sotrudnik *answer = find_best(task, space);
    itask++;
    //for output
    cout << "Task " << itask << " for " << answer->id << endl;
    for (int j = 0; j < 32; j++)
        cout << "-";
    cout << endl;
    task = UnNormalize(task, min_max_values[0], min_max_values[1]);
    cout << "| task     | ";
    for (int i = 0; i < task.size(); i++) {
        cout << task[i] << " | ";
    }
    cout << endl;
    cout << "| employee | ";
    vector<double> tmp = UnNormalize(answer->abilities, min_max_values[0], min_max_values[1]);
    for (unsigned int i = 0; i < answer->abilities.size(); ++i)
    {
        cout << tmp[i] << " | ";
    }
    cout << endl;
    for (int j = 0; j < 32; j++)
        cout << "-";
    cout << endl;
    cout << endl;
    //
    Fill_True();
    answer->busy++;
    return *answer;
}

vector<double> min_max(vector<Sotrudnik> space)
{
    double min = space[0].abilities[0];
    double max = space[0].abilities[0];
    
    for (int i = 0; i < space.size(); ++i) {
        for (int j = 0; j < space[i].abilities.size(); ++j) {
            if (space[i].abilities[j] > max)
                max = space[i].abilities[j];
            if (space[i].abilities[j] < min)
                min = space[i].abilities[j];
        }
    }
    return vector<double> {min, max};
}

string make_magic(int rowsSkills, int skillsCount, int rowsTasks, string data) {
    string result = "";
    vector<Sotrudnik> space;
    int size = static_cast<int>(data.size());
    int ind = 0;
    for (int i = 0; i < rowsSkills; i++)
    {
        string name = "";
        while (data[ind] != ' ')
        {
            name += data[ind];
            ind++;
        }
        ind++;
        //double* skills = new double[skillsCount];
        vector<double> skills = vector<double>{};
        for (int j = 0; j < skillsCount; j++)
            skills.push_back(0);
        int j = 0;
        while (data[ind] != '\n' && ind < size)
        {
            if (data[ind] >= '0' && data[ind] <= '9')
                skills[j] = skills[j] * 10 + data[ind] - '0';
            else
                j++;
            ind++;
        }
        ind++;
        /*cout << name << " ";
        for (j = 0; j < skillsCount; j++)
            cout << skills[j] << " ";
        cout << endl;*/
        space.push_back(Sotrudnik(skills, rand() % 100, 0, true, name, " "));
    }
    //unsigned int start_time = clock();
    //вывод сотрудников
    cout << "All employees\n";
    cout << "| ";
    cout << setfill(' ') << setw(10) << "id";
    cout << " | ";
    cout << setfill(' ') << setw(3) << "norm";
    cout << " | ";
    cout << setfill(' ') << setw(9) << "abilites";
    cout << " | ";
    cout << setfill(' ') << setw(3) << "ok";
    cout << " |\n";
    for (int j = 0; j < 33; j++)
        cout << "-";
    cout << endl;
    for (int i = 0; i < space.size(); i++)
    {
        cout << "| ";
        cout << setfill(' ') << setw(10) << space[i].name;
        cout << " | ";
        cout << setfill(' ') << setw(4) << space[i].norm;
        cout << " |";
        for (int j = 0; j < space[i].abilities.size(); j++)
        {
            cout << " ";
            cout << space[i].abilities[j];
        }
        cout << " | ";
        if (space[i].entry)
            cout << setfill(' ') << setw(3) << "yes";
        else
            cout << setfill(' ') << setw(3) << "no";
        cout << " |\n";
        for (int j = 0; j < 33; j++)
            cout << "-";
        cout << endl;
    }
    min_max_values = min_max(space);
    vector<vector<Sotrudnik>> clusters = forel(space, min_max_values[0], min_max_values[1]);
    Fill_True();
    for (int i = 0; i < rowsTasks; i++)
    {
        string name = "";
        while (data[ind] != '!')
        {
            name += data[ind];
            ind++;
        }
        cout << name << " ";
        ind += 2;
        vector<double> skills = vector<double>{};
        for (int j = 0; j < skillsCount; j++)
            skills.push_back(0);
        int j = 0;
        while (data[ind] != '\n' && ind < size)
        {
            if (data[ind] >= '0' && data[ind] <= '9')
                skills[j] = skills[j] * 10 + data[ind] - '0';
            else
                j++;
            ind++;
        }
        ind++;
        for (j = 0; j < skillsCount; j++)
            cout << skills[j] << " ";
        cout << endl;
        skills = normalize(skills, min_max_values[0], min_max_values[1]);
        result += name + "! " + give_task(skills, clusters).name + "\n";
    }
    for (int i = 0; i < clusters.size(); i++)
    {
        cout << "| employee | busy | norm |\n";
        cout << "--------------------------\n";
        for (int j = 0; j < clusters[i].size(); j++)
        {
            cout << "| ";
            cout << setfill(' ') << setw(8) << clusters[i][j].name;
            cout << " | ";
            cout << setfill(' ') << setw(4) << clusters[i][j].busy;
            cout << " | ";
            cout << setfill(' ') << setw(4) << clusters[i][j].norm;
            cout << " |\n";
            cout << "--------------------------\n";
        }
        cout << endl;
    }
    return result;
}

int main() {
    /*string data = "alex123 1 2 3 4 5 6 7 8 9 10 11 12\nkirill 0 0 0 0 10 0 0 4 0 5 0 0\nsanya 0 0 0 0 0 0 0 0 0 0 0 0\nanna 0 0 0 0 0 0 0 0 0 0 0 0\nkim 0 0 0 0 0 0 0 0 0 0 0 0\nnew_guy 10 0 0 9 0 0 0 0 9 0 0 0\nkiok 0 0 0 0 0 0 8 0 0 9 0 0\nPlus two numbers! 10 0 0 0 0 0 0 0 0 0 0 3\nRead poem! 0 0 6 0 0 0 0 0 0 0 0 0\nMultiply matrix by 4! 10 0 0 0 0 0 0 0 0 0 0 0\nName 7 countries of Africa! 0 0 0 0 0 0 0 0 0 10 0 0";
    int rowsSkills = 7;
    int skillsCount = 12;
    int rowsTasks = 4;
    cout << make_magic(rowsSkills, skillsCount, rowsTasks, data);*/
    unsigned int start_time = clock();
    vector<Sotrudnik> space; 
    cout << "Generating employees\n";
    for (int i = 0; i < 3000; i++) {
        space.push_back(Sotrudnik(i + 1, {static_cast<double>(rand() % 10), static_cast<double>(rand() % 10), static_cast<double>(rand() % 10), static_cast<double>(rand() % 10), static_cast<double>(rand() % 10), static_cast<double>(rand() % 10), static_cast<double>(rand() % 10), static_cast<double>(rand() % 10), static_cast<double>(rand() % 10)}, rand() % 100, 0, true));
    }
    //вывод сотрудников
    /*cout << "All employees\n";
    cout << "| ";
    cout << setfill(' ') << setw(4) << "id";
    cout << " | ";
    cout << setfill(' ') << setw(3) << "norm";
    cout << " | ";
    cout << setfill(' ') << setw(9) << "abilites";
    cout << " | ";
    cout << setfill(' ') << setw(3) << "ok";
    cout << " |\n";
    for (int j = 0; j < 33; j++)
        cout << "-";
    cout << endl;
    for (int i = 0; i < space.size(); i++)
    {
        cout << "| ";
        cout << setfill(' ') << setw(4) << space[i].id;
        cout << " | ";
        cout << setfill(' ') << setw(4) << space[i].norm;
        cout << " |";
        for (int j = 0; j < space[i].abilities.size(); j++)
        {
            cout << " ";
            cout << space[i].abilities[j];
        }
        cout << " | ";
        if (space[i].entry)
            cout << setfill(' ') << setw(3) << "yes";
        else
            cout << setfill(' ') << setw(3) << "no";
        cout << " |\n";
        for (int j = 0; j < 33; j++)
            cout << "-";
        cout << endl;
    }*/
    vector<vector<double>> tasks;
    cout << "Generating tasks\n";
    for (int i = 0; i < 7000; i++) {
        tasks.push_back({static_cast<double>(rand() % 10), static_cast<double>(rand() % 10), static_cast<double>(rand() % 10), static_cast<double>(rand() % 10), static_cast<double>(rand() % 10), static_cast<double>(rand() % 10), static_cast<double>(rand() % 10), static_cast<double>(rand() % 10), static_cast<double>(rand() % 10) });
    }
    min_max_values = min_max(space);
    cout << "Normalizing tasks\n";
    for (int i = 0; i < tasks.size(); i++)
        tasks[i] = normalize(tasks[i], min_max_values[0], min_max_values[1]);
    cout << "Clustering employees";
    vector<vector<Sotrudnik>> clusters = forel(space, min_max_values[0], min_max_values[1]);
    int a;
    Fill_True();
    cout << "Giving tasks\n";
    for (int i = 0; i < tasks.size(); i++)
        give_task(tasks[i], clusters);
    for (int i = 0; i < clusters.size(); i++)
    {
        cout << "| employee | busy | norm |\n";
        cout << "--------------------------\n";
        for (int j = 0; j < clusters[i].size(); j++)
        {
            cout << "| ";
            cout << setfill(' ') << setw(8) << clusters[i][j].id;
            cout << " | ";
            cout << setfill(' ') << setw(4) << clusters[i][j].busy;
            cout << " | ";
            cout << setfill(' ') << setw(4) << clusters[i][j].norm;
            cout << " |\n";
            cout << "--------------------------\n";
        }
        cout << endl;
    }
    cout << "Exuction time: " << start_time - clock() << " milliseconds";
    return 0;
}