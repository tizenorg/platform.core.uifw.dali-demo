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
const int NUM_TEST_ITERATIONS = 100; // Currently segfaults at around 86 to 91 on desktop for COMMON debug builds.
}

void AppInit( Dali::Application& application )
{
  application.Quit();
}

int main( int argc, char * argv[] )
{
  std::cerr << "Repeatedly running Application objects." << std::endl;
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
