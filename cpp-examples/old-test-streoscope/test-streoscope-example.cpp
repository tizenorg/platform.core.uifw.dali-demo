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
 */

#include <string>
#include <cstdarg>
#include <dali-toolkit/dali-toolkit.h>
#include "test-streoscope-example.h"
#include "../test-shared/common.h"

#define ENABLE_STREOSCOPE

const char* IMAGE_PATHS[] = {
    DALI_IMAGE_DIR "gallery-medium-1.jpg",
};

TestStreoscopeController::TestStreoscopeController( Application& application )
: mApplication( application ), mInit(false)
{
  std::cout << "TestStreoscopeController::TestStreoscopeController" << std::endl;

  // Connect to the Application's Init signal
  mApplication.InitSignal().Connect( this, &TestStreoscopeController::Create );
}

// The Init signal is received once (only) during the Application lifetime
void TestStreoscopeController::Create( Application& application )
{
  std::cout << "TestStreoscopeController::Create" << std::endl;

  // key handler
  Stage::GetCurrent().KeyEventSignal().Connect( this, &TestStreoscopeController::OnKeyEvent );

  // Respond to a click anywhere on the stage
  Stage stage = Stage::GetCurrent();
  stage.GetRootLayer().TouchedSignal().Connect( this, &TestStreoscopeController::OnTouch );
  stage.GetRootLayer().HoveredSignal().Connect( this, &TestStreoscopeController::OnHover );
  Vector2 stageSize= stage.GetSize();

#ifdef SENSOR_ENABLED
  CreateSensors();
#endif

  Actor ic = CreateImageCube(3,4,1, Vector3(70,70,70), 
                            Vector3(0,0,300),
                            Vector3(stageSize.x, stageSize.y, 300),
                            IMAGE_PATHS[0]);
  for(uint i=0; i<ic.GetChildCount(); ++i)
    ic.GetChildAt(i).HoveredSignal().Connect( this, &TestStreoscopeController::OnHover );
  stage.Add(ic);

  Actor cc = CreateCrossCube(3,4,1, 500, 
                            Vector3(0,0,0),
                            Vector3(stageSize.x, stageSize.y, 0));
  stage.Add(cc);

#ifdef ENABLE_STREOSCOPE
  application.SetViewMode(STEREO_HORIZONTAL);
#endif
}

bool IsActorHittableFunction(Actor actor, Dali::HitTestAlgorithm::TraverseType type)
{
  bool hittable = false;

  switch (type)
  {
    case Dali::HitTestAlgorithm::CHECK_ACTOR:
    {
      // Check whether the actor is visible and not fully transparent.
      if( actor.IsVisible()
       && actor.GetCurrentWorldColor().a > 0.01f) // not FULLY_TRANSPARENT
      {
        hittable = true;
      }
      break;
    }
    case Dali::HitTestAlgorithm::DESCEND_ACTOR_TREE:
    {
      if( actor.IsVisible() ) // Actor is visible, if not visible then none of its children are visible.
      {
        hittable = true;
      }
      break;
    }
    default:
    {
      break;
    }
  }

  return hittable;
}

#ifdef SENSOR_ENABLED
void sensor_cb2(sensor_h sensor, sensor_event_s* event, void *data)
{
  TestStreoscopeController* _this = static_cast<TestStreoscopeController*>(data);

  if(_this->mTypes[_this->mSensor2Index[sensor]] == SENSOR_GRAVITY)
    _this->mGravity = Vector4(event->values[0], event->values[1], event->values[2], 1);
  else if(_this->mTypes[_this->mSensor2Index[sensor]] == SENSOR_MAGNETIC)
    _this->mMagnet = Vector4(event->values[0], event->values[1], event->values[2], 1);
  else if(_this->mTypes[_this->mSensor2Index[sensor]] == SENSOR_GYROSCOPE)
    _this->mGyro = Vector4(event->values[0], event->values[1], event->values[2], 1);
  //else if(_this->mTypes[_this->mSensor2Index[sensor]] == SENSOR_ACCELEROMETER)
    //_this->mAccel = Vector4(event->values[0], event->values[1], event->values[2], 1);

  //if(_this->mGravity.w>0 && _this->mMagnet.w>0 && _this->mGyro.w>0 && _this->mAccel.w>0)
  if(_this->mGravity.w>0 && _this->mMagnet.w>0 && _this->mGyro.w>0)
  {
    ////////////////////////////////////////////
    // device frame

    // ang vel w.r.t. device frame (from gyro)
    Vector3 w_at_device(Radian(Degree(_this->mGyro[0])), Radian(Degree(_this->mGyro[1])), Radian(Degree(_this->mGyro[2])));

    // TODO : gyroscope calibration
    w_at_device += Vector3(0.014, -0.005, 0.006);

    //cout << "w_at_device " << w_at_device << endl;

    // device coord. (right hand coord.)
    Vector3 y_world_at_device(_this->mGravity.x,_this->mGravity.y,_this->mGravity.z);
    y_world_at_device.Normalize();

    Vector3 x_world_at_device;
    if(_this->mInit==false)
    {
      // initial x axis comes from magnetic sensor - too noisy!
      //Vector3 Vmag(_this->mMagnet.x,_this->mMagnet.y,_this->mMagnet.z);

      // initial x axis comes from axis to device upper part
      Vector3 Vmag(0,1,0);

      //// initial x axis comes from axis to device lower part
      //Vector3 Vmag(0,-1,0);

      Matrix3 Pxz_world_at_device = GetProjMatOnPlane(y_world_at_device);
      x_world_at_device = Mat3DotVec3(Pxz_world_at_device, Vmag);
      x_world_at_device.Normalize();

      _this->mInit = true;
    }
    else
    {
      // rot x_world_at_device using w_at_device
      Matrix3 P_yaxis = GetProjMatOnVector(y_world_at_device);
      Matrix3 Pxz_world_at_device = GetProjMatOnPlane(y_world_at_device);

      // TODO : not w_at_device projection to yaxis, but using geodesic minimum distance
      // TODO : not euler integration, but rk4 integration
      Vector3 w_yaxis_at_device = Mat3DotVec3(P_yaxis, w_at_device) * _this->mSensorInterval;

      w_yaxis_at_device *= .5;  // quaternion angular velocity
      Quaternion Q_yaxis_at_device = Quaternion(Vector4(w_yaxis_at_device.x, w_yaxis_at_device.y, w_yaxis_at_device.z, 0)).Exp();
      Q_yaxis_at_device.Invert();
      x_world_at_device = Q_yaxis_at_device * Mat3DotVec3(Pxz_world_at_device, _this->mPrev_x_world_at_device);
    }
    _this->mPrev_x_world_at_device = x_world_at_device;

    Vector3 z_world_at_device = x_world_at_device.Cross(y_world_at_device);

    //cout << "x_world_at_device " << x_world_at_device << endl;
    //cout << "y_world_at_device " << y_world_at_device << endl;
    //cout << "z_world_at_device " << stringf("%-10.5f", z_world_at_device.x) << " " << z_world_at_device.y << " " << z_world_at_device.z << endl;
    //cout << endl;

    //Matrix3 R_device_to_world(x_world_at_device.x,y_world_at_device.x,z_world_at_device.x,
                              //x_world_at_device.y,y_world_at_device.y,z_world_at_device.y,
                              //x_world_at_device.z,y_world_at_device.z,z_world_at_device.z);

    ////////////////////////////////////////////
    // dali frame

    Vector3 x_world_at_dali = DeviceCoord2DaliCoord(x_world_at_device);
    Vector3 y_world_at_dali = -DeviceCoord2DaliCoord(y_world_at_device);
    Vector3 z_world_at_dali = DeviceCoord2DaliCoord(z_world_at_device);
    //Vector3 w_at_dali = -DeviceCoord2DaliCoord(w_at_device);  // for ang vel

    //cout << "x_world_at_dali " << x_world_at_dali << endl;
    //cout << "y_world_at_dali " << y_world_at_dali << endl;
    //cout << "z_world_at_dali " << stringf("%-10.5f", z_world_at_dali.x) << " " << z_world_at_dali.y << " " << z_world_at_dali.z << endl;
    //cout << "w_at_dali " << w_at_dali << endl;
    //cout << endl;

    ////////////////////////////////////////////
    // transformation

    Matrix3 R_device_to_world(x_world_at_dali.x,y_world_at_dali.x,z_world_at_dali.x,
                              x_world_at_dali.y,y_world_at_dali.y,z_world_at_dali.y,
                              x_world_at_dali.z,y_world_at_dali.z,z_world_at_dali.z);

    Quaternion basicCamRot(Radian(Degree(-180)), Vector3(0,1,0));
    Quaternion streoRot(Radian(Degree(90)), Vector3(0,0,1));

    Quaternion Q_world_to_device(Mat3ToMat4(R_device_to_world));
    Q_world_to_device.Invert();

    CameraActor cameraActor = Stage::GetCurrent().GetRenderTaskList().GetTask(0).GetCameraActor();
    cameraActor.SetRotation(Quaternion());
    cameraActor.RotateBy(Q_world_to_device);
    cameraActor.RotateBy(basicCamRot);
    cameraActor.RotateBy(streoRot);


    //////////////////////////////////////////////
    //// scroll item view
    //float originOffsetX, originOffsetY;
    //_this->mItemView.ScreenToLocal(originOffsetX, originOffsetY, stageSize.width * 0.5f, stageSize.height * 0.5f);
    ////cout << originOffsetX-VR_SCREEN_WIDTH/2-600 << " " << originOffsetY-VR_SCREEN_HEIGHT/2 << endl;
    //float xOnView = originOffsetX-VR_SCREEN_WIDTH/2-700;
    ////float yOnView = originOffsetY-VR_SCREEN_HEIGHT/2;

    //if(xOnView < -VR_SCREEN_WIDTH/2 + SCROLL_MARGIN) // left scroll 
    //{
      //float diff = -VR_SCREEN_WIDTH/2 + SCROLL_MARGIN - xOnView;
      //float speed = diff/SCROLL_MARGIN; 

      //_this->mItemId -= speed;
      //if(_this->mItemId<0) _this->mItemId=0;
      ////cout << _this->mItemId << " " << speed << endl;

      //_this->mItemView.ScrollToItem((int)_this->mItemId, 1.);
    //}

    //else if(xOnView > VR_SCREEN_WIDTH/2 - SCROLL_MARGIN) // right scroll
    //{
      //float diff = xOnView - (VR_SCREEN_WIDTH/2 - SCROLL_MARGIN);
      //float speed = diff/SCROLL_MARGIN; 

      //_this->mItemId += speed;
      //if(_this->mItemId>(int)_this->GetNumberOfItems()-1) _this->mItemId=(int)_this->GetNumberOfItems()-1;
      ////cout << _this->mItemId << " " << speed <<  endl;

      //_this->mItemView.ScrollToItem((int)_this->mItemId, 1.);
    //}

    ////////////////////////////////////////////
    // item hit-test
    Vector2 stageSize = Stage::GetCurrent().GetSize();
    HitTestAlgorithm::Results hitTestResults;
    //HitTest( Stage::GetCurrent(), Vector2(0,0), hitTestResults, IsActorHittableFunction );
    HitTest( Stage::GetCurrent(), Vector2(stageSize.y/2,-stageSize.x/2), hitTestResults, IsActorHittableFunction );
    if(hitTestResults.actor)
    {
      //Dali::Actor handle( hitTestResults.actor );
      HoverEvent hevt;
      hevt.points.push_back(TouchPoint(0, TouchPoint::Motion, 0, 0));
      //hitTestResults.actor.HoveredSignal().Emit(handle, hevt);
      hitTestResults.actor.HoveredSignal().Emit(hitTestResults.actor, hevt);
    }

    ////////////////////////////////////////////
    _this->mGravity.w = 0;
    _this->mMagnet.w = 0;
    _this->mGyro.w = 0;

    //_this->mAccel.w = 0;

    //cout << "camera " << cameraActor.GetCurrentPosition() << endl;
    //cout << _this->cross.GetCurrentPosition() << endl;
    //cout << _this->cross.GetCurrentWorldPosition() << endl;
    //cout << endl;
  }

  //Vector2 stageSize = Stage::GetCurrent().GetSize();
  //float originOffsetX, originOffsetY;
  //_this->mItemView.ScreenToLocal(originOffsetX, originOffsetY, stageSize.width * 0.5f, stageSize.height * 0.5f);
  //cout << originOffsetX-VR_SCREEN_WIDTH/2-600 << " " << originOffsetY-VR_SCREEN_HEIGHT/2 << endl;

  //_this->PrintSensorValues(SENSOR_ACCELEROMETER);
  //PrintSensorValues(SENSOR_LINEAR_ACCELERATION);
  //_this->PrintSensorValues(SENSOR_GRAVITY);
  //PrintSensorValues(SENSOR_MAGNETIC);
  //PrintSensorValues(SENSOR_ROTATION_VECTOR);
  //PrintSensorValues(SENSOR_ORIENTATION);
  //_this->PrintSensorValues(SENSOR_GYROSCOPE);
  //PrintSensorValues(SENSOR_LIGHT);
  //PrintSensorValues(SENSOR_PROXIMITY);
  //PrintSensorValues(SENSOR_PRESSURE);
  //cout << endl;

}
#endif

#ifdef SENSOR_ENABLED
void TestStreoscopeController::CreateSensors()
{
  ////////////////////
  // redwood prints true for
  // SENSOR_ACCELEROMETER
  // SENSOR_GRAVITY 
  // SENSOR_LINEAR_ACCELERATION 
  // SENSOR_MAGNETIC 
  // SENSOR_ROTATION_VECTOR 
  // SENSOR_ORIENTATION 
  // SENSOR_GYROSCOPE 
  // SENSOR_LIGHT 
  // SENSOR_PROXIMITY 
  // SENSOR_PRESSURE 

  bool supported;
  sensor_is_supported(SENSOR_ALL, &supported);
  cout << "SENSOR_ALL " << supported << endl;
  sensor_is_supported(SENSOR_ACCELEROMETER , &supported);
  cout << "SENSOR_ACCELEROMETER " << supported << endl;
  sensor_is_supported(SENSOR_GRAVITY , &supported);
  cout << "SENSOR_GRAVITY " << supported << endl;
  sensor_is_supported(SENSOR_LINEAR_ACCELERATION , &supported);
  cout << "SENSOR_LINEAR_ACCELERATION " << supported << endl;
  sensor_is_supported(SENSOR_MAGNETIC , &supported);
  cout << "SENSOR_MAGNETIC " << supported << endl;
  sensor_is_supported(SENSOR_ROTATION_VECTOR , &supported);
  cout << "SENSOR_ROTATION_VECTOR " << supported << endl;
  sensor_is_supported(SENSOR_ORIENTATION , &supported);
  cout << "SENSOR_ORIENTATION " << supported << endl;
  sensor_is_supported(SENSOR_GYROSCOPE , &supported);
  cout << "SENSOR_GYROSCOPE " << supported << endl;
  sensor_is_supported(SENSOR_LIGHT , &supported);
  cout << "SENSOR_LIGHT " << supported << endl;
  sensor_is_supported(SENSOR_PROXIMITY , &supported);
  cout << "SENSOR_PROXIMITY " << supported << endl;
  sensor_is_supported(SENSOR_PRESSURE , &supported);
  cout << "SENSOR_PRESSURE " << supported << endl;
  sensor_is_supported(SENSOR_ULTRAVIOLET , &supported);
  cout << "SENSOR_ULTRAVIOLET " << supported << endl;
  sensor_is_supported(SENSOR_TEMPERATURE , &supported);
  cout << "SENSOR_TEMPERATURE " << supported << endl;
  sensor_is_supported(SENSOR_HUMIDITY , &supported);
  cout << "SENSOR_HUMIDITY " << supported << endl;
  cout << endl;

  mTypes.push_back(SENSOR_ACCELEROMETER);
  //mTypes.push_back(SENSOR_LINEAR_ACCELERATION);
  mTypes.push_back(SENSOR_GRAVITY);
  mTypes.push_back(SENSOR_MAGNETIC);

  //mTypes.push_back(SENSOR_ROTATION_VECTOR);
  //mTypes.push_back(SENSOR_ORIENTATION);
  mTypes.push_back(SENSOR_GYROSCOPE);

  //mTypes.push_back(SENSOR_LIGHT);
  //mTypes.push_back(SENSOR_PROXIMITY);
  //mTypes.push_back(SENSOR_PRESSURE);

  mType2String[SENSOR_ACCELEROMETER]="SENSOR_ACCELEROMETER";
  mType2String[SENSOR_GRAVITY]="SENSOR_GRAVITY";
  mType2String[SENSOR_LINEAR_ACCELERATION]="SENSOR_LINEAR_ACCELERATION";
  mType2String[SENSOR_MAGNETIC]="SENSOR_MAGNETIC";
  mType2String[SENSOR_ROTATION_VECTOR]="SENSOR_ROTATION_VECTOR";
  mType2String[SENSOR_ORIENTATION]="SENSOR_ORIENTATION";
  mType2String[SENSOR_GYROSCOPE]="SENSOR_GYROSCOPE";
  mType2String[SENSOR_LIGHT]="SENSOR_LIGHT";
  mType2String[SENSOR_PROXIMITY]="SENSOR_PROXIMITY";
  mType2String[SENSOR_PRESSURE]="SENSOR_PRESSURE";

  mSensors.resize(mTypes.size());
  mListeners.resize(mTypes.size());

  mSensorInterval = .01;

  for(uint i=0; i<mTypes.size(); ++i)
  {
    mType2Index[mTypes[i]] = i;
    sensor_get_default_sensor(mTypes[i], &mSensors[i]);
    sensor_create_listener(mSensors[i], &mListeners[i]);
    sensor_listener_set_event_cb(mListeners[i], mSensorInterval*1000, sensor_cb2, this);
    sensor_listener_start(mListeners[i]);
    mSensor2Index[mSensors[i]] = i;
  }

  mGravity.w = 0;
  mMagnet.w = 0;
}
#endif

bool TestStreoscopeController::OnTouch( Actor actor, const TouchEvent& event )
{
  return true;
}

bool TestStreoscopeController::OnHover( Actor actor, const HoverEvent& event )
{
  CameraActor cameraActor = Stage::GetCurrent().GetRenderTaskList().GetTask(0).GetCameraActor();
  
  //cout << Stage::GetCurrent().GetRenderTaskList().GetTask(0).GetCurrentViewportSize() << endl;

  //cameraActor.SetPosition(Vector3(0,0,1000));

  //Stage::GetCurrent().GetRootLayer().SetSize(200,200);
  Stage::GetCurrent().GetRootLayer().SetScale(.5);
  //Stage::GetCurrent().SetScale(20);

  //cout << cameraActor.GetCurrentPosition() << endl;
  //cout << cameraActor.GetCurrentWorldPosition() << endl;
  //cout << mApplication.GetStereoBase() << endl;

  cout << Stage::GetCurrent().GetSize() << endl;
  cout << "---------" << endl;

  //const TouchPoint& p = event.GetPoint(0);
  //cout << "state " << p.state << endl;
  //cout << "hitActor " << p.hitActor.GetName() << endl;
  //cout << "local " << p.local << endl;
  //cout << "screen " << p.screen << endl;

  cout << "RenderTask" << endl;
  RenderTaskList renderTasks = Stage::GetCurrent().GetRenderTaskList();
  for(uint i=0; i<renderTasks.GetTaskCount(); ++i)
  {
    CameraActor cameraActor = renderTasks.GetTask(i).GetCameraActor();
    cout << i << " " << cameraActor.GetCurrentWorldPosition() << endl;
  }
  cout << endl;

  return true;
}

void TestStreoscopeController::OnKeyEvent( const KeyEvent& event )
{
  if( event.state == KeyEvent::Down )
  {
    if ( IsKey( event, Dali::DALI_KEY_ESCAPE) || IsKey( event, Dali::DALI_KEY_BACK) )
    {
#ifdef SENSOR_ENABLED
      for(uint i=0; i<mTypes.size(); ++i)
      {
        sensor_listener_stop(mListeners[i]);
        sensor_destroy_listener(mListeners[i]);
      }
#endif
      mApplication.Quit();
    }
  }
}


void RunTest( Application& application )
{
  TestStreoscopeController test( application );

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
