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

// EXTERNAL INCLUDES
#include <dali/public-api/object/ref-object.h>
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/math/vector3.h>
#include <dali/public-api/object/base-object.h>

// INTERNAL INCLUDES
#include "cluster-style.h"

namespace Dali
{

namespace Demo
{

namespace Internal
{

class ClusterStyle;

typedef IntrusivePtr<ClusterStyle>       ClusterStylePtr;

/**
 * ClusterStyle internal implementation
 */
class ClusterStyle : public Dali::BaseObject
{
public:

  /**
   * @copydoc Demo::ClusterStyle::GetMaximumNumberOfChildren
   */
  unsigned int GetMaximumNumberOfChildren() const;

  /**
   * @copydoc Demo::ClusterStyle::ApplyStyle
   */
  virtual void ApplyStyle(Actor child, unsigned int index, AlphaFunction alpha, const TimePeriod& durationSeconds) = 0;

  /**
   * @copydoc Demo::ClusterStyle::ApplyStyleToBackground
   */
  virtual void ApplyStyleToBackground(Actor background, AlphaFunction alpha, const TimePeriod& durationSeconds);

  /**
   * @copydoc Demo::ClusterStyle::ApplyStyleToTitle
   */
  virtual void ApplyStyleToTitle(Actor title, AlphaFunction alpha, const TimePeriod& durationSeconds);

  /**
    * Set the size of cluster
    */
  void SetClusterSize( const Vector3& clusterSize );

protected:

  /**
   * Set the maximum number of children this Style can handle.
   * @param[in] The maximum number of children.
   */
  void SetMaximumNumberOfChildren(unsigned int children);

  /**
   * Set the title properties
   * @param[in] relativePosition Relative position of the title
   * @param[in] offsetPosition Offset position of the title
   * @param[in] size The size of the title
   */
  void SetTitleProperties(const Vector3& relativePosition,
                          const Vector3& offsetPosition,
                          const Vector3& size);

  /**
   * Set the background properties
   * @param[in] relativePosition Relative position of the background
   * @param[in] offsetPosition Offset position of the background
   * @param[in] size The size of the title
   */
  void SetBackgroundProperties(const Vector3& relativePosition,
                               const Vector3& offsetPosition,
                               const Vector3& size);

  /**
   * Get the size of cluster.
   * @return the cluster size
   */
   Vector3 GetClusterSize() const;

   /**
    * Apply the given position & size to the actor
    * @param[in] position The target position
    * @param[in] size The target size
    * @param[in] size The size to resize to
    * @param[in] alpha The alpha function to use.
    * @param[in] durationSeconds The time period to apply this style.
    */
   void Apply( Actor actor,
               const Vector3& position,
               const Vector3& size,
               AlphaFunction alpha,
               const TimePeriod& durationSeconds);
   /**
    * Apply the given position & size to the actor
    * @param[in] actor The actor to apply the changes
    * @param[in] position The target position
    * @param[in] size The target size
    * @param[in] rotation The target Quaternion value
    * @param[in] scale The target scale
    * @param[in] alpha The alpha function to use.
    * @param[in] durationSeconds The time period to apply this style.
    */
   void Apply( Actor actor,
               const Vector3& position,
               const Vector3& size,
               const Quaternion& rotation,
               const Vector3& scale,
               AlphaFunction alpha,
               const TimePeriod& durationSeconds);

protected:

  /**
   * Protected constructor see ClusterStyle::New().
   */
  ClusterStyle();

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~ClusterStyle();

protected:

  unsigned int mMaxChildren;                      ///< Maximum number of children that this style supports
  Vector3 mTitlePositionRelative;                 ///< Title's position relative to size of cluster
  Vector3 mTitlePositionOffset;                   ///< Title's position offset
  Vector3 mTitleSize;                             ///< Title's size relative to size of cluster
  Vector3 mBackgroundPositionRelative;            ///< Background's position relative to size of cluster
  Vector3 mBackgroundPositionOffset;              ///< Background's position offset
  Vector3 mBackgroundSize;                        ///< Background's size relative to size of cluster
  Vector3 mClusterSize;                           ///< The size of cluster
};

/**
 * ClusterStyleStandard internal implementation
 */
class ClusterStyleStandard : public ClusterStyle
{
public:

  typedef Demo::ClusterStyleStandard::StyleType StyleType;

public:

  /**
   * Create a new cluster style.
   * @param[in] style The style type to create.
   * @return A smart-pointer to the newly allocated ClusterStyle.
   */
  static ClusterStylePtr New(StyleType style);

public:

  /**
   * @copydoc Demo::ClusterStyle::ApplyStyle
   */
  void ApplyStyle(Actor child, unsigned int index, AlphaFunction alpha, const TimePeriod& durationSeconds);

private:

  /**
   * Set the relative sizes of the children
   * @param[in] size The list of sizes for the children
   */
  void SetSizes(const float *sizes);

  /**
   * Set the relative positions of the children
   * @param[in] positions The list of positions for the children
   */
  void SetPositions(const Vector3 *positions);

protected:

  /**
   * Protected constructor see ClusterStyleRandom::New().
   */
  ClusterStyleStandard(StyleType style);

private:

  const float *mSizes;                            ///< List of sizes
  const Vector3 *mPositions;                      ///< List of positions

};

/**
 * ClusterStyleRandom internal implementation
 */
class ClusterStyleRandom : public ClusterStyle
{
public:

  /**
   * Create a new cluster style.
   * @return A smart-pointer to the newly allocated ClusterStyle.
   */
  static ClusterStylePtr New();

public:

  /**
   * @copydoc Demo::ClusterStyle::ApplyStyle
   */
  void ApplyStyle(Actor child, unsigned int index, AlphaFunction alpha, const TimePeriod& durationSeconds);

protected:

  /**
   * Protected constructor see ClusterStyleRandom::New().
   */
  ClusterStyleRandom();

private:

};

} // namespace Internal

// Helpers for public-api forwarding methods

inline Internal::ClusterStyle& GetImpl(Demo::ClusterStyle& pub)
{
  DALI_ASSERT_ALWAYS(pub);

  Dali::RefObject& handle = pub.GetBaseObject();

  return static_cast<Internal::ClusterStyle&>(handle);
}

inline const Internal::ClusterStyle& GetImpl(const Demo::ClusterStyle& pub)
{
  DALI_ASSERT_ALWAYS(pub);

  const Dali::RefObject& handle = pub.GetBaseObject();

  return static_cast<const Internal::ClusterStyle&>(handle);
}

} // namespace Demo

} // namespace Dali
