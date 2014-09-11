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

namespace
{
const int NUM_TEST_ITERATIONS = 100; //< segfaults on first iteration.
}
/*
 Segfault happens because of null adaptor at this point:
 (gdb) bt
  #0  0x00007ffff78a5a11 in Dali::Adaptor::AddIdle(boost::function<void ()>) (this=0x0, callBack=...) at ../../../adaptors/public-api/adaptor-framework/adaptor.cpp:76
  #1  0x00007ffff7bce6b4 in Dali::Internal::Adaptor::Application::AddIdle(boost::function<void ()>) (this=0x615d50, callBack=...) at ../../../adaptors/common/application-impl.cpp:254
  #2  0x00007ffff7bcbd5f in Dali::Application::AddIdle(boost::function<void ()>) (this=0x7fffffffdcb0, callBack=...) at ../../../adaptors/public-api/adaptor-framework/application.cpp:116
  #3  0x000000000040177a in main (argc=1, argv=0x7fffffffddb8) at ../../../stability/stability-application-build-run-idle-destroy.cpp:47
 */

Dali::Application * gCurrentApp = 0;

void AppIdle(  )
{
  DALI_ASSERT_ALWAYS( gCurrentApp != 0 );
  gCurrentApp->Quit();
}

int main( int argc, char * argv[] )
{
  std::cerr << "Repeatedly running Application objects to idle." << std::endl;
  for( int i = 0; i < NUM_TEST_ITERATIONS; ++i )
  {
    std::cerr << "Test iteration " << i << std::endl;
    Dali::Application application = Dali::Application::New( &argc, &argv );
    gCurrentApp = &application;
    const bool addedIdle = application.AddIdle( AppIdle ); //< Segfault every time (null adaptor).
    DALI_ASSERT_ALWAYS( addedIdle );
    application.MainLoop();
  }
  std::cerr << std::endl << "Executed "<< NUM_TEST_ITERATIONS << " iterations and exiting cleanly." << std::endl;
  return 0;
}
