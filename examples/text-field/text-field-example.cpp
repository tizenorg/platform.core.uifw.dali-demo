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

} // unnamed namespace

/**
 * @brief The main class of the demo.
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
  }

  void SetUpOutsideTouchArea( ImageActor& imageActor )
  {
    imageActor = Dali::Toolkit::CreateSolidColorActor(Color::BLUE);
    imageActor.SetOpacity(0.0f);
    imageActor.SetAnchorPoint( AnchorPoint::CENTER );
    imageActor.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
    imageActor.SetZ( -1.0f );
  }

  void SetUpContainer( Control& control, Size stageSize )
  {
    control = Control::New();
    control.SetName( "Container" );
    control.SetParentOrigin( ParentOrigin::CENTER );
    control.SetSize( Vector2(stageSize.width*0.6f, stageSize.width*0.6f) );
    control.SetBackgroundImage( ResourceImage::New( BACKGROUND_IMAGE ) );
    control.GetChildAt(0).SetZ(-1.0f);
    control.SetScale( 0.3f );
  }

  void SetUpTextField( Control& field, Size stageSize )
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
  }

  void EnlargeAnimation( Control& control, Animation& animation, Vector3 destinationPosition = Vector3::ZERO )
  {
    Vector3 fullSize(1.0f,1.0f,1.0f) ;

    animation.AnimateTo(Property(control, Actor::Property::SCALE ), fullSize, AlphaFunction::EASE_IN_OUT );
    animation.AnimateTo(Property(control, Actor::Property::POSITION ), destinationPosition, AlphaFunction::EASE_OUT );
    animation.Play();
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
    Layer layer;
    SetUpLayer( layer, stageSize );

    stage.Add(layer);
    layer.RaiseToTop(); // Can be done on demand but this demo has nothing else to raise above

    // touch area to close folder with dimmed color
    ImageActor outsideFolderArea;
    SetUpOutsideTouchArea( outsideFolderArea );
    layer.Add(outsideFolderArea);
    //mFolderTapDetector.Attach(outsideFolderArea); todo

    SetUpContainer( mContainer, stageSize );
    layer.Add( mContainer );

    TextField textField;
    SetUpTextField( textField, stageSize );

    mContainer.Add( textField );

    Animation animation = Animation::New(.9f);
    EnlargeAnimation( mContainer, animation );

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
