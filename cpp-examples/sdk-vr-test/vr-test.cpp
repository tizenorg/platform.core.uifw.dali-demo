#ifndef __VR_GALLERY_EXAMPLE_H__
#define __VR_GALLERY_EXAMPLE_H__

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

#include <dali-toolkit/dali-toolkit.h>
#include <map>
#include <iostream>
#include <dlog.h>

using namespace std;
using namespace Dali;
using namespace Dali::Toolkit;

//#ifdef __arm__
#define SENSOR_ENABLED
//#endif

#ifdef SENSOR_ENABLED
#include <sensor/sensor.h>
#endif

class TestStreoscopeController : public ConnectionTracker
{
public:

  TestStreoscopeController( Application& application );
  ~TestStreoscopeController() {}

  void Create( Application& application );

  bool OnTouch( Actor actor, const TouchEvent& event );
  bool OnHover( Actor actor, const HoverEvent& event );
  void OnKeyEvent(const KeyEvent& event);

private:
  Application&  mApplication;

public:
  bool mInit;

#ifdef SENSOR_ENABLED
public:
  void CreateSensors();

  vector<sensor_type_e> mTypes;
  map<sensor_type_e,string> mType2String;
  map<sensor_type_e,int> mType2Index;

  vector<sensor_h> mSensors;
  map<sensor_h,int> mSensor2Index;

  vector<sensor_listener_h> mListeners;
  //Vector4 mGravity, mMagnet, mGyro;
  Vector4 mGravity, mMagnet, mGyro, mAccel;

  float mSensorInterval;
  Vector3 mPrev_x_world_at_device;
  //vector<Quaternion> mPrevQs;
#endif
};

#endif

Actor CreateImageCube(int num_x, int num_y, int num_z,
                      const Vector3& imageSize,
                      const Vector3& fromRange, const Vector3& toRange,
                      const string& imagePath)
{
  Actor cube = Actor::New();
  for(int x=0; x<num_x; ++x)
    for(int y=0; y<num_y; ++y)
      for(int z=0; z<num_z; ++z)
      {
        float tx = float(x)/(num_x<2 ? 1:num_x-1);
        float ty = float(y)/(num_y<2 ? 1:num_y-1);
        float tz = float(z)/(num_z<2 ? 1:num_z-1);

        Vector3 pos( (1-tx)*fromRange.x + tx*toRange.x,
                    (1-ty)*fromRange.y + ty*toRange.y,
                    (1-tz)*fromRange.z + tz*toRange.z );

        string name = "test";

        ImageView imageView = ImageView::New( imagePath );
        imageView.SetName(name);
        imageView.SetSize(imageSize);
        imageView.SetPosition(pos);
        cube.Add(imageView);

//        TextActor textActor = TextActor::New( stringf("%.0f %.0f %.0f", pos.x, pos.y, pos.z) );
//        textActor.SetColor(Vector4(1,1,1,1));
//        textActor.SetOutline(true, Vector4(0,0,0,1));
//        textActor.SetWeight(TextStyle::BOLD);
//        textActor.SetScale(1.1);
//        textActor.SetParentOrigin(ParentOrigin::CENTER);
//        textActor.SetPosition(Vector3(0, imageSize.y/2., .01));
//        imageActor.Add(textActor);

//        TextActor nameActor = TextActor::New( name );
//        nameActor.SetColor(Vector4(0,1,0,1));
//        nameActor.SetOutline(true, Vector4(0,0,0,1));
//        nameActor.SetWeight(TextStyle::BOLD);
//        nameActor.SetScale(1.2);
//        nameActor.SetParentOrigin(ParentOrigin::CENTER);
//        nameActor.SetPosition(Vector3(0, -imageSize.y/2., .01));
//        imageActor.Add(nameActor);
      }
  return cube;
}

Matrix Mat3ToMat4(const Matrix3& m3)
{
  const float* f = m3.AsFloat();
  Matrix m4;
  m4.SetIdentity();
  m4.SetXAxis(Vector3(f[0],f[3],f[6]));
  m4.SetYAxis(Vector3(f[1],f[4],f[7]));
  m4.SetZAxis(Vector3(f[2],f[5],f[8]));
  return m4;
}

Vector3 DeviceCoord2DaliCoord(const Vector3& v)
{
  return Vector3(v.x,-v.y,v.z);
}

Vector3 Mat3DotVec3(const Matrix3& m, const Vector3& v)
{
  const float* f = m.AsFloat();
  return Vector3(f[0],f[3],f[6])*v.x
        + Vector3(f[1],f[4],f[7])*v.y
        + Vector3(f[2],f[5],f[8])*v.z;
}

Matrix3 GetProjMatOnVector(const Vector3& onVector_unit)
{
  const Vector3& v = onVector_unit;
  return Matrix3(v.x*v.x, v.x*v.y, v.x*v.z,
                v.x*v.y, v.y*v.y, v.y*v.z,
                v.x*v.z, v.y*v.z, v.z*v.z);
}

Matrix3 mat3_sub_mat3(const Matrix3& m1, const Matrix3& m2)
{
  const float* f1 = m1.AsFloat();
  const float* f2 = m2.AsFloat();
  return Matrix3(f1[0]-f2[0],f1[1]-f2[1],f1[2]-f2[2],
                f1[3]-f2[3],f1[4]-f2[4],f1[5]-f2[5],
                f1[6]-f2[6],f1[7]-f2[7],f1[8]-f2[8]);
}

Matrix3 GetProjMatOnPlane(const Vector3& planeNormal_unit)
{
  return mat3_sub_mat3(Matrix3::IDENTITY, GetProjMatOnVector(planeNormal_unit));
}

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

#define ENABLE_STREOSCOPE
#define DALI_IMAGE_DIR "/opt/usr/apps/org.example.vr-test/res/images/"


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

#ifdef ENABLE_STREOSCOPE
  application.SetViewMode(STEREO_HORIZONTAL);
#endif
}

//bool IsActorHittableFunction(Actor actor, Dali::HitTestAlgorithm::TraverseType type)
//{
//  bool hittable = false;
//
//  switch (type)
//  {
//    case Dali::HitTestAlgorithm::CHECK_ACTOR:
//    {
//      // Check whether the actor is visible and not fully transparent.
//      if( actor.IsVisible()
//       && actor.GetCurrentWorldColor().a > 0.01f) // not FULLY_TRANSPARENT
//      {
//        hittable = true;
//      }
//      break;
//    }
//    case Dali::HitTestAlgorithm::DESCEND_ACTOR_TREE:
//    {
//      if( actor.IsVisible() ) // Actor is visible, if not visible then none of its children are visible.
//      {
//        hittable = true;
//      }
//      break;
//    }
//    default:
//    {
//      break;
//    }
//  }
//
//  return hittable;
//}

#ifdef SENSOR_ENABLED
void sensor_cb2(sensor_h sensor, sensor_event_s* event, void *data)
{
  TestStreoscopeController* _this = static_cast<TestStreoscopeController*>(data);

//	dlog_print(DLOG_ERROR, "DALI", "sensor_cb2 %d", _this->mTypes[_this->mSensor2Index[sensor]]);

  if(_this->mTypes[_this->mSensor2Index[sensor]] == SENSOR_GRAVITY)
    _this->mGravity = Vector4(event->values[0], event->values[1], event->values[2], 1);
  else if(_this->mTypes[_this->mSensor2Index[sensor]] == SENSOR_MAGNETIC)
    _this->mMagnet = Vector4(event->values[0], event->values[1], event->values[2], 1);
  else if(_this->mTypes[_this->mSensor2Index[sensor]] == SENSOR_GYROSCOPE)
    _this->mGyro = Vector4(event->values[0], event->values[1], event->values[2], 1);
  else if(_this->mTypes[_this->mSensor2Index[sensor]] == SENSOR_ACCELEROMETER)
    _this->mAccel = Vector4(event->values[0], event->values[1], event->values[2], 1);

  //if(_this->mGravity.w>0 && _this->mMagnet.w>0 && _this->mGyro.w>0 && _this->mAccel.w>0)
  //if(_this->mGravity.w>0 && _this->mMagnet.w>0 && _this->mGyro.w>0)
  if(_this->mAccel.w>0)
  //if(false)
  {
//	  dlog_print(DLOG_ERROR, "DALI", "sensor data received");

    ////////////////////////////////////////////
    // device frame

    // ang vel w.r.t. device frame (from gyro)
    Vector3 w_at_device(Radian(Degree(_this->mGyro[0])), Radian(Degree(_this->mGyro[1])), Radian(Degree(_this->mGyro[2])));
//
//    // TODO : gyroscope calibration
//    w_at_device += Vector3(0.014, -0.005, 0.006);

    //cout << "w_at_device " << w_at_device << endl;
//	   dlog_print(DLOG_ERROR, "DALI", "w_at_device %f %f %f", w_at_device.x, w_at_device.y, w_at_device.z);

    // device coord. (right hand coord.)
    //    Vector3 y_world_at_device(_this->mGravity.x,_this->mGravity.y,_this->mGravity.z);
    Vector3 y_world_at_device(-_this->mAccel.x,-_this->mAccel.y,-_this->mAccel.z);
    y_world_at_device.Normalize();

   //dlog_print(DLOG_ERROR, "DALI", "y_world_at_device %f %f %f", y_world_at_device.x, y_world_at_device.y, y_world_at_device.z);


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
    cameraActor.SetOrientation(Quaternion());
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

//    ////////////////////////////////////////////
//    // item hit-test
//    Vector2 stageSize = Stage::GetCurrent().GetSize();
//    HitTestAlgorithm::Results hitTestResults;
//    //HitTest( Stage::GetCurrent(), Vector2(0,0), hitTestResults, IsActorHittableFunction );
//    HitTest( Stage::GetCurrent(), Vector2(stageSize.y/2,-stageSize.x/2), hitTestResults, IsActorHittableFunction );
//    if(hitTestResults.actor)
//    {
//      //Dali::Actor handle( hitTestResults.actor );
//      HoverEvent hevt;
//      hevt.points.push_back(TouchPoint(0, TouchPoint::Motion, 0, 0));
//      //hitTestResults.actor.HoveredSignal().Emit(handle, hevt);
//      hitTestResults.actor.HoveredSignal().Emit(hitTestResults.actor, hevt);
//    }

    ////////////////////////////////////////////
    _this->mGravity.w = 0;
    _this->mMagnet.w = 0;
    _this->mGyro.w = 0;
    _this->mAccel.w = 0;

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
  int error;
  error = sensor_is_supported(SENSOR_ALL, &supported);
  cout << "SENSOR_ALL " << supported << endl;
  dlog_print(DLOG_ERROR, "DALI", "SENSOR_ALL supported %d, error %d", supported, error);

  error = sensor_is_supported(SENSOR_ACCELEROMETER , &supported);
  cout << "SENSOR_ACCELEROMETER " << supported << endl;
  dlog_print(DLOG_ERROR, "DALI", "SENSOR_ACCELEROMETER %d, error %d", supported, error);

  error = sensor_is_supported(SENSOR_GRAVITY , &supported);
  cout << "SENSOR_GRAVITY " << supported << endl;
  dlog_print(DLOG_ERROR, "DALI", "SENSOR_GRAVITY %d, error %d", supported, error);

  error = sensor_is_supported(SENSOR_LINEAR_ACCELERATION , &supported);
  cout << "SENSOR_LINEAR_ACCELERATION " << supported << endl;
  dlog_print(DLOG_ERROR, "DALI", "SENSOR_LINEAR_ACCELERATION %d, error %d", supported, error);

  error = sensor_is_supported(SENSOR_MAGNETIC , &supported);
  cout << "SENSOR_MAGNETIC " << supported << endl;
  dlog_print(DLOG_ERROR, "DALI", "SENSOR_MAGNETIC %d, error %d", supported, error);

  error = sensor_is_supported(SENSOR_ROTATION_VECTOR , &supported);
  cout << "SENSOR_ROTATION_VECTOR " << supported << endl;
  dlog_print(DLOG_ERROR, "DALI", "SENSOR_ROTATION_VECTOR% d, error %d", supported, error);

  error = sensor_is_supported(SENSOR_ORIENTATION , &supported);
  cout << "SENSOR_ORIENTATION " << supported << endl;
  dlog_print(DLOG_ERROR, "DALI", "SENSOR_ORIENTATION %d, error %d", supported, error);

  error = sensor_is_supported(SENSOR_GYROSCOPE , &supported);
  cout << "SENSOR_GYROSCOPE " << supported << endl;
  dlog_print(DLOG_ERROR, "DALI", "SENSOR_GYROSCOPE %d, error %d", supported, error);

  error = sensor_is_supported(SENSOR_LIGHT , &supported);
  cout << "SENSOR_LIGHT " << supported << endl;
  dlog_print(DLOG_ERROR, "DALI", "SENSOR_LIGHT %d, error %d", supported, error);

  error = sensor_is_supported(SENSOR_PROXIMITY , &supported);
  cout << "SENSOR_PROXIMITY " << supported << endl;
  dlog_print(DLOG_ERROR, "DALI", "SENSOR_PROXIMITY %d, error %d", supported, error);

  error = sensor_is_supported(SENSOR_PRESSURE , &supported);
  cout << "SENSOR_PRESSURE " << supported << endl;
  dlog_print(DLOG_ERROR, "DALI", "SENSOR_PRESSURE %d, error %d", supported, error);

  error = sensor_is_supported(SENSOR_ULTRAVIOLET , &supported);
  cout << "SENSOR_ULTRAVIOLET " << supported << endl;
  dlog_print(DLOG_ERROR, "DALI", "SENSOR_ULTRAVIOLET%d, error %d", supported, error);

  error = sensor_is_supported(SENSOR_TEMPERATURE , &supported);
  cout << "SENSOR_TEMPERATURE " << supported << endl;
  dlog_print(DLOG_ERROR, "DALI", "SENSOR_TEMPERATURE %d, error %d", supported, error);

  error = sensor_is_supported(SENSOR_HUMIDITY , &supported);
  cout << "SENSOR_HUMIDITY " << supported << endl;
  dlog_print(DLOG_ERROR, "DALI", "SENSOR_HUMIDITY %d, error %d", supported, error);

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

  // ms
//  mSensorInterval = 100;
  mSensorInterval = 16.6;
//  mSensorInterval = 11.1;
//  mSensorInterval = 10;
//  mSensorInterval = 5;
//  mSensorInterval = 1;
//  mSensorInterval = .1;

  for(uint i=0; i<mTypes.size(); ++i)
  {
    mType2Index[mTypes[i]] = i;
    sensor_get_default_sensor(mTypes[i], &mSensors[i]);
    sensor_create_listener(mSensors[i], &mListeners[i]);
    sensor_listener_set_event_cb(mListeners[i], mSensorInterval, sensor_cb2, this);
    sensor_listener_start(mListeners[i]);
    mSensor2Index[mSensors[i]] = i;
  }

  mGravity.w = 0;
  mMagnet.w = 0;
}
#endif

bool TestStreoscopeController::OnTouch( Actor actor, const TouchEvent& event )
{
	dlog_print(DLOG_ERROR, "DALI", "OnTouch");
  return false;
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
