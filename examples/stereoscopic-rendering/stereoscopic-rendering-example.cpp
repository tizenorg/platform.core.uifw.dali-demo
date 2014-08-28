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
    Stage stage( Stage::GetCurrent() );
    stage.SetBackgroundColor( Vector4(0.4f,0.4f,0.4f,1.0f));
    stage.KeyEventSignal().Connect(this, &StereoscopicRenderingController::OnKeyEvent);

    //Set stereoscopic mode to horizontal.
    application.SetViewMode( Dali::STEREO_HORIZONTAL );

    //Try to load binary model file.
    mModel = Model::New( gBinarySolidFile );
    mModel.LoadingFinishedSignal().Connect( this, &StereoscopicRenderingController::BinaryModelLoaded );

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
      //Binary model file load successfully.
      CreateActors();
    }
    else
    {
      //Binary model file load failed, try to load dae file
      mModel = Model::New(gSolidFile);
      mModel.LoadingFinishedSignal().Connect(this, &StereoscopicRenderingController::ModelLoaded);
    }
  }

  void ModelLoaded(Model model)
  {
    if( model.GetLoadingState() == ResourceLoadingSucceeded )
    {
      //dae file load successfully.

      //Save the model to a binary file for faster loading next time.
      model.Save(gBinarySolidFile);

      //Create actors
      CreateActors();
    }
    else
    {
      //Model file not founded or corrupted, quit.
      mApplication.Quit();
    }
  }

  void CreateActors()
  {
    Actor rootActor = ModelActorFactory::BuildActorTree(mModel, "");  // Gets root actor
    if (rootActor)
    {
      Stage stage( Stage::GetCurrent() );

      Actor actor = Dali::Actor::DownCast( rootActor.FindChildByName( "pCube1") );
      if( actor )
      {
        //Create actor
        actor.SetParentOrigin( ParentOrigin::CENTER );
        actor.SetScale( Vector3(10.0f, 10.0f, 10.0f ) );
        actor.SetPosition( -100.0f, 0.0f, -50.0f );
        actor.SetColor( Vector4(0.0f,0.0f,1.0f,1.0f));
        stage.Add( actor );

        //Create the animation for the actor
        const float duration = 5.0f;
        Animation anim = Animation::New( duration );
        anim.AnimateTo( Property( actor, Actor::ROTATION), AngleAxis( Degree(-180.0f), Vector3(1.0f,1.0f,0.0f)) );
        anim.AnimateTo( Property( actor, Actor::POSITION_Y), 150.0f, AlphaFunctions::EaseInOut,TimePeriod( 0.0f,0.5f*duration));
        anim.AnimateTo( Property( actor, Actor::POSITION_Y), 0.0f, AlphaFunctions::EaseInOut, TimePeriod( 0.5f*duration,0.5f*duration) );
        anim.AnimateTo( Property( actor, Actor::POSITION_Z), -250.0f, AlphaFunctions::EaseInOut,TimePeriod( 0.0f,0.5f*duration));
        anim.AnimateTo( Property( actor, Actor::POSITION_Z), -50.0f, AlphaFunctions::EaseInOut, TimePeriod( 0.5f*duration,0.5f*duration) );
        anim.SetLooping( true );
        anim.Play();
      }

      actor = Dali::Actor::DownCast( rootActor.FindChildByName( "pTorus1") );
      if( actor )
      {
        //Create actor
        actor.SetParentOrigin( ParentOrigin::CENTER );
        actor.SetScale( Vector3(50.0f, 50.0f, 50.0f ) );
        actor.SetPosition( 100.0f, 200.0f, -50.0f );
        actor.SetColor( Vector4(0.0f,1.0f,0.0f,1.0f));
        stage.Add( actor );

        //Create the animation for the actor
        const float duration = 3.0f;
        Animation anim = Animation::New( duration );
        anim.AnimateTo( Property( actor, Actor::ROTATION), AngleAxis( Degree(180.0f), Vector3(1.0f,1.0f,0.0f)) );
        anim.AnimateTo( Property( actor, Actor::POSITION_Z), -250.0f, AlphaFunctions::EaseInOut,TimePeriod( 0.0f,0.5f*duration));
        anim.AnimateTo( Property( actor, Actor::POSITION_Z), -50.0f, AlphaFunctions::EaseInOut, TimePeriod( 0.5f*duration,0.5f*duration) );
        anim.SetLooping( true );
        anim.Play();
      }

      actor = Dali::Actor::DownCast( rootActor.FindChildByName( "pSolid1") );
      if( actor )
      {
        //Create actor
        actor.SetParentOrigin( ParentOrigin::CENTER );
        actor.SetScale( Vector3(50.0f, 50.0f, 50.0f ) );
        actor.SetPosition( 150.0f, -200.0f, -10.0f );
        actor.SetColor( Vector4(1.0f,0.0f,0.0f,1.0f));
        actor.SetRotation( Degree(0.0f),Vector3(1.0f,0.0f,0.0f ) );
        stage.Add( actor );

        //Create the animation for the actor
        const float duration = 6.0f;
        Animation anim = Animation::New( duration );
        anim.AnimateTo( Property( actor, Actor::ROTATION), AngleAxis( Degree(120.0f), Vector3::YAXIS), TimePeriod( 0.0f,0.33f*duration) );
        anim.AnimateTo( Property( actor, Actor::ROTATION), AngleAxis( Degree(240.0f), Vector3::YAXIS), TimePeriod( 0.33f*duration,0.33f*duration) );
        anim.AnimateTo( Property( actor, Actor::ROTATION), AngleAxis( Degree(360.0f), Vector3::YAXIS), TimePeriod( 0.66f*duration,0.33f*duration) );
        anim.AnimateTo( Property( actor, Actor::POSITION_Y), -300.0f, AlphaFunctions::EaseInOut,TimePeriod( 0.0f,0.5f*duration));
        anim.AnimateTo( Property( actor, Actor::POSITION_Y), -200.0f, AlphaFunctions::EaseInOut, TimePeriod( 0.5f*duration,0.5f*duration) );
        anim.SetLooping( true );
        anim.Play();
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
    }
  }

private:
  Application&              mApplication;
  LightActor                mKeyLightActor;

  Model                     mModel;
};

void RunTest( Application& application )
{
  StereoscopicRenderingController test( application );

  application.MainLoop();
}

// Entry point for application
//
int main( int argc, char **argv )
{
  Application application = Application::New( &argc, &argv );

  RunTest( application );

  return 0;
}
