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

using namespace Dali;

namespace
{

  const char* gSolidFile = DALI_MODEL_DIR "solids.dae";
  const char* gBinarySolidFile = DALI_MODEL_DIR "solids.dali-bin";
}


class StereoscopicRenderingController : public ConnectionTracker
{
public:

  StereoscopicRenderingController( Application& application )
  : mApplication( application )
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &StereoscopicRenderingController::Create );
  }

  ~StereoscopicRenderingController()
  {
    // Nothing to do here;
  }

  // The Init signal is received once (only) during the Application lifetime
  void Create( Application& application )
  {
    //Set stereoscopic mode to horizontal
    application.SetViewMode( Dali::STEREO_HORIZONTAL );
    application.SetStereoBase( 65.0f );

    mModel = Model::New( gBinarySolidFile );
    mModel.LoadingFinishedSignal().Connect( this, &StereoscopicRenderingController::BinaryModelLoaded );

    Stage stage( Stage::GetCurrent() );
    stage.SetBackgroundColor( Vector4(0.4f,0.4f,0.4f,1.0f));

    //Create a Key light
    Light keylight = Light::New("KeyLight");
    keylight.SetFallOff(Vector2(10000.0f, 10000.0f));
    mKeyLightActor = LightActor::New();
    mKeyLightActor.SetParentOrigin(ParentOrigin::CENTER);
    mKeyLightActor.SetName(keylight.GetName());
    mKeyLightActor.SetLight(keylight);

    stage.Add( mKeyLightActor );
  }

  void BinaryModelLoaded(Model model)
  {
    if( model.GetLoadingState() == ResourceLoadingSucceeded )
    {
      ModelReady();
    }
    else
    {
      mModel = Model::New(gSolidFile);
      mModel.LoadingFinishedSignal().Connect(this, &StereoscopicRenderingController::ModelLoaded);
    }
  }

  void ModelLoaded(Model model)
  {
    if( model.GetLoadingState() == ResourceLoadingSucceeded )
    {
      model.Save(gBinarySolidFile);
      ModelReady();
    }
    else
    {
      mApplication.Quit();
    }
  }

  void ModelReady()
  {
    Actor rootActor = ModelActorFactory::BuildActorTree(mModel, "");  // Gets root actor
    if (rootActor)
    {
      Stage stage( Stage::GetCurrent() );

      Actor cube = Dali::Actor::DownCast( rootActor.FindChildByName( "pCube1") );
      if( cube )
      {
        cube.SetParentOrigin( ParentOrigin::CENTER );
        cube.SetScale( Vector3(10.0f, 10.0f, 10.0f ) );
        cube.SetPosition( -100.0f, 0.0f, -50.0f );

        stage.Add( cube );
        mSolid.push_back(cube);

        const float duration = 5.0f;
        mAnimation.push_back( Animation::New( duration ) );
        mAnimation[0].AnimateTo( Property( cube, Actor::ROTATION), AngleAxis( Degree(-180.0f), Vector3(1.0f,1.0f,0.0f)) );

        mAnimation[0].AnimateTo( Property( cube, Actor::POSITION_Y), 150.0f, AlphaFunctions::EaseInOut,TimePeriod( 0.0f,0.5f*duration));
        mAnimation[0].AnimateTo( Property( cube, Actor::POSITION_Y), 0.0f, AlphaFunctions::EaseInOut, TimePeriod( 0.5f*duration,0.5f*duration) );

        mAnimation[0].AnimateTo( Property( cube, Actor::POSITION_Z), -250.0f, AlphaFunctions::EaseInOut,TimePeriod( 0.0f,0.5f*duration));
        mAnimation[0].AnimateTo( Property( cube, Actor::POSITION_Z), -50.0f, AlphaFunctions::EaseInOut, TimePeriod( 0.5f*duration,0.5f*duration) );

        mAnimation[0].SetLooping( true );
        mAnimation[0].Play();
      }

      Actor torus = Dali::Actor::DownCast( rootActor.FindChildByName( "pTorus1") );
      if( torus )
      {
        torus.SetParentOrigin( ParentOrigin::CENTER );
        torus.SetScale( Vector3(50.0f, 50.0f, 50.0f ) );
        torus.SetPosition( 100.0f, 200.0f, -50.0f );
        stage.Add( torus );
        mSolid.push_back(torus);

        const float duration = 3.0f;
        mAnimation.push_back( Animation::New( duration ) );
        mAnimation[1].AnimateTo( Property( torus, Actor::ROTATION), AngleAxis( Degree(180.0f), Vector3(1.0f,1.0f,0.0f)) );

        mAnimation[1].AnimateTo( Property( torus, Actor::POSITION_Z), -250.0f, AlphaFunctions::EaseInOut,TimePeriod( 0.0f,0.5f*duration));
        mAnimation[1].AnimateTo( Property( torus, Actor::POSITION_Z), -50.0f, AlphaFunctions::EaseInOut, TimePeriod( 0.5f*duration,0.5f*duration) );

        mAnimation[1].SetLooping( true );
        mAnimation[1].Play();
      }

      Actor mine = Dali::Actor::DownCast( rootActor.FindChildByName( "pSolid1") );
      if( mine )
      {
        mine.SetParentOrigin( ParentOrigin::CENTER );
        mine.SetScale( Vector3(50.0f, 50.0f, 50.0f ) );
        mine.SetPosition( 150.0f, -200.0f, -10.0f );
        mine.SetRotation( Degree(0.0f),Vector3(1.0f,0.0f,0.0f ) );
        stage.Add( mine );
        mSolid.push_back(mine);

        const float duration = 6.0f;
        mAnimation.push_back( Animation::New( duration ) );
        mAnimation[2].AnimateTo( Property( mine, Actor::ROTATION), AngleAxis( Degree(120.0f), Vector3::YAXIS), TimePeriod( 0.0f,0.33f*duration) );
        mAnimation[2].AnimateTo( Property( mine, Actor::ROTATION), AngleAxis( Degree(240.0f), Vector3::YAXIS), TimePeriod( 0.33f*duration,0.33f*duration) );
        mAnimation[2].AnimateTo( Property( mine, Actor::ROTATION), AngleAxis( Degree(360.0f), Vector3::YAXIS), TimePeriod( 0.66f*duration,0.33f*duration) );

        mAnimation[2].AnimateTo( Property( mine, Actor::POSITION_Y), -300.0f, AlphaFunctions::EaseInOut,TimePeriod( 0.0f,0.5f*duration));
        mAnimation[2].AnimateTo( Property( mine, Actor::POSITION_Y), -200.0f, AlphaFunctions::EaseInOut, TimePeriod( 0.5f*duration,0.5f*duration) );

        mAnimation[2].SetLooping( true );
        mAnimation[2].Play();
      }
    }
  }
private:
  Application&              mApplication;
  LightActor                mKeyLightActor;

  Model                     mModel;

  std::vector<Actor>       mSolid;
  std::vector<Animation>   mAnimation;


};

void RunTest( Application& application )
{
  StereoscopicRenderingController test( application );

  application.MainLoop();
}

// Entry point for Linux & SLP applications
//
int main( int argc, char **argv )
{
  Application application = Application::New( &argc, &argv );

  RunTest( application );

  return 0;
}
