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

#include <dali/dali.h>
#include <map>

using namespace std;
using namespace Dali;
using namespace Dali::Toolkit;

//#ifdef __arm__
//#define SENSOR_ENABLED
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
  Vector4 mGravity, mMagnet, mGyro;
  //Vector4 mGravity, mMagnet, mGyro, mAccel;

  float mSensorInterval;
  Vector3 mPrev_x_world_at_device;
  //vector<Quaternion> mPrevQs;
#endif
};

#endif
