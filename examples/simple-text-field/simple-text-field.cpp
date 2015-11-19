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

/**
 * @file simple-text-field-example.cpp
 * @brief Very basic usage of TextField control
 */

// EXTERNAL INCLUDES
#include <dali-toolkit/dali-toolkit.h>
#include <iostream>

#include "shared/multi-language-strings.h"

using namespace Dali;
using namespace Dali::Toolkit;
using namespace MultiLanguageStrings;


/**
 * @brief The main class of the demo.
 */
class SimpleTextFieldExample : public ConnectionTracker
{
public:

  SimpleTextFieldExample( Application& application )
  : mApplication( application ),
    mLanguageId( 0 )
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &SimpleTextFieldExample::Create );
  }

  ~SimpleTextFieldExample()
  {
    // Nothing to do here.
  }

  /**
   * One-time setup in response to Application InitSignal.
   */
  void Create( Application& application )
  {
    Stage stage = Stage::GetCurrent();
    stage.SetBackgroundColor( Vector4( 0.04f, 0.345f, 0.392f, 1.0f ) );

    mTextField = TextField::New();
    mTextField.SetParentOrigin( ParentOrigin::CENTER );
    mTextField.SetSize( 300.f, 60.f );
    mTextField.SetBackgroundColor( Color::WHITE );
    mTextField.SetBackgroundColor( Vector4( 1.f, 1.f, 1.f, 0.15f ) );

    mTextField.SetProperty( TextField::Property::TEXT_COLOR, Color::BLACK );
    mTextField.SetProperty( TextField::Property::PLACEHOLDER_TEXT, "Unnamed folder" );
    mTextField.SetProperty( TextField::Property::PLACEHOLDER_TEXT_FOCUSED, "Enter folder name." );

    stage.Add( mTextField );

    PushButton button = PushButton::New();
    button.SetParentOrigin( ParentOrigin::CENTER );
    button.SetPosition( 0.f, -100.f );

    stage.Add( button );

    button.ClickedSignal().Connect( this, &SimpleTextFieldExample::OnButtonClicked );
  }

  bool OnButtonClicked( Button button )
  {
    const Language& language = LANGUAGES[ mLanguageId ];

    mTextField.SetProperty( TextField::Property::TEXT, language.text );

    if( ++mLanguageId >= NUMBER_OF_LANGUAGES )
    {
      mLanguageId = 0u;
    }

    return true;
  }

private:

  Application& mApplication;
  TextField mTextField;
  unsigned int mLanguageId;
};

void RunTest( Application& application )
{
  SimpleTextFieldExample test( application );

  application.MainLoop();
}

/** Entry point for Linux & Tizen applications */
int main( int argc, char **argv )
{
  // DALI_DEMO_THEME_PATH not passed to Application so TextField example uses default Toolkit style sheet.
  Application application = Application::New( &argc, &argv );

  RunTest( application );

  return 0;
}
