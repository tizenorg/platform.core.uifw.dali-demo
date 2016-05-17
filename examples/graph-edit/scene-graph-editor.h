#ifndef DALI_DEMO_SCENE_GRAPH_EDITOR_H
#define DALI_DEMO_SCENE_GRAPH_EDITOR_H

/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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

#include <dali-toolkit/dali-toolkit.h>


namespace Demo
{

namespace Internal
{
  class SceneGraphEditor;
}

/********************************************************************************
 * Handle to SceneGraphEditor implementation
 */
class SceneGraphEditor : public Dali::Toolkit::Control
{
public:

  /**
   * @brief The start and end property ranges for this control.
   */
  enum PropertyRange
  {
    PROPERTY_START_INDEX = Dali::Toolkit::Control::CONTROL_PROPERTY_END_INDEX + 1,
    PROPERTY_END_INDEX =   PROPERTY_START_INDEX + 1000,
  };

  /**
   * @brief An enumeration of properties belonging to the Button class.
   */
  struct Property
  {
    enum
    {
      /**
       * @brief name "testProperty", type INTEGER
       */
      TEST_PROPERTY = PROPERTY_START_INDEX,
    };
  };

  /**
   * Create a new SceneGraphEditor with default parameters (2 second animation,
   * no rotation, sweeping out a full circle).
   */
  static SceneGraphEditor New();

  /**
   * todor doxy
   */
  void Deploy();

  /**
   * Default constructor. Create an uninitialized handle.
   */
  SceneGraphEditor();

  /**
   * Copy constructor
   */
  SceneGraphEditor( const SceneGraphEditor& handle );

  /**
   * Assignment operator
   */
  SceneGraphEditor& operator=( const SceneGraphEditor& rhs );

  /**
   * Destructor
   */
  ~SceneGraphEditor();

  /**
   * Downcast method
   */
  static SceneGraphEditor DownCast( BaseHandle handle );

public: // Not for use by application developers

  SceneGraphEditor( Internal::SceneGraphEditor& impl );

  SceneGraphEditor( Dali::Internal::CustomActor* impl );
};

} // namespace Demo

#endif
