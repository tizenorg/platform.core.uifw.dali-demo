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
#include <algorithm>

GaussianBlurKernel::GaussianBlurKernel(int numSamples, float bellCurveWidth, float width, float height)
: mNumSamples ( numSamples ),
  mBlurBellCurveWidth( bellCurveWidth )
{
  float ofs;
  float w, totalWeights;
  unsigned int i;

  mUVOffsets = new Vector2[mNumSamples + 1];
  mWeights = new float[mNumSamples + 1];

  totalWeights = mWeights[0] = CalcGaussianWeight(0);
  mUVOffsets[0].x = 0.0f;
  mUVOffsets[0].y = 0.0f;

  for(i=0; i<mNumSamples >> 1; i++)
  {
    w = CalcGaussianWeight((float)(i + 1));
    mWeights[(i << 1) + 1] = w;
    mWeights[(i << 1) + 2] = w;
    totalWeights += w * 2.0f;

    // offset texture lookup to between texels, that way the bilinear filter in the texture hardware will average two samples with one lookup
    ofs = ((float)(i << 1)) + 1.5f;

    // get offsets from units of pixels into uv coordinates in [0..1]
    float ofsX = ofs / width;
    float ofsY = ofs / height;
    mUVOffsets[(i << 1) + 1].x = ofsX;
    mUVOffsets[(i << 1) + 1].y = ofsY;

    mUVOffsets[(i << 1) + 2].x = -ofsX;
    mUVOffsets[(i << 1) + 2].y = -ofsY;
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
  return (1.0f / sqrt(2.0f * M_PI * mBlurBellCurveWidth)) * exp(-(x * x) / (2.0f * mBlurBellCurveWidth * mBlurBellCurveWidth));
}
