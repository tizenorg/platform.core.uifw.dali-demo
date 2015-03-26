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


#include <string>
#include "dali-app.h"
using namespace Dali;




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


  DaliApplication daliApplication( Application::New( &argc, &argv ), layoutFileName, javascriptFileName );

  daliApplication.MainLoop();

  return 0;
}
