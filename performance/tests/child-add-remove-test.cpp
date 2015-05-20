//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#include <sstream>
#include <iomanip>
#include <dali/dali.h>

using namespace Dali;

namespace
{

} // unnamed namespace

class TestApp : public ConnectionTracker
{
public:
  TestApp( Application& application );
  ~TestApp();

  void OnInitialize( Application& application );
  void OnTerminate( Application& application );

  bool OnTimer();
  void OnTap( Actor actor, TapGesture gesture );
private:
  Application&        mApplication;
  Stage               mStage;
  Vector2             mStageSize;
  Timer               mTimer;
  TapGestureDetector  mTapDetector;

  Actor               mRootActor;
};


TestApp::TestApp( Application& application )
: mApplication( application )
{
  // Connect to the Application's Initialize signal
  mApplication.InitSignal().Connect( this, &TestApp::OnInitialize );

  // Connect to the Application's Terminate signal
  mApplication.TerminateSignal().Connect( this, &TestApp::OnTerminate );
}

TestApp::~TestApp()
{
}

void TestApp::OnInitialize( Application& application )
{
  mStage = Stage::GetCurrent();
  mStageSize = mStage.GetSize();

  mTapDetector = TapGestureDetector::New();
  mTapDetector.DetectedSignal().Connect( this, &TestApp::OnTap );

  mTimer = Timer::New( 10.0f );
  mTimer.TickSignal().Connect( this, &TestApp::OnTimer );
  mTimer.Start();

  mRootActor = Actor::New();
  mRootActor.SetParentOrigin( ParentOrigin::CENTER );
  mRootActor.ApplyConstraint( Constraint::New<Vector3>(Actor::SIZE, ParentSource( Actor::SIZE), EqualToConstraint() ) );
  mStage.Add( mRootActor );
  mTapDetector.Attach( mRootActor );
}

void TestApp::OnTerminate( Application& application )
{
  mTimer.Stop();
  mTimer.TickSignal().Disconnect( this, &TestApp::OnTimer );
  mTimer.Reset();

  mTapDetector.DetachAll();
  mTapDetector.DetectedSignal().Disconnect( this, &TestApp::OnTap );
  mTapDetector.Reset();

  mStage.Remove( mRootActor );
  mRootActor.Reset();
}

bool TestApp::OnTimer()
{
  if( mRootActor.GetChildCount() > 100 )
  {
    mRootActor.Remove( mRootActor.GetChildAt( static_cast<size_t>( Random::Range( 0.0f, 99.0f ))) );
  }
  else
  {
    if( Random::Chance( 0.25f ) )
    {
      for( int i = 0; i < 10; ++i )
      {
        int imageIndex( Random::Range( 0.0f, 31.0f ) );
        if( imageIndex > 21 )
        {
          imageIndex += 3;
        }
        else if( imageIndex > 16 )
        {
          imageIndex += 2;
        }
        else if( imageIndex > 11 )
        {
          ++imageIndex;
        }

        std::stringstream imageName;
        imageName << DALI_IMAGE_DIR "a_";
        imageName << std::setw(4) << std::setfill( '0' ) << imageIndex;
        imageName << ".jpg";
        Image image( Image::New( imageName.str() ) );
        ImageActor actor( ImageActor::New( image ) );
        actor.SetParentOrigin( Vector3(Random::Range(-1.0f, 1.0f), Random::Range(-1.0f, 1.0f), Random::Range(-0.5f, 0.5f)) );
        mRootActor.Add( actor );
      }
    }
  }

  return true;
}

void TestApp::OnTap( Actor actor, TapGesture gesture )
{
  if( gesture.numberOfTouches == 1 )
  {
    mApplication.Quit();
  }
}

/**
 * Native C++ application entry point
 */
int main( int argc, char **argv )
{
  // Create Dali::Application object
  Application application = Application::New( &argc, &argv );

  // Create local application
  TestApp testApp( application );

  // Start main loop
  application.MainLoop();

  return 0;
}
