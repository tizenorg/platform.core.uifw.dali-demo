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

const std::string FONTS[] = { "FreeSerif",
                              "FreeSans",
                              "DroidSerif",
                              "DroidSans",
                              "DejaVuSerif",
                              "DejaVuSans",
                              "LiberationSerif",
                              "LiberationSans" };
const unsigned int NUMBER_OF_FONTS = sizeof( FONTS ) / sizeof( std::string ); 

const std::string FONT_WIDTH[] = { "ultraCondensed",
                                   "extraCondensed",
                                   "condensed",
                                   "semiCondensed",
                                   "normal",
                                   "semiExpanded",
                                   "expanded",
                                   "extraExpanded",
                                   "ultraExpanded" };
const unsigned int NUMBER_OF_FONT_WIDTH = sizeof( FONT_WIDTH ) / sizeof( std::string );

const std::string FONT_WEIGHT[] = { "thin",
                                    "ultraLight",
                                    "extraLight",
                                    "light",
                                    "demiLight",
                                    "semiLight", 
                                    "book",
                                    "normal",
                                    "regular",
                                    "medium",
                                    "demiBold",
                                    "semiBold",
                                    "bold",
                                    "ultraBold",
                                    "extraBold",
                                    "black",
                                    "heavy",
                                    "extraBlack" };
const unsigned int NUMBER_OF_FONT_WEIGHT = sizeof( FONT_WEIGHT ) / sizeof( std::string );

const std::string FONT_SLANT[] = { "normal",
                                   "roman",
                                   "italic",
                                   "oblique" };
const unsigned int NUMBER_OF_FONT_SLANT = sizeof( FONT_SLANT ) / sizeof( std::string );

const float FONT_SIZE[] = { 10.f, 14.f, 18.f, 22.f, 26.f, 30.f };
const unsigned int NUMBER_OF_FONT_SIZE = sizeof( FONT_SIZE ) / sizeof( float );

/**
 * @brief The main class of the demo.
 */
class SimpleTextFieldExample : public ConnectionTracker
{
public:

  SimpleTextFieldExample( Application& application )
  : mApplication( application ),
    mWidth( "normal" ),
    mWeight( "normal" ),
    mSlant( "normal" ),
    mWidthDefined( false ),
    mWeightDefined( false ),
    mSlantDefined( false )
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

    mField2.SetProperty( TextField::Property::ENABLE_MARKUP, true );
    mField2.SetProperty( TextField::Property::TEXT_COLOR, Color::BLACK );
    mField2.SetProperty( TextField::Property::TEXT, "<color value='#FF0000'>Hello</color> <color value='0xff0000ff'>world</color> <color value='0xff00ff00'>demo</color>" );


    stage.Add( mField1 );
    stage.Add( mField2 );

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

      button.SetSize( 25.f, 25.f );
      button.SetPosition( ( 25.f * index ) - 225.f, 50.f );
      button.SetParentOrigin( ParentOrigin::CENTER );

      button.ClickedSignal().Connect( this, &SimpleTextFieldExample::SetStyle );

      stage.Add( button );
    }

    for( unsigned int index = 0u; index < NUMBER_OF_FONTS; ++index )
    {
      PushButton button = PushButton::New();
      std::ostringstream s;
      s << "font" << index;
      button.SetName( s.str() );

      button.SetProperty( Button::Property::LABEL, FONTS[index][0u] );

      button.SetSize( 25.f, 25.f );
      button.SetPosition( ( 25.f * index ) - 225.f, 75.f );
      button.SetParentOrigin( ParentOrigin::CENTER );

      button.ClickedSignal().Connect( this, &SimpleTextFieldExample::SetStyle );

      stage.Add( button );
    }

    for( unsigned int index = 0u; index < NUMBER_OF_FONT_WEIGHT; ++index )
    {
      PushButton button = PushButton::New();
      std::ostringstream s;
      if( index < 10 )
      {
        s << "weight0" << index;
      }
      else
      {
        s << "weight" << index;
      }
      button.SetName( s.str() );

      button.SetSize( 25.f, 25.f );
      button.SetPosition( ( 25.f * index ) - 225.f, 100.f );
      button.SetParentOrigin( ParentOrigin::CENTER );

      button.ClickedSignal().Connect( this, &SimpleTextFieldExample::SetStyle );

      stage.Add( button );
    }

    for( unsigned int index = 0u; index < NUMBER_OF_FONT_WIDTH; ++index )
    {
      PushButton button = PushButton::New();
      std::ostringstream s;
      s << "width" << index;
      button.SetName( s.str() );

      button.SetSize( 25.f, 25.f );
      button.SetPosition( ( 25.f * index ) - 225.f, 125.f );
      button.SetParentOrigin( ParentOrigin::CENTER );

      button.ClickedSignal().Connect( this, &SimpleTextFieldExample::SetStyle );

      stage.Add( button );
    }

    for( unsigned int index = 0u; index < NUMBER_OF_FONT_SLANT; ++index )
    {
      PushButton button = PushButton::New();
      std::ostringstream s;
      s << "slant" << index;
      button.SetName( s.str() );

      button.SetSize( 25.f, 25.f );
      button.SetPosition( ( 25.f * index ) - 225.f, 150.f );
      button.SetParentOrigin( ParentOrigin::CENTER );

      button.ClickedSignal().Connect( this, &SimpleTextFieldExample::SetStyle );

      stage.Add( button );
    }

    for( unsigned int index = 0u; index < NUMBER_OF_FONT_SIZE; ++index )
    {
      PushButton button = PushButton::New();
      std::ostringstream s;
      s << "size" << index;
      button.SetName( s.str() );

      button.SetSize( 25.f, 25.f );
      button.SetPosition( ( 25.f * index ) - 225.f, 175.f );
      button.SetParentOrigin( ParentOrigin::CENTER );

      button.ClickedSignal().Connect( this, &SimpleTextFieldExample::SetStyle );

      stage.Add( button );
    }
  }

  bool ProcessMarkup( Button button )
  {
    Property::Value markup = mField1.GetProperty( TextField::Property::TEXT );
    std::cout << "markup: [" << markup.Get< std::string >() << "]" << std::endl;

    mField2.SetProperty( TextField::Property::TEXT, markup.Get<std::string>() );

    return true;
  }

  bool SetStyle( Button button )
  {
    const std::string& name = button.GetName();

    if( "color" == name.substr( 0u, 5u ) )
    {
      const unsigned int index = strtoul( name.substr( 5u, 1u ).c_str(), NULL, 10u );
      mField2.SetProperty( TextField::Property::INPUT_COLOR, COLORS[index] );
    }
    else if( "font" == name.substr( 0u, 4u ) )
    {
      const unsigned int index = strtoul( name.substr( 4u, 1u ).c_str(), NULL, 10u );
      mField2.SetProperty( TextField::Property::INPUT_FONT_FAMILY, FONTS[index] );
    }
    else if( "weight" == name.substr( 0u, 6u ) )
    {
      const unsigned int index = strtoul( name.substr( 6u, 2u ).c_str(), NULL, 10u );
      mWeight = FONT_WEIGHT[index];
      mWeightDefined = true;

      SetFontStyle();
    }
    else if( "width" == name.substr( 0u, 5u ) )
    {
      const unsigned int index = strtoul( name.substr( 5u, 1u ).c_str(), NULL, 10u );
      mWidth = FONT_WIDTH[index];
      mWidthDefined = true;

      SetFontStyle();
    }
    else if( "slant" == name.substr( 0u, 5u ) )
    {
      const unsigned int index = strtoul( name.substr( 5u, 1u ).c_str(), NULL, 10u );
      mSlant = FONT_SLANT[index];
      mSlantDefined = true;

      SetFontStyle();
    }
    else if( "size" == name.substr( 0u, 4u ) )
    {
      const unsigned int index = strtoul( name.substr( 4u, 1u ).c_str(), NULL, 10u );
      mField2.SetProperty( TextField::Property::INPUT_POINT_SIZE, FONT_SIZE[index] );
    }

    return true;    
  }

  void SetFontStyle()
  {
    std::string style = "{";
    if( mWidthDefined )
    {
      style += "\"width\":\"" + mWidth + "\"";
    }
    if( mWeightDefined )
    {
      if( mWidthDefined )
      {
        style += ",";
      }
      style += "\"weight\":\"" + mWeight + "\"";
    }
    if( mSlantDefined )
    {
      if( mWidthDefined || mWeightDefined )
      {
        style += ",";
      }
      style += "\"slant\":\"" + mSlant + "\"";
    }
    style += "}";

    std::cout << "  style set : [" << style << "]" << std::endl;
    mField2.SetProperty( TextField::Property::INPUT_FONT_STYLE, style );
  }

private:

  Application& mApplication;
  TextField mField1;
  TextField mField2;

  std::string mWidth;
  std::string mWeight;
  std::string mSlant;
  bool mWidthDefined;
  bool mWeightDefined;
  bool mSlantDefined;
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
