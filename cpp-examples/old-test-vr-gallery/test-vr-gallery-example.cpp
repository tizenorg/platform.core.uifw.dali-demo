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
#include "test-vr-gallery-example.h"
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
const Vector3 INITIAL_OFFSCREEN_POSITION( 1000.0f, 0, -1000.0f );
const char* ITEM_BORDER_IMAGE_PATH( DALI_IMAGE_DIR "frame-128x128.png" );
const float ITEM_IMAGE_BORDER_LEFT   = 13.0f;
const float ITEM_IMAGE_BORDER_RIGHT  = 13.0f;
const float ITEM_IMAGE_BORDER_TOP    = 13.0f;
const float ITEM_IMAGE_BORDER_BOTTOM = 13.0f;
const Vector3 ITEM_BORDER_MARGIN_SIZE(24, 24, 0);

// TODO : change to sight angle value
const int VR_SCREEN_WIDTH = 4000;
const int VR_SCREEN_HEIGHT = 1000;
const int SCROLL_MARGIN = 1000;
const float MOVE_CHECK_DURATION = .3;
const float MOVE_CHECK_RADIUS = 20.;
const float SELECT_OUT_DURATION = .5;
const float SELECT_IN_DURATION = .3;
const float SELECT_OUT_RADIUS = 70.;
const float SELECT_IN_RADIUS = 35.;
const float RADIUS_ALPHA = .3;
const float SENSOR_INTERVAL = .01;
const float NUM_GROUPS = 4;
const float ROW_THUMBS = 4;
const float NUM_PICTURES = 5;

#define DRAW_SCROLL_MARGIN
#define DRAW_CENTER_LINE
#define DRAW_CROSS
//#define DRAW_CAMERA_RAY

struct BorderSizeConstraintFunction
{
  Vector3 operator()(const Vector3&       current,
                     const PropertyInput& parentSize)
  {
    return parentSize.GetVector3() + ITEM_BORDER_MARGIN_SIZE;
  }
};

VRGalleryController::VRGalleryController( Application& application )
: mApplication( application ), mInit(false)
{
  std::cout << "VRGalleryController::VRGalleryController" << std::endl;

  // Connect to the Application's Init signal
  mApplication.InitSignal().Connect( this, &VRGalleryController::Create );
}

#ifdef AUDIO_ENABLED 
//void audio_in_cb(audio_in_h handle, size_t nbytes, void *user_data)
//{
  //cout << nbytes << endl;
//}
void audio_in_cb(audio_io_interrupted_code_e code, void *user_data)
{
  cout << code << endl;
}
#endif

  // The Init signal is received once (only) during the Application lifetime
void VRGalleryController::Create( Application& application )
{
  std::cout << "VRGalleryController::Create" << std::endl;

  mMode = ITEM_VIEW;

  // key handler
  Stage::GetCurrent().KeyEventSignal().Connect( this, &VRGalleryController::OnKeyEvent );

  // Respond to a click anywhere on the stage
  Stage stage = Stage::GetCurrent();
  stage.GetRootLayer().TouchedSignal().Connect( this, &VRGalleryController::OnTouch );

  CameraActor cameraActor = Stage::GetCurrent().GetRenderTaskList().GetTask(0).GetCameraActor();

  //// image actor border
  //mBorderImage = Image::New(ITEM_BORDER_IMAGE_PATH);

  // TODO : repeat=3, then side text watched as not "doubled" object
  //int stepWidth = 300;
  //int repeat = 12;

  //cameraActor.SetPosition(Vector3(0,0,1500));
  //Vector3 camPos = cameraActor.GetCurrentPosition();
  //Vector3 camPos(0,0,2560);

  //Actor frontText = CreateTextGroup("Front", stepWidth, repeat);

  //Actor leftText = CreateTextGroup("Left", stepWidth, repeat);
  //leftText.RotateBy(Degree(90), Vector3(0,1,0));
  //leftText.SetPosition(Vector3(-camPos.z,0,camPos.z));

  //Actor rightText = CreateTextGroup("Right", stepWidth, repeat);
  //rightText.RotateBy(Degree(-90), Vector3(0,1,0));
  //rightText.SetPosition(Vector3(camPos.z,0,camPos.z)); 

  //Actor backText = CreateTextGroup("Back", stepWidth, repeat);
  //backText.RotateBy(Degree(180), Vector3(0,1,0));
  //backText.SetPosition(Vector3(0,0,2*camPos.z));

  //stage.Add(frontText);
  //stage.Add(leftText);
  //stage.Add(rightText);
  //stage.Add(backText);

#ifdef SENSOR_ENABLED
  CreateSensors();
#endif

#ifdef AUDIO_ENABLED
  audio_in_create(8000, AUDIO_CHANNEL_MONO, AUDIO_SAMPLE_TYPE_U8, &mAudioIn); 
  audio_in_prepare(mAudioIn);

  int size;
  audio_in_get_buffer_size(mAudioIn, &size);
  cout << size << endl;

  int rate;
  audio_in_get_sample_rate(mAudioIn, &rate);
  cout << rate << endl;

  audio_channel_e channel;
  audio_in_get_channel(mAudioIn, &channel);
  cout << channel << endl;

  audio_sample_type_e type;
  audio_in_get_sample_type(mAudioIn, &type);
  cout << type << endl;


  //int ret = audio_in_set_stream_cb(mAudioIn, audio_in_cb, this);
  //int ret = audio_in_set_interrupted_cb(mAudioIn, audio_in_cb, this);

  //cout << ret << endl;
  //cout << "-----------" << endl;

  //mBuffer = new char[4096];

#endif

  CreateItemView();

  //Actor coord = CreateCoordAxesText(100);
  //stage.Add(coord);

#ifdef DRAW_SCROLL_MARGIN
  Vector4 marginColor(1,1,1,.5);
  Actor margin_l = CreateVerticalLine(Vector3(-VR_SCREEN_WIDTH/2+SCROLL_MARGIN, 0, -.01), marginColor, 1020, 3);
  Actor margin_r = CreateVerticalLine(Vector3(VR_SCREEN_WIDTH/2-SCROLL_MARGIN, 0, -.01), marginColor, 1020, 3);
  stage.Add(margin_l);
  stage.Add(margin_r);
#endif

#ifdef DRAW_CENTER_LINE
  Vector4 centerLineColor(1,1,1,1);
  Actor centerLine = CreateVerticalLine(Vector3(0, 0, -.01), centerLineColor, 1020, 3);
  stage.Add(centerLine);
#endif

#ifdef DRAW_CROSS
  // TODO : draw UI widgets in same position in both screen
  Vector4 crossColor(1,0,0,.5);

  mCross = CreateCross(Vector3(0, 0, 2560-.01), crossColor, 50);
  stage.Add(mCross);

  mCross.SetParentOrigin(ParentOrigin::CENTER);
  mCross.SetSensitive(false);
#endif

#ifdef DRAW_CAMERA_RAY
  Actor ray = CreateTextLine(Vector3(0,0,0), Vector3(0,0,3000), Vector4(0,1,0,1));
  cameraActor.Add(ray);
#endif

  mSelectInRadius = ImageActor::New( Image::New( DALI_IMAGE_DIR "com.samsung.dali-demo.png" ) );
  mSelectInRadius.SetSize(Vector3(2*SELECT_IN_RADIUS, 2*SELECT_IN_RADIUS, 0));
  mSelectInRadius.SetParentOrigin(ParentOrigin::CENTER);
  mSelectInRadius.SetSensitive(false);
  mSelectInRadius.SetOpacity(0.);
  stage.Add(mSelectInRadius);

  mSelectInRadius2 = ImageActor::New( Image::New( DALI_IMAGE_DIR "com.samsung.dali-demo.png" ) );
  mSelectInRadius2.SetSize(Vector3(2*SELECT_IN_RADIUS, 2*SELECT_IN_RADIUS, 0));
  mSelectInRadius2.SetParentOrigin(ParentOrigin::CENTER);
  mSelectInRadius2.SetSensitive(false);
  mSelectInRadius2.SetOpacity(0.);
  stage.Add(mSelectInRadius2);

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

  mCBCount = 0;

  mItemId = 0;
  mItemView.ScrollToItem(mItemId, 0);

  //mPictureRoot = Actor::New();
  //mPictureRoot.SetPosition(Vector3(0,VR_SCREEN_HEIGHT,-VR_SCREEN_HEIGHT/2));
  //mPictureRoot.SetParentOrigin(ParentOrigin::CENTER);
  //mPictureRoot.SetOpacity(0.);
  //stage.Add(mPictureRoot);

  //for(int i=0; i<NUM_PICTURES; ++i)
  //{
    //mPictures[i] = ImageActor::New();
    //mPictures[i].SetSize(VR_SCREEN_HEIGHT, VR_SCREEN_HEIGHT);
    //mPictures[i].SetParentOrigin(ParentOrigin::CENTER);
    //mPictureRoot.Add(mPictures[i]);
  //}

  mCurrImage = ImageActor::New();
  mCurrImage.SetSize(VR_SCREEN_WIDTH/4, VR_SCREEN_HEIGHT);
  mCurrImage.SetPosition(Vector3(0,VR_SCREEN_HEIGHT,-VR_SCREEN_HEIGHT/2));
  mCurrImage.SetParentOrigin(ParentOrigin::CENTER);
  mCurrImage.SetOpacity(0.);
  stage.Add(mCurrImage);

  mPrevImage = ImageActor::New();
  mPrevImage.SetSize(mCurrImage.GetSize());
  mPrevImage.SetPosition(Vector3(-VR_SCREEN_HEIGHT/2, 0, -VR_SCREEN_HEIGHT/4));
  mPrevImage.SetParentOrigin(ParentOrigin::CENTER);
  mPrevImage.SetOpacity(.75);
  mCurrImage.Add(mPrevImage);

  mNextImage = ImageActor::New();
  mNextImage.SetSize(mCurrImage.GetSize());
  mNextImage.SetPosition(Vector3(VR_SCREEN_HEIGHT/2, 0, -VR_SCREEN_HEIGHT/4));
  mNextImage.SetParentOrigin(ParentOrigin::CENTER);
  mNextImage.SetOpacity(.75);
  mCurrImage.Add(mNextImage);

  mPrevImage2 = ImageActor::New();
  mPrevImage2.SetSize(mCurrImage.GetSize());
  mPrevImage2.SetPosition(Vector3(-VR_SCREEN_HEIGHT, 0, -VR_SCREEN_HEIGHT/2));
  mPrevImage2.SetParentOrigin(ParentOrigin::CENTER);
  mPrevImage2.SetOpacity(.5);
  mCurrImage.Add(mPrevImage2);

  mNextImage2 = ImageActor::New();
  mNextImage2.SetSize(mCurrImage.GetSize());
  mNextImage2.SetPosition(Vector3(VR_SCREEN_HEIGHT, 0, -VR_SCREEN_HEIGHT/2));
  mNextImage2.SetParentOrigin(ParentOrigin::CENTER);
  mNextImage2.SetOpacity(.5);
  mCurrImage.Add(mNextImage2);

  mEnterLargeImageAni = Animation::New(.5);
  mEnterLargeImageAni.MoveBy(mItemView, 0,-VR_SCREEN_HEIGHT,-VR_SCREEN_HEIGHT/2);
  mEnterLargeImageAni.OpacityTo(mItemView, 0.);
  mEnterLargeImageAni.MoveBy(mCurrImage, 0,-VR_SCREEN_HEIGHT,VR_SCREEN_HEIGHT/2);
  mEnterLargeImageAni.OpacityTo(mCurrImage, 1.);

  mBackToItemViewAni = Animation::New(.5);
  mBackToItemViewAni.MoveBy(mItemView, 0,VR_SCREEN_HEIGHT,VR_SCREEN_HEIGHT/2);
  mBackToItemViewAni.OpacityTo(mItemView, 1.);
  mBackToItemViewAni.MoveBy(mCurrImage, 0,VR_SCREEN_HEIGHT,-VR_SCREEN_HEIGHT/2);
  mBackToItemViewAni.OpacityTo(mCurrImage, 0.);

  // streo 3d
  application.SetViewMode(STEREO_HORIZONTAL);
}




void VRGalleryController::CreateItemView()
{
  Stage stage = Stage::GetCurrent();

  mItemView = ItemView::New(*this);
  mItemView.SetName("ItemView");
  mItemView.SetParentOrigin(ParentOrigin::CENTER);
  mItemView.SetAnchorPoint(AnchorPoint::CENTER);
  //mItemView.RotateBy(Degree(-90), Vector3(0,0,1));
  //mItemView.EnableScrollComponent(Scrollable::HorizontalScrollBar);
  //mItemView.DisableScrollComponent(Scrollable::HorizontalScrollBar);
  //mItemView.DisableScrollComponent(Scrollable::VerticalScrollBar);
  stage.Add( mItemView );

  mGridLayout = GridLayout::New();
  mItemView.AddLayout(*mGridLayout);
  //mRollLayout = RollLayout::New();
  //mItemView.AddLayout(*mRollLayout);
  UseLayout(0, 0.f);

  //mItemView.HoveredSignal().Connect(this, &VRGalleryController::OnHoverItemView);
}

void VRGalleryController::UseLayout(int layoutId, float duration)
{
  int vrScreenWidth = VR_SCREEN_WIDTH;
  int vrScreenHeight = VR_SCREEN_WIDTH/4;
  mItemView.GetLayout(layoutId)->SetOrientation(ControlOrientation::Left);
  mItemView.ActivateLayout(layoutId, Vector3(vrScreenWidth, vrScreenHeight, vrScreenWidth), duration);



  //uint mOrientation = 0;

  // Set the new orientation to the layout
  //mItemView.GetLayout(layoutId)->SetOrientation(static_cast<ControlOrientation::Type>(mOrientation / 90));
  //mItemView.GetLayout(layoutId)->SetOrientation(ControlOrientation::Right);
  //mItemView.GetLayout(layoutId)->SetOrientation(ControlOrientation::Down);

  //mItemView.GetLayout(layoutId)->SetOrientation(ControlOrientation::Up);

  //Vector2 stageSize = Stage::GetCurrent().GetSize();

  //if(layoutId == SPIRAL_LAYOUT)
  //{
    //mSpiralLayout->SetRevolutionDistance(stageSize.height / Stage::GetCurrent().GetDpi().y * 45.0f);
  //}

  //if(layoutId == GRID_LAYOUT)
  //{
    //mGridLayout->SetNumberOfColumns(mGridLayout->GetNumberOfColumns()*5);

    //// Set up the grid layout according to the new orientation
    //float layoutWidth = Toolkit::IsHorizontal(mGridLayout->GetOrientation()) ? stageSize.height : stageSize.width;
    //float gridItemSize = (layoutWidth / mGridLayout->GetNumberOfColumns()) * 0.5f;
    //mGridLayout->SetScrollSpeedFactor(mGridLayout->GetNumberOfColumns() / gridItemSize);

    //float toolbarHeight = mToolBar.GetCurrentSize().y;
    //mGridLayout->SetTopMargin(toolbarHeight + mGridLayout->GetRowSpacing());
  //}

  //if(layoutId == DEPTH_LAYOUT)
  //{
    //// Set up the depth layout according to the new orientation
    //if(Toolkit::IsVertical(mDepthLayout->GetOrientation()))
    //{
      //mDepthLayout->SetRowSpacing(stageSize.height * DEPTH_LAYOUT_ROW_SPACING_FACTOR);
      //mDepthLayout->SetNumberOfRows(DEPTH_LAYOUT_ROWS_PORTRAIT);
      //mDepthLayout->SetTiltAngle( Degree( DEPTH_LAYOUT_TILT_ANGLE_PORTRAIT - mDepthLayout->GetNumberOfColumns() ) );
      //mDepthLayout->SetItemSizeFunction(DepthLayoutItemSizeFunctionPortrait);
      //mDepthLayout->SetBottomMarginFunction(DepthLayoutBottomMarginFunctionPortrait);
    //}
    //else
    //{
      //mDepthLayout->SetRowSpacing(stageSize.width * DEPTH_LAYOUT_ROW_SPACING_FACTOR);
      //mDepthLayout->SetNumberOfRows(DEPTH_LAYOUT_ROWS_LANDSCAPE);
      //mDepthLayout->SetTiltAngle( Degree( DEPTH_LAYOUT_TILT_ANGLE_LANDSCAPE - mDepthLayout->GetNumberOfColumns() ) );
      //mDepthLayout->SetItemSizeFunction(DepthLayoutItemSizeFunctionLandscape);
      //mDepthLayout->SetBottomMarginFunction(DepthLayoutBottomMarginFunctionLandscape);
    //}
  //}

  //// Enable anchoring for depth layout only
  //mItemView.SetAnchoring(layoutId == DEPTH_LAYOUT);

  // Activate the layout
  //mItemView.ActivateLayout(layoutId, Vector3(stageSize.x, stageSize.y, stageSize.x), duration);
  //mItemView.ActivateLayout(layoutId, Vector3(stageSize.x*10, stageSize.y, stageSize.x), duration);
}

Actor VRGalleryController::CreateTextGroup( const string& groupName, int stepWidth, int repeat )
{
  Actor actor = Actor::New();
  actor.SetParentOrigin( ParentOrigin::CENTER );
  for(int i=0; i<repeat; ++i)
    for(int j=0; j<repeat; ++j)
    {
      Vector4 color(1,1,1,1);
      if(i==repeat/2 && j==repeat/2)
        color = Vector4(1,0,0,1);
      else if(i==repeat/2)
        color = Vector4(0,1,0,1);
      else if(j==repeat/2)
        color = Vector4(0,0,1,1);
      TextActor baseText = CreateText(groupName, Vector3(-.5*(repeat-1)*stepWidth+i*stepWidth, -.5*(repeat-1)*stepWidth+j*stepWidth,0), color);
      //TextActor baseText = CreateText(groupName, Vector3(0,0,0), color);
      actor.Add(baseText);
    }
  return actor;
}

TextActor VRGalleryController::CreateText( const string& name, const Vector3& position, const Vector4& color )
{
  ////////////////////
  // ys

  // Initialize the actor
  TextActor baseText = TextActor::New(name);
  baseText.MoveBy(position);
  baseText.SetColor(color);

  // Reposition the actor
  baseText.SetParentOrigin( ParentOrigin::CENTER );

  // camera
  CameraActor cameraActor = Stage::GetCurrent().GetRenderTaskList().GetTask(0).GetCameraActor();
  //cameraActor.SetPosition(Vector3(0,0,1000));


  // more text
  TextActor nearText = TextActor::New("Near");
  TextActor farText = TextActor::New("Far");
  //TextActor rotText = TextActor::New("Rot");
  nearText.SetParentOrigin(ParentOrigin::CENTER);
  farText.SetParentOrigin(ParentOrigin::CENTER);
  //rotText.SetParentOrigin(ParentOrigin::CENTER);
  baseText.Add(nearText);
  baseText.Add(farText);
  //baseText.Add(rotText);

  nearText.MoveBy(Vector3(0,50,50));
  farText.MoveBy(Vector3(0,-50,-50));

  //// animation
  //Animation ani1;
  //ani1 = Animation::New(4);
  //ani1.RotateBy(baseText, Degree(360), Vector3(0,1,0));
  //ani1.SetEndAction(Animation::Bake);
  //ani1.SetLooping(true);
  //ani1.Play();

  return baseText;
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

bool IsViewHittableFunction(Actor actor, Dali::HitTestAlgorithm::TraverseType type)
{
  bool hittable = false;

  switch (type)
  {
    case Dali::HitTestAlgorithm::CHECK_ACTOR:
    {
      // Check whether the actor is visible and not fully transparent.
      if(actor.GetName()=="ItemView")
      {
        hittable = true;
      }
      break;
    }
    case Dali::HitTestAlgorithm::DESCEND_ACTOR_TREE:
    {
      if(actor.GetName()=="ItemView")
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
void sensor_cb(sensor_h sensor, sensor_event_s* event, void *data)
{
  VRGalleryController* _this = static_cast<VRGalleryController*>(data);

  if(_this->mTypes[_this->mSensor2Index[sensor]] == SENSOR_GRAVITY)
  {
    //_this->PrintSensorValues2(sensor, *event);

    Vector3 defaultGravity(-1,0,0);
    Vector3 currentGravity(event->values[0],-event->values[1],event->values[2]);

    // device coord -> world coord
    // in steroe mode,
    // +x -> +y
    // +y -> -x
    double temp_0 = defaultGravity[0];
    defaultGravity[0] = -defaultGravity[1];
    defaultGravity[1] = temp_0;
    temp_0 = currentGravity[0];
    currentGravity[0] = -currentGravity[1];
    currentGravity[1] = temp_0;

    currentGravity.Normalize();
    Vector3 rotVec = GetRotVecBtwnTwoVecs(currentGravity, defaultGravity);
    double angle = rotVec.Length();
    rotVec.Normalize();

    Quaternion basicCamRot(Radian(Degree(-180)), Vector3(0,1,0));
    Quaternion gravRot(angle, rotVec);

    //cout << Degree(Radian(angle)) << endl;

    CameraActor cameraActor = Stage::GetCurrent().GetRenderTaskList().GetTask(0).GetCameraActor();
    cameraActor.SetRotation(Quaternion());
    cameraActor.RotateBy(gravRot);
    cameraActor.RotateBy(basicCamRot);
  }
  else if(_this->mTypes[_this->mSensor2Index[sensor]] == SENSOR_MAGNETIC)
  {
    //_this->PrintSensorValues2(sensor, *event);
    Vector3 currentMagnet(event->values[0],event->values[1],event->values[2]);
    currentMagnet.Normalize();
    cout << currentMagnet << endl;
  }
}
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

    if(_this->mMode == ITEM_VIEW)
    {
      if(p_z0.x < -VR_SCREEN_WIDTH/2 + SCROLL_MARGIN) // left scroll 
      {
        float speed = (SENSOR_INTERVAL/.01)*(-VR_SCREEN_WIDTH/2 + SCROLL_MARGIN - p_z0.x) / SCROLL_MARGIN; 
        _this->mItemId -= speed;
        if(_this->mItemId<0) _this->mItemId=0;
      }
      else if(p_z0.x > VR_SCREEN_WIDTH/2 - SCROLL_MARGIN) // right scroll
      {
        float speed = (SENSOR_INTERVAL/.01)*(p_z0.x - (VR_SCREEN_WIDTH/2 - SCROLL_MARGIN)) / SCROLL_MARGIN; 
        _this->mItemId += speed;
        if(_this->mItemId>(int)_this->GetNumberOfItems()-1) _this->mItemId=(int)_this->GetNumberOfItems()-1;
      }
      _this->mItemView.ScrollToItem((int)_this->mItemId, 1.);
    }

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

    if(_this->mMode==ITEM_VIEW)
    {
      if(hitImageResults.actor!=_this->mLastHitImage)
      {
        if(hitImageResults.actor) 
        {
          HoverEvent hevt;
          hevt.points.push_back(TouchPoint(0, TouchPoint::Motion, 0, 0));
          hitImageResults.actor.HoveredSignal().Emit(hitImageResults.actor, hevt);
        }
        if(_this->mLastHitImage)
        { 
          HoverEvent hevt;
          hevt.points.push_back(TouchPoint(0, TouchPoint::Leave, 0, 0));
          _this->mLastHitImage.HoveredSignal().Emit(_this->mLastHitImage, hevt);
        }
        _this->mLastHitImage = hitImageResults.actor;
      }
      //if(hitViewResults.actor)
      //{
        //HoverEvent hevt;
        //hevt.points.push_back(TouchPoint(0, TouchPoint::Motion, 0, 0));
        //hitViewResults.actor.HoveredSignal().Emit(hitViewResults.actor, hevt);
      //}
    }

    ////////////////////////////////////////////
    // cursor moving check
    _this->mCBCount++;
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


        //if(_this->mSelectMinX > p_z0.x)
          //_this->mSelectMinX = p_z0.x;
        //else if(_this->mSelectMaxX < p_z0.x)
          //_this->mSelectMaxX = p_z0.x;

        //if(_this->mSelectStartHitActor == hitImageResults.actor)
        //{
          //if(p_z0.x - _this->mSelectMinX > SELECT_RADIUS)
          //{
            //_this->DoCancel(hitImageResults.actor);
            //_this->StopSelection();
          //}
          //else if(_this->mSelectMaxX - p_z0.x > SELECT_RADIUS)
          //{
            //_this->DoSelect(hitImageResults.actor);
            //_this->StopSelection();
          //}
        //}
      //}

      
      //int framePerCheck = 5;
      //float checkDuration = .5;
      //uint maxLen = checkDuration / (framePerCheck*_this->mSensorInterval);
      //bool moved = false;
      //float radius = 100;
      //if(_this->mCBCount % framePerCheck == 0)
      //{
        //_this->mXYPosz.push_back(p_z0);
        //if(_this->mXYPosz.size() > maxLen)
          //_this->mXYPosz.pop_front();

        //for(list<Vector3>::iterator it=_this->mXYPosz.begin(); it!=_this->mXYPosz.end(); ++it)
          //if((*it - *_this->mXYPosz.begin()).Length() > radius)
          //{
            //moved = true;
            //break;
          //}
        ////cout << moved << endl;
      //}
      //_this->mCBCount++;

      ////if(moved==false)
        ////_this->ShowSelectionInterface(true, *_this->mXYPosz.end());
      ////else
        ////_this->ShowSelectionInterface(false, *_this->mXYPosz.end());
    //}

    ////////////////////////////////////////////
    _this->mGravity.w = 0;
    _this->mMagnet.w = 0;
    _this->mGyro.w = 0;

  }

  //Vector2 stageSize = Stage::GetCurrent().GetSize();
  //float originOffsetX, originOffsetY;
  //_this->mItemView.ScreenToLocal(originOffsetX, originOffsetY, stageSize.width * 0.5f, stageSize.height * 0.5f);
  //cout << originOffsetX-VR_SCREEN_WIDTH/2-600 << " " << originOffsetY-VR_SCREEN_HEIGHT/2 << endl;

  //_this->PrintSensorValues(SENSOR_ACCELEROMETER);
  //PrintSensorValues(SENSOR_LINEAR_ACCELERATION);
  //_this->PrintSensorValues(SENSOR_GRAVITY);
  //_this->PrintSensorValues(SENSOR_MAGNETIC);
  //PrintSensorValues(SENSOR_ROTATION_VECTOR);
  //PrintSensorValues(SENSOR_ORIENTATION);
  //_this->PrintSensorValues(SENSOR_GYROSCOPE);
  //PrintSensorValues(SENSOR_LIGHT);
  //PrintSensorValues(SENSOR_PROXIMITY);
  //PrintSensorValues(SENSOR_PRESSURE);
  //cout << endl;

//#ifdef AUDIO_ENABLED
  //int ret = audio_in_read(_this->mAudioIn, _this->mBuffer, 128);
  //cout << ret << endl;
  //cout << "-----------" << endl;
//#endif

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
  //cout << "start selection" <<  " " << mCBCount << endl;
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
  //ShowSelectInRadius(false);
  //cout << "stop selection" << " " << mCBCount << endl;
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

  //cout << "SELECT" << endl;
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

bool VRGalleryController::OnTouchImage( Actor actor, const TouchEvent& event )
{
  if( event.GetPointCount() > 0 )
  {
    const TouchPoint& tp = event.GetPoint(0);
    if(tp.state==TouchPoint::Finished)
    {
      mEnterLargeImageAni.Play();

      mCurrImage.SetImage(ImageActor::DownCast(actor).GetImage());

      uint curr_id = mItemView.GetItemId(actor);
      if(curr_id > 1)
        mPrevImage.SetImage(ImageActor::DownCast(mItemView.GetItem(curr_id-1)).GetImage());
      if(curr_id > 2)
        mPrevImage2.SetImage(ImageActor::DownCast(mItemView.GetItem(curr_id-2)).GetImage());
      if(curr_id < GetNumberOfItems()-1)
        mNextImage.SetImage(ImageActor::DownCast(mItemView.GetItem(curr_id+1)).GetImage());
      if(curr_id < GetNumberOfItems()-2)
        mNextImage2.SetImage(ImageActor::DownCast(mItemView.GetItem(curr_id+2)).GetImage());

      mMode = IMAGE_VIEW;
    }
  }
  return true;
}

bool VRGalleryController::OnHoverImage( Actor actor, const HoverEvent& event )
{
  //cout << "TouchPoint::Started " << TouchPoint::Started << endl;
  //cout << "TouchPoint::Finished " << TouchPoint::Finished << endl;
  //cout << "TouchPoint::Down " << TouchPoint::Down << endl;
  //cout << "TouchPoint::Up " << TouchPoint::Up << endl;
  //cout << "TouchPoint::Motion " << TouchPoint::Motion << endl;
  //cout << "TouchPoint::Leave " << TouchPoint::Leave << endl;
  //cout << "TouchPoint::Stationary " << TouchPoint::Stationary << endl;
  //cout << "TouchPoint::Interrupted " << TouchPoint::Interrupted << endl;
  //cout << "TouchPoint::Last " << TouchPoint::Last << endl;

  //cout << "hover " << actor.GetId() << endl;
  //cout << "state " << touch.GetPoint(0).state << endl;
  //cout << endl;

  //const TouchPoint& p = event.GetPoint(0);
  //cout << "state " << p.state << endl;
  //cout << "hitActor " << p.hitActor.GetName() << endl;
  //cout << "local " << p.local << endl;
  //cout << "screen " << p.screen << endl;
  //cout << endl;

  //float xOnItemView, yOnItemView;
  //mItemView.ScreenToLocal(xOnItemView, yOnItemView, p.screen.x, p.screen.y);
  //cout << xOnItemView << " " << yOnItemView << endl;

  if( event.GetPointCount() > 0 )
  {
    const TouchPoint& tp = event.GetPoint(0);
    if(tp.state==TouchPoint::Motion)
    {
      //actor.FindChildByName("border").SetVisible(true);
      actor.SetScale(1.15);
    }
    else if(tp.state==TouchPoint::Leave)
    {
      //actor.FindChildByName("border").SetVisible(false);
      actor.SetScale(1.);
    }
  }
  return true;
}

//bool VRGalleryController::OnHoverItemView( Actor actor, const HoverEvent& event )
//{
  //if( event.GetPointCount() > 0 && actor )
  //{
    //const TouchPoint& p = event.GetPoint(0);
    ////cout << "state " << p.state << endl;
    //////cout << "hitActor " << p.hitActor.GetName() << endl;
    ////cout << "local " << p.local << endl;
    ////cout << "screen " << p.screen << endl;

    //float xOnItemView = p.local.x;
    //cout << p.local.x << endl;

    //if(xOnItemView < SCROLL_MARGIN) // left scroll 
    //{
      //float speed = (SCROLL_MARGIN - xOnItemView)/SCROLL_MARGIN; 
      //mItemId -= speed;
      //if(mItemId<0) mItemId=0;
      //mItemView.ScrollToItem((int)mItemId, 1.);
    //}
    //else if(xOnItemView > mItemView.GetSize().x - SCROLL_MARGIN) // right scroll
    //{
      //float speed = (xOnItemView - (mItemView.GetSize().x - SCROLL_MARGIN)) / SCROLL_MARGIN; 
      //mItemId += speed;
      //if(mItemId>(int)GetNumberOfItems()-1) mItemId=(int)GetNumberOfItems()-1;
      //mItemView.ScrollToItem((int)mItemId, 1.);
    //}
  //}

  //return true;
//}

bool VRGalleryController::OnTouch( Actor actor, const TouchEvent& touch )
{
#ifdef AUDIO_ENABLED
  const char buffer[512];
  int ret = audio_in_peek(mAudioIn, &buffer, 512);
  cout << ret << endl;
  cout << "-----------" << endl;
  for(int i=0; i<512; ++i)
    cout << << buffer[i] << " ";
  cout << endl;
#endif

  //UseLayout(0, 1.f);

  //CameraActor cameraActor = Stage::GetCurrent().GetRenderTaskList().GetTask(0).GetCameraActor();
  //cout << "camera " << cameraActor.GetCurrentPosition() << endl;

#ifdef SENSOR_ENABLED
  //PrintSensorValues(SENSOR_ACCELEROMETER);
  //PrintSensorValues(SENSOR_LINEAR_ACCELERATION);
  //PrintSensorValues(SENSOR_GRAVITY);
  //PrintSensorValues(SENSOR_MAGNETIC);

  //PrintSensorValues(SENSOR_ROTATION_VECTOR);
  //PrintSensorValues(SENSOR_ORIENTATION);
  //PrintSensorValues(SENSOR_GYROSCOPE);

  //PrintSensorValues(SENSOR_LIGHT);
  //PrintSensorValues(SENSOR_PROXIMITY);
  //PrintSensorValues(SENSOR_PRESSURE);
  //cout << endl;
#endif

  // quit the application
  //mApplication.Quit();
  return true;
}
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
      {
      }
      else if(mMode==ITEM_VIEW)
      {
      }
      else if(mMode==IMAGE_VIEW)
      {
        mBackToItemViewAni.Play();
        mMode = ITEM_VIEW;
      }
    }
  }
}

unsigned int VRGalleryController::GetNumberOfItems()
{
  return NUM_IMAGES * 10;
}



Actor VRGalleryController::NewItem(unsigned int itemId)
{
  // Create an image actor for this item
  Image image = Image::New( IMAGE_PATHS[itemId % NUM_IMAGES] );
  ImageActor actor = ImageActor::New(image);
  actor.SetPosition( INITIAL_OFFSCREEN_POSITION );

  //ImageActor borderActor = ImageActor::New(mBorderImage);
  //borderActor.SetName("border");
  //borderActor.SetParentOrigin( ParentOrigin::CENTER );
  //borderActor.SetAnchorPoint( AnchorPoint::CENTER );
  //borderActor.SetPosition( 0.f, 0.f, 1.f );
  //borderActor.SetStyle( ImageActor::STYLE_NINE_PATCH );
  //borderActor.SetNinePatchBorder( Vector4( ITEM_IMAGE_BORDER_LEFT, ITEM_IMAGE_BORDER_TOP, ITEM_IMAGE_BORDER_RIGHT, ITEM_IMAGE_BORDER_BOTTOM ) );
  //borderActor.SetColorMode( USE_OWN_MULTIPLY_PARENT_COLOR ); // darken with parent image-actor
  //borderActor.SetVisible(false);

  //Constraint constraint = Constraint::New<Vector3>( Actor::SIZE, ParentSource( Actor::SIZE ), BorderSizeConstraintFunction() );
  //borderActor.ApplyConstraint(constraint);
  //actor.Add(borderActor);

  actor.TouchedSignal().Connect(this, &VRGalleryController::OnTouchImage);
  actor.HoveredSignal().Connect(this, &VRGalleryController::OnHoverImage);

  return actor;
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
