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

#ifndef SUPER_BLUR_GENERATOR_OPTION_PARSER_H_
#define SUPER_BLUR_GENERATOR_OPTION_PARSER_H_

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
