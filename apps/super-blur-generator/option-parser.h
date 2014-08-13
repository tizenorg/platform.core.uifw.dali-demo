#ifndef SUPER_BLUR_GENERATOR_OPTION_PARSER_H_
#define SUPER_BLUR_GENERATOR_OPTION_PARSER_H_

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

#include<string>

#include "implementation-pointer.h"

class OptionParserImpl;

/**
 * Application option parser
 */
class OptionParser
{
public:
  /**
   * Constructor
   */
  OptionParser();

  /**
   * Parse options
   * @param[in] argc Number of arguments in argv
   * @param[in] argv Array of null terminated char* representing the arguments
   * @return false if parsing options failed.
   */
  bool ParseOptions( int argc, char** argv );

  /**
   * Destructor
   */
  ~OptionParser();

  /**
   * Returns the filename of the image to be blurred
   * @return string with the image file name
   */
  const std::string& GetImageFileName() const;

  /**
   * Returns the path to the folder where the blur levels should be saved
   * @return string with the path to save the files
   */
  const std::string& GetOutputFolderPath() const;

  const std::string& GetOutputFileName() const;

  /**
   * Prints the usage for the command
   * @param[in] command Execution command for this app
   */
  void PrintUsage( const char* command );

private:
  ImplementationPointer<OptionParserImpl> mImpl;

public:
  OptionParserImpl& GetImpl()
  {
    return *mImpl;
  }
  const OptionParserImpl& GetImpl() const
  {
    return *mImpl;
  }
};

#endif // SUPER_BLUR_GENERATOR_OPTION_PARSER_H_
