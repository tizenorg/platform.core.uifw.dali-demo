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

#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/devel-api/controls/renderer-factory/renderer-factory.h>
#include <dali-toolkit/devel-api/controls/renderer-factory/control-renderer.h>
#include <string.h>

using namespace Dali;

namespace
{
const char*  const CHANGE_IMAGE_ICON(DEMO_IMAGE_DIR "icon-change.png");
const char * const CHANGE_IMAGE_ICON_SELECTED( DEMO_IMAGE_DIR "icon-change-selected.png" );
const char*  const RESET_IMAGE_ICON(DEMO_IMAGE_DIR "icon-reset.png");
const char * const RESET_IMAGE_ICON_SELECTED( DEMO_IMAGE_DIR "icon-reset-selected.png" );


const float MAX_SCALE = 6.f;

const char* SVG_IMAGES[] =
{
    DEMO_IMAGE_DIR "drawing.svg",
    DEMO_IMAGE_DIR "Bunny.svg",
    DEMO_IMAGE_DIR "Olive_Wreath_clip_art.svg",
    DEMO_IMAGE_DIR "drops.svg",
    DEMO_IMAGE_DIR "Camera.svg",
    DEMO_IMAGE_DIR "Contacts.svg",
    DEMO_IMAGE_DIR "Crown_of_Italy.svg",
    DEMO_IMAGE_DIR "Flamingo.svg",
    DEMO_IMAGE_DIR "FoodChain.svg",
    DEMO_IMAGE_DIR "Kid1.svg",
    DEMO_IMAGE_DIR "Mail.svg",
    DEMO_IMAGE_DIR "Message.svg",
    DEMO_IMAGE_DIR "Phone.svg",
    DEMO_IMAGE_DIR "Settings.svg",
    DEMO_IMAGE_DIR "Tiger.svg",
    DEMO_IMAGE_DIR "Twitter.svg",
    DEMO_IMAGE_DIR "World.svg",
    DEMO_IMAGE_DIR "butterfly.svg",
    DEMO_IMAGE_DIR "car.svg",
    DEMO_IMAGE_DIR "embrace.svg",
    DEMO_IMAGE_DIR "picasso.svg",
    DEMO_IMAGE_DIR "spring.svg",
    DEMO_IMAGE_DIR "spring_bicicleta.svg",
    DEMO_IMAGE_DIR "firefox.svg",
    DEMO_IMAGE_DIR "Lemon_HOUSE.svg",
    DEMO_IMAGE_DIR "Lemon_Svg_Theme_clip_art.svg",
    DEMO_IMAGE_DIR "Svg_Globe_Green_clip_art.svg",
    DEMO_IMAGE_DIR "Christmas_L5.svg",
};
const unsigned int NUM_SVG_IMAGES( sizeof( SVG_IMAGES ) / sizeof( SVG_IMAGES[0] ) );
}

// This example shows how to display svg images with ImageView
//
class ImageSvgController : public ConnectionTracker
{
public:

  ImageSvgController( Application& application )
  : mApplication( application ),
    mScale( 1.f ),
    mIndex( 0 )
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &ImageSvgController::Create );
  }

  ~ImageSvgController()
  {
  }

  // The Init signal is received once (only) during the Application lifetime
  void Create( Application& application )
  {
    // Get a handle to the stage
    Stage stage = Stage::GetCurrent();
    stage.SetBackgroundColor( Color::WHITE );
    Vector2 stageSize = stage.GetSize();
    mActorSize = stageSize/2.f,

    // Background, for receiving gestures
    mStageBackground = Actor::New();
    mStageBackground.SetAnchorPoint( AnchorPoint::TOP_CENTER );
    mStageBackground.SetParentOrigin( ParentOrigin::TOP_CENTER );
    mStageBackground.SetSize( stageSize.x, stageSize.y );
    stage.Add(mStageBackground);

    Toolkit::PushButton changeButton = Toolkit::PushButton::New();
    changeButton.SetButtonImage( ResourceImage::New( CHANGE_IMAGE_ICON ) );
    changeButton.SetSelectedImage( ResourceImage::New( CHANGE_IMAGE_ICON_SELECTED ) );
    changeButton.SetScale(3.f);
    changeButton.SetAnchorPoint( AnchorPoint::TOP_RIGHT );
    changeButton.SetParentOrigin( ParentOrigin::TOP_RIGHT );
    stage.Add( changeButton );
    changeButton.ClickedSignal().Connect( this, &ImageSvgController::OnChangeButtonClicked );

    Toolkit::PushButton resetButton = Toolkit::PushButton::New();
    resetButton.SetButtonImage( ResourceImage::New( RESET_IMAGE_ICON ) );
    resetButton.SetSelectedImage( ResourceImage::New( RESET_IMAGE_ICON_SELECTED ) );
    resetButton.SetScale(3.f);
    resetButton.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    resetButton.SetParentOrigin( ParentOrigin::TOP_LEFT );
    stage.Add( resetButton );
    resetButton.ClickedSignal().Connect( this, &ImageSvgController::OnResetButtonClicked );

    stage.KeyEventSignal().Connect(this, &ImageSvgController::OnKeyEvent);

    for( unsigned int i=0; i<4u; i++)
    {
      mSvgActor[i] = Toolkit::ImageView::New(SVG_IMAGES[mIndex+i]);
      mSvgActor[i].SetSize( mActorSize );
      stage.Add( mSvgActor[i] );
    }
    mSvgActor[0].SetParentOrigin( ParentOrigin::CENTER );
    mSvgActor[0].SetAnchorPoint( AnchorPoint::BOTTOM_RIGHT );
    mSvgActor[1].SetParentOrigin( ParentOrigin::CENTER );
    mSvgActor[1].SetAnchorPoint( AnchorPoint::BOTTOM_LEFT );
    mSvgActor[2].SetParentOrigin( ParentOrigin::CENTER );
    mSvgActor[2].SetAnchorPoint( AnchorPoint::TOP_RIGHT );
    mSvgActor[3].SetParentOrigin( ParentOrigin::CENTER );
    mSvgActor[3].SetAnchorPoint( AnchorPoint::TOP_LEFT );


    mPanGestureDetector = PanGestureDetector::New();
    mPanGestureDetector.DetectedSignal().Connect( this, &ImageSvgController::OnPanGesture );
    mPanGestureDetector.Attach( mStageBackground );

    mPinchGestureDetector = PinchGestureDetector::New();
    mPinchGestureDetector.Attach( mStageBackground);
    mPinchGestureDetector.DetectedSignal().Connect(this, &ImageSvgController::OnPinch);
  }

  bool OnChangeButtonClicked( Toolkit::Button button )
  {
    mIndex = (mIndex+4) % NUM_SVG_IMAGES;
    for( unsigned int i=0; i<4u; i++)
    {
      mSvgActor[i].SetImage(SVG_IMAGES[mIndex+i]);
    }

    return true;
  }

  bool OnResetButtonClicked( Toolkit::Button button )
  {
    for( unsigned int i=0; i<4u; i++)
    {
      mSvgActor[i].SetSize(mActorSize);
      mSvgActor[i].SetPosition( Vector3::ZERO );
      mScale = 1.f;
    }

    return true;
  }

  void OnPanGesture( Actor actor, const PanGesture& gesture )
  {
    if( gesture.state == Gesture::Continuing )
    {
      for( unsigned int i=0; i<4u; i++)
      {
        mSvgActor[i].TranslateBy(Vector3(gesture.displacement));
      }
    }
  }

  void OnPinch(Actor actor, const PinchGesture& gesture)
  {
    if (gesture.state == Gesture::Started)
    {
      mScaleAtPinchStart = mScale;
    }
    if( gesture.state == Gesture::Finished )
    {
      mScale = mScaleAtPinchStart * gesture.scale;
      mScale = mScale > MAX_SCALE ? MAX_SCALE : mScale;
      for( unsigned int i=0; i<4u; i++)
      {
        mSvgActor[i].SetSize( mActorSize * mScale);
      }
    }
  }

  /**
    * Main key event handler
    */
   void OnKeyEvent(const KeyEvent& event)
   {
     if(event.state == KeyEvent::Down)
     {
       if( IsKey( event, Dali::DALI_KEY_ESCAPE) || IsKey( event, Dali::DALI_KEY_BACK) )
       {
         mApplication.Quit();
       }
       else
       {
         const char* keyName = event.keyPressedName.c_str();
         if( strcmp(keyName, "Left") == 0 )
         {
           mScale /= 1.1f;
           for( unsigned int i=0; i<4u; i++)
           {
             mSvgActor[i].SetSize( mActorSize * mScale);
           }
         }
         else if( strcmp(keyName, "Right") == 0 )
         {
           if( mScale < MAX_SCALE )
           {
             mScale *= 1.1f;
           }
           for( unsigned int i=0; i<4u; i++)
           {
             mSvgActor[i].SetSize( mActorSize * mScale);
           }
         }
       }
     }
   }

private:
  Application&  mApplication;
  Actor            mStageBackground;
  PanGestureDetector    mPanGestureDetector;
  PinchGestureDetector  mPinchGestureDetector;
  Toolkit::ControlRenderer       mSvgRenderer;

  Toolkit::ImageView  mSvgActor[4];
  Vector2       mActorSize;
  float         mScale;
  float         mScaleAtPinchStart;

  unsigned int  mIndex;
};

void RunTest( Application& application )
{
  ImageSvgController test( application );

  application.MainLoop();
}

// Entry point for Linux & Tizen applications
//
int main( int argc, char **argv )
{
  Application application = Application::New( &argc, &argv );

  RunTest( application );

  return 0;
}
