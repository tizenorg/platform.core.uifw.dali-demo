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

#include "scene-graph-editor.h"
#include "scene-graph-editor-impl.h"

namespace Demo
{

SceneGraphEditor::SceneGraphEditor()
{
}

SceneGraphEditor::SceneGraphEditor(const SceneGraphEditor& handle)
: Control( handle )
{
}

SceneGraphEditor& SceneGraphEditor::operator=(const SceneGraphEditor& rhs)
{
  if( &rhs != this )
  {
    Control::operator=(rhs);
  }
  return *this;
}

SceneGraphEditor::~SceneGraphEditor()
{
}

SceneGraphEditor SceneGraphEditor::DownCast( BaseHandle handle )
{
  return Control::DownCast< SceneGraphEditor, SceneGraphEditor >(handle);
}

SceneGraphEditor SceneGraphEditor::New()
{
  return Internal::SceneGraphEditor::New();
}

SceneGraphEditor::SceneGraphEditor( Demo::Internal::SceneGraphEditor& impl )
: Control( impl )
{
}

SceneGraphEditor::SceneGraphEditor( Dali::Internal::CustomActor* impl )
: Control( impl )
{
  VerifyCustomActorPointer< SceneGraphEditor >(impl);
}

void SceneGraphEditor::Deploy()
{
  GetImpl( *this ).Deploy();
}


} // namespace Demo
