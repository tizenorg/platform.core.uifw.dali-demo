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

#include <dali/dali.h>
#include <dali-toolkit/dali-toolkit.h>
#include <cstring>
#include <fstream>

using namespace Dali;
using namespace Toolkit;

namespace
{
  const std::string JPG_FILENAME1 = DEMO_IMAGE_DIR "gallery-large-1.jpg";
  const std::string JPG_FILENAME2 = DEMO_IMAGE_DIR "gallery-large-2.jpg";
}

// This example shows how to create and use a NativeImageSource as the target of the render task.
//
class NativeImageSourceController : public ConnectionTracker
{
public:

  NativeImageSourceController( Application& application )
  : mApplication( application ),
    mChangeImageFlag( false ),
    mChangeRefreshRateFlag( false )
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &NativeImageSourceController::Create );
  }

  ~NativeImageSourceController()
  {
    // Nothing to do here;
  }

  // The Init signal is received once (only) during the Application lifetime
  void Create( Application& application )
  {
    // Get a handle to the stage
    Stage stage = Stage::GetCurrent();
    stage.SetBackgroundColor( Color::WHITE );
    stage.GetRootLayer().TouchedSignal().Connect( this, &NativeImageSourceController::OnTouch );
    stage.KeyEventSignal().Connect(this, &NativeImageSourceController::OnKeyEvent);

    CreateScene();

    // timer for switching between native image and resource image.
    mSetImageTimer = Timer::New( 2000 );
    mSetImageTimer.TickSignal().Connect( this, &NativeImageSourceController::SetImageTick );
    mSetImageTimer.Start();
  }

  bool CreateScene()
  {
    Stage stage = Stage::GetCurrent();
    Vector2 size = stage.GetSize();

    // Create the native image source
    mNativeImageSourcePtr = NativeImageSource::New( size.width, size.height, NativeImageSource::COLOR_DEPTH_DEFAULT );

    // Create a image view as source actor to be renderer to the native image source
    Actor sourceActor = ImageView::New(JPG_FILENAME1);
    sourceActor.SetParentOrigin( ParentOrigin::CENTER);
    sourceActor.SetAnchorPoint( AnchorPoint::CENTER );
    stage.Add( sourceActor );

    Animation animation = Animation::New(2.f);
    Degree relativeRotationDegrees(90.0f);
    Radian relativeRotationRadians(relativeRotationDegrees);
    animation.AnimateTo( Property( sourceActor, Actor::Property::ORIENTATION ), Quaternion( relativeRotationRadians, Vector3::ZAXIS ), AlphaFunction::LINEAR, TimePeriod(0.f, 0.5f));
    animation.AnimateBy( Property( sourceActor, Actor::Property::ORIENTATION ), Quaternion( relativeRotationRadians, Vector3::ZAXIS ), AlphaFunction::LINEAR, TimePeriod(0.5f, 0.5f));
    animation.AnimateBy( Property( sourceActor, Actor::Property::ORIENTATION ), Quaternion( relativeRotationRadians, Vector3::ZAXIS ), AlphaFunction::LINEAR, TimePeriod(1.f, 0.5f));
    animation.AnimateBy( Property( sourceActor, Actor::Property::ORIENTATION ), Quaternion( relativeRotationRadians, Vector3::ZAXIS ), AlphaFunction::LINEAR, TimePeriod(1.5f, 0.5f));
    animation.SetLooping(true);
    animation.Play();

    // create a offscreen renderer task to render content into the native image source
    FrameBufferImage targetBuffer = FrameBufferImage::New( *mNativeImageSourcePtr );

    CameraActor cameraActor = CameraActor::New(size);
    cameraActor.SetParentOrigin(ParentOrigin::CENTER);
    stage.Add(cameraActor);

    RenderTaskList taskList = stage.GetRenderTaskList();
    mOffscreenRenderTask = taskList.CreateTask();
    mOffscreenRenderTask.SetSourceActor( sourceActor );
    mOffscreenRenderTask.SetExclusive(true);
    mOffscreenRenderTask.SetClearColor( Color::WHITE );
    mOffscreenRenderTask.SetClearEnabled(true);
    mOffscreenRenderTask.SetCameraActor(cameraActor);
    mOffscreenRenderTask.GetCameraActor().SetInvertYAxis(true);
    mOffscreenRenderTask.SetTargetFrameBuffer( targetBuffer );
    mOffscreenRenderTask.SetRefreshRate( RenderTask::REFRESH_ALWAYS );

    // Display the native image on the screen
    mNativeImage = NativeImage::New( *mNativeImageSourcePtr );
    mImageView = ImageView::New( mNativeImage );
    mImageView.SetParentOrigin( ParentOrigin::CENTER );
    mImageView.SetAnchorPoint( AnchorPoint::CENTER );
    stage.Add( mImageView );

    mResourceImage = ResourceImage::New( JPG_FILENAME2 );

    return false;
  }

  bool SetImageTick()
  {
    if( mChangeImageFlag == false )
    {
      mImageView.SetImage( mResourceImage );
    }
    else
    {
      mImageView.SetImage( mNativeImage );
    }

    mChangeImageFlag = !mChangeImageFlag;
    return true;
  }

  bool OnTouch( Actor actor, const TouchEvent& touch )
  {
    const TouchPoint &point = touch.GetPoint(0);
    if(point.state == TouchPoint::Down)
    {
      if( mChangeRefreshRateFlag )
      {
        mOffscreenRenderTask.SetRefreshRate( RenderTask::REFRESH_ALWAYS );
      }
      else
      {
        mOffscreenRenderTask.SetRefreshRate( RenderTask::REFRESH_ONCE );
      }

      mChangeRefreshRateFlag = !mChangeRefreshRateFlag;
    }
    return true;
  }

  void OnKeyEvent(const KeyEvent& event)
  {
    if(event.state == KeyEvent::Down)
    {
      if( IsKey( event, Dali::DALI_KEY_ESCAPE) || IsKey( event, Dali::DALI_KEY_BACK) )
      {
        mSetImageTimer.Stop();
        mApplication.Quit();
      }
    }
  }

private:
  Application&  mApplication;
  Timer mSetImageTimer;
  NativeImageSourcePtr mNativeImageSourcePtr;
  NativeImage mNativeImage;
  ImageView mImageView;
  ResourceImage mResourceImage;
  RenderTask mOffscreenRenderTask;
  bool mChangeImageFlag;
  bool mChangeRefreshRateFlag;
};

void RunTest( Application& application )
{
  NativeImageSourceController test( application );

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
