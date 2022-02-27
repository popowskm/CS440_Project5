/* This is a skeleton code for Optimized Merge Sort, you can make modifications as long as you meet 
   all question requirements*/  

#include <bits/stdc++.h>
#include "record_class.h"

using namespace std;

//defines how many blocks are available in the Main Memory 
#define buffer_size 22

Records buffers[buffer_size]; //use this class object of size 22 as your main memory

/***You can change return type and arguments as you want.***/

bool write_buffer_to_run(fstream &run, string type) {
    if (type == "emp") {
        for (int i = 0; i < 21; i++) {
            Records current = buffers[i];
            if (current.no_values != -1) {
                run << current.emp_record.eid << ',' << current.emp_record.ename << ',' << current.emp_record.age << ',' << current.emp_record.salary << endl;
            }
            else {
                run.flush();
                return false;
            }
        }
    }
    else {
        for (int i = 0; i < 21; i++) {
            Records current = buffers[i];
            if (current.no_values != -1) {
                run << current.dept_record.did << ',' << current.dept_record.dname << ',' << current.dept_record.budget << ',' << current.dept_record.managerid << endl;
            }
            else {
                run.flush();
                return false;
            }
        }
    }
    run.flush();
    return true;
}


//Sorting the buffers in main_memory and storing the sorted records into a temporary file (runs) 
void Sort_Buffer(int amt){
    //Remember: You can use only [AT MOST] 22 blocks for sorting the records / tuples and create the runs
    sort(buffers,buffers+amt);
    return;
}

//Prints out the attributes from empRecord and deptRecord when a join condition is met 
//and puts it in file Join.csv
void PrintJoin(Records dept, Records emp, fstream &merge) {
    merge << emp.emp_record.eid << ',' << dept.dept_record.did << ',' << dept.dept_record.dname << ',' << dept.dept_record.budget << ',' << emp.emp_record.ename << ',' << emp.emp_record.age << ',' << emp.emp_record.salary << endl;
    return;
}

//Use main memory to Merge and Join tuples 
//which are already sorted in 'runs' of the relations Dept and Emp 
void Merge_Join_Runs(vector<fstream*> emp_runs, vector<fstream*> dept_runs, fstream &output){
    int dept_size = dept_runs.size();
    int emp_size = emp_runs.size();
    for (int i = 0; i < dept_size; i++) {
        buffers[i] = Grab_Dept_Record(*dept_runs[i]);
    }
    for (int i = 0; i < emp_size; i++) {
        buffers[i + dept_size] = Grab_Emp_Record(*emp_runs[i]);
    }

    bool flag = true;
    while(flag) {
        flag = false;
        vector<bool> incompletes(22, false);
        vector<bool> matched(22, false);
        for (int i = 0; i < dept_size; i++) {
            for (int j = dept_size; j < dept_size + emp_size; j++) {
                int mid = buffers[i].dept_record.managerid;
                if (buffers[i].no_values == -1) {
                    break;
                }
                int eid = buffers[j].emp_record.eid;
                if (buffers[j].no_values != -1) {
                    flag = true;
                    if (mid == eid) {
                        PrintJoin(buffers[i], buffers[j], output);
                        incompletes[j] = true;
                        incompletes[i] = false;
                        break;
                    }
                    else if (mid < eid) {
                        incompletes[j] = true;
                    }
                    else {
                        incompletes[i] = true;
                    } 

                }
            }
            for (int k = 0; k < dept_size; k++) {
                if (!incompletes[k]) {
                    buffers[k] = Grab_Dept_Record(*dept_runs[k]);
                }
            }
            for (int k = 0; k < emp_size; k++) {
                if (!incompletes[k + dept_size]) {                
                    buffers[k + dept_size] = Grab_Emp_Record(*emp_runs[k]);
                }
            }

        }
    }

    //and store the Joined new tuples using PrintJoin() 
    return;
}


//Create the runs from input files
vector<int> Generate_Runs(fstream &empin, fstream &deptin) {
    bool flag = true;
    int emp_counter = 0;
    while (flag) {
        int count = 0;
        for (int i = 0; i < 21; i++) {
            buffers[i] = Grab_Emp_Record(empin);
            if (buffers[i].no_values != -1) {
                count++;
            }
        }
        if (count > -1) {
            Sort_Buffer(count);
        }

        fstream run;
        run.open("emp_run_" + to_string(emp_counter) + ".csv", fstream::out | fstream::trunc);
        flag = write_buffer_to_run(run, "emp");
        run.close();
        emp_counter++;
    }
    int dept_counter = 0;
    flag = true;
    while (flag) {
        int count = 0;
        for (int i = 0; i < 21; i++) {
            buffers[i] = Grab_Dept_Record(deptin);
            if (buffers[i].no_values != -1) {
                count++;
            }
        }
        if (count > -1) {
            Sort_Buffer(count);
        }
        
        fstream run;
        run.open("dept_run_" + to_string(dept_counter) + ".csv", fstream::out | fstream::trunc);
        flag = write_buffer_to_run(run, "dept");
        run.close();
        dept_counter++;
    }
    return {emp_counter, dept_counter};
}

int main() {

    //Open file streams to read and write
    //Opening out two relations Emp.csv and Dept.csv which we want to join
    fstream empin;
    fstream deptin;
    empin.open("Emp.csv", ios::in);
    deptin.open("Dept.csv", ios::in);
   
    //Creating the Join.csv file where we will store our joined results
    fstream joinout;
    joinout.open("Join.csv", ios::out | ios::trunc);

    //1. Create runs for Dept and Emp which are sorted using Sort_Buffer()
    vector<int> filecounts;
    filecounts = Generate_Runs(empin, deptin);

    vector<fstream*> emp_runs;
    for(int i = 0; i < filecounts[0]; i++) {
        string infile = "emp_run_" + to_string(i) + ".csv";
        fstream* temp = new fstream(infile, ios::in);
        emp_runs.push_back(temp);
    }

    vector<fstream*> dept_runs;
    for(int i = 0; i < filecounts[1]; i++) {
        string infile = "dept_run_" + to_string(i) + ".csv";
        fstream* temp = new fstream(infile, ios::in);
        dept_runs.push_back(temp);
    }

    //2. Use Merge_Join_Runs() to Join the runs of Dept and Emp relations 
    Merge_Join_Runs(emp_runs, dept_runs, joinout);
    // Please delete the temporary files (runs) after you've joined both Emp.csv and Dept.csv
    for(int i = 0; i < filecounts[0]; i++) {
        remove(("emp_run_" + to_string(i) + ".csv").c_str());
    }
    for(int i = 0; i < filecounts[1]; i++) {
        remove(("dept_run_" + to_string(i) + ".csv").c_str());
    }

    return 0;
}
