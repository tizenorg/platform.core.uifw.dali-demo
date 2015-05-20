//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
#include <iostream>
#include <fstream>
#include <sstream>
#include <assert.h>
#include <sys/time.h>
#include <vector>
#include <stdlib.h>
#include <iterator>
#include <unistd.h>

const int REQUIRED_ARGS(2);

void WriteFile(const char* fileName, const char* content)
{
  ofstream outFile(fileName);
  if(outFile.is_open())
  {
    outFile << content;
    outFile.close();
  }
}

void ShowFileContent(const char* fileName)
{
  ifstream inFile(fileName);
  if(inFile.is_open())
  {
    string line;
    getline (inFile,line);
    cout<<fileName<<": "<<line<<endl;
    inFile.close();
  }
}

int main( int argc, char **argv )
{
  if(argc <= REQUIRED_ARGS)
  {
    cout << "Performance Gather - Copyright (c) 2000-2013 Samsung Electronics Co., Ltd All Rights Reserved" << endl;
    cout << endl;
    cout << "Usage: performance-gather <performance testing app> <seconds>" << endl;
    return 0;
  }

  int num;
  istringstream ( argv[2] ) >> num;
  if(num <= 0)
  {
     cout << "the second argument value = " << num << ": should be an positive integer" <<endl;
     return 0;
  }
  unsigned int num_records = static_cast<unsigned int>(num);

  /**
   * The /proc/pid/stat file contains status information about a process with the specified PID
   * The file is a single-line text file with entries separated by whitespaces.
   */
  //CPU time spent in user code, measured in jiffies; the 14th field in /proc/<pid>/stat
  vector<unsigned int> utimeVec;
  utimeVec.resize(num_records);
  assert(utimeVec.capacity() == num_records);
  //CPU time spent in kernel code, measured in jiffies; the 15th field in /proc/<pid>/stat
  vector<unsigned int> stimeVec;
  stimeVec.resize(num_records);
  assert(stimeVec.capacity() == num_records);
  //size of the process' resident set (in bytes); the 24th field in /proc/<pid>/stat
  vector<unsigned int> rssVec;
  rssVec.resize(num_records);
  assert(rssVec.capacity() == num_records);

  cout<<"==========Set the system up for performance========="<<endl;
  // Enable all CPU cores on device, On Redwood
  //echo 1 > /sys/devices/system/cpu/cpu1/online
  WriteFile("/sys/devices/system/cpu/cpu1/online", "1");
  //echo 1 > /sys/devices/system/cpu/cpu2/online
  WriteFile("/sys/devices/system/cpu/cpu2/online", "1");
  //echo 1 > /sys/devices/system/cpu/cpu3/online
  WriteFile("/sys/devices/system/cpu/cpu3/online", "1");

  //You can set the minimum clock speed by writing the value in kHz to sysfs
  // Set the minimum frequency to match the maximum frequency:
  // cat /sys/devices/system/cpu/cpu0/cpufreq/scaling_max_freq
  // > /sys/devices/system/cpu/cpu0/cpufreq/scaling_min_freq
  ifstream inFile("/sys/devices/system/cpu/cpu0/cpufreq/scaling_max_freq");
  string maxFreq;
  if(inFile.is_open())
  {
     getline (inFile,maxFreq);
     cout<<"/sys/devices/system/cpu/cpu0/cpufreq/scaling_max_freq: "<<maxFreq<<endl;
     inFile.close();
  }
  WriteFile( "/sys/devices/system/cpu/cpu0/cpufreq/scaling_min_freq", maxFreq.c_str() );
  ShowFileContent( "/sys/devices/system/cpu/cpu0/cpufreq/scaling_min_freq" );

 // to ensure mali is running full clock speed:(440MHz for Proxima, 533MHz for Redwood)
 // echo 533 > /sys/module/mali/parameters/mali_dvfs_control
  WriteFile( "/sys/module/mali/parameters/mali_dvfs_control", "533" );
  ShowFileContent( "/sys/module/mali/parameters/mali_dvfs_control" );

  // check by cat /sys/devices/system/cpu/cpu0/cpufreq/scaling_governor
  //if ondemand then cpu will sleep
  WriteFile( "/sys/devices/system/cpu/cpu0/cpufreq/scaling_governor", "performance" );
  ShowFileContent( "/sys/devices/system/cpu/cpu0/cpufreq/scaling_governor" );

  //To check current cpu frequency
  //cat /sys/devices/system/cpu/cpu0/cpufreq/cpuinfo_cur_freq
  ShowFileContent( "/sys/devices/system/cpu/cpu0/cpufreq/cpuinfo_cur_freq" );

  //To check how many cpu cores are active type:
  //cat /proc/cpuinfo | grep processor
  inFile.open( "/proc/cpuinfo" );
  if ( inFile.is_open() )
   {
     int numActiveCores = 0;
     string str;
     while( inFile.good() )
     {
       getline( inFile, str );
       //count how many lines started with 'processor'
       if( str.compare( 0, 9, "processor") == 0 )
       {
         numActiveCores++;
       }
     }
     inFile.close();
     cout<<"/proc/cpuinfo: " << numActiveCores << " cpu cores are active"<<endl;
   }
  cout<<endl;

  unsigned int recordIndex = 0;
  stringstream filename;

  // use fork to start the performance test app
  stringstream path;
  path <<  argv[1];
  pid_t pid;
  switch( pid = fork() )
  {
    case -1:
    {
      // Error while creating a new process.
       cout<<"Error while creating a new process"<<endl;
       exit (-1);
       break;
    }
    case 0:
    {
      // Child process.
      // Replace current (child) process by selected demo process.
      int success = execl( path.str().c_str(), path.str().c_str(), NULL );
      if( -1 == success )
      {
        std::string error( "Error when calling the " );
        error += path.str();
        cout<<error<<endl;
        exit (-1);
      }
      break;
    }
    default:
    {
      // Parent process. Nothing to do.
      (void)pid; // avoid a compiler error.
    }
  }

  //read performance metrics from /proc/<pid>/stat
  filename << "/proc/" << pid << "/stat";
  ifstream statFile;

  while(recordIndex < num_records)
  {
    sleep(1);
    statFile.open( filename.str().c_str() );
    if ( statFile.is_open() )
    {
      // read the space separated content from file and put into the string vector
      std::vector<std::string> content ((istream_iterator<string>(statFile)),
                                         istream_iterator<string>());
      statFile.close();
      // write the 14th fields 'utime', 15th fields 'stime' and 24th fields 'rrs' into vector
      istringstream ( content[13] ) >> utimeVec[recordIndex];
      istringstream ( content[14] ) >> stimeVec[recordIndex];
      istringstream ( content[23] ) >> rssVec[recordIndex];
    }
    else
    {
       break;
    }
    ++recordIndex;
  }

  sleep(1);
  cout << endl
       << "output metrics: "
       << "utime differences" << ", "
       << "stime differences" << ", "
       << "rss";
  const char* fpsTracking = getenv("DALI_FPS_TRACKING");
  if(fpsTracking)
  {
    inFile.open("/tmp/dalifps.txt");
    cout << ", " << "fps";
  }
  cout << endl;
  string str;
  //first record
  cout << utimeVec[0] << ", " << stimeVec[0] << ", " << rssVec[0];
  if(inFile.good())
  {
    getline( inFile, str );
    cout<<", "<<str;
  }
  cout<<endl;
  //following records
  for( size_t row = 1; row < num_records; row++ )
  {
    cout << utimeVec[row]-utimeVec[row-1] << ", "
         << stimeVec[row]-stimeVec[row-1] << ", "
         << rssVec[row];
    if(inFile.good())
    {
      getline( inFile, str );
      cout<<", "<<str;
    }
    cout<<endl;
  }
  inFile.close();
  return 0;
}


