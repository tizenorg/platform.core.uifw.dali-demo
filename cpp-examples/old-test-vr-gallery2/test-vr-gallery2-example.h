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

enum Mode { GROUP_VIEW, THUMB_VIEW, PICTURE_VIEW };

class VRGalleryController : public ConnectionTracker
{
public:

  VRGalleryController( Application& application );
  ~VRGalleryController() {}

  void Create( Application& application );
  void CreateCursor();
  void CreateImages();
  void CreateGroups();

  void ActivateLayout(Mode mode, Actor selected, float duration);
  void RelayoutGroups(Actor selectedGroup, float duration);
  void RelayoutThumbs(Actor group, Actor selectedThumb, float duration);
  void RelayoutPictures(Actor picture, float duration);
  void HideGroups(Actor exceptGroup, float duration);
  void PushGroups(Actor exceptGroup, float duration);
  void ShowGroups(float duration);
  void ScrollTo(float scrollOffset);
  //void CheckVisibleActors(float duration);

  void OnKeyEvent(const KeyEvent& event);
  bool OnHover( Actor actor, const HoverEvent& event );
  bool OnTouch( Actor actor, const TouchEvent& event );
  //void OnFinishAni(Animation& ani);

  void InitSelectInRadius();
  void ShowSelectInRadius();
  void StartSelection(const Vector3& startPos, Actor startActor);
  void StopSelection();
  void DoSelect(Actor actor);
  void DoCancel(Actor actor);

  //void InitPictures(int itemIds);
  //void RotatePictures(int offset);  // +offset : rotate left

private:
  Application&  mApplication;

public:
  Mode mMode;

  vector<Actor> mGroups;
  map<Actor, Actor> mPic2Group;
  map<Actor, uint> mPic2IndexInGroup;
  vector<Image> mImages;
  map<Actor, float> mPic2Scale;
  map<Actor, uint> mGroup2Index;

  Actor mPrevHoveredActor;
  Actor mCurGroup;
  Actor mCurPicture;

  float mScrollPictureIdx;
  float mScrollGroupIdx;

  bool mInit;
  Actor cross;
  Actor mLastHitImage; 
  float mItemId;

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
