//************************************************
//*                                              *
//*                 Calibrate.C                  *
//*              Katarzyna Rusiecka              *
//*    katarzyna.rusiecka@doctoral.uj.edu.pl     * 
//*                Created in 2019               *
//*                                              *
//************************************************

// Runs digitization/calibration of the whole measurement
// series. Logfile containing list of measurement names and
// corresponding source positions is opened. Digitization
// is performed for the measurements included in this logfile. 
// For each measurement program digit (part of the DesktopDigitizer6)
// is executed, so correct configuration files config.txt should
// be provided. 
//
// To compile type:
//   gcc Calibrate.C -lstdc++
//
// To run type:
//   ./Calibrate.o path/to/log.txt

#include <iostream>
#include <fstream>
#include <string> 
#include <stdlib.h>
#include <cstring>

int main(int argc, char **argv){
  
  if(argc!=2){
    std::cout << "to run type: ./Calibrate.o path/to/log.txt" << std::endl;
    return 0;
  }
  
  std::ifstream log(argv[1]); 
  
  if(!log.is_open()){
   std::cout << "Couldn't open log file!" << std::endl;
   return 0;
  }
  
  const char *data_path = std::getenv("SFDATA");
  std::string dummy;
  std::string meas_name;
  std::string meas_path;
  int status = 0;
  
  while(log.good()){
   getline(log,dummy);
   log >> meas_name;
   meas_path = std::string(data_path)+meas_name+"/";
   getline(log,dummy);
   getline(log,dummy);
   getline(log,dummy);
   std::cout << "\n\nDIGITIZING MEASUREMENT " << meas_path << std::endl;
   char command[50] = "digit ";  
   strcat(command,meas_path.c_str());
   puts(command);
   status = system(command);
   std::cout << "Status: " << status << std::endl;
   if(status==0){
     std::cerr << "\n\nERROR OCCURED!" << std::endl;
     std::cerr << command << std::endl;
     return 0;
   }
  }
  
  return 1;
}
