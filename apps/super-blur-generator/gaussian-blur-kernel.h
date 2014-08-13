#ifndef GAUSSIAN_BLUR_KERNEL_H
#define GAUSSIAN_BLUR_KERNEL_H

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

struct Vector2
{
  float x;
  float y;
  Vector2()
  : x(0), y(0)
  {
  }

  Vector2( float anX, float aY )
  : x(anX), y(aY)
  {
  }
};

class GaussianBlurKernel
{
public:
  /**
   * Constructor
   * @param[in] numSamples The number of pixels to sample
   * @param[in] bellCurveWidth The width of the bell curve of the gaussian distribution
   * @param[in] width The width of the source texture
   * @param[in] height The height of the source texture
   */
  GaussianBlurKernel(int numSamples, float bellCurveWidth, float width, float height);

  /**
   * Generate a new kernel with the given source texture's width and height
   * @param[in] width The width of the source texture
   * @param[in] height The height of the source texture
   */
  void Generate(float width, float height);

  /**
   * Get the texture UV offset at the given index
   * @param[in] index The index in the sample list
   */
  Vector2 GetOffset(int index);

  /**
   * Get the weight to apply at the given index
   * @param[in] index The index in the weight list
   */
  float GetWeight(int index);

private:
  /**
   * Set the bell curve width, clammped to avoid zero.
   * @param[in] blurBellCurveWidth The width of the bell curve
   */
  void SetBlurBellCurveWidth(float blurBellCurveWidth);

  /**
   * Calculate the gaussian weight for the given x offset and the pre-loaded bell curve width
   * Choose a bell curve width that suits the maximum X offset.
   * @param[in] x The X value for which to find the weight
   */
  float CalcGaussianWeight(float x);

private:
  unsigned int mNumSamples;       ///< number of blur samples in each of horiz/vert directions
  float mBlurBellCurveWidth;      ///< constant used when calculating the gaussian weights

  float* mWeights;
  Vector2* mUVOffsets;
};

#endif
