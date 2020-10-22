#include <iostream>
#include <fstream>
#include <random>
#include <cmath>
#include "CSVReader.hpp"
#include "CSVWriter.hpp"

using namespace std;

class SerialKmeans {
  public:
     vector<vector<double>> data;
     int k, repetitions;

     SerialKmeans(vector<vector<double>> &data, int k, int repetitions){
          this->data = data;
          this->k = k;
          this->repetitions = repetitions;
     };


     void initializeCentroids(){
          srand(1850493);
          int random_var = 0 + rand() % data.size();
          cout << random_var;
     }
};

static void readCSV(const string &input, vector<vector<double>> &data){
     ifstream inStream(input);
     CSVReader reader(inStream);

     vector<double> row;

     while(reader.read(row)){
          data.push_back(row);
     }
}

static void writeCSV(const string &output, vector<vector<double>> &data){
     ofstream outStream(output);
	CSVWriter writer(outStream);

     for(int i = 0; data.size(); i++){
          writer.write(data[i]);
     }
}

int main(int argc, char *argv[]){
     vector<vector<double>> data;
     string input_file, output_file, trace_file, debug_file;
     int k, repetitions;


     for(int i = 0; i < argc; ++i){
          string arg = argv[i];
          if(arg.find("--input") == 0){
               i++;
               input_file = argv[i];
               // cout << "Input file: " << input_file;
          }
          else if(arg.find("--output") == 0){
               i++;
               output_file = argv[i];
               // cout << "Output file: " << output_file;
          }
          else if(arg.find("--k") == 0){
               i++;
               k = strtol(argv[i], NULL, 10);
               // cout << "k: " << k;
          }
          else if(arg.find("--repetition") == 0){
               i++;
               repetitions = strtol(argv[i], NULL, 10);
               // cout << "repetitions: " << repetitions;
          }
          else if(arg.find("--trace") == 0){
               i++;
               trace_file = argv[i];
               // cout << "Trace file: " << trace_file;
          }
          else if(arg.find("--debug") == 0){
               i++;
               debug_file = argv[i];
               // cout << "Debug file: " << debug_file;
          }
     }

     readCSV(input_file, data);

     SerialKmeans kmeans(data, k, repetitions);
     kmeans.initializeCentroids();
}