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

using namespace Dali;
using namespace std;

#ifdef __arm__
#define SENSOR_ENABLED
#endif

#ifdef SENSOR_ENABLED
#include <sensor/sensor.h>
#endif

class TestCAPISensorController : public ConnectionTracker
{
public:

  TestCAPISensorController( Application& application );
  ~TestCAPISensorController() {}

  void Create( Application& application );

  bool OnTouch( Actor actor, const TouchEvent& touch );
  void OnKeyEvent(const KeyEvent& event);

private:
  Application&  mApplication;

#ifdef SENSOR_ENABLED
public:
  void CreateSensors();
  void PrintSensorValues(sensor_type_e sensorType);
private:
  vector<sensor_type_e> mTypes;
  map<sensor_type_e,string> mType2String;
  map<sensor_type_e,int> mType2Index;
  vector<sensor_h> mSensors;
  vector<sensor_listener_h> mListeners;
#endif
};

#endif
