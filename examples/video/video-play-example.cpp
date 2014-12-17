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

#include <dali/dali.h>
#include <dali-toolkit/dali-toolkit.h>
#include <dali/integration-api/debug.h>

using namespace Dali;

static const char TestVideoPath[256] = "/opt/usr/media/Videos/Color.mp4";
static const float TestVideoWidth  = 720.0f;
static const float TestVideoHeight = 1280.0f;

/**
 *  This example shows how to play video using video-view control.
 */
class TestVideoPlayer : public ConnectionTracker
{
public:

  TestVideoPlayer(Application &app);
  ~TestVideoPlayer();

  void OnInit(Application& app);

private:

  Application& mApp;

  bool mInit;

  Toolkit::VideoView mView;
  Animation mAnimation;
};

TestVideoPlayer::TestVideoPlayer(Application &app)
  : mApp(app), mInit(false)
{
  // Connect to the Application's Init signal
  app.InitSignal().Connect(this, &TestVideoPlayer::OnInit);
}

TestVideoPlayer::~TestVideoPlayer()
{
  // video stop & destroy

  if(mView)
    mView.Stop();
}

void TestVideoPlayer::OnInit(Application& app)
{
  mView = Toolkit::VideoView::New();
  mView.SetVideoFile(TestVideoPath);
  Stage::GetCurrent().Add(mView);

  mView.SetAnchorPoint(AnchorPoint::CENTER);
  mView.SetParentOrigin(ParentOrigin::CENTER);
  mView.SetSize(600, 300);

  mAnimation = Animation::New(2.0f);
  mAnimation.RotateBy(mView, Degree(360), Vector3::YAXIS);
  mAnimation.SetLooping(true);
  mAnimation.Play();

  mView.Play();
}

void RunTest(Application& app)
{
  TestVideoPlayer test(app);
  app.MainLoop();
}

// Entry point for tizen applications
//
int main(int argc, char **argv)
{
  Application app = Application::New(&argc, &argv);

  RunTest(app);

  return 0;
}
