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

#include "super-blur-generator.h"

#include <string>

#include "context.h"
#include "render-surface.h"
#include "texture.h"
#include "textured-quad-renderer.h"
#include "program.h"
#include "logging.h"

namespace
{
const std::string VERT_SOURCE( RESOURCES_DIR "gaussian-blur.vert" );
const std::string FRAG_SOURCE( RESOURCES_DIR "gaussian-blur.frag" );

}

class SuperBlurGeneratorImpl
{
public:
  SuperBlurGeneratorImpl( const Dali::BitmapLoader& inputImage, const std::string& outputFolder )
  : mContext(),
    mRenderSurface( mContext, inputImage.GetImageHeight(), inputImage.GetImageWidth() ),
    mTexture( mContext, inputImage ),
    mProgram( mContext, VERT_SOURCE, FRAG_SOURCE )
  {
  }

  void Generate( float level )
  {
    mProgram.Activate();
    mProgram.SetFloatUniform( "uLevel", level );
    TexturedQuadRenderer renderer( mContext, mProgram, mTexture );

    // Save output to a file...
  }

private:
  Context mContext;  // Order is relevant!
  RenderSurface mRenderSurface;
  Texture mTexture;
  Program mProgram;
};

SuperBlurGenerator::SuperBlurGenerator(
  const Dali::BitmapLoader& inputImage,
  const std::string& outputFolder )
: mImpl( new SuperBlurGeneratorImpl( inputImage, outputFolder ) )
{
}

SuperBlurGenerator::~SuperBlurGenerator()
{
}

void SuperBlurGenerator::Generate( float level )
{
  GetImpl().Generate( level );
}
