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
#include "test-vr-gallery2-example.h"
#include "../test-shared/common.h"

const char* IMAGE_PATHS[] = {
    DALI_IMAGE_DIR "gallery-large-1.jpg",
    DALI_IMAGE_DIR "gallery-large-2.jpg",
    DALI_IMAGE_DIR "gallery-large-3.jpg",
    DALI_IMAGE_DIR "gallery-large-4.jpg",
    DALI_IMAGE_DIR "gallery-large-5.jpg",
    DALI_IMAGE_DIR "gallery-large-6.jpg",
    DALI_IMAGE_DIR "gallery-large-7.jpg",
    DALI_IMAGE_DIR "gallery-large-8.jpg",
    DALI_IMAGE_DIR "gallery-large-9.jpg",
    DALI_IMAGE_DIR "gallery-large-10.jpg",
    DALI_IMAGE_DIR "gallery-large-11.jpg",
    DALI_IMAGE_DIR "gallery-large-12.jpg",
    DALI_IMAGE_DIR "gallery-large-13.jpg",
    DALI_IMAGE_DIR "gallery-large-14.jpg",
    DALI_IMAGE_DIR "gallery-large-15.jpg",
    DALI_IMAGE_DIR "gallery-large-16.jpg",
    DALI_IMAGE_DIR "gallery-large-17.jpg",
    DALI_IMAGE_DIR "gallery-large-18.jpg",
    DALI_IMAGE_DIR "gallery-large-19.jpg",
    DALI_IMAGE_DIR "gallery-large-20.jpg",
    DALI_IMAGE_DIR "gallery-large-21.jpg",
};

const unsigned int NUM_IMAGES = sizeof(IMAGE_PATHS) / sizeof(char*);

const int VR_SCREEN_WIDTH = 4500;
const int VR_SCREEN_HEIGHT = 1000;
const int SCROLL_MARGIN = 1000;

const float MOVE_CHECK_DURATION = .2;
const float MOVE_CHECK_RADIUS = 40.;
const float SELECT_OUT_DURATION = .5;
const float SELECT_IN_DURATION = .5;
const float SELECT_OUT_RADIUS = 70.;
const float SELECT_IN_RADIUS = 35.;
const float RADIUS_ALPHA = .3;
const float SENSOR_INTERVAL = .01;

const unsigned int NUM_GROUPS = 4;
const unsigned int NUM_GROUPITEMS = 200;

const unsigned int ROW_THUMBS = 4;

const float GROUP_SCALE = .3;
const float GROUP_THUMB_SCALE_RATIO = 1;

const float THUMB_SCALE = (1. / ROW_THUMBS)*.9;

const Vector3 HOVER_MOVE(0,0,70);
const float HOVER_SCALE = 1.2;

//const float PICTURE_SCALE = 1.;
//const float PICTURE_SCALE = .5;
const float PICTURE_SCALE = 1./HOVER_SCALE;
const float PICTURE_OTHER_SCALE_START = .6;
const float PICTURE_OTHER_SCALE_RATIO = .9;
const float PICTURE_OTHER_DARKER_START = .5;
const float PICTURE_OTHER_DARKER_RATIO = .8;

const float GROUP_GAP = VR_SCREEN_HEIGHT * GROUP_SCALE * 2.;
//const float GROUP_GAP = VR_SCREEN_HEIGHT * GROUP_SCALE * 1.;
const float THUMB_GAP = VR_SCREEN_HEIGHT * (1. / ROW_THUMBS);
const Vector3 THUMB_GAP_IN_GROUP(50,-50,-10);
const float NUM_THUMB_IN_GROUP = 4;

const float DARKEREST_RATIO = .3;

//const unsigned int NUM_MAIN_PICTURES = 5;
const unsigned int NUM_MAIN_PICTURES = 3;
const unsigned int NUM_SIDE_PICTURES = 7;
const float PICTURE_GAP_X = VR_SCREEN_HEIGHT*1.;
const float PICTURE_GAP_Z = -100;

const float LAYOUT_DURATION = .4;
const float HOVER_DURATION = .4;

//#define DRAW_SCROLL_MARGIN
#define DRAW_CENTER_LINE
#define DRAW_CROSS

VRGalleryController::VRGalleryController( Application& application )
: mApplication( application ), mInit(false)
{
  std::cout << "VRGalleryController::VRGalleryController" << std::endl;

  // Connect to the Application's Init signal
  mApplication.InitSignal().Connect( this, &VRGalleryController::Create );
}

// The Init signal is received once (only) during the Application lifetime
void VRGalleryController::Create( Application& application )
{
  std::cout << "VRGalleryController::Create" << std::endl;

  // key handler
  Stage::GetCurrent().KeyEventSignal().Connect( this, &VRGalleryController::OnKeyEvent );

  Stage::GetCurrent().GetRootLayer().TouchedSignal().Connect(this, &VRGalleryController::OnTouch);
  Stage::GetCurrent().GetRootLayer().HoveredSignal().Connect(this, &VRGalleryController::OnHover);

  // Respond to a click anywhere on the stage
  Stage stage = Stage::GetCurrent();

#ifdef SENSOR_ENABLED
  CreateSensors();
#endif

#ifdef DRAW_SCROLL_MARGIN
  Vector4 marginColor(1,1,1,.5);
  Actor margin_l = CreateVerticalLine(Vector3(-VR_SCREEN_WIDTH/2+SCROLL_MARGIN, 0, -.01), marginColor, 1020, 3);
  Actor margin_r = CreateVerticalLine(Vector3(VR_SCREEN_WIDTH/2-SCROLL_MARGIN, 0, -.01), marginColor, 1020, 3);
  stage.Add(margin_l);
  stage.Add(margin_r);
#endif

#ifdef DRAW_CENTER_LINE
  Vector4 centerLineColor(1,1,1,1);
  Actor centerLine = CreateVerticalLine(Vector3(0, 0, -.01), centerLineColor, 1020, 2);
  stage.Add(centerLine);
#endif

#ifdef DRAW_CROSS
  Vector4 crossColor(1,0,0,.5);

  mCross = CreateCross(Vector3(0, 0, 2560-.01), crossColor, 30);
  stage.Add(mCross);

  mCross.SetParentOrigin(ParentOrigin::CENTER);
  mCross.SetSensitive(false);
#endif

  CreateCursor();
  CreateImages();
  CreateGroups();

  ActivateLayout(GROUP_VIEW, mGroups[mGroups.size()/2], 0);

  mCBCount = 0;
  mItemId = 0;

  // streo 3d
  application.SetViewMode(STEREO_HORIZONTAL);
}

bool VRGalleryController::OnHover( Actor actor, const HoverEvent& event )
{
  if(event.GetPointCount() > 0)
  {
    const TouchPoint& tp = event.GetPoint(0);
    if(tp.state==TouchPoint::Motion) 
    {
      if(mMode==GROUP_VIEW)
      {
        Actor curGroup;
        if(mPic2Group.count(actor) > 0)
          curGroup = mPic2Group[actor];

        Actor prevGroup;
        if(mPrevHoveredActor)
          if(mPic2Group.count(mPrevHoveredActor) > 0)
            prevGroup = mPic2Group[mPrevHoveredActor];

        if(curGroup!=prevGroup)
        {
          Animation ani = Animation::New(HOVER_DURATION);
          if(curGroup)
          {
            ani.ScaleTo(curGroup, GROUP_SCALE*HOVER_SCALE, GROUP_SCALE*HOVER_SCALE, 1.);
            ani.MoveBy(curGroup, HOVER_MOVE, AlphaFunctions::EaseInOut);
          }
          if(prevGroup)
          {
            ani.ScaleTo(prevGroup, GROUP_SCALE, GROUP_SCALE, 1.);
            ani.MoveBy(prevGroup, -HOVER_MOVE, AlphaFunctions::EaseInOut);
          }
          ani.Play();
        }
      }
      else if(mMode==THUMB_VIEW)
      {
        Actor curActor = actor;
        Actor prevActor = mPrevHoveredActor;
        if(curActor!=prevActor)
        {
          Animation ani = Animation::New(HOVER_DURATION);
          if(curActor && curActor!=Stage::GetCurrent().GetRootLayer())
          {
            ani.ScaleTo(curActor, HOVER_SCALE, HOVER_SCALE, 1.);
            ani.MoveBy(curActor, HOVER_MOVE, AlphaFunctions::EaseInOut);
          }
          if(prevActor && prevActor!=Stage::GetCurrent().GetRootLayer())
          {
            ani.ScaleTo(prevActor, 1, 1, 1);
            ani.MoveBy(prevActor, -HOVER_MOVE, AlphaFunctions::EaseInOut);
          }
          ani.Play();
        }
      }
      else if(mMode==PICTURE_VIEW)
      {
        Actor curActor = actor;
        Actor prevActor = mPrevHoveredActor;
        if(curActor!=prevActor)
        {
          Animation ani = Animation::New(HOVER_DURATION);
          if(curActor && curActor!=Stage::GetCurrent().GetRootLayer())// &&
              //curActor != mCurPicture)
          {
            if(mPic2Scale.count(curActor) > 0)
            {
              float prevPicScale = mPic2Scale[curActor];
              ani.ScaleTo(curActor, prevPicScale*HOVER_SCALE, prevPicScale*HOVER_SCALE, 1.);
              ani.MoveBy(curActor, HOVER_MOVE, AlphaFunctions::EaseInOut);
            }
          }
          if(prevActor && prevActor!=Stage::GetCurrent().GetRootLayer())// &&
              //prevActor != mCurPicture)
          {
            if(mPic2Scale.count(prevActor) > 0)
            {
              float prevPicScale = mPic2Scale[prevActor];
              ani.ScaleTo(prevActor, prevPicScale, prevPicScale, 1);
              ani.MoveBy(prevActor, -HOVER_MOVE, AlphaFunctions::EaseInOut);
            }
          }
          ani.Play();
        }
      }
      mPrevHoveredActor = actor;  // for desktop support
    }
  }
  return true;
}

bool VRGalleryController::OnTouch( Actor actor, const TouchEvent& event )
{
  if(event.GetPointCount() > 0 && mPic2Group.count(actor) > 0)
  {
    const TouchPoint& tp = event.GetPoint(0);
    if(tp.state==TouchPoint::Finished) 
    {
      if(mMode==GROUP_VIEW)
      {
        mCurGroup = mPic2Group[actor];
        ActivateLayout(THUMB_VIEW, mCurGroup.GetChildAt(0), LAYOUT_DURATION);
      }
      else if(mMode==THUMB_VIEW)
        ActivateLayout(PICTURE_VIEW, actor, LAYOUT_DURATION);
      else if(mMode==PICTURE_VIEW)
        ActivateLayout(PICTURE_VIEW, actor, LAYOUT_DURATION);
    }
  }
  return true;
}

void VRGalleryController::ScrollTo( float scrollOffset )
{
  if(mMode==GROUP_VIEW)
  {
  }
  else if(mMode==THUMB_VIEW || mMode==PICTURE_VIEW)
  {
    float speed = (SENSOR_INTERVAL/.01) * scrollOffset;
    if(mMode==THUMB_VIEW)
      speed *= .0005;
    else if(mMode==PICTURE_VIEW)
      speed *= .00005;

    mScrollPictureIdx += speed;
    if(mScrollPictureIdx < 0)
      mScrollPictureIdx = 0;
    if(mScrollPictureIdx > mCurGroup.GetChildCount()-1) 
      mScrollPictureIdx = mCurGroup.GetChildCount()-1;

    if(mMode==THUMB_VIEW)
      RelayoutThumbs(mCurGroup, mCurGroup.GetChildAt(int(mScrollPictureIdx)), LAYOUT_DURATION);
    else if(mMode==PICTURE_VIEW)
      RelayoutPictures(mCurGroup.GetChildAt(int(mScrollPictureIdx)), LAYOUT_DURATION);
  }
}

//void VRGalleryController::RelayoutGroups(Actor selectedGroup, float duration, Actor exceptGroup)
//{
  //Animation ani = Animation::New(duration);

  //uint selectedIdx = mGroup2Index[selectedGroup];
  //float selectedOffsetX = selectedIdx * GROUP_GAP;

  //for(uint i=0; i<NUM_GROUPS; ++i)
  //{
    //Actor group = mGroups[i];
    //if(group==exceptGroup)
      //continue;
    //ani.MoveTo(group, i*GROUP_GAP-selectedOffsetX, 0, -HOVER_MOVE.z);
    //ani.ScaleTo(group, GROUP_SCALE, GROUP_SCALE, 1);

    //for(uint j=0; j<group.GetChildCount(); ++j)
    //{
      //Vector3 scale(1,1,1);
      //if(j<NUM_THUMB_IN_GROUP)
      //{
        //ani.MoveTo(group.GetChildAt(j), THUMB_GAP_IN_GROUP*j,
                  //AlphaFunctions::EaseInOut);
        //Vector4 color(1,1,1,1);
        //color *= (1. - (1.-DARKEREST_RATIO)*j/NUM_THUMB_IN_GROUP);
        //color.w = 1.;
        //ani.ColorTo(group.GetChildAt(j), color);
        //scale.x *= pow(GROUP_THUMB_SCALE_RATIO, j);
        //scale.y *= pow(GROUP_THUMB_SCALE_RATIO, j);
      //}
      //else
      //{
        //int pos_j = NUM_THUMB_IN_GROUP-1;
        //ani.MoveTo(group.GetChildAt(j), THUMB_GAP_IN_GROUP*pos_j,
                  //AlphaFunctions::EaseInOut);
        //ani.OpacityTo(group.GetChildAt(j), 0.);
      //}
      //ani.ScaleTo(group.GetChildAt(j), scale, AlphaFunctions::Default);
    //}
  //}
  //ani.Play();
  //mPrevHoveredActor = NULL;
//}

void VRGalleryController::RelayoutGroups(Actor selectedGroup, float duration)
{
  Animation ani = Animation::New(duration);

  uint selectedIdx = mGroup2Index[selectedGroup];
  float selectedOffsetX = selectedIdx * GROUP_GAP;

  for(uint i=0; i<NUM_GROUPS; ++i)
  {
    Actor group = mGroups[i];
    ani.MoveTo(group, i*GROUP_GAP-selectedOffsetX, 0, -HOVER_MOVE.z);
    ani.ScaleTo(group, GROUP_SCALE, GROUP_SCALE, 1);

    for(uint j=0; j<group.GetChildCount(); ++j)
    {
      Vector3 scale(1,1,1);
      if(j<NUM_THUMB_IN_GROUP)
      {
        ani.MoveTo(group.GetChildAt(j), THUMB_GAP_IN_GROUP*j,
                  AlphaFunctions::EaseInOut);
        Vector4 color(1,1,1,1);
        color *= (1. - (1.-DARKEREST_RATIO)*j/NUM_THUMB_IN_GROUP);
        color.w = 1.;
        ani.ColorTo(group.GetChildAt(j), color);
        scale.x *= pow(GROUP_THUMB_SCALE_RATIO, j);
        scale.y *= pow(GROUP_THUMB_SCALE_RATIO, j);
      }
      else
      {
        int pos_j = NUM_THUMB_IN_GROUP-1;
        ani.MoveTo(group.GetChildAt(j), THUMB_GAP_IN_GROUP*pos_j,
                  AlphaFunctions::EaseInOut);
        ani.OpacityTo(group.GetChildAt(j), 0.);
      }
      ani.ScaleTo(group.GetChildAt(j), scale, AlphaFunctions::Default);
    }
  }
  ani.Play();
  //mPrevHoveredActor = NULL;
}

void VRGalleryController::PushGroups(Actor exceptGroup, float duration)
{
  Animation ani = Animation::New(duration);
  for(uint i=0; i<NUM_GROUPS; ++i)
  {
    Actor group = mGroups[i];
    if(group==exceptGroup)
      continue;
    ani.MoveBy(group, 0, -VR_SCREEN_HEIGHT, -500);
  }
  ani.Play();
}

void VRGalleryController::HideGroups(Actor exceptGroup, float duration)
{
  Animation ani = Animation::New(duration);
  for(uint i=0; i<NUM_GROUPS; ++i)
  {
    Actor group = mGroups[i];
    if(group==exceptGroup)
      continue;
    ani.OpacityTo(group, 0);
  }
  ani.Play();
}

void VRGalleryController::ShowGroups(float duration)
{
  Animation ani = Animation::New(duration);
  for(uint i=0; i<NUM_GROUPS; ++i)
  {
    Actor group = mGroups[i];
    ani.OpacityTo(group, 1);
  }
  ani.Play();
}

void VRGalleryController::RelayoutPictures(Actor picture, float duration)
{
  Animation ani = Animation::New(duration);
  Actor curGroup = mPic2Group[picture];
  uint curIdx = mPic2IndexInGroup[picture];

  ani.MoveTo(curGroup, 0,0,0);
  ani.ScaleTo(curGroup, 1, 1, 1);

  for(uint i=0; i<curGroup.GetChildCount(); ++i)
  {
    Actor pic = curGroup.GetChildAt(i);
    int offset = int(i-curIdx);

    Vector3 move(PICTURE_GAP_X*offset, 0, PICTURE_GAP_Z*abs(offset));
    Vector3 scale(PICTURE_SCALE, PICTURE_SCALE, 1);
    Vector4 color(1,1,1,1);
    //if(i == curIdx)
    //{
    //}
    if(int(i) >= int(curIdx)-int(NUM_MAIN_PICTURES/2) && 
        int(i) <= int(curIdx)+int(NUM_MAIN_PICTURES/2) )
    {
      move.z = PICTURE_GAP_Z;
      mPic2Scale[mCurGroup.GetChildAt(i)] = scale.x;
    }
    else if(int(i) >= int(curIdx)-int(NUM_MAIN_PICTURES/2) - int(NUM_SIDE_PICTURES)/2 && 
            int(i) <= int(curIdx)+int(NUM_MAIN_PICTURES/2) + int(NUM_SIDE_PICTURES)/2)
    {
      //int halfWidth = 1;
      int halfWidth = int(NUM_MAIN_PICTURES/2);
      scale.x *= (PICTURE_OTHER_SCALE_START * pow(PICTURE_OTHER_SCALE_RATIO, abs(offset)-halfWidth)); 
      scale.y *= (PICTURE_OTHER_SCALE_START * pow(PICTURE_OTHER_SCALE_RATIO, abs(offset)-halfWidth)); 
      //move.x *= (PICTURE_OTHER_SCALE_START * pow(PICTURE_OTHER_SCALE_RATIO, abs(offset)-halfWidth)); 
      color *= (PICTURE_OTHER_DARKER_START * pow(PICTURE_OTHER_DARKER_RATIO, abs(offset)-halfWidth));
      color.w = 1.;
      //mPic2Scale[mCurGroup.GetChildAt(i)] = scale.x;
    }
    else
      color *= 0.;

    ani.MoveTo(pic, move, AlphaFunctions::EaseInOut);
    ani.ScaleTo(pic, scale, AlphaFunctions::Default);
    ani.ColorTo(pic, color);
  }

  ani.Play();

  mCurGroup = curGroup;
  mCurPicture = picture;
  //mPrevHoveredActor = NULL;
}

void VRGalleryController::RelayoutThumbs(Actor group, Actor selectedThumb, float duration)
{
  Animation ani = Animation::New(duration);
  //ani.FinishedSignal().Connect(this, &VRGalleryController::OnFinishAni);

  ani.MoveTo(group, 0,0,-HOVER_MOVE.z);
  ani.ScaleTo(group, THUMB_SCALE, THUMB_SCALE, 1);

  //Vector3 totalOffset(-VR_SCREEN_HEIGHT,-VR_SCREEN_HEIGHT*.5+THUMB_GAP*.5,0); 
  Vector3 totalOffset(0,-VR_SCREEN_HEIGHT*.5+THUMB_GAP*.5,0); 
  totalOffset *= (1./THUMB_SCALE);

  uint selectedIdx = mPic2IndexInGroup[selectedThumb];
  uint selectedCol = selectedIdx / ROW_THUMBS;
  float selectedOffsetX = selectedCol*THUMB_GAP/THUMB_SCALE;

  uint idx = 0;
  for(uint c=0; c<NUM_GROUPITEMS/ROW_THUMBS+1; ++c)
    for(uint r=0; r<ROW_THUMBS; ++r)
    {
      if(idx < NUM_GROUPITEMS)
      {
        ani.MoveTo(group.GetChildAt(idx), 
                  Vector3(c*THUMB_GAP/THUMB_SCALE-selectedOffsetX, r*THUMB_GAP/THUMB_SCALE, 0) + totalOffset,
                  AlphaFunctions::EaseInOut);
        ani.ScaleTo(group.GetChildAt(idx), 1,1,1);

        if(int(c - selectedCol) < (-.5*VR_SCREEN_WIDTH)/THUMB_GAP || 
            int(c - selectedCol) > (.5*VR_SCREEN_WIDTH)/THUMB_GAP)
          ani.ColorTo(group.GetChildAt(idx), Vector4(1,1,1,0));
        else
          ani.ColorTo(group.GetChildAt(idx), Vector4(1,1,1,1));
      }
      ++idx;
    }

  ani.Play();

  mCurGroup = group;
  //mPrevHoveredActor = NULL;
}

void VRGalleryController::ActivateLayout(Mode mode, Actor selected, float duration)
{
  mMode = mode;
  if(mode==GROUP_VIEW)
  {
    ShowGroups(duration);
    RelayoutGroups(selected, duration);

    //RelayoutGroups(selected, duration);
  }
  else if(mode==THUMB_VIEW)
  {
    HideGroups(mCurGroup, duration);
    RelayoutGroups(mCurGroup, duration);

    //PushGroups(mCurGroup, duration);

    RelayoutThumbs(mCurGroup, selected, duration);
  }
  else if(mode==PICTURE_VIEW)
  {
    RelayoutPictures(selected, duration);
    mScrollPictureIdx = mPic2IndexInGroup[selected];
  }
}

void VRGalleryController::CreateImages()
{
  for(uint i=0; i<NUM_IMAGES; ++i)
    mImages.push_back( Image::New( IMAGE_PATHS[i] ));
}

void VRGalleryController::CreateGroups()
{
  for(uint g=0; g<NUM_GROUPS; ++g)
  {
    Actor group = Actor::New();
    //cout << group.GetId() << endl;
    group.SetParentOrigin(ParentOrigin::CENTER);
    for(uint i=0; i<NUM_GROUPITEMS; ++i)
    {
      Image image = mImages[(NUM_IMAGES/NUM_GROUPS)*g + i%(NUM_IMAGES/NUM_GROUPS)];
      ImageActor imageAct = ImageActor::New( image );
      imageAct.SetSize(VR_SCREEN_HEIGHT*(image.GetWidth()/image.GetHeight()), VR_SCREEN_HEIGHT);
      group.Add(imageAct);

      mPic2Group[imageAct] = group;
      mPic2IndexInGroup[imageAct] = i;

      //cout << imageAct.GetId() << " " << group.GetId() << endl;
      //cout << g << " " << i << " " << (NUM_IMAGES/NUM_GROUPS)*g + i%(NUM_IMAGES/NUM_GROUPS) << endl;

      imageAct.TouchedSignal().Connect(this, &VRGalleryController::OnTouch);
      imageAct.HoveredSignal().Connect(this, &VRGalleryController::OnHover);
    }
    mGroups.push_back(group);
    mGroup2Index[group] = g;

    Stage::GetCurrent().Add(group);
  }
}

void VRGalleryController::CreateCursor()
{
  mSelectInRadius = ImageActor::New( Image::New( DALI_IMAGE_DIR "com.samsung.dali-demo.png" ) );
  mSelectInRadius.SetSize(Vector3(2*SELECT_IN_RADIUS, 2*SELECT_IN_RADIUS, 0));
  mSelectInRadius.SetParentOrigin(ParentOrigin::CENTER);
  mSelectInRadius.SetSensitive(false);
  mSelectInRadius.SetOpacity(0.);
  Stage::GetCurrent().Add(mSelectInRadius);

  mSelectInRadius2 = ImageActor::New( Image::New( DALI_IMAGE_DIR "com.samsung.dali-demo.png" ) );
  mSelectInRadius2.SetSize(Vector3(2*SELECT_IN_RADIUS, 2*SELECT_IN_RADIUS, 0));
  mSelectInRadius2.SetParentOrigin(ParentOrigin::CENTER);
  mSelectInRadius2.SetSensitive(false);
  mSelectInRadius2.SetOpacity(0.);
  Stage::GetCurrent().Add(mSelectInRadius2);

  mDoSelectAni = Animation::New(.5);
  mDoSelectAni.OpacityTo(mSelectInRadius, 0.);
  mDoSelectAni.OpacityTo(mSelectInRadius2, 0.);
  mDoSelectAni.ScaleTo(mSelectInRadius2, 2.,2.,0);

  mDoCancelAni = Animation::New(.5);
  mDoCancelAni.OpacityTo(mSelectInRadius, .0);
  //mDoCancelAni.MoveBy(mSelectInRadius2, -4*SELECT_OUT_RADIUS,0,0);
  mDoCancelAni.OpacityTo(mSelectInRadius2, .0);
  mDoCancelAni.ScaleTo(mSelectInRadius2, 1.,1.,0);

  mRadiusAni = Animation::New(SELECT_OUT_DURATION);
  mRadiusAni.OpacityTo(mSelectInRadius, .0);
  mRadiusAni.OpacityTo(mSelectInRadius2, .0);
}

bool IsImageHittableFunction(Actor actor, Dali::HitTestAlgorithm::TraverseType type)
{
  bool hittable = false;

  switch (type)
  {
    case Dali::HitTestAlgorithm::CHECK_ACTOR:
    {
      // Check whether the actor is visible and not fully transparent.
      if( actor.IsVisible() && 
          actor.IsSensitive() &&
          actor.GetCurrentWorldColor().a > 0.01f) // not FULLY_TRANSPARENT
      {
        hittable = true;
      }
      break;
    }
    case Dali::HitTestAlgorithm::DESCEND_ACTOR_TREE:
    {
      if( actor.IsVisible() &&
          actor.IsSensitive()) // Actor is visible, if not visible then none of its children are visible.
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
  VRGalleryController* _this = static_cast<VRGalleryController*>(data);

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
    _this->mCBCount++;

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
      Vector3 w_yaxis_at_device = Mat3DotVec3(P_yaxis, w_at_device) * SENSOR_INTERVAL;

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

    // center camera (not used for rendering)
    CameraActor cameraActor = Stage::GetCurrent().GetRenderTaskList().GetTask(0).GetCameraActor();
    cameraActor.SetRotation(Quaternion());
    cameraActor.RotateBy(Q_world_to_device);
    cameraActor.RotateBy(basicCamRot);
    cameraActor.RotateBy(streoRot);

    ////////////////////////////////////////////
    // scroll item view

    //Vector2 stageSize = Stage::GetCurrent().GetSize();
    //Vector2 screenCenter(stageSize.x*.5, stageSize.y*.75);
    //float xOnItemView, yOnItemView;
    ///_this->mItemView.ScreenToLocal(xOnItemView, yOnItemView, screenCenter.x, screenCenter.y);
    //cout << xOnItemView << " " << yOnItemView << endl;

    Vector3 camPos = cameraActor.GetCurrentWorldPosition();
    Vector3 tarPos = Q_world_to_device * -camPos + camPos;

    // line through cmaPos & tarPos
    Vector3& p1 = camPos;
    Vector3& p2 = tarPos;
    Vector3 p_z0(0,0,0);
    p_z0.x = (p2.x - p1.x) * (-p1.z / (p2.z - p1.z));
    p_z0.y = (p2.y - p1.y) * (-p1.z / (p2.z - p1.z));

    if(p_z0.x < -VR_SCREEN_WIDTH/2 + SCROLL_MARGIN) // left scroll 
      _this->ScrollTo(p_z0.x - (-VR_SCREEN_WIDTH/2 + SCROLL_MARGIN));
    else if(p_z0.x > VR_SCREEN_WIDTH/2 - SCROLL_MARGIN) // right scroll
      _this->ScrollTo(p_z0.x - (VR_SCREEN_WIDTH/2 - SCROLL_MARGIN));

    //if(_this->mMode == THUMB_VIEW)
    //{
      //if(p_z0.x < -VR_SCREEN_WIDTH/2 + SCROLL_MARGIN) // left scroll 
      //{
        //float speed = (SENSOR_INTERVAL/.01)*(-VR_SCREEN_WIDTH/2 + SCROLL_MARGIN - p_z0.x) / SCROLL_MARGIN; 
        //_this->mItemId -= speed;
        //if(_this->mItemId<0) _this->mItemId=0;
      //}
      //else if(p_z0.x > VR_SCREEN_WIDTH/2 - SCROLL_MARGIN) // right scroll
      //{
        //float speed = (SENSOR_INTERVAL/.01)*(p_z0.x - (VR_SCREEN_WIDTH/2 - SCROLL_MARGIN)) / SCROLL_MARGIN; 
        //_this->mItemId += speed;
        //if(_this->mItemId>(int)_this->GetNumberOfItems()-1) _this->mItemId=(int)_this->GetNumberOfItems()-1;
      //}
      //_this->mItemView.ScrollToItem((int)_this->mItemId, 1.);
    //}

    // draw cross
    _this->mCross.SetPosition(p_z0+Vector3(0,0,.01));

    ////////////////////////////////////////////
    // hit-test
    Vector2 stageSize = Stage::GetCurrent().GetSize();
    Vector2 screenCenter(stageSize.x*.5, stageSize.y*.75);

    HitTestAlgorithm::Results hitImageResults, hitViewResults;
    HitTest( Stage::GetCurrent(), screenCenter, hitImageResults, IsImageHittableFunction );

    //RenderTask renderTask = Stage::GetCurrent().GetRenderTaskList().GetTask(0);
    //HitTest( renderTask, screenCenter, hitImageResults, IsImageHittableFunction );
    //HitTest( Stage::GetCurrent(), screenCenter, hitViewResults, IsViewHittableFunction );

    if(hitImageResults.actor) 
    {
      HoverEvent hevt;
      hevt.points.push_back(TouchPoint(0, TouchPoint::Motion, 0, 0));
      hitImageResults.actor.HoveredSignal().Emit(hitImageResults.actor, hevt);
    }

    ////////////////////////////////////////////
    // cursor moving check

    //if(hitImageResults.actor) 
    {
      int framePerCheck = 5;
      if(_this->mCBCount % framePerCheck == 0)
      {
        uint maxLen = MOVE_CHECK_DURATION / (framePerCheck*SENSOR_INTERVAL);
        bool moved = false;

        _this->mXYPosz.push_back(p_z0);
        if(_this->mXYPosz.size() > maxLen)
          _this->mXYPosz.pop_front();

        for(list<Vector3>::iterator it=_this->mXYPosz.begin(); it!=_this->mXYPosz.end(); ++it)
          if((*it - *_this->mXYPosz.begin()).Length() > MOVE_CHECK_RADIUS)
          {
            moved = true;
            break;
          }

        //cout << moved << endl;
        if(moved==false && _this->mSelecting==false)
        {
          if(hitImageResults.actor)
            _this->StartSelection(p_z0, hitImageResults.actor);
          else
            _this->StartSelection(p_z0, Actor::New());
        }
      }
    }

    ////////////////////////////////////////////
    // selection check
    if(_this->mSelecting) 
    {
      if(_this->mReachOutRadius==false && 
        _this->IsInSelectionInDuration()==false)
          _this->StopSelection();
      else if (_this->mReachOutRadius==true &&
              _this->IsInSelectionOutDuration()==false)
          _this->StopSelection();
      else
      {
        Vector3 start_to_farmost = _this->mFarmostPosFromStartPos-_this->mSelectStartPos;
        if(_this->mReachOutRadius == false)
        {
          if((p_z0-_this->mSelectStartPos).Length() > start_to_farmost.Length())
            _this->mFarmostPosFromStartPos = p_z0;
          if(start_to_farmost.Length() > SELECT_OUT_RADIUS)
          {
            _this->mReachOutRadius = true;
            _this->mSelectStartCBCount = _this->mCBCount;
            _this->ShowSelectInRadius();
            //cout << "reach out " << _this->mCBCount << endl;
          }
        }
        else
        {
          Vector3 start_to_curpos = p_z0 - _this->mSelectStartPos;
          if(start_to_curpos.Length() < SELECT_IN_RADIUS)
          {
            if(start_to_farmost.x > 0)  // right move
              _this->DoSelect(hitImageResults.actor);
            else    // left move
              _this->DoCancel(hitImageResults.actor);
            _this->StopSelection();
          }
        }
      }
    }

    ////////////////////////////////////////////
    _this->mGravity.w = 0;
    _this->mMagnet.w = 0;
    _this->mGyro.w = 0;

  }
}

bool VRGalleryController::IsInSelectionInDuration()
{
  if((mCBCount-mSelectStartCBCount) * SENSOR_INTERVAL > SELECT_IN_DURATION)
    return false;
  else
    return true;
}

bool VRGalleryController::IsInSelectionOutDuration()
{
  if((mCBCount-mSelectStartCBCount) * SENSOR_INTERVAL > SELECT_OUT_DURATION)
    return false;
  else
    return true;
}

#endif

void VRGalleryController::StartSelection(const Vector3& startPos, Actor startActor)
{
  mSelecting = true;
  mReachOutRadius = false;
  mSelectStartPos = startPos;
  mFarmostPosFromStartPos = startPos;
  mSelectStartCBCount = mCBCount;
  mSelectStartHitActor = startActor;
}

void VRGalleryController::ShowSelectInRadius()
{
  if(mDoSelectAni)
    mDoSelectAni.Stop();
  if(mDoCancelAni)
    mDoCancelAni.Stop();

  InitSelectInRadius();
  mRadiusAni.Play();
}

void VRGalleryController::InitSelectInRadius()
{
  mSelectInRadius.SetOpacity(1.);
  mSelectInRadius.SetPosition(mSelectStartPos + Vector3(0,0,.01));
  mSelectInRadius.SetColor(Vector4(1,1,1,RADIUS_ALPHA));
  mSelectInRadius.SetScale(1.);

  mSelectInRadius2.SetOpacity(1.);
  mSelectInRadius2.SetPosition(mSelectStartPos + Vector3(0,0,.02));
  mSelectInRadius2.SetColor(Vector4(1,1,1,RADIUS_ALPHA));
  mSelectInRadius2.SetScale(1.);
}

void VRGalleryController::StopSelection()
{
  mSelecting = false;
}

void VRGalleryController::DoSelect(Actor actor)
{
  if(mRadiusAni)
    mRadiusAni.Stop();
  if(mDoCancelAni)
    mDoCancelAni.Stop();

  InitSelectInRadius();
  mDoSelectAni.Play();

  TouchEvent evt;
  evt.points.push_back(TouchPoint(0, TouchPoint::Finished, 0, 0));
  mSelectStartHitActor.TouchedSignal().Emit(mSelectStartHitActor, evt);
}

void VRGalleryController::DoCancel(Actor actor)
{
  if(mRadiusAni)
    mRadiusAni.Stop();
  if(mDoSelectAni)
    mDoSelectAni.Stop();

  InitSelectInRadius();
  mSelectInRadius2.SetScale(2.);
  mDoCancelAni.Play();

  KeyEvent evt;
  evt.state = KeyEvent::Down;
  evt.keyCode = Dali::DALI_KEY_CANCEL;
  Stage::GetCurrent().KeyEventSignal().Emit(evt);
}

#ifdef SENSOR_ENABLED
void VRGalleryController::CreateSensors()
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

  for(uint i=0; i<mTypes.size(); ++i)
  {
    mType2Index[mTypes[i]] = i;
    sensor_get_default_sensor(mTypes[i], &mSensors[i]);
    sensor_create_listener(mSensors[i], &mListeners[i]);
    sensor_listener_set_event_cb(mListeners[i], SENSOR_INTERVAL*1000, sensor_cb2, this);
    sensor_listener_start(mListeners[i]);
    mSensor2Index[mSensors[i]] = i;
  }

  mGravity.w = 0;
  mMagnet.w = 0;

  mSelecting = false;
}
#endif

#ifdef SENSOR_ENABLED
void VRGalleryController::PrintSensorValues(sensor_type_e sensorType)
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
void VRGalleryController::PrintSensorValues2(sensor_h sensor, const sensor_event_s& event)
{
  string strSensor = stringf("%-27s", mType2String[mTypes[mSensor2Index[sensor]]].c_str());
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

void VRGalleryController::OnKeyEvent( const KeyEvent& event )
{
  if( event.state == KeyEvent::Down )
  {
    //cout << event.keyCode << endl;
    //cout << event.keyPressedName << endl;
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
    else if ( event.keyPressedName == "BackSpace" || event.keyCode == Dali::DALI_KEY_CANCEL)
    {
      if(mMode==GROUP_VIEW)
      {}
      else if(mMode==THUMB_VIEW)
        ActivateLayout(GROUP_VIEW, mCurGroup, LAYOUT_DURATION);
      else if(mMode==PICTURE_VIEW)
        ActivateLayout(THUMB_VIEW, mCurPicture, LAYOUT_DURATION);
    }
  }
}

void RunTest( Application& application )
{
  VRGalleryController test( application );

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
