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
 */

#include <cstdio>
#include <string>

#include "logging.h"
#include "option-parser.h"
#include "super-blur-generator.h"

int main( int argc, char **argv )
{
  OptionParser optionParser;
  if ( ! optionParser.ParseOptions( argc, argv ) )
  {
    LOGE( "There were errors while parsing the parameters\n" );
    optionParser.PrintUsage( argv[0] );
    return -1;
  }

  const std::string& imageFilePath = optionParser.GetImageFileName();
  const std::string& outputFolderPath = optionParser.GetOutputFolderPath();
  const std::string& outputFileName = optionParser.GetOutputFileName();
  int blurLevel = optionParser.GetBlurLevel();

  Dali::BitmapLoader image = Dali::BitmapLoader::New( imageFilePath );
  if( image.GetPixelData() == NULL )
  {
    printf("File not found\n");
  }
  else
  {
    // view set to targetSize; output framebuffer.width = targetSize.width / (2^(blurLevel+1))

    SuperBlurGenerator blurGenerator( image, outputFolderPath, outputFileName, blurLevel );
    blurGenerator.Generate();
  }
  return 0;
}
