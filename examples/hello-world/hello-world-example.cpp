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
#include <vector>
#include <cmath>

using namespace Dali;
using namespace Dali::Toolkit;
using namespace std;

namespace
{
const char* CIRCLE_IMAGE_PATH(DALI_IMAGE_DIR "circle_transparent.png");
const char* BLEND_IMAGE_PATH(DALI_IMAGE_DIR "blend.png");

const int CIRCLE_COUNT(25);
}

// This example shows how to create and display Hello World! using a simple TextActor
//
class HelloWorldController : public ConnectionTracker
{
public:

  enum SceneType
  {
    SCENE_ONE = 0,
    SCENE_TWO = 1
  };

  HelloWorldController( Application& application )
  : mApplication( application )
  , mSceneType(SCENE_ONE)
  , mRadius(0.0f)
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &HelloWorldController::Create );
  }

  ~HelloWorldController()
  {
    // Nothing to do here;
  }

  // The Init signal is received once (only) during the Application lifetime
  void Create( Application& application )
  {
    // Get a handle to the stage
    Stage stage = Stage::GetCurrent();
    stage.SetBackgroundColor( Color::BLACK );

    Vector2 stageSize = stage.GetSize();
    mOriginPoint = stageSize / 2.0f;

    mRoot = Actor::New();
    mRoot.SetParentOrigin(ParentOrigin::CENTER);
    mRoot.SetAnchorPoint(AnchorPoint::CENTER);
    mRoot.SetSize(stageSize);
    stage.Add(mRoot);

    mRotateAnimation = Animation::New(10.0f);
    mRotateAnimation.SetLooping(true);
    mRotateAnimation.AnimateBy(Property(mRoot, Actor::Property::ORIENTATION), Quaternion(Radian(Math::PI * 2), Vector3::ZAXIS) );
    // mRotateAnimation.Play();

    CreateCircles(CIRCLE_COUNT);
    DoLayout(mSceneType);

    ResourceImage res = ResourceImage::New(BLEND_IMAGE_PATH);
    mBlend = ImageActor::New(res);
    mBlend.SetParentOrigin(ParentOrigin::CENTER);
    mBlend.SetAnchorPoint(AnchorPoint::CENTER);
    mBlend.SetSize(stageSize);
    mBlend.SetBlendMode(BlendingMode::ON);
    mBlend.SetBlendFunc(BlendingFactor::DST_COLOR, BlendingFactor::ZERO, BlendingFactor::ZERO, BlendingFactor::ONE);
    mBlend.SetZ(1.0f);
    mRoot.Add(mBlend);

    // Respond to a click anywhere on the stage
    stage.GetRootLayer().TouchedSignal().Connect( this, &HelloWorldController::OnTouch );
  }

  void CreateCircles(int count)
  {
    mCircles.reserve(count);

    for (int c = 0; c != count; ++c)
    {
      ResourceImage res = ResourceImage::New(CIRCLE_IMAGE_PATH);
      ImageActor image = ImageActor::New(res);
      image.SetSize(Vector3(300.0f, 300.0f, 0.0f));
      image.SetPosition(mOriginPoint);
      mCircles.push_back(image);
    }
  }

  void DoLayout(SceneType sceneType)
  {
    switch (sceneType)
    {
      case SCENE_ONE:
      {
        float rad = 0.0f;
        float offset = Math::PI * 2.0f / CIRCLE_COUNT;
        Animation animation = Animation::New(2.0f);
        for (vector<Actor>::iterator iter = mCircles.begin(); iter != mCircles.end(); ++iter, rad += offset)
        {
          Actor circle = *iter;
          circle.SetAnchorPoint(AnchorPoint::BOTTOM_CENTER);
          circle.SetOrientation(Radian(rad), Vector3::ZAXIS);
          mRoot.Add(circle);

          animation.AnimateTo(Property(circle, Actor::Property::POSITION), mOriginPoint);
          animation.AnimateTo(Property(circle, Actor::Property::SIZE), Vector3(300.0f, 300.0f, 0.0f));
        }
        animation.Play();
        mSceneType = SCENE_TWO;
      }
      break;

      case SCENE_TWO:
      {
        Stage stage = Stage::GetCurrent();
        float rad = 0.0f;
        float offset = Math::PI * 2.0f / CIRCLE_COUNT;
        Animation animation = Animation::New(2.0f);
        for (vector<Actor>::iterator iter = mCircles.begin(); iter != mCircles.end(); ++iter, rad += offset)
        {
          Actor circle = *iter;
          circle.SetAnchorPoint(AnchorPoint::BOTTOM_CENTER);
          circle.SetPosition(mOriginPoint);
          circle.SetOrientation(Radian(rad), Vector3::ZAXIS);
          mRoot.Add(circle);

          mRadius = 100.0f;
          Vector3 offset = Vector3(-sin(rad) * mRadius,
                                   cos(rad) * mRadius, 
                                   0.0f);

          animation.AnimateTo(Property(circle, Actor::Property::SIZE), Vector3(500.0f, 500.0f, 0.0f));
          animation.AnimateBy(Property(circle, Actor::Property::POSITION), offset);
        }
        animation.Play();
        mSceneType = SCENE_ONE;
      }
      break;
    }
  }


  bool OnTouch( Actor actor, const TouchEvent& touch )
  {
    if (touch.GetPoint(0).state == TouchPoint::Up)
    {
      DoLayout(mSceneType);
    }
    return true;
  }

private:
  Application&  mApplication;
  SceneType mSceneType;
  Actor mRoot;
  ImageActor mBlend;
  std::vector<Actor> mCircles;
  float mRadius;
  Vector3 mOriginPoint;
  Animation mRotateAnimation;
};

void RunTest( Application& application )
{
  HelloWorldController test( application );

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
