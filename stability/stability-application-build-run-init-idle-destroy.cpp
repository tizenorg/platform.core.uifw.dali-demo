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
#include <iostream>
#include <dali/dali.h>
#include <dali-toolkit/dali-toolkit.h>

/*
Crash:
Program received signal SIGSEGV, Segmentation fault.
0x00007ffff78a5a15 in Dali::Adaptor::AddIdle(boost::function<void ()>) (this=0x7ffff7bce192, callBack=...) at ../../../adaptors/public-api/adaptor-framework/adaptor.cpp:76
(gdb) bt
#0  0x00007ffff78a5a15 in Dali::Adaptor::AddIdle(boost::function<void ()>) (this=0x7ffff7bce192, callBack=...) at ../../../adaptors/public-api/adaptor-framework/adaptor.cpp:76
#1  0x00007ffff7bce6b4 in Dali::Internal::Adaptor::Application::AddIdle(boost::function<void ()>) (this=0x7fffffffd720, callBack=...) at ../../../adaptors/common/application-impl.cpp:254
#2  0x00007ffff7bce161 in Dali::Internal::Adaptor::Application::Quit (this=0x7fffffffd720) at ../../../adaptors/common/application-impl.cpp:155
#3  0x00007ffff7bcbc70 in Dali::Application::Quit (this=0x7fffffffd6e0) at ../../../adaptors/public-api/adaptor-framework/application.cpp:101
#4  0x000000000040168e in AppIdle () at ../../../stability/stability-application-build-run-init-idle-destroy.cpp:32
#7  0x00007ffff78b5188 in Dali::Internal::Adaptor::(anonymous namespace)::IdleCallback (data=0x6765d0) at ../../../adaptors/common/ecore-callback-manager.cpp:101
#19 0x00007ffff7bcbc2c in Dali::Application::MainLoop (this=0x7fffffffdca0) at ../../../adaptors/public-api/adaptor-framework/application.cpp:91
#20 0x0000000000401890 in main (argc=1, argv=0x7fffffffdda8) at ../../../stability/stability-application-build-run-init-idle-destroy.cpp:51

The segv happens when Application tries to add an idle handler to do the quit.

void Application::Quit()
{
  // Actually quit the application.
  AddIdle(boost::bind(&Application::QuitFromMainLoop, this));
}
*/

namespace
{
const int NUM_TEST_ITERATIONS = 100; // Segfaults on iteration 0 at present.
}


Dali::Application * gCurrentApp = 0;

void AppIdle(  )
{
  DALI_ASSERT_ALWAYS( gCurrentApp != 0 );
  gCurrentApp->Quit();
}

void AppInit( Dali::Application& application )
{
  gCurrentApp = &application;
  DALI_ASSERT_ALWAYS( gCurrentApp != 0 );
  const bool addedIdle = application.AddIdle( AppIdle );
  DALI_ASSERT_ALWAYS( addedIdle );
}

int main( int argc, char * argv[] )
{
  std::cerr << "Repeatedly running Application objects to idle." << std::endl;
  for( int i = 0; i < NUM_TEST_ITERATIONS; ++i )
  {
    std::cerr << "Test iteration " << i << std::endl;
    Dali::Application application = Dali::Application::New( &argc, &argv );
    application.InitSignal().Connect( AppInit );
    application.MainLoop();
  }
  std::cerr << std::endl << "Executed "<< NUM_TEST_ITERATIONS << " iterations and exiting cleanly." << std::endl;
  return 0;
}
