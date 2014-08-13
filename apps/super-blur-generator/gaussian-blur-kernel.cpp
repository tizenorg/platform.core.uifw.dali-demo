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

#include "gaussian-blur-kernel.h"
#include <cmath>
#include <cstdio>
#include <algorithm>

GaussianBlurKernel::GaussianBlurKernel(int numSamples, float bellCurveWidth, float width, float height)
: mNumSamples ( numSamples ),
  mBlurBellCurveWidth( bellCurveWidth )
{
  mUVOffsets = new Vector2[mNumSamples*2 + 1];
  mWeights = new float[mNumSamples*2 + 1];

  Generate( width, height );
}

void GaussianBlurKernel::Generate(float width, float height)
{
  float ofs;
  float weight, totalWeights;
  unsigned int i;

  totalWeights = mWeights[0] = CalcGaussianWeight(0);
  mUVOffsets[0].x = 0.0f;
  mUVOffsets[0].y = 0.0f;

  for(i=0; i<mNumSamples/2; i++)
  {
    int index = i*2+1;
    weight = CalcGaussianWeight((float)(i + 1));
    mWeights[index] = weight;
    mWeights[index+1] = weight;
    totalWeights += weight * 2.0f;

    // offset texture lookup to between texels, that way the bilinear filter in the texture hardware will average two samples with one lookup
    ofs = i * 2.0f + 1.5f;

    // get offsets from units of pixels into uv coordinates in [0..1]
    float ofsX = ofs / width;
    float ofsY = ofs / height;

    mUVOffsets[index].x = ofsX;
    mUVOffsets[index].y = ofsY;

    mUVOffsets[index+1].x = -ofsX;
    mUVOffsets[index+1].y = -ofsY;
  }

  for(i=0; i<mNumSamples; i++)
  {
    mWeights[i] /= totalWeights;
  }
}

Vector2 GaussianBlurKernel::GetOffset(int index)
{
  return mUVOffsets[index];
}

float GaussianBlurKernel::GetWeight(int index)
{
  return mWeights[index];
}

void GaussianBlurKernel::SetBlurBellCurveWidth(float blurBellCurveWidth)
{
  // a value of zero leads to undefined Gaussian weights, do not allow user to do this
  mBlurBellCurveWidth = std::max( blurBellCurveWidth, 0.001f );
}

float GaussianBlurKernel::CalcGaussianWeight(float x)
{
  // e ^ (-x²/2*width²) /  2*PI * width

  return (1.0f / sqrt(2.0f * M_PI * mBlurBellCurveWidth)) * exp(-(x * x) / (2.0f * mBlurBellCurveWidth * mBlurBellCurveWidth));
}
