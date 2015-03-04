#ifndef __DALI_DEMO_EDIT_LAYOUT_H__
#define __DALI_DEMO_EDIT_LAYOUT_H__

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
#include <dali-toolkit/dali-toolkit.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{
class EditLayout;
} // namespace Internal

class EditLayout : public Toolkit::Control
{
public:
  static EditLayout New();

  void SetTopPanel( Control panel );

  EditLayout();

  EditLayout( const EditLayout& handle );

  EditLayout& operator=( const EditLayout& handle );

  ~EditLayout();

  EditLayout( Internal::EditLayout& internal );

  explicit EditLayout( Dali::Internal::CustomActor* internal );

  EditLayout DownCast( BaseHandle handle );

  Internal::EditLayout& GetImpl( EditLayout& verticalLayout );

private:
};

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_DEMO_EDIT_LAYOUT_H__
