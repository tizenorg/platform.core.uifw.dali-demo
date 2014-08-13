#ifndef SUPER_BLUR_GENERATOR_SUPER_BLUR_GENERATOR_H_
#define SUPER_BLUR_GENERATOR_SUPER_BLUR_GENERATOR_H_

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
#include "program.h"
#include <dali/dali.h>

class SuperBlurGeneratorImpl;

class SuperBlurGenerator
{
public:
  enum Direction
  {
    HORIZONTAL,
    VERTICAL
  };

  /**
   * Constructor
   * @param[in] Filename of the image to generate blur
   */
  SuperBlurGenerator( const Dali::BitmapLoader& inputImage, const std::string& outputFolder, const std::string& outputFileName );

  /**
   * Destructor
   */
  ~SuperBlurGenerator();

  /**
   * Generate image
   */
  void Generate( float level );

private:
  ImplementationPointer<SuperBlurGeneratorImpl> mImpl;

  void SetKernelUniforms(Program program, Direction direction);

public:
  SuperBlurGeneratorImpl& GetImpl()
  {
    return *mImpl;
  }
  const SuperBlurGeneratorImpl& GetImpl() const
  {
    return *mImpl;
  }
};

#endif // SUPER_BLUR_GENERATOR_SUPER_BLUR_GENERATOR_H_
