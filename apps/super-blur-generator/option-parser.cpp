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

/**
 * SuperBlurGerator creates blur levels from an image file to be used with
 * the SuperBlurView
 */

#include "option-parser.h"

#include<utility>
#include<getopt.h>

#include"logging.h"

namespace
{

const char* INPUT_FILE_NAME_OPTION( "input-image" );
const char* INPUT_FILE_NAME_OPTION_DESCRIPTION(
    "File name of the image that is will be blurred." );

const char* OUTPUT_FOLDER_NAME_OPTION( "output-folder" );
const char* OUTPUT_FOLDER_NAME_OPTION_DESCRIPTION(
    "Path to folder where the blur images will be written to." );

enum Option {
  INPUT_FILE_NAME,
  OUTPUT_FOLDER_NAME
};

static const option LONG_OPTIONS[] =
{
  { INPUT_FILE_NAME_OPTION, required_argument, 0, 0 },
  { OUTPUT_FOLDER_NAME_OPTION, required_argument, 0, 0 },
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

  void PrintUsage( const char* command )
  {
    LOGE( "Usage: %s --%s <%s> --%s <%s>\n",
          command,
          INPUT_FILE_NAME_OPTION,
          INPUT_FILE_NAME_OPTION,
          OUTPUT_FOLDER_NAME_OPTION,
          OUTPUT_FOLDER_NAME_OPTION );
    LOGE( "\t--%s <%s>\t\t%s\n",
          INPUT_FILE_NAME_OPTION,
          INPUT_FILE_NAME_OPTION,
          INPUT_FILE_NAME_OPTION_DESCRIPTION );
    LOGE( "\t--%s <%s>\t\t%s\n",
          OUTPUT_FOLDER_NAME_OPTION,
          OUTPUT_FOLDER_NAME_OPTION,
          OUTPUT_FOLDER_NAME_OPTION_DESCRIPTION );
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

void OptionParser::PrintUsage( const char* command )
{
  GetImpl().PrintUsage( command );
}

