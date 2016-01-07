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
#include <string>
#include <iostream>
#include <sstream>

using namespace Dali;
using namespace Dali::Toolkit;

/**
 * @brief The main class of the demo.
 */
class SimpleTextFieldExample : public ConnectionTracker
{
public:

  SimpleTextFieldExample( Application& application )
  : mApplication( application )
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
    const Size size = stage.GetSize();

    mField1 = TextField::New();
    mField1.SetParentOrigin( ParentOrigin::TOP_CENTER );
    mField1.SetAnchorPoint( AnchorPoint::TOP_CENTER );
    mField1.SetPosition( 0.f, 10.f );
    mField1.SetSize( floorf( 0.9f * size.width ), 60.f );
    mField1.SetBackgroundColor( Color::WHITE );
    mField1.SetBackgroundColor( Vector4( 1.f, 1.f, 1.f, 0.15f ) );

    mField1.SetProperty( TextField::Property::TEXT_COLOR, Color::BLACK );
    mField1.SetProperty( TextField::Property::PLACEHOLDER_TEXT, "Tap to edit" );
    mField1.SetProperty( TextField::Property::PLACEHOLDER_TEXT_FOCUSED, "Type markup string" );
    mField1.SetProperty( TextField::Property::MAX_LENGTH, 99999 );

    mField2 = TextField::New();
    mField2.SetParentOrigin( ParentOrigin::TOP_CENTER );
    mField2.SetAnchorPoint( AnchorPoint::TOP_CENTER );
    mField2.SetPosition( 0.f, 120.f );
    mField2.SetSize( floorf( 0.9f * size.width ), floorf( 0.3f * size.height ) );
    mField2.SetBackgroundColor( Color::WHITE );
    mField2.SetBackgroundColor( Vector4( 1.f, 1.f, 1.f, 0.15f ) );

    stage.Add( mField1 );
    stage.Add( mField2 );

    PushButton button = PushButton::New();
    button.SetParentOrigin( ParentOrigin::TOP_CENTER );
    button.SetAnchorPoint( AnchorPoint::TOP_CENTER );
    button.SetPosition( 0.f, 80.f );
    button.ClickedSignal().Connect( this, &SimpleTextFieldExample::ProcessMarkup );

    stage.Add( button );
  }

  bool ProcessMarkup( Button button )
  {
    Property::Value markup = mField1.GetProperty( TextField::Property::TEXT );
    std::cout << "markup: [" << markup.Get< std::string >() << "]" << std::endl;

    mField2.SetProperty( TextField::Property::TEXT, markup.Get<std::string>() );

    return true;
  }

private:

  Application& mApplication;
  TextField mField1;
  TextField mField2;
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
