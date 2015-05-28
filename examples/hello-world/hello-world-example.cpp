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
#include <iostream>
#include <vector>

using namespace Dali;
using namespace Dali::Toolkit;
using namespace std;

namespace
{
const char* const POINT_IMAGE_PATH = DALI_IMAGE_DIR "circle.png";
const char* const RADIUS_PROPERTY_NAME = "Radius";
const char* const RADIAN_PROPERTY_NAME = "Radian";
const int POINT_COUNT = 12;
const int CIRCLE_COUNT = 5;
const Vector2 POINT_SIZE = Vector2(10.0f, 10.0f);

const Vector4 COLORS[CIRCLE_COUNT] =
{
  Vector4(0.0f, 128.0f/255.0f, 128.0f/255.0f, 1.0f),
  Vector4(128.0f/255.0f, 0.0f, 128.0f/255.0f, 1.0f),
  Vector4(0.0f, 0.0f, 128.0f/255.0f, 1.0f),
  Vector4(0.0f, 1.0f, 1.0f, 1.0f),
  Vector4(1.0f, 0.0f, 1.0f, 1.0f)
};

struct RadiusConstraint
{
  RadiusConstraint()
  {
  }

  void operator()(Vector3& current, const PropertyInputContainer& inputs)
  {
    float radius = inputs[0]->GetFloat();
    float radian = inputs[1]->GetFloat();

    current =  Vector3(radius * cos(radian),
                       radius * sin(radian),
                       0.0f);
  }
};

}

// This example shows how to create and display Hello World! using a simple TextActor
//
class HelloWorldController : public ConnectionTracker
{
public:
  enum SceneType
  {
    SCENE_ONE,
    SCENE_TWO,
    SCENE_THREE
  };

  HelloWorldController( Application& application )
  : mApplication( application )
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
    mSceneType = SCENE_ONE;

    // Get a handle to the stage
    Stage stage = Stage::GetCurrent();

    mCircles.resize(CIRCLE_COUNT);
    mPoints.resize(CIRCLE_COUNT);

    for (int i = 0, radius = 30; i != CIRCLE_COUNT; ++i, radius += 20)
    {
      mCircles[i] = CreateCirclePoints(POINT_COUNT, radius, COLORS[i], mPoints[i]);

      if (i % 2)
      {
        mCircles[i].SetOrientation(Radian(Math::PI / POINT_COUNT), Vector3::ZAXIS);
      }
      stage.Add(mCircles[i]);
    }

    mAnimation = Animation::New(4.0f);
    mAnimation.SetLooping(true);

    // Respond to a click anywhere on the stage
    stage.GetRootLayer().TouchedSignal().Connect( this, &HelloWorldController::OnTouch );
  }

  bool OnTouch( Actor actor, const TouchEvent& touch )
  {
    mAnimation.Clear();

    if (touch.GetPoint(0).state == TouchPoint::Up)
    {
      switch (mSceneType)
      {
        case SCENE_ONE :
        {
          for (int i = 0; i != CIRCLE_COUNT; ++i)
          {
            mAnimation.AnimateBy(Property(mCircles[i], Actor::Property::ORIENTATION), Quaternion(Radian(Math::PI * 2.0f), Vector3::ZAXIS));
          }
          mSceneType = SCENE_TWO;
        }
        break;

        case SCENE_TWO :
        {
          for (int i = 0; i != CIRCLE_COUNT; ++i)
          {
            mAnimation.AnimateBy(Property(mCircles[i], Actor::Property::ORIENTATION), Quaternion(Radian(Math::PI * 2.0f), Vector3::ZAXIS));
            Property::Index radiusPropertyIndex = mCircles[i].GetPropertyIndex(RADIUS_PROPERTY_NAME);
            mAnimation.AnimateTo(Property(mCircles[i], radiusPropertyIndex), static_cast<float>((i + 1) * POINT_SIZE.x), AlphaFunction::BOUNCE);
          }
          mSceneType = SCENE_THREE;
        }
        break;

        case SCENE_THREE :
        {
          mSceneType = SCENE_ONE;
        }
        break;
      }

      mAnimation.Play();
    }
    return true;
  }

  Actor CreateCirclePoints(int count, float radius, const Vector4& color, vector<Actor>& points)
  {
    points.reserve(count);

    Actor root = Actor::New();
    root.SetParentOrigin(ParentOrigin::CENTER);
    root.SetAnchorPoint(AnchorPoint::CENTER);
    Property::Index radiusPropertyIndex = root.RegisterProperty(RADIUS_PROPERTY_NAME, radius);

    float radian = 0;
    const float offset = Math::PI * 2 / static_cast<float>(count);

    for (int i = 0; i != count; ++i)
    {
      ResourceImage image = ResourceImage::New(POINT_IMAGE_PATH);
      Actor point = ImageActor::New(image);
      point.SetParentOrigin(ParentOrigin::CENTER);
      point.SetAnchorPoint(AnchorPoint::CENTER);
      point.SetColor(color);
      point.SetSize(POINT_SIZE);
      root.Add(point);

      Property::Index radianPropertyIndex = point.RegisterProperty(RADIAN_PROPERTY_NAME, radian);
      Constraint constraint = Constraint::New<Vector3>(point, Actor::Property::POSITION, RadiusConstraint());
      constraint.AddSource(ParentSource(radiusPropertyIndex));
      constraint.AddSource(LocalSource(radianPropertyIndex));
      constraint.Apply();

      points.push_back(point);

      radian += offset;
    }

    return root;
  }


private:
  Application&  mApplication;
  SceneType mSceneType;
  vector<Actor> mCircles;
  vector<vector<Actor> > mPoints;
  Animation mAnimation;
};

void RunTest( Application& application )
{
  HelloWorldController test(application);

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
