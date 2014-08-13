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
#include <cstdio>
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

const char* DOWNSCALE_OPTION( "downscale" );
const char* DOWNSCALE_OPTION_DESCRIPTION(
  "The downscale factor (to calculate the size of the first framebuffer)." );

const char* OUTPUT_SCALE_OPTION( "output-scale" );
const char* OUTPUT_SCALE_OPTION_DESCRIPTION(
  "The output scale factor (to calculate the size of the last framebuffer)." );

const char* FRAMEBUFFER_SIZE_OPTION( "framebuffer-size" );
const char* FRAMEBUFFER_SIZE_OPTION_DESCRIPTION(
  "The framebuffer size" );

enum Option {
  INPUT_FILE_NAME,
  OUTPUT_FOLDER_NAME,
  OUTPUT_FILE_NAME,
  BLUR_LEVEL,
  DOWNSCALE,
  OUTPUT_SCALE,
  FRAMEBUFFER_SIZE
};

static const option LONG_OPTIONS[] =
{
  { INPUT_FILE_NAME_OPTION, required_argument, 0, 0 },
  { OUTPUT_FOLDER_NAME_OPTION, required_argument, 0, 0 },
  { OUTPUT_FILE_NAME_OPTION, required_argument, 0, 0 },
  { BLUR_LEVEL_OPTION, required_argument, 0, 0 },
  { DOWNSCALE_OPTION, required_argument, 0, 0 },
  { OUTPUT_SCALE_OPTION, required_argument, 0, 0},
  { FRAMEBUFFER_SIZE_OPTION, required_argument, 0, 0},
};

} // namespace


class OptionParserImpl
{
public:
  OptionParserImpl()
  : mBlurLevel(1),
    mDownscale(1.0f),
    mOutputScale(1.0f),
    mFramebufferSize(240.0f, 400.0f)
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

  float GetDownscale() const
  {
    return mDownscale;
  }

  float GetOutputScale() const
  {
    return mOutputScale;
  }

  Vector2 GetFramebufferSize() const
  {
    return mFramebufferSize;
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
          BLUR_LEVEL_OPTION,
          DOWNSCALE_OPTION,
          DOWNSCALE_OPTION,
          OUTPUT_SCALE_OPTION,
          OUTPUT_SCALE_OPTION,
          FRAMEBUFFER_SIZE_OPTION,
          FRAMEBUFFER_SIZE_OPTION);
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
    LOGE( "\t--%s <%s>\t\t%s\n",
          DOWNSCALE_OPTION,
          DOWNSCALE_OPTION,
          DOWNSCALE_OPTION_DESCRIPTION );
    LOGE( "\t--%s <%s>\t\t%s\n",
          OUTPUT_SCALE_OPTION,
          OUTPUT_SCALE_OPTION,
          OUTPUT_SCALE_OPTION_DESCRIPTION );
    LOGE( "\t--%s <%s>\t\t%s\n",
          FRAMEBUFFER_SIZE_OPTION,
          FRAMEBUFFER_SIZE_OPTION,
          FRAMEBUFFER_SIZE_OPTION_DESCRIPTION );
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
      case DOWNSCALE:
      {
        mDownscale = atof(optarg);
        LOGV( "Option \'%s\' has value \'%f\'\n",
              DOWNSCALE_OPTION,
              mDownscale );
        break;
      }
      case OUTPUT_SCALE:
      {
        mOutputScale = atof(optarg);
        LOGV( "Option \'%s\' has value \'%f\'\n",
              OUTPUT_SCALE_OPTION,
              mOutputScale );
        break;
      }
      case FRAMEBUFFER_SIZE:
      {
        int width, height;
        sscanf(optarg, "%dx%d", &width, &height);
        mFramebufferSize = Vector2( width, height );
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
  float mDownscale;
  float mOutputScale;
  Vector2 mFramebufferSize;
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

float OptionParser::GetDownscale() const
{
  return GetImpl().GetDownscale();
}

float OptionParser::GetOutputScale() const
{
  return GetImpl().GetOutputScale();
}

Vector2 OptionParser::GetFramebufferSize() const
{
  return GetImpl().GetFramebufferSize();
}

void OptionParser::PrintUsage( const char* command )
{
  GetImpl().PrintUsage( command );
}
