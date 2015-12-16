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

const Vector4 COLORS[] = { Color::RED,
                           Color::GREEN,
                           Color::BLUE,
                           Color::YELLOW,
                           Color::CYAN,
                           Color::MAGENTA,
                           Color::WHITE,
                           Color::BLACK };
const unsigned int NUMBER_OF_COLORS = sizeof( COLORS ) / sizeof( Vector4 );

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

    mField = TextField::New();
    mField.SetParentOrigin( ParentOrigin::TOP_CENTER );
    mField.SetAnchorPoint( AnchorPoint::TOP_CENTER );
    mField.SetPosition( 0.f, 10.f );
    mField.SetSize( floorf( 0.9f * size.width ), 60.f );
    mField.SetBackgroundColor( Color::WHITE );
    mField.SetBackgroundColor( Vector4( 1.f, 1.f, 1.f, 0.15f ) );

    mField.SetProperty( TextField::Property::TEXT_COLOR, Color::BLACK );
    mField.SetProperty( TextField::Property::PLACEHOLDER_TEXT, "Tap to edit" );
    mField.SetProperty( TextField::Property::PLACEHOLDER_TEXT_FOCUSED, "Type markup string" );
    mField.SetProperty( TextField::Property::MAX_LENGTH, 99999 );

    mLabel = TextLabel::New();
    mLabel.SetParentOrigin( ParentOrigin::TOP_CENTER );
    mLabel.SetAnchorPoint( AnchorPoint::TOP_CENTER );
    mLabel.SetPosition( 0.f, 120.f );
    mLabel.SetSize( floorf( 0.9f * size.width ), floorf( 0.3f * size.height ) );
    mLabel.SetBackgroundColor( Color::WHITE );
    mLabel.SetBackgroundColor( Vector4( 1.f, 1.f, 1.f, 0.15f ) );

    mLabel.SetProperty( TextField::Property::TEXT_COLOR, Color::BLACK );
    mLabel.SetProperty( TextField::Property::TEXT, "<color value='#FF0000'>Hello</color> <color value='0xff0000ff'>world</color> <color value='0xff00ff00'>demo</color>" );


    stage.Add( mField );
    stage.Add( mLabel );

    PushButton button = PushButton::New();
    button.SetParentOrigin( ParentOrigin::TOP_CENTER );
    button.SetAnchorPoint( AnchorPoint::TOP_CENTER );
    button.SetPosition( 0.f, 80.f );
    button.ClickedSignal().Connect( this, &SimpleTextFieldExample::ProcessMarkup );

    stage.Add( button );

    for( unsigned int index = 0u; index < NUMBER_OF_COLORS; ++index )
    {
      PushButton button = PushButton::New();
      std::ostringstream s;
      s << "color" << index;
      button.SetName( s.str() );
      button.SetProperty( Button::Property::UNSELECTED_COLOR, COLORS[index] );
      button.SetProperty( Button::Property::SELECTED_COLOR, COLORS[index] );

      button.SetSize( 50.f, 50.f );
      button.SetPosition( ( 50.f * index ) - 225.f, 0.f );
      button.SetParentOrigin( ParentOrigin::CENTER );

      button.ClickedSignal().Connect( this, &SimpleTextFieldExample::SetColor );

      stage.Add( button );
    }
  }

  bool ProcessMarkup( Button button )
  {
    Property::Value markup = mField.GetProperty( TextField::Property::TEXT );
    std::cout << "markup: [" << markup.Get< std::string >() << "]" << std::endl;

    mLabel.SetProperty( TextLabel::Property::TEXT, markup.Get<std::string>() );

    return true;
  }

  bool SetColor( Button button )
  {
    const std::string& name = button.GetName();

    if( "color" == name.substr( 0u, 5u ) )
    {
      const unsigned int index = strtoul( name.substr( 5u, 1u ).c_str(), NULL, 10u );
      mField.SetProperty( TextField::Property::INPUT_COLOR, COLORS[index] );
    }

    return true;
  }

private:

  Application& mApplication;
  TextField mField;
  TextLabel mLabel;
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
