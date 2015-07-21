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

using namespace Dali;
using Dali::Toolkit::ModelView;

namespace
{
const int MODEL_NUMBER(3);

const char * const MODEL_FILE[] = {
    DALI_MODEL_DIR "Dino.obj",
    DALI_MODEL_DIR "ToyRobot-Metal.obj",
    DALI_MODEL_DIR "Toyrobot-Plastic.obj"
};

const char * const MATERIAL_FILE[] = {
    DALI_MODEL_DIR "Dino.mtl",
    DALI_MODEL_DIR "ToyRobot-Metal.mtl",
    DALI_MODEL_DIR "Toyrobot-Plastic.mtl"
};

const char * const IMAGE_PATH( DALI_IMAGE_DIR "" );
}

// This example shows how to create and display Hello World! using a simple TextActor
//
class ModelViewController : public ConnectionTracker
{
public:

  ModelViewController( Application& application )
  : mApplication( application )
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &ModelViewController::Create );
  }

  ~ModelViewController()
  {
    // Nothing to do here;
  }

  // The Init signal is received once (only) during the Application lifetime
  void Create( Application& application )
  {
    // Get a handle to the stage
    Stage stage = Stage::GetCurrent();
    Vector2 screenSize = stage.GetSize();

    stage.SetBackgroundColor( Color::RED );

    mModelCounter = 0;

    RenderTaskList taskList = stage.GetRenderTaskList();
    Dali::RenderTask task = taskList.GetTask(0);
    CameraActor camera = task.GetCameraActor();
    float cameraFOV = camera.GetFieldOfView();

    mModelView = ModelView::New( MODEL_FILE[0], MATERIAL_FILE[0], IMAGE_PATH );
    mModelView.SetParentOrigin( ParentOrigin::CENTER );
    mModelView.SetAnchorPoint( AnchorPoint::CENTER );
    mModelView.SetName( "model-view-control" );
    mModelView.SetResizePolicy(ResizePolicy::FIXED, Dimension::ALL_DIMENSIONS);
    mModelView.SetSize(screenSize);

    mModelView.SetProperty(ModelView::Property::CAMERAFOV, cameraFOV);
    mModelView.SetProperty(ModelView::Property::LIGHTPOSITION, Vector3(0,10.,0));

    stage.GetRootLayer().SetBehavior(Layer::LAYER_3D);
    stage.Add( mModelView );

    Animation lightAnimation = Animation::New(3.f);
    lightAnimation.AnimateTo(Property(mModelView, ModelView::Property::LIGHTPOSITION), Vector3(0,-10.0,0) );
    lightAnimation.SetLooping(true);
    lightAnimation.Play();

    Animation rotationAnimation = Animation::New(15.f);
    rotationAnimation.AnimateBy(Property(mModelView, Actor::Property::ORIENTATION), Quaternion(Degree(0.f), Degree(360.f), Degree(0.f)) );
    rotationAnimation.SetLooping(true);
    rotationAnimation.Play();


    // Respond to a click anywhere on the stage
    stage.GetRootLayer().TouchedSignal().Connect( this, &ModelViewController::OnTouch );
  }

  bool OnTouch( Actor actor, const TouchEvent& touch )
  {
    const TouchPoint &point = touch.GetPoint(0);

    switch( point.state )
    {
      case TouchPoint::Up:
      case TouchPoint::Leave:
      case TouchPoint::Interrupted:
      {
        mModelCounter = (mModelCounter + 1) % MODEL_NUMBER;
        mModelView.SetProperty(ModelView::Property::OBJ_URL, MODEL_FILE[mModelCounter]);
        mModelView.SetProperty(ModelView::Property::MTL_URL, MATERIAL_FILE[mModelCounter]);
        mModelView.SetProperty(ModelView::Property::IMAGES_URL, IMAGE_PATH);
        break;
      }
      default:
        break;
    }
    return true;
  }

private:
  Application&  mApplication;

  int mModelCounter;
  ModelView mModelView;
};

void RunTest( Application& application )
{
  ModelViewController test( application );

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
