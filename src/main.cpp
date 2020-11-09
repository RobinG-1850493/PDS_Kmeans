#include <iostream>
#include <fstream>
#include <random>
#include <cmath>
#include "CSVReader.hpp"
#include "CSVWriter.hpp"
#include "timer.hpp"

#include <omp.h>

using namespace std;

class SerialKmeans {
  public:
     ofstream traceStream;
     string output, trace;
     int random_var, seed;

     
     /**
      * SerialKmeans constructor
      * 
      * Creates an instance of SerialKmeans.
      * @params data - represents the input file specified by the user.
      * @params output - the name of the output file to write the results to.
      * @params trace - the name of the trace file to write the traces to.
      * @params k - represents the amount of clusters chosen by the user.
      * @params repetitions - how many times the Kmeans algorithm should run.
      * @params seed - the seed for the random function.
      */
     SerialKmeans(string output, string trace, int seed){
          this->output = output;
          this->trace = trace;
          
          /**
           * check if the trace parameter has been used by the user.
           * if it has not been used, there is no need to open a stream. 
           */
          if(trace != ""){
               traceStream.open(trace, ios_base::trunc);
          }
          srand(seed);
     };

     /**
      * Initialize centroids based on seed.
      * 
      * Pick k centroids by using random with specific seed from the data.
      */
     vector<vector<double>> initializeCentroids(vector<vector<double>> &data, int k){
          vector<vector<double>> centroids;
          for(int i = 0; i < k; i++){
               centroids.push_back(data[0 + rand() % data.size()]);
          }
          
          return centroids;
     }

     /**
      * run the Kmeans algorithm
      * 
      * this Function starts the Kmeans algorithm and writes the output / trace
      * if specified by the user.
      */
     void kMeans(vector<vector<double>> &data, int k, int repetitions){
          double cost;
          double lowest_cost = 1000000000;
          vector<double> clusters;
          vector<double> endCluster;
          vector<vector<double>> centroids;
          vector<vector<double>> upCentroids;
          Timer timer1;
          
          timer1.start();
          for(int i = 0; i < repetitions; i++){
               centroids.clear();
               clusters.clear();

               centroids = initializeCentroids(data, k);
               clusters = getClosestCentroid(data, centroids, k);    

               if(trace != "" && i == 0){
                    writeTraceCSV(clusters);
               }
                          
               upCentroids = updateCentroids(data, clusters, k);
               int counter = 0;

               lowest_cost = costFunction(upCentroids, data, clusters, k);
               endCluster = clusters;
               
               while(upCentroids != centroids && counter < 500){
                    centroids = upCentroids;
                    upCentroids.clear();
                    clusters = getClosestCentroid(data, centroids, k);
                    upCentroids = updateCentroids(data, clusters, k);
                    counter++;
                    
                    if(trace != "" && i == 0){
                         writeTraceCSV(clusters);
                    }
               }


               cost = costFunction(upCentroids, data, clusters, k);
               if(cost < lowest_cost){       
                    lowest_cost = cost;
                    endCluster = clusters;
               }
          }
          timer1.stop();
          cout << timer1.duration() << endl;
          if(output != ""){
               writeOutputCSV(endCluster);
          }
     }

      /**
      * Calculates the cost of all clusters per repetition.
      */
     double costFunction(vector<vector<double>> upCentroids, vector<vector<double>> &data, vector<double> &clusters, int k){
          double total = 0;

          for(int i = 0; i < data.size(); i++){  
               for(int j = 0; j < k; j++){   
                    if(clusters[i] == j){
                         total += euclidDistance(data[i], upCentroids[j]);
                    }
               }
          }

          return total;
     }
     
     /**
      * Writes the output of each repetition to the output file.
      */
     void writeOutputCSV(vector<double> &endCluster){
          ofstream outStream(output, ios_base::trunc); 
          CSVWriter writer(outStream);

          writer.write(endCluster);
     }

     /**
      * Writes the trace of each iteration to the trace file.
      */
     void writeTraceCSV(vector<double> &clusters){
          CSVWriter writer(traceStream);
          writer.write(clusters);
     }

     /**
      * calculates te closest centroids
      * 
      * This function calculates the lowest distance between the points and the chosen centroids.
      */
     vector<double> getClosestCentroid(vector<vector<double>> &data, vector<vector<double>> &centroids, int k){
          double lowestDist, clusterOfLowestDistance;
          unsigned long data_size = data.size();
          unsigned long cluster_size = k;
          vector<double> clusters;
          omp_set_dynamic(0);   
          omp_set_num_threads(6);
          #pragma omp parallel
          {
               vector<double> priv_clusters;
               #pragma omp for firstprivate(data_size, cluster_size) private(lowestDist, clusterOfLowestDistance) nowait schedule(static)
               for(int i = 0; i < data_size; i++){  
                    clusterOfLowestDistance = 0;
                    lowestDist = 10000000;
                    vector<double> &data_point = data[i];
                    for(int j = 0; j < cluster_size; j++){   
                         vector<double> &centroid = centroids[j];
                         
                         if(data_point != centroid){
                              double currentDist = euclidDistance(data_point, centroid);
                              if(currentDist <= lowestDist){
                                   lowestDist = currentDist;
                                   clusterOfLowestDistance = j;
                              }  
                         }
                    }
                    priv_clusters.push_back(clusterOfLowestDistance);
               }
               #pragma omp for schedule(static) ordered
               for(int i = 0; i<omp_get_num_threads(); i++){
                    #pragma omp ordered
                    clusters.insert(clusters.end(), priv_clusters.begin(), priv_clusters.end());
               }
               
          }
          return clusters;
     }

     /* Updates the old centroids
     *
     * This function calculates new centroids based on the mean of the sum of points 
     * inside each cluster.
     * @return a 2D vector with new Centroids.
     */
     vector<vector<double>> updateCentroids(vector<vector<double>> &data, vector<double> clusters, int k){
          double mean = 0, total = 0;
          int cluster_size = 0;
          int temp = 0;
          vector<double> punt;
          vector<vector<double>> newCentroids;

          for(int i = 0; i < k; i++){
               for(int j = 0; j < data[0].size(); j++){ 
                    for(int a = 0; a < data.size(); a++){       
                         if(clusters[a] == i){      
                              total += data[a][j];
                              cluster_size++;
                         }    
                    } 

                    mean = total / cluster_size;
                    punt.push_back(mean);
                    total= 0;
                    mean = 0;
                    cluster_size = 0;
               }
               newCentroids.push_back(punt);
               punt.clear();
          }

          return newCentroids;
     }

     /* Performs the euclidian distance 
     *
     * Calculates the distance between two or a given set of points.
     * @params punt - represent a vector of type double points
     * @params centroid - represent a vector of type double with the points of a centroid
     * @return a double value which represents the distance between the punt and the centroid
     */
     double euclidDistance(vector<double> punt, vector<double> centroid)
     {
          double dist;

          for(int i = 0; i < punt.size(); i++){
               dist += pow(punt[i] - centroid[i], 2);            
          }  
                       
          dist = sqrt(dist); 
	     return dist;
     }
};

/* reads a CSV file
*
* This function reads an CSV file and puts the content into an 2D vector.
* @params input - the name of the CSV file you want to read.
* @params data - the 2D vector where the content gets stored.
*/
static void readCSV(const string &input, vector<vector<double>> &data){
     ifstream inStream(input);
     CSVReader reader(inStream);

     vector<double> row;

     while(reader.read(row)){
          data.push_back(row);
     }
}

/* main function
*
* checks all parameters given by the user and starts the program.
*/
int main(int argc, char *argv[]){
     vector<vector<double>> data;
     string input_file, output_file = "", trace_file = "";
     int k, repetitions = 1, seed = 1850493;
     omp_set_nested(1);

     for(int i = 0; i < argc; ++i){
          string arg = argv[i];
          if(arg.find("--input") == 0){
               i++;
               input_file = argv[i];
          }
          else if(arg.find("--output") == 0){
               i++;
               output_file = argv[i];
          }
          else if(arg.find("--k") == 0){
               i++;
               k = strtol(argv[i], NULL, 10);
          }
          else if(arg.find("--repetition") == 0){
               i++;
               repetitions = strtol(argv[i], NULL, 10);
          }
          else if(arg.find("--trace") == 0){
               i++;
               trace_file = argv[i];
          }
          else if(arg.find("--seed") == 0){
               i++;
               seed = strtol(argv[i], NULL, 10);
          }
     }

     readCSV(input_file, data);

     SerialKmeans kmeans(output_file, trace_file, seed);
     kmeans.kMeans(data, k, repetitions);
}