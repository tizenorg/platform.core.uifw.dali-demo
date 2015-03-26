/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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

// CLASS HEADER
#include "launcher.h"

// EXTERNAL INCLUDES
#include <string>



int main( int argc, char* argv[] )
{
  // pull out the JSON file and JavaScript file from the command line arguments
  std::string javascriptFileName = "";
  std::string layoutFileName;

  for( int i = 1 ; i < argc ; ++i )
  {
    std::string arg( argv[i] );

    size_t idx = std::string::npos;

    idx = arg.find( ".json" );
    if( idx != std::string::npos )
    {
      layoutFileName = arg;
    }
    else
    {
      idx = arg.find( ".js" );
      if( idx != std::string::npos )
      {
        javascriptFileName = arg;
      }

    }
  }

  Launcher daliApplication( Dali::Application::New( &argc, &argv ), layoutFileName, javascriptFileName );

  daliApplication.MainLoop();

  return 0;
}
