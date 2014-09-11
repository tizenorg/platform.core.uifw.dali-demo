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
const int NUM_TEST_ITERATIONS = 100;
}

void AppIdle()
{
}

int main( int argc, char * argv[] )
{
  std::cerr << "Repeatedly creating Application objects and calling member funcs on them." << std::endl;
  for( int i = 0; i < NUM_TEST_ITERATIONS; ++i )
  {
    std::cerr << "Test iteration " << i << std::endl;
    Dali::Application application = Dali::Application::New( &argc, &argv );

    // Do all the gets ahead of any sets:
    Dali::Orientation orientation = application.GetOrientation();
    Dali::ViewMode viewMode = application.GetViewMode();
    float stereoBase = application.GetStereoBase();
    Dali::Window window = application.GetWindow();

    // Perform all modifying operations:
    application.SetViewMode( viewMode );
    application.SetStereoBase( stereoBase );
    application.Lower();
    const bool addedIdle = application.AddIdle( AppIdle );
    DALI_ASSERT_ALWAYS( addedIdle );
    application.Quit();

    // See if the application will still start after it has been prodded:
    application.MainLoop();
  }
  std::cerr << std::endl << "Executed "<< NUM_TEST_ITERATIONS << " iterations and exiting cleanly." << std::endl;
  return 0;
}
