/*
Copyright (c) 2000-2013 Samsung Electronics Co., Ltd All Rights Reserved

This file is part of Dali Adaptor

PROPRIETARY/CONFIDENTIAL

This software is the confidential and proprietary information of
SAMSUNG ELECTRONICS ("Confidential Information"). You shall not
disclose such Confidential Information and shall use it only in
accordance with the terms of the license agreement you entered
into with SAMSUNG ELECTRONICS.

SAMSUNG make no representations or warranties about the suitability
of the software, either express or implied, including but not limited
to the implied warranties of merchantability, fitness for a particular
purpose, or non-infringement. SAMSUNG shall not be liable for any
damages suffered by licensee as a result of using, modifying or
distributing this software or its derivatives.
*/

#include<cstdio>
#include<string>

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

  Dali::BitmapLoader image = Dali::BitmapLoader::New( imageFilePath );
  SuperBlurGenerator blurGenerator( image, outputFolderPath );

  blurGenerator.Generate( 0.1f );

  return 0;
}
