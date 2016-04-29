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

#include <dali-toolkit/dali-toolkit.h>

//#define USE_APPCORE_WATCH

#ifdef USE_APPCORE_WATCH
#include <appcore-watch/watch_app.h>
#endif

using namespace Dali;
using namespace Dali::Toolkit;

// This example shows how to use sensor using a simple TextLabel
//
class WatchController : public ConnectionTracker
{
public:
  WatchController( Watch& application )
  : mApplication( application )
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &WatchController::Create );
  }

  ~WatchController()
  {
    // Nothing to do here;
  }

  // The Init signal is received once (only) during the Application lifetime
  void Create( Application& application )
  {
    mImageDirectory = DEMO_IMAGE_DIR;//      app_get_resource_path();

    // Get a handle to the stage
    Stage stage = Stage::GetCurrent();
    stage.SetBackgroundColor( Color::BLUE);

    mTextLabel = TextLabel::New( "Watch Demo" );
    mTextLabel.SetParentOrigin( ParentOrigin::CENTER );
    mTextLabel.SetAnchorPoint( AnchorPoint::CENTER );
    mTextLabel.SetProperty( TextLabel::Property::HORIZONTAL_ALIGNMENT, "CENTER" );
    mTextLabel.SetProperty( TextLabel::Property::VERTICAL_ALIGNMENT, "CENTER" );
    mTextLabel.SetProperty( TextLabel::Property::TEXT_COLOR, Color::WHITE );
    mTextLabel.SetProperty( TextLabel::Property::POINT_SIZE, 150.0f );
    mTextLabel.SetName( "WatchLabel" );
    stage.Add( mTextLabel );

    CreateClockGUI();

    // Respond to a click anywhere on the stage
    stage.GetRootLayer().TouchedSignal().Connect( this, &WatchController::OnTouch );
    mApplication.TimeTickSignal().Connect( this, &WatchController::OnTimeTick );
    mApplication.AmbientChangedSignal().Connect( this, &WatchController::OnAmbientChange );
  }

  void CreateClockGUI()
  {
    //////////////////////////////////////////////////////////////
    // Create Root actor.
    //////////////////////////////////////////////////////////////
    Actor mClockStage = Actor::New();
    mClockStage.SetAnchorPoint(AnchorPoint::CENTER);
    mClockStage.SetParentOrigin(ParentOrigin::CENTER);

    // Add clock stage on Dali::Stage.
    Stage::GetCurrent().Add(mClockStage);

    //////////////////////////////////////////////////////////////
    // Create Background.
    //////////////////////////////////////////////////////////////
    mBackground = ImageView::New(mImageDirectory + "style_info_1_bg.png");

    // Set Anchor and Parent Property.
    mBackground.SetAnchorPoint(AnchorPoint::CENTER);
    mBackground.SetParentOrigin(ParentOrigin::CENTER);

    // Add Background on Stage.
    mClockStage.Add(mBackground);

    //////////////////////////////////////////////////////////////
    // Create Hour Hand.
    //////////////////////////////////////////////////////////////
    mHourHand = ImageView::New( mImageDirectory + "info_3_h_l.png");

    // Set Anchor and Parent Property.
    const Vector3 HOUR_HAND_ANCHOR_POINT(0.5f, 104.f/158.f, 0.5f);
    mHourHand.SetAnchorPoint(HOUR_HAND_ANCHOR_POINT);
    mHourHand.SetParentOrigin(ParentOrigin::CENTER);

    // Set Z Position.
    mHourHand.SetZ(0.1f);

    // Add Hour Hand on Background.
    mClockStage.Add(mHourHand);

    //////////////////////////////////////////////////////////////
    // Create Minute Hand.
    //////////////////////////////////////////////////////////////
    mMinuteHand = ImageView::New( mImageDirectory + "info_3_m_l.png");

    // Set Anchor and Parent Property.
    const Vector3 MINUTE_HAND_ANCHOR_POINT(0.5f, 155.f/209.f, 0.5f);
    mMinuteHand.SetAnchorPoint(MINUTE_HAND_ANCHOR_POINT);
    mMinuteHand.SetParentOrigin(ParentOrigin::CENTER);

    // Set Z Position.
    mMinuteHand.SetZ(0.2f);

    // Add Hour Hand on Background.
    mClockStage.Add(mMinuteHand);

    //////////////////////////////////////////////////////////////
    // Create Second Hand.
    //////////////////////////////////////////////////////////////
    mSecondHand = ImageView::New( mImageDirectory + "info_3_sec.png");

    // Set Anchor and Parent Property.
    const Vector3 SECOND_HAND_ANCHOR_POINT(0.5f, 160.f/236.f, 0.5f);
    mSecondHand.SetAnchorPoint(SECOND_HAND_ANCHOR_POINT);
    mSecondHand.SetParentOrigin(ParentOrigin::CENTER);

    // Set Z Position.
    mSecondHand.SetZ(0.3f);

    // Add Hour Hand on Background.
    mClockStage.Add(mSecondHand);
  }

  bool OnTouch( Actor actor, const TouchEvent& touch )
  {
    // quit the application
    mApplication.Quit();
    return true;
  }

  void OnTimeTick( Application& application, void *time )
  {
#ifdef USE_APPCORE_WATCH
      watch_time_h watchTime = (watch_time_h)time;
      int hour, min, sec;

      // Get current local time
      watch_time_get_hour(watchTime, &hour);
      watch_time_get_minute(watchTime, &min);
      watch_time_get_second(watchTime, &sec);

      // Update the angle of each hands
      mHourHand.RotateBy(Degree(hour * 30.f /* 30 degree per hour */ + min * 0.5f /* 0.5 degree per min */), Vector3::ZAXIS);
      mMinuteHand.RotateBy(Degree(min * 6.f /* 6 degree per min */ + sec * 0.1f /* 0.1 degree per sec */), Vector3::ZAXIS);
      mSecondHand.RotateBy(Degree(sec * 6.f /* 6 degree per sec */), Vector3::ZAXIS);
#endif
  }

  void OnAmbientChange( Application& application, bool ambient )
  {
  }

private:
  Watch&  mApplication;
  std::string mImageDirectory;
  TextLabel mTextLabel;

  Actor mClockStage;
  ImageView mBackground;
  ImageView mHourHand;
  ImageView mMinuteHand;
  ImageView mSecondHand;
};

// Entry point for Linux & Tizen applications
//
int DALI_EXPORT_API main( int argc, char **argv )
{
  Watch application = Watch::New( &argc, &argv );
  WatchController test( application );

  application.MainLoop();
  return 0;
}

