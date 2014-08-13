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

int NumSamples(int blurLevel)
{
  return 11 + 10*blurLevel;
}

float BellCurveWidth(int blurLevel)
{
  return 4.5f + 5.0f*blurLevel;
}

std::string ShaderDefines(int numSamples)
{
  std::ostringstream oss;
  oss << "#define NUM_SAMPLES " << numSamples << "\n;" ;
  return oss.str();
}

}

class SuperBlurGeneratorImpl
{
public:
  /**
   * level - 1-6
   */
  SuperBlurGeneratorImpl( const Dali::BitmapLoader& inputImage, const std::string& outputFolder, const std::string& outputFileName, int level, Vector2 framebufferSize, Vector2 outputSize )
  : mInputSize(inputImage.GetImageWidth(), inputImage.GetImageHeight()),
    mFramebufferSize(framebufferSize),
    mOutputSize(outputSize),
    mContext(),
    mRenderSurface( mContext, mOutputSize.x, mOutputSize.y),
    mTexture( mContext, inputImage ),
    mNumSamples( NumSamples(level-1) ),
    mBlurProgram( mContext, ShaderDefines( mNumSamples), VERT_SOURCE, BLUR_FRAG_SOURCE ),
    mKernel(mNumSamples, BellCurveWidth(level-1), mOutputSize.x, mOutputSize.y),
    mOutputPath( outputFolder + "/" + outputFileName)
  {
    printf("Input size: (%f, %f)\nFramebuffer size: (%f, %f)\nOutput size: (%f, %f)\n",
           mInputSize.x, mInputSize.y,
           mFramebufferSize.x, mFramebufferSize.y,
           mOutputSize.x, mOutputSize.y );
  }

  void Generate()
  {
    Program simpleShader(mContext, "", VERT_SOURCE, PLAIN_FRAG_SOURCE);
    simpleShader.Activate();

    // Draw to a smaller frame buffer to get some cheap blurring from GL
    Framebuffer fb1( mFramebufferSize.x, mFramebufferSize.y );
    {
      fb1.Prepare();
      mBlurProgram.Activate();
      mKernel.Generate( mFramebufferSize.x, mFramebufferSize.y );
      SetKernelUniforms(mBlurProgram, SuperBlurGenerator::HORIZONTAL);
      TexturedQuadRenderer renderer( mContext, mBlurProgram, mTexture );
    }

    // Render to main render surface
    mContext.Prepare(mOutputSize.x, mOutputSize.y );
    mBlurProgram.Activate();

    mKernel.Generate( mOutputSize.x, mOutputSize.y );
    SetKernelUniforms(mBlurProgram, SuperBlurGenerator::VERTICAL);
    TexturedQuadRenderer renderer( mContext, mBlurProgram, fb1 );

    unsigned char* buffer = mRenderSurface.ReadPixelData();
    if( buffer != NULL )
    {
      Dali::EncodeToFile( buffer, mOutputPath.c_str(), Dali::Pixel::RGBA8888, mOutputSize.x, mOutputSize.y );
      delete[] buffer;
    }
  }

  void workingGenerate()
  {
    mContext.Prepare(mOutputSize.x, mOutputSize.y);

    Program simpleShader(mContext, "", VERT_SOURCE, PLAIN_FRAG_SOURCE);
    simpleShader.Activate();
    TexturedQuadRenderer renderer( mContext, simpleShader, mTexture );

    unsigned char* buffer = mRenderSurface.ReadPixelData();
    if( buffer != NULL )
    {
      Dali::EncodeToFile( buffer, mOutputPath.c_str(), Dali::Pixel::RGBA8888, mOutputSize.x, mOutputSize.y );
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
  Vector2 mInputSize;
  Vector2 mFramebufferSize;
  Vector2 mOutputSize;

  Context mContext;  // Order is relevant!
  RenderSurface mRenderSurface;
  Texture mTexture;
  int mNumSamples;
  Program mBlurProgram;
  GaussianBlurKernel mKernel;
  std::string mOutputPath;
  };

SuperBlurGenerator::SuperBlurGenerator(
  const Dali::BitmapLoader& inputImage,
  const std::string& outputFolder,
  const std::string& outputFileName,
  int blurLevel,
  Vector2 framebufferSize,
  Vector2 outputSize )
: mImpl( new SuperBlurGeneratorImpl( inputImage, outputFolder, outputFileName, blurLevel, framebufferSize, outputSize ) )
{
}

SuperBlurGenerator::~SuperBlurGenerator()
{
}

void SuperBlurGenerator::Generate()
{
  GetImpl().Generate();
}
