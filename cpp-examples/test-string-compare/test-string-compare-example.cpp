/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include <strings.h>
#include <dali/dali.h>
#include <iostream>
#include <dali-toolkit/dali-toolkit.h>

using namespace Dali;

// This example shows how to create and display Hello World! using a simple TextActor
//
class HelloWorldController : public ConnectionTracker
{
public:

  HelloWorldController( Application& application )
  : mApplication( application )
  {
    std::cout << "HelloWorldController::HelloWorldController" << std::endl;

    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &HelloWorldController::Create );
  }

  ~HelloWorldController()
  {
    // Nothing to do here;
  }

  // The Init signal is received once (only) during the Application lifetime
  void Create( Application& application )
  {
    std::cout << "HelloWorldController::Create" << std::endl;

    ///////////////////////////////////
    // performance test
    using namespace std;

    uint count = 100000000;
    clock_t start, end;
    string teststr;

    cout << "count " << count << endl;
    cout << endl;
      
    start = clock();
    for(uint i=0; i<count; ++i)
    {
      if(true) {}
    }
    end = clock();
    cout << "case: blank" << endl;
    cout << "elapsed time (ms) " << (end - start) * 1000 / CLOCKS_PER_SEC << endl;
    cout << endl;

    teststr = "localfilelocalfile";
    start = clock();
    for(uint i=0; i<count; ++i)
    {
      if(strncasecmp( teststr.c_str(), "http://", 7 )==0) {}
    }
    end = clock();
    cout << "case: one strncasecmp with localfile" << endl;
    cout << "elapsed time (ms) " << (end - start) * 1000 / CLOCKS_PER_SEC << endl;
    cout << endl;

    teststr = "localfilelocalfile";
    start = clock();
    for(uint i=0; i<count; ++i)
    {
      if(strncasecmp( teststr.c_str(), "http://", 7 )==0 ||
         strncasecmp( teststr.c_str(), "https://", 8 )==0 ) {}
    }
    end = clock();
    cout << "case: two strncasecmp with localfile" << endl;
    cout << "elapsed time (ms) " << (end - start) * 1000 / CLOCKS_PER_SEC << endl;
    cout << endl;

    teststr = "http://mindprod.com/image/coord/pixelgrid.png";
    start = clock();
    for(uint i=0; i<count; ++i)
    {

      if(strncasecmp( teststr.c_str(), "http://", 7 )==0) {}
    }
    end = clock();
    cout << "case: one strncasecmp with http address" << endl;
    cout << "elapsed time (ms) " << (end - start) * 1000 / CLOCKS_PER_SEC << endl;
    cout << endl;

    teststr = "http://mindprod.com/image/coord/pixelgrid.png";
    start = clock();
    for(uint i=0; i<count; ++i)
    {

      if(strncasecmp( teststr.c_str(), "http://", 7 )==0 ||
         strncasecmp( teststr.c_str(), "https://", 8 )==0 ) {}
    }
    end = clock();
    cout << "case: two strncasecmp with http address" << endl;
    cout << "elapsed time (ms) " << (end - start) * 1000 / CLOCKS_PER_SEC << endl;
    cout << endl;

    teststr = "localfilelocalfile";
    start = clock();
    for(uint i=0; i<count; ++i)
    {
      if( (teststr[0]=='h' || teststr[0]=='H') &&
          (teststr[1]=='t' || teststr[1]=='T') &&
          (teststr[2]=='t' || teststr[2]=='T') &&
          (teststr[3]=='p' || teststr[3]=='P') &&
          (teststr[4]==':') &&
          (teststr[5]=='/') &&
          (teststr[6]=='/') ) {}
    }
    end = clock();
    cout << "case: char comparison with localfile" << endl;
    cout << "elapsed time (ms) " << (end - start) * 1000 / CLOCKS_PER_SEC << endl;
    cout << endl;

    teststr = "http://mindprod.com/image/coord/pixelgrid.png";
    start = clock();
    for(uint i=0; i<count; ++i)
    {
      if( (teststr[0]=='h' || teststr[0]=='H') &&
          (teststr[1]=='t' || teststr[1]=='T') &&
          (teststr[2]=='t' || teststr[2]=='T') &&
          (teststr[3]=='p' || teststr[3]=='P') &&
          (teststr[4]==':') &&
          (teststr[5]=='/') &&
          (teststr[6]=='/') ) {}
    }
    end = clock();
    cout << "case: char comparison with http address" << endl;
    cout << "elapsed time (ms) " << (end - start) * 1000 / CLOCKS_PER_SEC << endl;
    cout << endl;


//|| case: blank
//|| elapsed time (ms) 201
//|| 
//|| case: one strncasecmp with localfile
//|| elapsed time (ms) 672
//|| 
//|| case: two strncasecmp with localfile
//|| elapsed time (ms) 1318
//|| 
//|| case: one strncasecmp with http address
//|| elapsed time (ms) 672
//|| 
//|| case: two strncasecmp with http address
//|| elapsed time (ms) 699
//|| 
//|| case: char comparison with localfile
//|| elapsed time (ms) 646
//|| 
//|| case: char comparison with http address
//|| elapsed time (ms) 2036

    mApplication.Quit();
  }

  bool OnTouch( Actor actor, const TouchEvent& touch )
  {
    // quit the application
    //mApplication.Quit();
    return true;
  }

private:
  Application&  mApplication;
};

void RunTest( Application& application )
{
  HelloWorldController test( application );

  application.MainLoop();
}

// Entry point for Linux & SLP applications
//
int main( int argc, char **argv )
{
  Application application = Application::New( &argc, &argv );

  RunTest( application );

  return 0;
}
