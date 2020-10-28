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
     ofstream traceStream;
     vector<double> clusters, endCluster;
     string output, trace;
     int k, repetitions, random_var, seed;
     double cost;
     double lowest_cost = 1000000000;

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
     SerialKmeans(vector<vector<double>> &data, string output, string trace, int k, int repetitions, int seed){
          this->data = data;
          this->k = k;
          this->repetitions = repetitions; 
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
     void initializeCentroids(){
          for(int i = 0; i < k; i++){
               centroids.push_back(data[0 + rand() % data.size()]);
          }
     }

     /**
      * run the Kmeans algorithm
      * 
      * this Function starts the Kmeans algorithm and writes the output / trace
      * if specified by the user.
      */
     void kMeans(){
          vector<vector<double>> upCentroids;
          
          for(int i = 0; i < repetitions; i++){
               centroids.clear();
               newCentroids.clear();
               clusters.clear();

               initializeCentroids();
               getClosestCentroid();    

               if(trace != "" && i == 0){
                    writeTraceCSV();
               }
                          
               upCentroids = updateCentroids();
               int counter = 0;

               lowest_cost = costFunction(upCentroids);
               endCluster = clusters;
               
               while(upCentroids != centroids){
                    centroids = upCentroids;
                    upCentroids.clear();
                    getClosestCentroid();
                    upCentroids = updateCentroids();
                    counter++;
                    
                    if(trace != "" && i == 0){
                         writeTraceCSV();
                    }
               }


               cost = costFunction(upCentroids);
               if(cost < lowest_cost){       
                    lowest_cost = cost;
                    endCluster = clusters;
               }
          }

          if(output != ""){
               cout << "enter write output";
               writeOutputCSV();
          }
     }

      /**
      * Calculates the cost of all clusters per repetition.
      */
     double costFunction(vector<vector<double>> upCentroids){
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
     void writeOutputCSV(){
          ofstream outStream(output, ios_base::trunc); 
          CSVWriter writer(outStream);

          writer.write(endCluster);
     }

     /**
      * Writes the trace of each iteration to the trace file.
      */
     void writeTraceCSV(){
          CSVWriter writer(traceStream);
          writer.write(clusters);
     }

     /**
      * calculates te closest centroids
      * 
      * This function calculates the lowest distance between the points and the chosen centroids.
      */
     void getClosestCentroid(){
          double currentDist, lowestDist = 1000000000, clusterOfLowestDistance;

          clusters.clear();

          for(int i = 0; i < data.size(); i++){  
               for(int j = 0; j < k; j++){   
                    if(data[i] != centroids[j]){
                         currentDist = euclidDistance(data[i], centroids[j]);
                         if(currentDist <= lowestDist){
                              lowestDist = currentDist;
                              clusterOfLowestDistance = j;
                         }  
                    }
               }
               clusters.push_back(clusterOfLowestDistance);
               lowestDist = 1000000;
          }
     }

     /* Updates the old centroids
     *
     * This function calculates new centroids based on the mean of the sum of points 
     * inside each cluster.
     * @return a 2D vector with new Centroids.
     */
     vector<vector<double>> updateCentroids(){
          double mean = 0, total = 0;
          int cluster_size = 0;
          int temp = 0;
          vector<double> punt;
          newCentroids.clear();

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
               double temp = punt[i] - centroid[i];
               dist += pow(temp, 2);    
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

/* writes data into a CSV file
*
* This function writes data represented into an 2D vector to a CSV file.
* @params iutput - the name of the CSV file you want to write to.
* @params data - 2D vector which holds the data to write.
*/
static void writeCSV(const string &output, vector<vector<double>> &data){
     ofstream outStream(output);
	CSVWriter writer(outStream);

     for(int i = 0; data.size(); i++){
          writer.write(data[i]);
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

     SerialKmeans kmeans(data, output_file, trace_file, k, repetitions, seed);
     kmeans.kMeans();
}