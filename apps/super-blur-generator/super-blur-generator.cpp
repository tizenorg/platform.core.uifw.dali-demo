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

#include "super-blur-generator.h"

#include <string>
#include <sstream>
#include <dali/dali.h>

#include "context.h"
#include "framebuffer.h"
#include "gaussian-blur-kernel.h"
#include "render-surface.h"
#include "texture.h"
#include "textured-quad-renderer.h"
#include "program.h"
#include "logging.h"

namespace
{
const std::string VERT_SOURCE( RESOURCES_DIR "textured-quad.vert" );
const std::string BLUR_FRAG_SOURCE( RESOURCES_DIR "gaussian-blur.frag" );
const std::string PLAIN_FRAG_SOURCE( RESOURCES_DIR "plain-texture.frag" );

const int NUM_SAMPLES(16);
const float GAUSSIAN_BLUR_BLUR_BELL_CURVE_WIDTH = 1.5f;
const std::string PREFIX("#define NUM_SAMPLES 16\n;");
}

class SuperBlurGeneratorImpl
{
public:
  SuperBlurGeneratorImpl( const Dali::BitmapLoader& inputImage, const std::string& outputFolder, const std::string& outputFileName )
  : mWidth(inputImage.GetImageWidth()),
    mHeight(inputImage.GetImageHeight()),
    mContext(),
    mRenderSurface( mContext, mHeight, mWidth ),
    mTexture( mContext, inputImage ),
    mBlurProgram( mContext, PREFIX, VERT_SOURCE, BLUR_FRAG_SOURCE ),
    mKernel( NUM_SAMPLES, GAUSSIAN_BLUR_BLUR_BELL_CURVE_WIDTH, mWidth, mHeight ),
    mNumSamples( NUM_SAMPLES ),
    mOutputPath( outputFolder + "/" + outputFileName )
  {
  }

  void Generate( float level )
  {
    mContext.Prepare(mWidth, mHeight);
    // Draw to a smaller frame buffer to get some free blurring from GL
    Framebuffer fb1( mWidth*0.5f, mHeight*0.5f );
    {
      fb1.Prepare();
      mBlurProgram.Activate();
      SetKernelUniforms(mBlurProgram, SuperBlurGenerator::HORIZONTAL);
      TexturedQuadRenderer renderer( mContext, mBlurProgram, mTexture );
    }

    // Render to main render surface
    mContext.Prepare(mWidth, mHeight);
    SetKernelUniforms(mBlurProgram, SuperBlurGenerator::VERTICAL);
    TexturedQuadRenderer renderer( mContext, mBlurProgram, fb1 );

    unsigned char* buffer = mRenderSurface.ReadPixelData();
    if( buffer != NULL )
    {
      Dali::EncodeToFile( buffer, mOutputPath.c_str(), Dali::Pixel::RGBA8888, mWidth, mHeight );
      delete[] buffer;
    }
  }

  void SetKernelUniforms(Program& program, SuperBlurGenerator::Direction direction)
  {
    Vector2 axis(1.0f, 0.0f);
    if( direction == SuperBlurGenerator::VERTICAL )
    {
      axis = Vector2(0.0f, 1.0f);
    }

    for (int i = 0; i < mNumSamples; ++i )
    {
      float xValue = mKernel.GetOffset(i).x * axis.x;
      float yValue = mKernel.GetOffset(i).y * axis.y;
      float weight = mKernel.GetWeight(i);

      {
        std::ostringstream oss;
        oss << "uSampleOffsets[" << i << "]";
        program.SetVector2Uniform( oss.str(), xValue, yValue );
      }
      {
        std::ostringstream oss;
        oss << "uSampleWeights[" << i << "]";
        program.SetFloatUniform( oss.str(), weight );
      }
    }
  }

private:
  std::size_t mWidth;
  std::size_t mHeight;
  Context mContext;  // Order is relevant!
  RenderSurface mRenderSurface;
  Texture mTexture;
  Program mBlurProgram;
  GaussianBlurKernel mKernel;
  int mNumSamples;
  std::string mOutputPath;
};

SuperBlurGenerator::SuperBlurGenerator(
  const Dali::BitmapLoader& inputImage,
  const std::string& outputFolder,
  const std::string& outputFileName)
: mImpl( new SuperBlurGeneratorImpl( inputImage, outputFolder, outputFileName ) )
{
}

SuperBlurGenerator::~SuperBlurGenerator()
{
}

void SuperBlurGenerator::Generate( float level )
{
  GetImpl().Generate( level );
}
