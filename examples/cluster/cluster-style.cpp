/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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
 *
 */

// CLASS HEADER
#include "cluster-style.h"

// EXTERNAL INCLUDES
#include <limits>

// INTERNAL INCLUDES
#include "cluster-style-impl.h"

using namespace Dali;
using namespace Dali::Demo;

namespace Dali
{

namespace Demo
{

// ClusterStyle ///////////////////////////////////////////////////////////////

const unsigned int ClusterStyle::UNLIMITED_CHILDREN = std::numeric_limits<unsigned int>::max();

ClusterStyle::ClusterStyle()
{
}

ClusterStyle::~ClusterStyle()
{
}

ClusterStyle::ClusterStyle(Internal::ClusterStyle* internal)
: BaseHandle(internal)
{
}

unsigned int ClusterStyle::GetMaximumNumberOfChildren() const
{
  return GetImpl(*this).GetMaximumNumberOfChildren();
}

void ClusterStyle::ApplyStyle(Actor child, unsigned int index, AlphaFunction alpha, const TimePeriod& durationSeconds)
{
  GetImpl(*this).ApplyStyle(child, index, alpha, TimePeriod(durationSeconds));
}

void ClusterStyle::ApplyStyleToBackground(Actor background, AlphaFunction alpha, const TimePeriod& durationSeconds)
{
  GetImpl(*this).ApplyStyleToBackground(background, alpha, TimePeriod(durationSeconds));
}

void ClusterStyle::ApplyStyleToTitle(Actor title, AlphaFunction alpha, const TimePeriod& durationSeconds)
{
  GetImpl(*this).ApplyStyleToTitle(title, alpha, TimePeriod(durationSeconds));
}

// ClusterStyleStandard ///////////////////////////////////////////////////////

ClusterStyleStandard ClusterStyleStandard::New(StyleType style)
{
  Internal::ClusterStylePtr internal = Internal::ClusterStyleStandard::New(style);

  return ClusterStyleStandard(internal.Get());
}

ClusterStyleStandard::ClusterStyleStandard(Internal::ClusterStyle* internal)
: ClusterStyle(internal)
{
}

// ClusterStyleRandom /////////////////////////////////////////////////////////

ClusterStyleRandom ClusterStyleRandom::New()
{
  Internal::ClusterStylePtr internal = Internal::ClusterStyleRandom::New();

  return ClusterStyleRandom(internal.Get());
}

ClusterStyleRandom::ClusterStyleRandom(Internal::ClusterStyle* internal)
: ClusterStyle(internal)
{
}

} // namespace Demo

} // namespace Dali
