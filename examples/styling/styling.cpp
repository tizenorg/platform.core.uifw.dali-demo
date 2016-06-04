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
 *
 */

#include <dali/dali.h>
#include <dali-toolkit/dali-toolkit.h>

using namespace Dali;
using namespace Dali::Toolkit;

const Vector4 CUSTOM_BLUE_COLOR = Vector4( 0.26f, 0.71f, 0.78f, 1.0f );
const Vector4 SELECT_CUSTOM_BLUE_COLOR = Vector4( 0.55f, 0.9f, 0.9f, 1.0f );

// This example shows how to create and display Hello World! using a simple TextActor
//
class StylingDemo : public ConnectionTracker
{
public:

  StylingDemo( Application& application )
  : mApplication( application )
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &StylingDemo::Create );
  }

  ~StylingDemo()
  {
    // Nothing to do here;
  }

  // The Init signal is received once (only) during the Application lifetime
  void Create( Application& application )
  {
    // Get a handle to the stage
    mStage = Stage::GetCurrent();
    mStyleManager = StyleManager::Get();

    mStageWidth = mStage.GetSize().width;
    mStageHeight = mStage.GetSize().height;

    mStage.SetBackgroundColor( Color::WHITE );

    TextLabel setThemeFileLabel = CreateTitleLabel( "Set Theme File", 0.0, 0.0 );

    TextLabel defaultTextLabel = TextLabel::New();
    defaultTextLabel.SetPosition( mStageWidth*0.1, mStageHeight*0.15 );
    defaultTextLabel.SetSize( mStageWidth*0.3, mStageHeight*0.11 );
    defaultTextLabel.SetProperty( TextLabel::Property::TEXT, "Default" );
    mStage.Add( defaultTextLabel );

    PushButton setThemeFileButton = CreatePushButton( "Set Theme", mStageWidth*0.6, mStageHeight*0.15 );
    setThemeFileButton.ClickedSignal().Connect( this, &StylingDemo::SetThemeFileButtonClicked );


    TextLabel setStyleLabel = CreateTitleLabel( "Set Specific Style", 0.0, mStageHeight*0.33 );

    mStyledTextLabel = TextLabel::New();
    mStyledTextLabel.SetPosition( mStageWidth*0.1, mStageHeight*0.48 );
    mStyledTextLabel.SetSize( mStageWidth*0.3, mStageHeight*0.11 );
    mStyledTextLabel.SetProperty( TextLabel::Property::TEXT, "Styled" );
    mStage.Add( mStyledTextLabel );

    PushButton setStyleButton = CreatePushButton( "Set Style", mStageWidth*0.6, mStageHeight*0.48 );
    setStyleButton.ClickedSignal().Connect( this, &StylingDemo::SetStyleButtonClicked );


    TextLabel setDefaultThemeLabel = CreateTitleLabel( "Set Default Theme", 0.0, mStageHeight*0.66 );

    PushButton setDefaultThemeButton = CreatePushButton( "Set Default Theme", mStageWidth*0.1, mStageHeight*0.81 );
    setDefaultThemeButton.ClickedSignal().Connect( this, &StylingDemo::SetDefaultThemeClicked );
    setDefaultThemeButton.SetSize( mStageWidth*0.8, mStageHeight*0.11 );
    Actor buttonInnerLabel = setDefaultThemeButton.GetChildAt( 1 );
    buttonInnerLabel.SetSize( mStageWidth*0.8, mStageHeight*0.11 );

  }

  TextLabel CreateTitleLabel( const std::string text, float positionX, float positionY )
  {


    // Make TextLabel Object and set Main or Sub Title Label's text
    TextLabel labelObj = TextLabel::New( text );
    // Set TextLabel's anchor point to TOP_LEFT ( default is CENTER )
    labelObj.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    // Set TextLabel's Position
    labelObj.SetPosition( positionX, positionY );
    // Set TextLabel's Vertical alignment to CENTER ( default is TOP )
    labelObj.SetProperty( TextLabel::Property::VERTICAL_ALIGNMENT, "CENTER" );
    labelObj.SetProperty( TextLabel::Property::HORIZONTAL_ALIGNMENT, "CENTER" );
    labelObj.SetProperty( Control::Property::BACKGROUND_COLOR, Vector4( 0.0f, 0.0f, 0.0f, 0.0f ) );
    labelObj.SetProperty( TextLabel::Property::TEXT_COLOR, Color::WHITE );
    labelObj.SetProperty( TextLabel::Property::POINT_SIZE, 5 );

    labelObj.SetSize( mStageWidth, mStageHeight*0.080 );
    labelObj.SetBackgroundColor( CUSTOM_BLUE_COLOR );
    // Add TextLabel on stage
    mStage.Add( labelObj );

    // Return TextLabel Object that have Main or Sub TextLabel's property
    return labelObj;
  }

  PushButton CreatePushButton( const std::string text, float positionX, float positionY  )
  {

    PushButton buttonObj = PushButton::New();
    buttonObj.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    buttonObj.SetSize( mStageWidth*0.3, mStageHeight*0.11 );
    buttonObj.SetPosition( positionX, positionY );
    buttonObj.SetProperty( Button::Property::UNSELECTED_COLOR, CUSTOM_BLUE_COLOR );
    buttonObj.SetProperty( Button::Property::SELECTED_COLOR, SELECT_CUSTOM_BLUE_COLOR );

    TextLabel labelObj = TextLabel::New( text );
    labelObj.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    labelObj.SetParentOrigin( ParentOrigin::TOP_LEFT );
    labelObj.SetProperty( TextLabel::Property::VERTICAL_ALIGNMENT, "CENTER" );
    labelObj.SetProperty( TextLabel::Property::HORIZONTAL_ALIGNMENT, "CENTER" );
    labelObj.SetProperty( TextLabel::Property::TEXT_COLOR, Color::WHITE );
    labelObj.SetProperty( TextLabel::Property::POINT_SIZE, 5 );

    labelObj.SetSize( mStageWidth*0.3, mStageHeight*0.11 );

    // Add PushButton Label to PushButton
    buttonObj.Add( labelObj );
    mStage.Add( buttonObj );

    return buttonObj;
  }

  bool SetThemeFileButtonClicked( Button button ){
	mStyleManager.ApplyTheme("/usr/apps/com.samsung.dali-demo/scripts/styling-demo.json");
    return true;
  }

  bool SetStyleButtonClicked( Button button ){
	mStyleManager.ApplyStyle( mStyledTextLabel, "/usr/apps/com.samsung.dali-demo/scripts/styling-demo.json", "myStyledTextLabel");
    return true;
  }

  bool SetDefaultThemeClicked( Button button ){
	mStyleManager.ApplyDefaultTheme();
    return true;
  }


private:
  Application&  mApplication;
  Stage mStage;
  StyleManager mStyleManager;
  float mStageWidth;
  float mStageHeight;
  TextLabel mStyledTextLabel;
};

void RunTest( Application& application )
{
  StylingDemo test( application );

  application.MainLoop();
}

// Entry point for Linux & Tizen applications
//
int DALI_EXPORT_API main( int argc, char **argv )
{
  Application application = Application::New( &argc, &argv );

  RunTest( application );

  return 0;
}
