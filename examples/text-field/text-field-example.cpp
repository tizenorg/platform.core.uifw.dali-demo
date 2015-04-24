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
 * @file text-field-example.cpp
 * @brief Basic usage of TextField control
 */

// EXTERNAL INCLUDES
#include <dali-toolkit/dali-toolkit.h>
#include <dali/public-api/text-abstraction/text-abstraction.h>

// INTERNAL INCLUDES
#include "shared/view.h"

using namespace Dali;
using namespace Dali::Toolkit;

namespace
{

const char* const BACKGROUND_IMAGE = DALI_IMAGE_DIR "button-up.9.png";

const float BORDER_WIDTH = 4.0f;

const Size ACTIVATE_BUTTON_SIZE( 60.0f, 60.0f );

} // unnamed namespace

/**
 * @brief The main class of the demo.
 * Demo shows a Text field appearing in a container after the start button is pressed.
 * Touching outside of the text field and it's container hides them until the button is pressed again.
 */
class TextFieldExample : public ConnectionTracker
{
public:

  TextFieldExample( Application& application )
  : mApplication( application )
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &TextFieldExample::Create );
  }

  ~TextFieldExample()
  {
    // Nothing to do here.
  }

  void SetUpLayer( Layer& layer, Size size )
  {
    layer = Layer::New();
    layer.SetAnchorPoint(Dali::AnchorPoint::CENTER);
    layer.SetParentOrigin(Dali::ParentOrigin::CENTER);
    layer.SetSize( size );
    layer.SetTouchConsumed( true ); // Touch Events will be blocked past this Layer.
  }

  void OnShrinkAnimationFinished( Animation&)
  {
    mLayer.Remove( mContainer );
    mActivationButton.SetOpacity(1.0f);
    mOutsideArea.SetOpacity(0.0f);
  }

  void OnTapOutsideArea( Actor actor, const TapGesture& gesture )
  {
    if ( actor == mOutsideArea )
    {
      Animation animation = Animation::New(.5f);
      Vector3 halfSize(0.15f,0.15f,0.15f) ;
      animation.AnimateTo(Property( mContainer, Actor::Property::SCALE ), halfSize, AlphaFunction::EASE_IN_OUT );
      animation.Play();
      animation.FinishedSignal().Connect( this, &TextFieldExample::OnShrinkAnimationFinished );
      mTapDetector.Detach(mOutsideArea);
      mTapDetector.DetectedSignal().Disconnect(this, &TextFieldExample::OnTapOutsideArea );
    }
  }

  void SetUpOutsideTouchArea( ImageActor& imageActor, Actor& parent )
  {
    imageActor = Dali::Toolkit::CreateSolidColorActor(Color::BLUE);
    imageActor.SetOpacity(0.0f);
    imageActor.SetAnchorPoint( AnchorPoint::CENTER );
    imageActor.SetParentOrigin( ParentOrigin::CENTER );
    imageActor.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
    imageActor.SetZ( -1.0f );
    parent.Add( mOutsideArea );
  }

  void SetUpActivationButton( PushButton& button, Actor& parent )
  {
    button = PushButton::New();
    button.SetButtonImage( ResourceImage::New( BACKGROUND_IMAGE ) );
    button.SetParentOrigin( ParentOrigin::CENTER );
    button.SetSize( ACTIVATE_BUTTON_SIZE );
    parent.Add( button );
    button.ClickedSignal().Connect( this, &TextFieldExample::OnActivateButtonClicked );
  }

  void SetUpContainer( Control& control, Size stageSize )
  {
    control = Control::New();
    control.SetParentOrigin( ParentOrigin::CENTER );
    control.SetSize( Vector2(stageSize.width*0.6f, stageSize.width*0.6f) );
    control.SetBackgroundImage( ResourceImage::New( BACKGROUND_IMAGE ) );
    control.SetScale( 0.3f );
  }

  void SetUpTextField( Control& field, Size stageSize, Actor& parent )
  {
    field = TextField::New();
    field.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    field.SetResizePolicy( ResizePolicy::FIXED, Dimension::ALL_DIMENSIONS );
    field.SetProperty( TextField::Property::TEXT, "Unnamed folder" );
    field.SetProperty( TextField::Property::DECORATION_BOUNDING_BOX, Rect<int>( BORDER_WIDTH, BORDER_WIDTH, stageSize.width - BORDER_WIDTH*2, stageSize.height - BORDER_WIDTH*2 ) );
    field.SetSize( Size(150.0f, 40.0f ));
    field.SetBackgroundColor(Vector4( 0.0f, 0.0f, 0.0f, 0.25f));
    field.SetParentOrigin( ParentOrigin::CENTER );
    field.SetAnchorPoint( AnchorPoint::BOTTOM_CENTER );
    parent.Add( field );
  }

  void EnlargeAnimation( Control& control, Animation& animation, Vector3 destinationPosition = Vector3::ZERO )
  {
    Vector3 fullSize(1.0f, 1.0f, 1.0f);

    animation.AnimateTo(Property(control, Actor::Property::SCALE ), fullSize, AlphaFunction::EASE_IN_OUT );
    animation.AnimateTo(Property(control, Actor::Property::POSITION ), destinationPosition, AlphaFunction::EASE_OUT );
    animation.Play();
  }

  bool OnActivateButtonClicked( Button button )
  {
    button.SetOpacity(0.0f);
    mLayer.Add( mContainer );
    Animation animation = Animation::New(.9f);
    EnlargeAnimation( mContainer, animation );
    mOutsideArea.SetOpacity(0.2f);
    mTapDetector = TapGestureDetector::New();
    mTapDetector.Attach(mOutsideArea);
    mTapDetector.Attach(mContainer);
    mTapDetector.DetectedSignal().Connect(this, &TextFieldExample::OnTapOutsideArea);
    return true;
  }

  /**
   * One-time setup in response to Application InitSignal.
   */
  void Create( Application& application )
  {
    DemoHelper::RequestThemeChange();

    Stage stage = Stage::GetCurrent();

    stage.KeyEventSignal().Connect(this, &TextFieldExample::OnKeyEvent);
    Size stageSize = stage.GetSize();

    // Create layer for dimmed background image and to parent popup
    SetUpLayer( mLayer, stageSize );
    stage.Add( mLayer );
    mLayer.RaiseToTop(); // Can be done on demand but this demo has nothing else to raise above

    // touch area to close folder when touched
    SetUpOutsideTouchArea( mOutsideArea, mLayer );

    // button to show text field
    SetUpActivationButton( mActivationButton, mLayer );

    // container to hold text field
    SetUpContainer( mContainer, stageSize );

    TextField textField;
    SetUpTextField( textField, stageSize, mContainer );
  }

  /**
   * Main key event handler
   */
  void OnKeyEvent(const KeyEvent& event)
  {
    if(event.state == KeyEvent::Down)
    {
      if( IsKey( event, DALI_KEY_ESCAPE) || IsKey( event, DALI_KEY_BACK ) )
      {
        mApplication.Quit();
      }
    }
  }

private:

  Application& mApplication;

  Control mContainer;
  Layer mLayer;
  ImageActor mOutsideArea;
  PushButton mActivationButton;
  TapGestureDetector mTapDetector;
};

void RunTest( Application& application )
{
  TextFieldExample test( application );

  application.MainLoop();
}

/** Entry point for Linux & Tizen applications */
int main( int argc, char **argv )
{
  Application application = Application::New( &argc, &argv );

  RunTest( application );

  return 0;
}
