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
#include "test-capi-sensor-example.h"
#include "../test-shared/common.h"

TestCAPISensorController::TestCAPISensorController( Application& application )
: mApplication( application )
{
  std::cout << "TestCAPISensorController::TestCAPISensorController" << std::endl;

  // Connect to the Application's Init signal
  mApplication.InitSignal().Connect( this, &TestCAPISensorController::Create );
}

  // The Init signal is received once (only) during the Application lifetime
void TestCAPISensorController::Create( Application& application )
{
  std::cout << "TestCAPISensorController::Create" << std::endl;

  // Initialize the actor
  TextActor baseText = TextActor::New( "Touch & watch sdb shell output!" );

  // Reposition the actor
  baseText.SetParentOrigin( ParentOrigin::CENTER );

  // Get a handle to the stage
  Stage stage = Stage::GetCurrent();

  // Display the actor on the stage
  stage.Add( baseText );

  // Respond to a click anywhere on the stage
  stage.GetRootLayer().TouchedSignal().Connect( this, &TestCAPISensorController::OnTouch );

  // key handler
  Stage::GetCurrent().KeyEventSignal().Connect( this, &TestCAPISensorController::OnKeyEvent );

#ifdef SENSOR_ENABLED
  CreateSensors();
#endif
}

#ifdef SENSOR_ENABLED
void sensor_cb(sensor_h sensor, sensor_event_s* event, void *data)
{
  //cout << event->value_count << endl;
}
#endif

#ifdef SENSOR_ENABLED
void TestCAPISensorController::CreateSensors()
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
  mTypes.push_back(SENSOR_GRAVITY);
  mTypes.push_back(SENSOR_LINEAR_ACCELERATION);
  mTypes.push_back(SENSOR_MAGNETIC);
  mTypes.push_back(SENSOR_ROTATION_VECTOR);
  mTypes.push_back(SENSOR_ORIENTATION);
  mTypes.push_back(SENSOR_GYROSCOPE);
  mTypes.push_back(SENSOR_LIGHT);
  mTypes.push_back(SENSOR_PROXIMITY);
  mTypes.push_back(SENSOR_PRESSURE);

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

  for(uint i=0; i<mTypes.size(); ++i)
  {
    mType2Index[mTypes[i]] = i;
    sensor_get_default_sensor(mTypes[i], &mSensors[i]);
    sensor_create_listener(mSensors[i], &mListeners[i]);
    sensor_listener_set_event_cb(mListeners[i], 200, sensor_cb, NULL);
    sensor_listener_start(mListeners[i]);
  }
}
#endif

#ifdef SENSOR_ENABLED
void TestCAPISensorController::PrintSensorValues(sensor_type_e sensorType)
{
  sensor_event_s event;
  sensor_listener_read_data(mListeners[mType2Index[sensorType]], &event);

  string strSensor = stringf("%-27s", mType2String[sensorType].c_str());
  string strAccuracy = stringf("a %d", event.accuracy);
  string strValues = "v ";
  for(int i=0; i<event.value_count; ++i)
  {
    string temp = stringf("%10.4f", event.values[i]);
    strValues += temp;
  }

  cout << strSensor << "  " << strAccuracy << "  " << strValues << endl;
}
#endif

bool TestCAPISensorController::OnTouch( Actor actor, const TouchEvent& touch )
{
#ifdef SENSOR_ENABLED
  //PrintSensorValues(SENSOR_ACCELEROMETER);
  //PrintSensorValues(SENSOR_LINEAR_ACCELERATION);
  PrintSensorValues(SENSOR_GRAVITY);
  PrintSensorValues(SENSOR_MAGNETIC);

  //PrintSensorValues(SENSOR_ROTATION_VECTOR);
  //PrintSensorValues(SENSOR_ORIENTATION);
  PrintSensorValues(SENSOR_GYROSCOPE);

  //PrintSensorValues(SENSOR_LIGHT);
  //PrintSensorValues(SENSOR_PROXIMITY);
  //PrintSensorValues(SENSOR_PRESSURE);
  cout << endl;
#endif

  // quit the application
  //mApplication.Quit();
  return true;
}
void TestCAPISensorController::OnKeyEvent( const KeyEvent& event )
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
  TestCAPISensorController test( application );

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
