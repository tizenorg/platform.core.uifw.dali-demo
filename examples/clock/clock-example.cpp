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

#include <dali/dali.h>

using namespace Dali;

namespace
{

  const unsigned int FAR_PLANE = 20.0f;
  const float FIELD_OF_VIEW_RADIANS = Math::PI / 4.0f; // 45 degrees
  const float FRUSTUM_HEIGHT = 1.656854249; // tan(45/2) * 2 * 2
  const Vector3 CAMERA_POSITION = Vector3( 0.0f, 0.0f, 2.5f );

  // Clock Textures
  const char* CLOCK_BACKGROUND_IMAGE(  DALI_IMAGE_DIR "BG_H.png" );
  const char* CLOCK_HOUR_HAND_IMAGE(  DALI_IMAGE_DIR "Clock_hands_hour_01.png" );
  const char* CLOCK_MINUTE_HAND_IMAGE(  DALI_IMAGE_DIR "Clock_hands_min_01.png" );
  const char* CLOCK_SECOND_HAND_IMAGE(  DALI_IMAGE_DIR "Clock_hands_sec.png" );
  const char* CLOCK_PIVOT_IMAGE(  DALI_IMAGE_DIR "Clock_hands_01.png" );
  const char* CLOCK_T1_IMAGE(  DALI_IMAGE_DIR "Info_T1.png" );
  const char* CLOCK_T2_IMAGE(  DALI_IMAGE_DIR "Info_T2.png" );
  const char* CLOCK_CHROME_IMAGE( DALI_IMAGE_DIR "Info_chrom_H.png" );
  const char* CLOCK_INFO_HOUR_IMAGE( DALI_IMAGE_DIR "Info_Info_hour.png" );
  const char* CLOCK_INFO_MIN_IMAGE( DALI_IMAGE_DIR "Info_Info_min.png" );
  const char* CLOCK_BATTERY_METER_IMAGE( DALI_IMAGE_DIR "batdial.png" );
  const char* CLOCK_DATE_IMAGE( DALI_IMAGE_DIR "23.png" );
  const char* CLOCK_WEATHER_IMAGE( DALI_IMAGE_DIR "sun.png" );

  // Clock Component positions
  const Vector3 BACKGROUND_POSITION( 0.0f, 0.0f, -2.1f );
  const Vector3 SECOND_HAND_POSITION( 0.0f, 0.0f, -2.0f );
  const Vector3 MINUTE_HAND_POSITION( 0.0f, 0.0f, -2.01f );
  const Vector3 HOUR_HAND_POSITION( 0.0f, 0.0f, -2.01f );
  const Vector3 PIVOT_A_POSITION( 0.0f, 0.0f, -1.9f );
  const Vector3 PIVOT_B_POSITION( 0.0f, 0.53f, -2.04f );
  const Vector3 T1_A_POSITION( 0.0f, -0.53f, -2.07f );
  const Vector3 T2_A_POSITION( 0.0f, -0.53f, -2.08f );
  const Vector3 CHROME_INFO_A_POSITION( 0.0f, -0.53f, -2.06f );
  const Vector3 T1_B_POSITION( 0.0f, 0.53f, -2.07f );
  const Vector3 T2_B_POSITION( 0.0f, 0.53f, -2.08f );
  const Vector3 CHROME_INFO_B_POSITION( 0.0f, 0.53f, -2.06f );
  const Vector3 BATTERY_METER_POSITION( 0.0f, 0.525f, -2.055f );
  const Vector3 INFO_HOUR_HAND_POSITION( 0.0f, -0.53f, -2.05f );
  const Vector3 INFO_MIN_HAND_POSITION( 0.0f, 0.53f, -2.05f );
  const Vector3 DATE_POSITION( -0.5f, 0.0f, -2.06f );
  const Vector3 WEATHER_POSITION( 0.52f, 0.0f, -2.06f );

  // Clock Component mesh scales
  const Vector2 BACKGROUND_SCALE( 1.0f, 1.295f );
  const Vector2 SECOND_HAND_SCALE( 0.1f / 1.8f, 1.0f / 1.8f );
  const Vector2 MINUTE_HAND_SCALE( 0.1f / 2.0f, 1.0f / 2.0f );
  const Vector2 HOUR_HAND_SCALE( 0.1f / 2.2f, 1.0f / 2.25f );
  const Vector2 PIVOT_A_SCALE( 0.1f / 3.0f, 0.1f / 3.0f );
  const Vector2 T1_A_SCALE( 0.1f / 0.295f, 0.1f / 0.295f );
  const Vector2 T2_A_SCALE( 0.1f / 0.335f, 0.1f / 0.335f );
  const Vector2 CHROME_INFO_A_SCALE( 0.1f / 0.32f, 0.1f / 0.32f );
  const Vector2 T1_B_SCALE( 0.1f / 0.295f, 0.1f / 0.295f );
  const Vector2 T2_B_SCALE( 0.1f / 0.335f, 0.1f / 0.335f );
  const Vector2 CHROME_INFO_B_SCALE( 0.1f / 0.32f, 0.1f / 0.32f );
  const Vector2 PIVOT_B_SCALE( 0.1f / 3.5f, 0.1f / 3.5f );
  const Vector2 BATTERY_METER_SCALE( 0.22f, 0.22f );
  const Vector2 INFO_HOUR_HAND_SCALE( 0.08f / 1.5f, 0.25f / 1.5f );
  const Vector2 INFO_MIN_HAND_SCALE( 0.1f / 1.75f, 0.31f / 1.75f );
  const Vector2 DATE_SCALE( 0.1f, 0.1f );
  const Vector2 WEATHER_SCALE( 0.1f, 0.1f );

  // Clock Component mesh pivot points
  const float SECOND_HAND_PIVOT( -0.558f );
  const float MINUTE_HAND_PIVOT( -0.65f );
  const float HOUR_HAND_PIVOT( -0.55f );
  const float INFO_HOUR_HAND_PIVOT( -0.6f );
  const float INFO_MIN_HAND_PIVOT( -0.65f );

  // Quad vertex data
  const unsigned short quadTriangles[] = { 0, 0, 0,
                                           2, 2, 2,
                                           3, 3, 3,
                                           3, 3, 3,
                                           1 ,1, 1,
                                           0, 0, 0,
                                           -1 };

  const float quadVertices[] = { -1.0f, -1.0f, 0.0f,
                                 1.0f, -1.0f, 0.0f,
                                -1.0f, 1.0f, 0.0f,
                                 1.0f, 1.0f, 0.0f };

  const float quadNormals[] = { 0.0f, 0.0f, 1.0f,
                                0.0f, 0.0f, 1.0f,
                                0.0f, 0.0f, 1.0f,
                                0.0f, 0.0f, 1.0f };

  const float quadTextCoords[] = { 0.0f, 0.0f,
                                   1.0f, 0.0f,
                                   0.0f, 1.0f,
                                   1.0f, 1.0f };

  // Current time structure
  struct ClockTime
  {
    unsigned int mSeconds;
    unsigned int mMinutes;
    unsigned int mHours;
    bool mAmPm;
  };

}

class ClockController : public ConnectionTracker
{
public:

  ClockController( Application& application )
  : mApplication( application )
  {

    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &ClockController::Create );
  }

  ~ClockController()
  {
    // Nothing to do here;
  }

  // The Init signal is received once (only) during the Application lifetime
  void Create( Application& application )
  {

    // Get a handle to the stage
    mStage = Stage::GetCurrent();
    mStageSize = mStage.GetSize();

    // Load in required assets
    mBackgroundImage = Image::New( CLOCK_BACKGROUND_IMAGE );
    mPivotImage = Image::New( CLOCK_PIVOT_IMAGE );
    mT1Image = Image::New( CLOCK_T1_IMAGE );
    mT2Image = Image::New( CLOCK_T2_IMAGE );
    mChromeImage = Image::New( CLOCK_CHROME_IMAGE );
    mInfoHourImage = Image::New( CLOCK_INFO_HOUR_IMAGE );
    mInfoMinImage = Image::New( CLOCK_INFO_MIN_IMAGE );
    mClockBatteryMeterImage = Image::New( CLOCK_BATTERY_METER_IMAGE );
    mClockDateImage = Image::New( CLOCK_DATE_IMAGE );
    mClockWeatherImage = Image::New( CLOCK_WEATHER_IMAGE );

    mSecondHandImage = Image::New( CLOCK_SECOND_HAND_IMAGE );
    mMinuteHandImage = Image::New( CLOCK_MINUTE_HAND_IMAGE );
    mHourHandImage = Image::New( CLOCK_HOUR_HAND_IMAGE );

    mHourHandImage.LoadingFinishedSignal().Connect( this, &ClockController::Create2 );

  }

  void Create2( Image loadedImage )
  {

    // Read current time
    mCurrentTime = CurrentTime();

    // Work out how long through each minute and hour we are for correct rotation of hands
    float minutes = ( static_cast< float >( mCurrentTime.mMinutes ) + ( 1.0f / ( 60.f / static_cast< float >( mCurrentTime.mSeconds ) ) ) );
    float hours = ( static_cast< float >( mCurrentTime.mHours ) + ( mCurrentTime.mMinutes / 60.f ) );

   // Camera setup
    mCameraActor = CameraActor::New();
    mCameraActor.SetPositionInheritanceMode( Dali::DONT_INHERIT_POSITION );
    mCameraActor.SetFieldOfView( FIELD_OF_VIEW_RADIANS );
    mCameraActor.SetNearClippingPlane( 0.3f );
    mCameraActor.SetFarClippingPlane( FAR_PLANE );
    mCameraActor.SetAspectRatio( mStageSize.width / mStageSize.height );
    mCameraActor.SetPosition( CAMERA_POSITION );
    mCameraActor.SetType( Dali::Camera::FREE_LOOK );
    Quaternion cameraOrientation( Radian(Degree(180.f)), Vector3::YAXIS );
    mCameraActor.SetRotation( cameraOrientation );
    mStage.Add( mCameraActor );

    mViewWidth  = FRUSTUM_HEIGHT * ( mStageSize.width / mStageSize.height);
    mViewHeight = FRUSTUM_HEIGHT;

    mActorRoot = Actor::New();
    mActorRoot.SetParentOrigin( ParentOrigin::CENTER );
    mActorRoot.SetSize( mViewWidth, mViewHeight );
    mStage.Add( mActorRoot );

    mBackground = CreateTexturedObject( "Background", mBackgroundImage, BACKGROUND_POSITION, BACKGROUND_SCALE );

    // Create hand and set initial rotation based on seconds
    mSecondHand = CreateTexturedObject( "Second Hand", mSecondHandImage, SECOND_HAND_POSITION, SECOND_HAND_SCALE, SECOND_HAND_PIVOT );
    mSecondHand.SetRotation( Degree( mCurrentTime.mSeconds * 6.0f ), Vector3::ZAXIS );

    // Set secondhand going with 'stuttering' animation
    Animation secondHandAnimation = Animation::New( 1.0f );
    secondHandAnimation.RotateBy( mSecondHand, Degree( 6.0f ), Vector3::ZAXIS, AlphaFunctions::Default, 0.975f, 0.025f );
    secondHandAnimation.FinishedSignal().Connect( this, &ClockController::OnTick );
    secondHandAnimation.Play();

    // Create Hand and set initial rotation based on minutes
    mMinuteHand = CreateTexturedObject( "Minute Hand", mMinuteHandImage, MINUTE_HAND_POSITION, MINUTE_HAND_SCALE, MINUTE_HAND_PIVOT );
    mMinuteHand.SetRotation( Degree( minutes * 6.0f ), Vector3::ZAXIS );

    // Set smooth sweep of minute hand ( slerp means we can't move 360 degrees, so we do 180 degrees ( could do 359.99..) )
    Animation minuteHandAnimation = Animation::New( 60.0f * 30.0f );
    minuteHandAnimation.RotateBy( mMinuteHand, Degree( 180.0f) , Vector3::ZAXIS );
    minuteHandAnimation.FinishedSignal().Connect( this, &ClockController::OnHalfMovement );
    minuteHandAnimation.Play();

    // Create hand and set initial rotation based on hours - hand is not animated but moved by secondhand signal
    mHourHand = CreateTexturedObject( "Hour Hand", mHourHandImage, HOUR_HAND_POSITION, HOUR_HAND_SCALE, HOUR_HAND_PIVOT );
    mHourHand.SetRotation( Degree( hours * 30.0f ), Vector3::ZAXIS );

    // Create the top info components
    CreateTexturedObject( "Pivota", mPivotImage, PIVOT_A_POSITION, PIVOT_A_SCALE );
    CreateTexturedObject( "T1a", mT1Image, T1_A_POSITION, T1_A_SCALE );
    CreateTexturedObject( "T2a", mT2Image, T2_A_POSITION, T2_A_SCALE );
    CreateTexturedObject( "Chrome Infoa", mChromeImage, CHROME_INFO_A_POSITION, CHROME_INFO_A_SCALE );
    mInfoHour = CreateTexturedObject( "Hour Info", mInfoHourImage, INFO_HOUR_HAND_POSITION, INFO_HOUR_HAND_SCALE, INFO_HOUR_HAND_PIVOT );

    // Create the lower info components
    CreateTexturedObject( "T1b", mT1Image, T1_B_POSITION, T1_B_SCALE );
    CreateTexturedObject( "T2b", mT2Image, T2_B_POSITION, T2_B_SCALE );
    CreateTexturedObject( "Chrome Infob", mChromeImage, CHROME_INFO_B_POSITION, CHROME_INFO_B_SCALE );
    CreateTexturedObject( "Battery Meter", mClockBatteryMeterImage, BATTERY_METER_POSITION, BATTERY_METER_SCALE );
    CreateTexturedObject( "Pivotb", mPivotImage, PIVOT_B_POSITION, PIVOT_B_SCALE );
    mInfoMin = CreateTexturedObject( "Min Info", mInfoMinImage, INFO_MIN_HAND_POSITION, INFO_MIN_HAND_SCALE, INFO_MIN_HAND_PIVOT );
    mInfoMin.SetRotation( Degree( -210.0f ), Vector3::ZAXIS );

    // Create the date and weather actors
    CreateTexturedObject( "Date", mClockDateImage, DATE_POSITION, DATE_SCALE );
    CreateTexturedObject( "Weather", mClockWeatherImage, WEATHER_POSITION, WEATHER_SCALE );

    // Temporary print of text
    TextActor textActor = TextActor::New( "BATTERY" );
    textActor.SetPositionInheritanceMode( Dali::DONT_INHERIT_POSITION );
    textActor.SetPosition( 0.0f, 0.25f, -2.06f );
    textActor.SetScale( 1.0f / 360.0f, 1.0f / 360.0f, 1.0f / 2.0f );
    mActorRoot.Add( textActor );

    TextActor textActor2 = TextActor::New( "NYC" );
    textActor2.SetPositionInheritanceMode( Dali::DONT_INHERIT_POSITION );
    textActor2.SetPosition( 0.0f, -0.815f, -2.06f );
    textActor2.SetScale( 1.0f / 360.0f, 1.0f / 360.0f, 1.0f / 2.0f );
    mActorRoot.Add( textActor2 );

    // Test animation for info hour hand
    Animation animation = Animation::New( 2.0f);
    animation.RotateBy( mInfoHour, Degree( 180.0f ), Vector3::ZAXIS );
    animation.FinishedSignal().Connect( this, &ClockController::OnHalfMovement );
    animation.Play();

    // Test animation for battery level hand
    Animation animation1 = Animation::New( 500.0f);
    animation1.RotateBy( mInfoMin, Degree( -300.0f ), Vector3::ZAXIS );
    animation1.SetLooping( true );
    animation1.Play();

    // Point camera to actor tree in stage for default actors
    mRenderTaskList = mStage.GetRenderTaskList();
    mRenderTaskList.GetTask( 0 ).SetCameraActor( mCameraActor );
    mRenderTaskList.GetTask( 0 ).SetSourceActor( mActorRoot );
  }

  MeshActor CreateTexturedObject( const std::string& name, const Image& image, const Vector3& origin, const Vector2& scale, const float pivot = 0.0f )
  {
    MeshData::VertexContainer vertices;
    MeshData::FaceIndices faces;
    ConstructMesh( vertices, faces, quadTriangles, quadVertices, quadNormals, quadTextCoords, Vector3( Vector3::ZERO ), pivot );

    Material material = Material::New( name );
    material.SetDiffuseTexture( image );
    MeshData meshData;
    meshData.SetHasColor( false );
    meshData.SetHasNormals( true );
    meshData.SetHasTextureCoords(true);
    meshData.SetMaterial( material );
    meshData.SetVertices( vertices );
    meshData.SetFaceIndices( faces );

    MeshActor actor = MeshActor::New( Mesh::New( meshData ) );
    actor.SetPositionInheritanceMode( Dali::DONT_INHERIT_POSITION );
    actor.SetPosition( origin );
    actor.SetScale( Vector3( scale.x, scale.y, 0.0f ) );
    actor.SetAffectedByLighting( false );
    actor.SetFilterMode( FilterMode::LINEAR, FilterMode::LINEAR );
    mActorRoot.Add( actor );
    return actor;
  }

  void ConstructMesh( MeshData::VertexContainer& vertices,
                      MeshData::FaceIndices& faces,
                      const unsigned short* triangleData,
                      const float* vertexData,
                      const float* normalData,
                      const float* textCoordData,
                      const Vector3& axisFixup,
                      const float y_offset )
  {

    // Count the number of vertices in the triangles ( we terminate the array with unsigned -1 )
    unsigned short triangleCount = 0;
    while ( triangleData[ triangleCount++ ] != static_cast< unsigned short >( - 1 ) ) {};

    for ( unsigned short i = 0; i < ( ( triangleCount - 1 ) / 3 ); i++ )
    {
      Vector2 currentUV;
      Vector3 currentVertex, currentNormal;

      unsigned int index = *( triangleData++ );
      currentVertex.x = vertexData[ index * 3 ];
      currentVertex.y = vertexData[ ( index * 3 ) + 1 ] + y_offset;
      currentVertex.z = vertexData[ ( index * 3 ) + 2 ];

      index = *( triangleData++ );
      currentNormal.x = normalData[ index * 3 ];
      currentNormal.y = normalData[ ( index * 3 ) + 1 ];
      currentNormal.z = normalData[ ( index * 3 ) + 2 ];

      index = *( triangleData++ );
      float u = textCoordData[ index * 2 ];
      float v = textCoordData[ ( index * 2 ) + 1 ];

      currentUV.x = u;
      currentUV.y = v;

      bool foundVertex = false;
      for ( unsigned int vert = 0; vert < vertices.size(); vert++ )
      {
        if ( currentVertex.x == vertices[ vert ].x &&
             currentVertex.y == vertices[ vert ].y &&
             currentVertex.z == vertices[ vert ].z &&
             currentNormal.x == vertices[ vert ].nX &&
             currentNormal.y == vertices[ vert ].nY &&
             currentNormal.z == vertices[ vert ].nZ &&
             currentUV.x == vertices[ vert ].u &&
             currentUV.y == vertices[ vert ].v )
        {
          // Vertex already exists, so write this into the face list
          faces.push_back( vert );
          foundVertex = true;
          break;
        }
      }

      if ( !foundVertex )
      {
        // Have to construct a new vertex, first push the new triangle index
        faces.push_back( static_cast< unsigned short >( vertices.size() ) );
        vertices.push_back( MeshData::Vertex( currentVertex, currentUV, currentNormal ) );
      }
    }
  }

  void OnTick( Dali::Animation& animation )
  {

    mCurrentTime.mSeconds++;
    mHourHand.RotateBy( Degree( 1.0f / 120.0f ), Vector3::ZAXIS );
    if ( mCurrentTime.mSeconds == 60 )
    {
      mCurrentTime.mSeconds = 0;
      mCurrentTime.mMinutes++;
      if ( mCurrentTime.mMinutes == 60 )
      {
        mCurrentTime.mMinutes = 0;
        mCurrentTime.mHours++;
        if ( mCurrentTime.mHours == 12 )
        {
          mCurrentTime.mHours = 0;
          mCurrentTime.mAmPm = ( mCurrentTime.mAmPm == true ? false : true );
        }
      }
    }
    animation.Play();
  }

  void OnHalfMovement( Dali::Animation& animation )
  {
    animation.Play();
  }

  ClockTime CurrentTime()
  {
    ClockTime currentTime;
    time_t rawtime;
    struct tm* timeinfo;
    time ( &rawtime );
    timeinfo = localtime ( &rawtime );

    currentTime.mSeconds = static_cast< unsigned int >( timeinfo->tm_sec );
    currentTime.mHours = static_cast< unsigned int >( timeinfo->tm_hour );
    currentTime.mMinutes = static_cast< unsigned int >( timeinfo->tm_min );

    if ( currentTime.mHours / 12 )
    {
      currentTime.mAmPm = true;
    }
    else
    {
      currentTime.mAmPm = false;
    }

    currentTime.mHours %= 12;
    return currentTime;
  }


  bool OnTouch( Actor actor, const TouchEvent& touch )
  {
    // quit the application
    mApplication.Quit();
    return true;
  }

private:
  Application&  mApplication;
  Stage mStage;
  Vector2 mStageSize;
  float mViewHeight;
  float mViewWidth;


  Actor mActorRoot;
  CameraActor mCameraActor;
  RenderTaskList mRenderTaskList;

  Image mBackgroundImage;
  Image mSecondHandImage;
  Image mMinuteHandImage;
  Image mHourHandImage;
  Image mPivotImage;
  Image mT1Image;
  Image mT2Image;
  Image mChromeImage;
  Image mInfoHourImage;
  Image mInfoMinImage;
  Image mClockBatteryMeterImage;
  Image mClockDateImage;
  Image mClockWeatherImage;

  MeshActor mBackground;
  MeshActor mSecondHand;
  MeshActor mMinuteHand;
  MeshActor mHourHand;
  MeshActor mInfoHour;
  MeshActor mInfoMin;

  ClockTime mCurrentTime;

};

void RunTest( Application& application )
{
  ClockController test( application );

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
