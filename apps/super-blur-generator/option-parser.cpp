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

/**
 * SuperBlurGerator creates blur levels from an image file to be used with
 * the SuperBlurView
 */

#include "option-parser.h"

#include <cstdlib>
#include <utility>
#include <getopt.h>

#include "logging.h"

namespace
{

const char* INPUT_FILE_NAME_OPTION( "input-image" );
const char* INPUT_FILE_NAME_OPTION_DESCRIPTION(
    "File name of the image that is will be blurred." );

const char* OUTPUT_FOLDER_NAME_OPTION( "output-folder" );
const char* OUTPUT_FOLDER_NAME_OPTION_DESCRIPTION(
    "Path to folder where the blur images will be written to." );

const char* OUTPUT_FILE_NAME_OPTION( "output-image" );
const char* OUTPUT_FILE_NAME_OPTION_DESCRIPTION(
    "Name of the blurred image." );

const char* BLUR_LEVEL_OPTION( "blur-level" );
const char* BLUR_LEVEL_OPTION_DESCRIPTION(
    "The amount to blur." );

enum Option {
  INPUT_FILE_NAME,
  OUTPUT_FOLDER_NAME,
  OUTPUT_FILE_NAME,
  BLUR_LEVEL,
};

static const option LONG_OPTIONS[] =
{
  { INPUT_FILE_NAME_OPTION, required_argument, 0, 0 },
  { OUTPUT_FOLDER_NAME_OPTION, required_argument, 0, 0 },
  { OUTPUT_FILE_NAME_OPTION, required_argument, 0, 0 },
  { BLUR_LEVEL_OPTION, required_argument, 0, 0 },
};

} // namespace


class OptionParserImpl
{
public:
  OptionParserImpl()
  {
  }

  bool ParseOptions( int argc, char** argv )
  {
    bool failed = false;
    while ( ! failed )
    {
      int optionIndex = 0;
      int opt = getopt_long_only( argc, argv, "", LONG_OPTIONS, &optionIndex );
      if ( opt == -1 )
      {
        break;
      }
      switch ( opt )
      {
        case 0: // Long options, it's the only case we are handling
        {
          ParseOption( Option( optionIndex ) );
          break;
        }
        default:
        {
          LOGW( "Invalid option 0%o\n", opt );
          failed = true;
        }
      }
    }
    return ( ! failed ) &&
        ( ! mImageFileName.empty() ) &&
        ( ! mOutputFolderPath.empty() );
  }

  const std::string& GetImageFileName() const
  {
    return mImageFileName;
  }

  const std::string& GetOutputFolderPath() const
  {
    return mOutputFolderPath;
  }

  const std::string& GetOutputFileName() const
  {
    return mOutputFileName;
  }

  int GetBlurLevel() const
  {
    return mBlurLevel;
  }

  void PrintUsage( const char* command )
  {
    LOGE( "Usage: %s --%s <%s> --%s <%s> --%s <%s>\n",
          command,
          INPUT_FILE_NAME_OPTION,
          INPUT_FILE_NAME_OPTION,
          OUTPUT_FOLDER_NAME_OPTION,
          OUTPUT_FOLDER_NAME_OPTION,
          OUTPUT_FILE_NAME_OPTION,
          OUTPUT_FILE_NAME_OPTION,
          BLUR_LEVEL_OPTION,
          BLUR_LEVEL_OPTION);
    LOGE( "\t--%s <%s>\t\t%s\n",
          INPUT_FILE_NAME_OPTION,
          INPUT_FILE_NAME_OPTION,
          INPUT_FILE_NAME_OPTION_DESCRIPTION );
    LOGE( "\t--%s <%s>\t\t%s\n",
          OUTPUT_FOLDER_NAME_OPTION,
          OUTPUT_FOLDER_NAME_OPTION,
          OUTPUT_FOLDER_NAME_OPTION_DESCRIPTION );
    LOGE( "\t--%s <%s>\t\t%s\n",
          OUTPUT_FILE_NAME_OPTION,
          OUTPUT_FILE_NAME_OPTION,
          OUTPUT_FILE_NAME_OPTION_DESCRIPTION );
    LOGE( "\t--%s <%s>\t\t%s\n",
          BLUR_LEVEL_OPTION,
          BLUR_LEVEL_OPTION,
          BLUR_LEVEL_OPTION_DESCRIPTION );
  }

private:

  bool ParseOption( Option option )
  {
    bool failed = false;
    switch ( option )
    {
      case INPUT_FILE_NAME:
      {
        mImageFileName = std::string( optarg );
        LOGV( "Option \'%s\' has value \'%s\'\n",
              INPUT_FILE_NAME_OPTION,
              optarg );
        break;
      }
      case OUTPUT_FOLDER_NAME:
      {
        mOutputFolderPath = std::string( optarg );
        LOGV( "Option \'%s\' has value \'%s\'\n",
              OUTPUT_FOLDER_NAME_OPTION,
              optarg );
        break;
      }
      case OUTPUT_FILE_NAME:
      {
        mOutputFileName = std::string( optarg );
        LOGV( "Option \'%s\' has value \'%s\'\n",
              OUTPUT_FILE_NAME_OPTION,
              optarg );
        break;
      }
      case BLUR_LEVEL:
      {
        mBlurLevel = atoi( optarg );
        LOGV( "Option \'%s\' has value \'%d\'\n",
              BLUR_LEVEL_OPTION,
              mBlurLevel );
        break;
      }
      default:
      {
        failed = true;
        LOGW( "Invalid option 0%o\n", option );
        break;
      }
    }
    return !failed;
  }

  std::string mImageFileName;
  std::string mOutputFolderPath;
  std::string mOutputFileName;
  int mBlurLevel;
};

OptionParser::OptionParser()
: mImpl( new OptionParserImpl() )
{
}

bool OptionParser::ParseOptions( int argc, char** argv )
{
  return GetImpl().ParseOptions( argc, argv );
}

OptionParser::~OptionParser()
{
}

const std::string& OptionParser::GetImageFileName() const
{
  return GetImpl().GetImageFileName();
}

const std::string& OptionParser::GetOutputFolderPath() const
{
  return GetImpl().GetOutputFolderPath();
}

const std::string& OptionParser::GetOutputFileName() const
{
  return GetImpl().GetOutputFileName();
}

int OptionParser::GetBlurLevel() const
{
  return GetImpl().GetBlurLevel();
}

void OptionParser::PrintUsage( const char* command )
{
  GetImpl().PrintUsage( command );
}
