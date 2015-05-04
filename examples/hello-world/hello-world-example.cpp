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

#include <stdio.h>
#include <dali-toolkit/dali-toolkit.h>

using namespace Dali;
using namespace Toolkit;

#define BLUR_LEVELS 3

const char* BLUR_IMAGES[BLUR_LEVELS] =
{
  DALI_IMAGE_DIR "blur_1.png",
  DALI_IMAGE_DIR "blur_2.png",
  DALI_IMAGE_DIR "blur_3.png"
};

#if 1
struct ActorOpacityConstraint
{
  ActorOpacityConstraint(int totalImageNum, int currentImageIdx)
  {
    float rangeLength = 1.f / static_cast<float>( totalImageNum );
    float index = static_cast<float>( currentImageIdx );
    mRange = Vector2( index*rangeLength, (index+1.f)*rangeLength );
  }

  void operator()(float& current, const PropertyInputContainer& inputs)
  {
    const float& blurStrength = inputs[0]->GetFloat();
    if (blurStrength <= mRange.x)
    {
      current = 1.f;
    }
    else if (blurStrength > mRange.y)
    {
      current = 0.f;
    }
    else
    {
      current = (mRange.y - blurStrength)/(mRange.y-mRange.x);
    }
  }

  Vector2 mRange;
};
#else
struct ActorOpacityConstraint
{
  ActorOpacityConstraint(int totalImageNum, int currentImageIdx)
  {
    float rangeLength = 1.f / static_cast<float>( totalImageNum );
    float index = static_cast<float>( currentImageIdx );
    mRange = Vector2( index*rangeLength, (index+1.f)*rangeLength );
  }

  float operator()( float current, const PropertyInput& blurProperty )
  {
    float blurStrength = blurProperty.GetFloat();
    if(blurStrength <= mRange.x)
    {
      return 1.f;
    }
    else if(blurStrength > mRange.y)
    {
      return 0.f;
    }
    else
    {
      return (mRange.y - blurStrength)/(mRange.y-mRange.x);
    }
  }

  Vector2 mRange;
};
#endif

// This example shows how to create and display Hello World! using a simple TextActor
//
class HelloWorldController : public ConnectionTracker
{
public:

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
    // Get a handle to the stage
    Stage stage = Stage::GetCurrent();
    //stage.SetBackgroundColor( Color::WHITE );

    mBaseActor = Actor::New();
    mBaseActor.SetParentOrigin(ParentOrigin::CENTER);
    mBaseActor.SetAnchorPoint(AnchorPoint::CENTER);
    stage.Add(mBaseActor);

    mBlurStrengthPropertyIndex = mBaseActor.RegisterProperty("blur-strength", 0.f);

    for (int i = 0; i < BLUR_LEVELS; i++)
    {
      mBlurActors[i] = ImageActor::New(ResourceImage::New(BLUR_IMAGES[i])); // CreateSolidColorActor(Color::BLUE);
      mBlurActors[i].SetColor(Color::RED);
      mBlurActors[i].SetParentOrigin(ParentOrigin::CENTER);
      mBlurActors[i].SetAnchorPoint(AnchorPoint::CENTER);
      stage.Add(mBlurActors[i]);

      if (i)
      {
        mBlurActors[i].SetOpacity(0.f);
      }

      Constraint constraint = Constraint::New<float>(mBlurActors[i],
                                                     Actor::Property::COLOR_ALPHA,
                                                     ActorOpacityConstraint(BLUR_LEVELS, i));
      constraint.AddSource(Source(mBaseActor, mBlurStrengthPropertyIndex));
      constraint.Apply();
    }

    // Respond to a click anywhere on the stage
    stage.GetRootLayer().TouchedSignal().Connect( this, &HelloWorldController::OnTouch );
  }

  bool OnTouch( Actor actor, const TouchEvent& touch )
  {
    if (TouchPoint::Down == touch.GetPoint(0).state)
    {
      printf("Touched down \n");

      Animation anim = Animation::New(1.f);
      anim.SetEndAction(Animation::Discard);
      anim.AnimateTo(Property(mBaseActor, mBlurStrengthPropertyIndex), Property::Value(1.f), AlphaFunction::LINEAR);

      for (int i = 0; i < BLUR_LEVELS; i++)
      {
        anim.AnimateTo(Property(mBlurActors[i], Actor::Property::COLOR_ALPHA), Property::Value(0.f), AlphaFunction::LINEAR);
        anim.AnimateTo(Property(mBlurActors[i], Actor::Property::SIZE), Property::Value(Vector3(300.f, 300.f, 1.f)), AlphaFunction::LINEAR);
      }

      anim.Play();

      //Animation
    }

    return true;
  }

private:
  Application&    mApplication;

  Actor           mBaseActor;
  ImageActor      mBlurActors[BLUR_LEVELS];
  Property::Index mBlurStrengthPropertyIndex;
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
