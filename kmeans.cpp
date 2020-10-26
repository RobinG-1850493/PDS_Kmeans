#include <iostream>
#include <fstream>
#include <random>
#include <cmath>
#include "CSVReader.hpp"
#include "CSVWriter.hpp"


using namespace std;

class SerialKmeans {
  public:
     vector<vector<double>> data, centroids, newCentroids;
     vector<vector<vector<double>>> clusters;
     int k, repetitions, random_var, seed;

     SerialKmeans(vector<vector<double>> &data, int k, int repetitions, int seed){
          this->data = data;
          this->k = k;
          this->repetitions = repetitions; 
          this->seed = seed;
     };


     void initializeCentroids(){
          srand(seed);

          for(int i = 0; i < k; i++){
               centroids.push_back(data[0 + rand() % data.size()]);
          }
          //double test = SerialKmeans::euclidDistance(data[0], data[1]);
          //cout << test;
     }

     void run(){
          vector<vector<double>> upCentroids;
          
          initializeCentroids();
          getClosestCentroid();
          
          
          upCentroids = updateCentroids();
          int counter = 0;
          while(upCentroids != centroids){

               if(counter%500 == 0) {
                    cout << centroids[0][0] << "--" << setprecision(15) << upCentroids[0][0] << endl;
                    cout << centroids[1][0] << "--" << setprecision(15) << upCentroids[1][0] << endl;
                    //cout << centroids[2][0] << "--" << setprecision(15) << upCentroids[2][0] << endl;
               }

               centroids.clear();
               centroids = upCentroids;
               upCentroids.clear();
               getClosestCentroid();
               upCentroids = updateCentroids();
               counter++;
          }
          
          cout << "reached";
          
     }

     void getClosestCentroid(){
          double currentDist, lowestDist = 1000000000, clusterOfLowestDistance;

          clusters.clear();

          for(int i = 0; i < k; i++){
               clusters.push_back(centroids);
          }

          for(int i = 0; i < data.size(); i++){            
               for(int j = 0; j < centroids.size(); j++){
                    if(data[i] != centroids[j]){
                         currentDist = euclidDistance(data[i], centroids[j]);
                         if(currentDist < lowestDist){
                              lowestDist = currentDist;
                              clusterOfLowestDistance = j;
                         }  
                    }
               }
               clusters[clusterOfLowestDistance].push_back(data[i]);
          }
     }

     vector<vector<double>> updateCentroids(){
          double mean, total;
          vector<double> punt;
          newCentroids.clear();

          for(int i = 0; i < k; i++){
               for(int a = 0; a < data[0].size(); a++){
                    for(int j = 0; j < clusters[i].size(); j++){
                         total += clusters[i][j][a];
                    }
                    mean = total / clusters[i].size();
                    punt.push_back(mean);
               }
               total = 0;
               newCentroids.push_back(punt);
               punt.clear();
          }

          return newCentroids;
     }


     double euclidDistance(vector<double> punt, vector<double> centroid)
     {
          double dist;

          for(int i = 0; i < punt.size(); i++){
               double temp = punt[i] - centroid[i];
               dist += pow(temp, 2);      //calculating Euclidean distance
          }           

          dist = sqrt(dist); 

	     return dist;
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
     int k, repetitions, seed = 1850493;


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
          else if(arg.find("--seed") == 0){
               i++;
               seed = strtol(argv[i], NULL, 10);
               // cout << "Debug file: " << debug_file;
          }
     }

     readCSV(input_file, data);

     SerialKmeans kmeans(data, k, repetitions, seed);
     kmeans.run();
}