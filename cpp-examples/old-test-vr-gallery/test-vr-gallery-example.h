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
#include <list>

using namespace std;
using namespace Dali;
using namespace Dali::Toolkit;

#ifdef __arm__
#define SENSOR_ENABLED
//#define AUDIO_ENABLED
#endif

#ifdef SENSOR_ENABLED
#include <sensor/sensor.h>
#endif

#ifdef AUDIO_ENABLED 
#include <media/audio_io.h>
#endif

enum Mode { GROUP_VIEW, ITEM_VIEW, IMAGE_VIEW };

class VRGalleryController : public ConnectionTracker, public ItemFactory
{
public:

  VRGalleryController( Application& application );
  ~VRGalleryController() {}

  void Create( Application& application );
  TextActor CreateText( const string& name, const Vector3& position, const Vector4& color );
  Actor CreateTextGroup( const string& groupName, int stepWidth, int repeat );
  void CreateItemView();
  void UseLayout(int layoutId, float duration);

  bool OnTouch( Actor actor, const TouchEvent& touch );
  void OnKeyEvent(const KeyEvent& event);

  bool OnTouchImage( Actor actor, const TouchEvent& event );
  bool OnHoverImage( Actor actor, const HoverEvent& event );
  bool OnHoverImage( Actor actor, const TouchEvent& event );

  //bool OnHoverItemView( Actor actor, const HoverEvent& event );

  virtual unsigned int GetNumberOfItems();
  virtual Actor NewItem(unsigned int itemId);

  void InitSelectInRadius();
  void ShowSelectInRadius();
  void StartSelection(const Vector3& startPos, Actor startActor);
  void StopSelection();
  void DoSelect(Actor actor);
  void DoCancel(Actor actor);

  void InitPictures(int itemIds);
  void RotatePictures(int offset);  // +offset : rotate left

private:
  Application&  mApplication;
  GridLayoutPtr mGridLayout;
  //RollLayoutPtr mRollLayout;
  //Image mBorderImage;

public:
  Mode mMode;

  bool mInit;
  Actor cross;
  ItemView mItemView;
  float mItemId;
  Actor mLastHitImage; 

  //Actor mPictureRoot;
  //vector<ImageActor> mPictures;
  //ImageActor mExtraPicture;

  ImageActor mCurrImage;
  ImageActor mPrevImage;
  ImageActor mNextImage;
  ImageActor mPrevImage2;
  ImageActor mNextImage2;

  Actor mCross;
  Actor mSelectInRadius;
  Actor mSelectInRadius2;

  uint mCBCount;
  list<Vector3> mXYPosz;

  bool mSelecting;
  bool mReachOutRadius;
  Vector3 mSelectStartPos;
  Vector3 mFarmostPosFromStartPos;

  Actor mSelectStartHitActor;
  uint mSelectStartCBCount;
  Animation mDoSelectAni;
  Animation mDoCancelAni;
  Animation mRadiusAni;
  Animation mEnterLargeImageAni;
  Animation mBackToItemViewAni;
  

#ifdef SENSOR_ENABLED
public:
  void CreateSensors();
  void PrintSensorValues(sensor_type_e sensorType);
  void PrintSensorValues2(sensor_h sensor, const sensor_event_s& event);

  vector<sensor_type_e> mTypes;
  map<sensor_type_e,string> mType2String;
  map<sensor_type_e,int> mType2Index;

  vector<sensor_h> mSensors;
  map<sensor_h,int> mSensor2Index;

  vector<sensor_listener_h> mListeners;
  Vector4 mGravity, mMagnet, mGyro;
  //Vector4 mGravity, mMagnet, mGyro, mAccel;

  Vector3 mPrev_x_world_at_device;
  //vector<Quaternion> mPrevQs;

  bool IsInSelectionInDuration();
  bool IsInSelectionOutDuration();
#endif
#ifdef AUDIO_ENABLED
public:
  audio_in_h mAudioIn;
  char* mBuffer;
#endif

};

#endif
