#include <iostream>
#include <string>
#include <fstream>
#include <math.h>
#include <list>
#include <vector>
#include "Sotrudnik.h"
#include <ctime>

using namespace std;

vector<vector<double>> centrs;
vector<bool> entry_centr;
vector<double> min_distances;
vector<vector<int>> min_distances_inds;
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
    //vector<double> sum;
    //fill(sum.begin(), sum.begin() + employees[0].abilities.size(), 0);
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
        ////удалить этот код
        //for (unsigned int i = 0; i < cluster.size(); i++)
        //    cluster[i].abilities = UnNormalize(cluster[i].abilities, xMin, xMax);
        ////тута конец удаления
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
    //entry_centr[ind] = false;
    return ind;
}

//didnt overwrite
Sotrudnik *find_best(vector<double> task, vector<vector<Sotrudnik>> &space) {
    double min;
    int ind_centr = find_closer_centr(task);
    entry_centr[ind_centr] = space[ind_centr][0].entry;
    Sotrudnik *answer = &space[ind_centr][0];
   // vector<double> min_distances;
    //space[] //find(task, space)
    int ind = 0;
    if (!space.empty()) {
        min = norm(space[ind_centr][0].abilities, task) + space[ind_centr][0].coefficient();
            //cout << "Distance to " << space[ind_centr][0].id << " " << norm(space[ind_centr][0].abilities, task) + space[ind_centr][0].coefficient() << "\n----------------------------------------------" << endl;
        int tmp;
        //entry_centr[ind_centr] = false;
        for (int i = 1; i < space[ind_centr].size(); ++i) {
            /*if (norm(space[ind_centr][i].abilities, task) < min && space[ind_centr][i].entry) {
                min = norm(space[ind_centr][i].abilities, task);
                answer = space[ind_centr][i];
                ind = i;
                entry_centr[ind_centr] = true;
            }*/
            //double coef = /*space[ind_centr][i].busy;*/exp((static_cast<double>(space[ind_centr][0].busy) *
              //  static_cast<double>(space[ind_centr][0].busy)) / static_cast<double>(space[ind_centr][0].norm) - 1);
            if (norm(space[ind_centr][i].abilities, task) + space[ind_centr][i].coefficient() < min &&
                space[ind_centr][i].entry) {
                min = norm(space[ind_centr][i].abilities, task) + space[ind_centr][i].coefficient();
                answer = &space[ind_centr][i];
                ind = i;
                entry_centr[ind_centr] = true;
            }
            //cout << "Distance to "  << space[ind_centr][i].id  << " " << norm(space[ind_centr][i].abilities, task) + space[ind_centr][i].coefficient() << "\n----------------------------------------------" << endl;
        }
        
        if (min > sqrt(2) / 16 && (min_distances.size() < space.size())) {
            min_distances.push_back(min);
            min_distances_inds.push_back({ ind_centr, ind });
            entry_centr[ind_centr] = false;
        }
        
        /*for (int i = 0; i < centrs.size(); i++) {
            if (entry_centr[i]) {
                if (norm(task, centrs[i]) < min)
                    entry_centr[ind_centr] = false;
            }
        }*/

        if (min_distances.size() == space.size())
            for (int i = 0; i < min_distances.size(); i++)
                if (min_distances[i] < min) {
                    min = min_distances[i];
                    entry_centr[ind_centr] = true;
                    answer = &space[min_distances_inds[i][0]][min_distances_inds[i][1]];
                }
        

        if (!entry_centr[ind_centr]) {
            answer = find_best(task, space);
            
            /*vector<double> distance_to_task;
            distance_to_task.push_back(min);*/
        }
    }
    /*centrs = [{1}, {2}, {3}]
    * centrs_return = centrs
    * centrs = [{1}, {coef * 2}, {3}]
    * find_closer - 1
    * if (centrs[])
    * if (!centrs.empty())
    *   if (coef * min > centrs[find_closer_centr(task)]) {
    *       find_best(task, clusters[find_closer_centr(task)]);
    * }
    */
    //cout << "Distance " << min << endl;
    return answer;
}

void Fill_True() {
    entry_centr.clear();
    for (int i = 0; i < centrs.size(); ++i)
        entry_centr.push_back(true);
}

int itask = 0;


void give_task(vector<double> task, vector<vector<Sotrudnik>> &space)
{
    min_distances.clear();
    min_distances_inds.clear();
    Sotrudnik *answer = find_best(task, space);
    //Sotrudnik vivod = &answer;
    itask++;
    cout << "Task " << itask << " for " << answer->id << endl;
    //answer->busy = static_cast<int>(static_cast<double>(answer->busy) + 1 + (task[0] + task[1]) / 2);
    task = UnNormalize(task, 0, 21);
    for (int i = 0; i < task.size(); i++) {
        
        cout << task[i] << " ";
    }
    cout << endl;
    cout << "Sotrudnik ";
    vector<double> tmp = UnNormalize(answer->abilities, 0, 21);
    for (unsigned int i = 0; i < answer->abilities.size(); ++i)
    {
        
        /*cout << answer->abilities[i] << " ";*/
        cout << tmp[i] << " ";
    }
    cout << endl;
    
    Fill_True();
    //answer.busyer();
    answer->busy++;
    
    /*cout << "Busy = " << answer->busy << endl;
    cout << "Coef = " << answer->coefficient() << endl;
    cout << "================================" << endl;*/
    cout << "================================" << endl;
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

int main() {
    unsigned int start_time = clock();
    vector<Sotrudnik> space;
    /*space.push_back(Sotrudnik(1, vector<double> {1, 1}, 10, 0, true));
    space.push_back(Sotrudnik(2, vector<double> {3, 2}, 10, 0, true));
    space.push_back(Sotrudnik(3, vector<double> {2, 3}, 10, 0, true));
    space.push_back(Sotrudnik(4, vector<double> {2, 2}, 10, 0, true));
    space.push_back(Sotrudnik(5, vector<double> {0, 2}, 10, 0, true));
    space.push_back(Sotrudnik(6, vector<double> {11, 11}, 10, 0, true));
    space.push_back(Sotrudnik(7, vector<double> {13, 12}, 10, 0, true));
    space.push_back(Sotrudnik(8, vector<double> {12, 13}, 10, 0, true));
    space.push_back(Sotrudnik(9, vector<double> {12, 12}, 10, 0, true));
    space.push_back(Sotrudnik(10, vector<double> {11, 21}, 10, 0, true));
    space.push_back(Sotrudnik(11, vector<double> {7, 5}, 10, 0, true));
    space.push_back(Sotrudnik(12, vector<double> {9, 6}, 20, 0, true));
    space.push_back(Sotrudnik(13, vector<double> {8, 7}, 10, 0, true));
    space.push_back(Sotrudnik(14, vector<double> {8, 6}, 10, 0, true));
    space.push_back(Sotrudnik(15, vector<double> {7, 6}, 10, 0, true));
    space.push_back(Sotrudnik(16, vector<double> {20, 20}, 10, 0, true));
    space.push_back(Sotrudnik(17, vector<double> {18, 18}, 10, 0, true));*/



    //space.push_back(Sotrudnik(1, vector<double> {1, 1}, 1500, 0, true));
    //space.push_back(Sotrudnik(2, vector<double> {3, 2}, 1002, 0, true));
    //space.push_back(Sotrudnik(3, vector<double> {2, 3}, 1300, 0, true));
    //space.push_back(Sotrudnik(4, vector<double> {2, 2}, 1100, 0, true));
    //space.push_back(Sotrudnik(5, vector<double> {0, 2}, 800, 0, true));
    //space.push_back(Sotrudnik(6, vector<double> {11, 11}, 200, 0, true));
    //space.push_back(Sotrudnik(7, vector<double> {13, 12}, 500, 0, true));
    //space.push_back(Sotrudnik(8, vector<double> {12, 13}, 300, 0, true));
    //space.push_back(Sotrudnik(9, vector<double> {12, 12}, 10000, 0, true));
    //space.push_back(Sotrudnik(10, vector<double> {11, 21}, 1200, 0, true));
    //space.push_back(Sotrudnik(11, vector<double> {7, 5}, 1700, 0, true));
    //space.push_back(Sotrudnik(12, vector<double> {9, 6}, 2100, 0, true));
    //space.push_back(Sotrudnik(13, vector<double> {8, 7}, 2300, 0, true));
    //space.push_back(Sotrudnik(14, vector<double> {8, 6}, 2200, 0, true));
    //space.push_back(Sotrudnik(15, vector<double> {7, 6}, 2100, 0, true));
    //space.push_back(Sotrudnik(16, vector<double> {20, 20}, 2800, 0, true));
    //space.push_back(Sotrudnik(17, vector<double> {18, 18}, 3500, 0, true));


    /*
    int count = 0;
    for (int i = 0; i < space.size(); i++)
    {
        count += space[i].norm;
    }*/


    /*for (int i = 0; i < 100; i++) {
        space.push_back(Sotrudnik(i + 1, vector<double> {rand() % 20, static_cast<double>(rand() % 20),
            static_cast<double>(rand() % 20), static_cast<double>(rand() % 20), static_cast<double>(rand() % 20) }, rand() % 200, 0, true));
    }*/
        
    for (int i = 0; i < 4000; i++) {
        double tmp = rand() % 20;
        double tmp1 = rand() % 20;
        double tmp2 = rand() % 20;
        double tmp3 = rand() % 20;
        double tmp4 = rand() % 20;
        double tmp5 = rand() % 200;
        space.push_back(Sotrudnik(i + 1, vector<double>{tmp, tmp1, tmp4, tmp5, tmp2}, 20000, 0, true));
    }
        
   /* vector<double> task1 = { 12, 22 };
    vector<double> task2 = { 30, 30 };
    vector<double> task3 = { 10, 10 };
    vector<double> task4 = { 2, 1 };
    vector<double> task5 = { 19, 19 };*/
    vector<vector<double>> tasks;
    //for (int i = 0; i < 100000/*count * 10*/; i++) {
    //    double tmp1 = static_cast<double>(rand() % 20);
    //    double tmp2 = static_cast<double>(rand() % 20);

    //    tasks.push_back({ tmp1, tmp2 });
    //}
    //for (int i = 0; i < 1000/*count * 10*/; i++) {
    //    double tmp1 = static_cast<double>(rand() % 20);
    //    double tmp2 = static_cast<double>(rand() % 20);
    //    
    //    tasks.push_back({ tmp1, tmp2, static_cast<double>(rand() % 20), static_cast<double>(rand() % 20), static_cast<double>(rand() % 20) });
    //}
    for (int i = 0; i < 100000/*count * 10*/; i++) {
        double tmp1 = rand() % 20;
        double tmp2 = rand() % 20;
        double tmp3 = rand() % 20;
        double tmp4 = rand() % 20;
        double tmp5 = rand() % 20;

        tasks.push_back({ tmp1, tmp2, tmp3, tmp4, tmp5 });
    }
    /*vector<double> task1 = { 12, 22, 0, 0, 0 };
    vector<double> task2 = { 12, 0, 0, 0, 0 };
    vector<double> task3 = { 8, 7, 61, 1, 1 };
    vector<double> task4 = { 20, 22, 30, 33, 12 };
    space.push_back(Sotrudnik(1, vector<double> {1, 1, 2, 3, 4}, 1, 1, true));
    space.push_back(Sotrudnik(2, vector<double> {3, 2, 2, 1, 3}, 1, 1, true));
    space.push_back(Sotrudnik(3, vector<double> {2, 3, 3, 2, 1}, 1, 1, true));
    space.push_back(Sotrudnik(4, vector<double> {2, 2, 14, 12, 3}, 1, 1, true));
    space.push_back(Sotrudnik(5, vector<double> {1, 2, 0, 1, 1}, 1, 1, true));
    space.push_back(Sotrudnik(6, vector<double> {11, 11, 1, 1, 1}, 1, 1, true));
    space.push_back(Sotrudnik(7, vector<double> {13, 12, 33, 12, 1}, 1, 1, true));
    space.push_back(Sotrudnik(8, vector<double> {12, 13, 22, 22, 1}, 1, 1, true));
    space.push_back(Sotrudnik(9, vector<double> {12, 12, 1, 1, 1}, 1, 1, true));
    space.push_back(Sotrudnik(10, vector<double> {11, 21, 0, 0, 0}, 1, 1, false));
    space.push_back(Sotrudnik(11, vector<double> {7, 5, 20, 20, 20}, 1, 1, true));
    space.push_back(Sotrudnik(12, vector<double> {9, 6, 15, 15, 15}, 1, 1, true));
    space.push_back(Sotrudnik(13, vector<double> {8, 7, 1, 1, 1}, 1, 1, true));
    space.push_back(Sotrudnik(14, vector<double> {8, 6, 1, 2, 3}, 1, 1, true));
    space.push_back(Sotrudnik(15, vector<double> {7, 6, 6, 6, 6}, 1, 1, true));
    space.push_back(Sotrudnik(16, vector<double> {1, 1, 0, 3, 4}, 1, 1, true));
    space.push_back(Sotrudnik(17, vector<double> {3, 2, 2, 1, 3}, 1, 1, true));
    space.push_back(Sotrudnik(18, vector<double> {2, 3, 3, 9, 1}, 1, 1, true));
    space.push_back(Sotrudnik(19, vector<double> {2, 2, 14, 0, 3}, 1, 1, true));
    space.push_back(Sotrudnik(20, vector<double> {1, 10, 0, 1, 1}, 1, 1, true));
    space.push_back(Sotrudnik(21, vector<double> {11, 11, 1, 0, 1}, 1, 1, true));
    space.push_back(Sotrudnik(22, vector<double> {13, 12, 33, 32, 1}, 1, 1, false));
    space.push_back(Sotrudnik(23, vector<double> {12, 13, 22, 0, 1}, 1, 1, true));
    space.push_back(Sotrudnik(24, vector<double> {12, 12, 1, 1, 10}, 1, 1, true));
    space.push_back(Sotrudnik(25, vector<double> {11, 21, 0, 1, 0}, 1, 1, true));
    space.push_back(Sotrudnik(26, vector<double> {7, 5, 2, 20, 20}, 1, 1, true));
    space.push_back(Sotrudnik(27, vector<double> {9, 6, 15, 15, 15}, 1, 1, true));
    space.push_back(Sotrudnik(28, vector<double> {8, 7, 61, 1, 1}, 1, 1, true));
    space.push_back(Sotrudnik(29, vector<double> {8, 6, 50, 12, 3}, 1, 1, true));
    space.push_back(Sotrudnik(30, vector<double> {7, 6, 60, 6, 6}, 1, 1, true));*/
    vector<double> min_max_values = min_max(space);
    /*task1 = normalize(task1, min_max_values[0], min_max_values[1]);
    task2 = normalize(task2, min_max_values[0], min_max_values[1]);
    task3 = normalize(task3, min_max_values[0], min_max_values[1]);
    task4 = normalize(task4, min_max_values[0], min_max_values[1]);
    task5 = normalize(task5, min_max_values[0], min_max_values[1]);*/
    for (int i = 0; i < tasks.size(); i++)
        tasks[i] = normalize(tasks[i], min_max_values[0], min_max_values[1]);

    vector<vector<Sotrudnik>> clusters = forel(space, min_max_values[0], min_max_values[1]);
    //unsigned int end_time = clock();
    int a;
    Fill_True();
    
    /*Fill_True();
    Sotrudnik best1 = find_best(task1, clusters);
    Sotrudnik best2 = find_best(task2, clusters);

    Sotrudnik best3 = find_best(task3, clusters);
 
    Sotrudnik best4 = find_best(task4, clusters);*/

    /*for (unsigned int i = 0; i < clusters.size(); i++) {
        cout << i + 1 << " cluster\nCenter: ";
        if (!clusters[i].empty()) {
            for (unsigned int j = 0; j < centrs[i].size(); j++) {
                cout << centrs[i][j] << " ";
            }
            cout << endl;
        }
        for (unsigned int j = 0; j < clusters[i].size(); j++) {
            cout << j + 1 << " employee - id" << clusters[i][j].id << endl;
            for (unsigned int k = 0; k < clusters[i][j].abilities.size(); k++)
                cout << clusters[i][j].abilities[k] << " ";
            cout << endl;
        }
        cout << endl;

    }*/
    /*give_task(task1, clusters);
    give_task(task2, clusters);
    give_task(task3, clusters);
    give_task(task4, clusters);
    give_task(task5, clusters);
    give_task(task5, clusters);*/
    for (int i = 0; i < tasks.size(); i++)
        give_task(tasks[i], clusters);
  

    for (unsigned int i = 0; i < clusters.size(); i++) {
        cout << i + 1 << " cluster\nCenter: ";
        if (!clusters[i].empty()) {
            for (unsigned int j = 0; j < centrs[i].size(); j++) {
                cout << centrs[i][j] << " ";
            }
            cout << endl;
        }
        for (unsigned int j = 0; j < clusters[i].size(); j++) {
            cout << j + 1 << " employee - id" << clusters[i][j].id << endl;
            cout << "Busy = " << clusters[i][j].busy << endl;
            cout << "Norm = " << clusters[i][j].norm;
            cout << endl;
        }
        cout << endl;

    }
    int n = 0;
    for (int i = 0; i < clusters.size(); i++) {
        for (int j = 0; j < clusters[i].size(); j++)
            n += clusters[i][j].busy;
    }
    cout << n << endl;
    unsigned int end_time = clock();
    cout << end_time - start_time << endl;
    //cin >> n;
    return 0;
}