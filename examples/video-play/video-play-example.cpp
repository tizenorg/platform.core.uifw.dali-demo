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

static const char TestVideoPath[256] = "/opt/usr/media/Videos/Color.mp4";
static const float VideoWidth  = 600.0f;
static const float VideoHeight = 300.0f;

/**
 *  This example shows how to play video using video-view control.
 */
class TestVideoPlayer : public ConnectionTracker
{
public:

  TestVideoPlayer(Application &app);
  ~TestVideoPlayer();

  void Create(Application& app);

private:

  Application& mApplication;

  Toolkit::VideoView mView;
  Animation mAnimation;
};

TestVideoPlayer::TestVideoPlayer(Application &app)
  : mApplication(app)
{
  // Connect to the Application's Init signal
  mApplication.InitSignal().Connect(this, &TestVideoPlayer::Create);
}

TestVideoPlayer::~TestVideoPlayer()
{
  // video stop & destroy
  if(mView)
    mView.Stop();
}

void TestVideoPlayer::Create(Application& app)
{
  mView = Toolkit::VideoView::New();
  mView.SetVideoFile(TestVideoPath);
  Stage::GetCurrent().Add(mView);

  mView.SetAnchorPoint(AnchorPoint::CENTER);
  mView.SetParentOrigin(ParentOrigin::CENTER);
  mView.SetSize(VideoWidth, VideoHeight);

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
