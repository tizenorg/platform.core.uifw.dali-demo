/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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

#include <dali-toolkit/dali-toolkit.h>
#include <dali/devel-api/rendering/renderer.h>
#include <dali/public-api/common/dali-common.h>
#include <dali/integration-api/resource-policies.h>
#include <dali/integration-api/debug.h>
#include <iostream>

using namespace Dali;
using namespace Dali::Toolkit;


namespace
{
const char* IMAGE_PATH[] = {
                            DALI_IMAGE_DIR "gallery-medium-1.jpg",
                            DALI_IMAGE_DIR "gallery-medium-2.jpg",
                            DALI_IMAGE_DIR "gallery-medium-3.jpg",
                            DALI_IMAGE_DIR "gallery-medium-4.jpg",
                            DALI_IMAGE_DIR "gallery-medium-5.jpg",
                            DALI_IMAGE_DIR "gallery-medium-6.jpg",
                            DALI_IMAGE_DIR "gallery-medium-7.jpg",
                            DALI_IMAGE_DIR "gallery-medium-8.jpg",
                            DALI_IMAGE_DIR "gallery-medium-9.jpg",
                            DALI_IMAGE_DIR "gallery-medium-10.jpg",
                            DALI_IMAGE_DIR "gallery-medium-11.jpg",
                            DALI_IMAGE_DIR "gallery-medium-12.jpg",
                            DALI_IMAGE_DIR "gallery-medium-13.jpg",
                            DALI_IMAGE_DIR "gallery-medium-14.jpg",
                            DALI_IMAGE_DIR "gallery-medium-15.jpg",
                            DALI_IMAGE_DIR "gallery-medium-16.jpg",
                            DALI_IMAGE_DIR "gallery-medium-17.jpg",
                            DALI_IMAGE_DIR "gallery-medium-18.jpg",
                            DALI_IMAGE_DIR "gallery-medium-19.jpg",
                            DALI_IMAGE_DIR "gallery-medium-20.jpg",
                            DALI_IMAGE_DIR "gallery-medium-21.jpg",
                            DALI_IMAGE_DIR "gallery-medium-22.jpg",
                            DALI_IMAGE_DIR "gallery-medium-23.jpg",
                            DALI_IMAGE_DIR "gallery-medium-24.jpg",
                            DALI_IMAGE_DIR "gallery-medium-25.jpg",
                            DALI_IMAGE_DIR "gallery-medium-26.jpg",
                            DALI_IMAGE_DIR "gallery-medium-27.jpg",
                            DALI_IMAGE_DIR "gallery-medium-28.jpg",
                            DALI_IMAGE_DIR "gallery-medium-29.jpg",
                            DALI_IMAGE_DIR "gallery-medium-30.jpg",
                            DALI_IMAGE_DIR "gallery-medium-31.jpg",
                            DALI_IMAGE_DIR "gallery-medium-32.jpg",
                            DALI_IMAGE_DIR "gallery-medium-33.jpg",
                            DALI_IMAGE_DIR "gallery-medium-34.jpg",
                            DALI_IMAGE_DIR "gallery-medium-35.jpg",
                            DALI_IMAGE_DIR "gallery-medium-36.jpg",
                            DALI_IMAGE_DIR "gallery-medium-37.jpg",
                            DALI_IMAGE_DIR "gallery-medium-38.jpg",
                            DALI_IMAGE_DIR "gallery-medium-39.jpg",
                            DALI_IMAGE_DIR "gallery-medium-40.jpg",
                            DALI_IMAGE_DIR "gallery-medium-41.jpg",
                            DALI_IMAGE_DIR "gallery-medium-42.jpg",
                            DALI_IMAGE_DIR "gallery-medium-43.jpg",
                            DALI_IMAGE_DIR "gallery-medium-44.jpg",
                            DALI_IMAGE_DIR "gallery-medium-45.jpg",
                            DALI_IMAGE_DIR "gallery-medium-46.jpg",
                            DALI_IMAGE_DIR "gallery-medium-47.jpg",
                            DALI_IMAGE_DIR "gallery-medium-48.jpg",
                            DALI_IMAGE_DIR "gallery-medium-49.jpg",
                            DALI_IMAGE_DIR "gallery-medium-50.jpg",
                            DALI_IMAGE_DIR "gallery-medium-51.jpg",
                            DALI_IMAGE_DIR "gallery-medium-52.jpg",
                            DALI_IMAGE_DIR "gallery-medium-53.jpg",
};

const unsigned int NUM_IMAGES = sizeof(IMAGE_PATH) / sizeof(char*);

const float ANIMATION_TIME ( 5.0f ); // animation length in seconds

struct VertexWithTexture
{
  Vector2 position;
  Vector2 texCoord;
};

VertexWithTexture gQuadWithTexture[] = {
                                        { Vector2( -0.5f, -0.5f ), Vector2( 0.0f, 0.0f ) },
                                        { Vector2(  0.5f, -0.5f ), Vector2( 1.0f, 0.0f ) },
                                        { Vector2( -0.5f,  0.5f ), Vector2( 0.0f, 1.0f ) },
                                        { Vector2(  0.5f,  0.5f ), Vector2( 1.0f, 1.0f ) }
};

const char* VERTEX_SHADER_TEXTURE = DALI_COMPOSE_SHADER(
    attribute mediump vec2 aPosition;\n
    attribute mediump vec2 aTexCoord;\n
    uniform mediump mat4 uMvpMatrix;\n
    varying mediump vec2 vTexCoord;\n
    void main()\n
    {\n
      gl_Position = uMvpMatrix * vec4( aPosition, 0.0, 1.0 );\n
      vTexCoord = aTexCoord;\n
    }\n
);

const char* FRAGMENT_SHADER_TEXTURE = DALI_COMPOSE_SHADER(
    uniform lowp vec4 uColor;\n
    uniform sampler2D sTexture;\n
    varying mediump vec2 vTexCoord;\n

    void main()\n
    {\n
      gl_FragColor = texture2D( sTexture, vTexCoord ) * uColor;\n
    }\n
);


Geometry& QuadMesh()
{
  static Geometry mesh;
  if( !mesh )
  {
    PropertyBuffer vertexBuffer;
    Property::Map vertexFormat;
    vertexFormat["aPosition"] = Property::VECTOR2;
    vertexFormat["aTexCoord"] = Property::VECTOR2;

    //Create a vertex buffer for vertex positions and texture coordinates
    vertexBuffer = PropertyBuffer::New( vertexFormat, 4u );
    vertexBuffer.SetData( gQuadWithTexture );

    //Create the geometry
    mesh = Geometry::New();
    mesh.AddVertexBuffer( vertexBuffer );
    mesh.SetGeometryType(Geometry::TRIANGLE_STRIP );
  }
  return mesh;
}

Renderer CreateRenderer( unsigned int index )
{
  static Renderer renderers[NUM_IMAGES];
  if( !renderers[index] )
  {
    //Create the renderer
    Shader shader = Shader::New( VERTEX_SHADER_TEXTURE, FRAGMENT_SHADER_TEXTURE );
    Image image = ResourceImage::New(IMAGE_PATH[index]);
    Sampler textureSampler = Sampler::New( image, "sTexture" );
    Material material = Material::New( shader );
    material.AddSampler(textureSampler);
    material.SetBlendMode( BlendingMode::OFF );
    renderers[index] = Renderer::New( QuadMesh(), material );
  }
  return renderers[index];
}

Actor CreateMeshActor( unsigned int index)
{
  Renderer renderer = CreateRenderer(index);
  Actor meshActor = Actor::New();
  meshActor.AddRenderer( renderer );
  return meshActor;
}

bool gUseMesh(false);
bool gUseImageView(false);
unsigned int gRowsPerPage(25);
unsigned int gColumnsPerPage( 25 );
unsigned int gPageCount(13);


}
// Test application to compare performance between ImageActor and ImageView
// By default, the application consist of 10 pages of 25x25 ImageActors, this can be modified using the following command line arguments:
// -r NumberOfRows  (Modifies the number of rows per page)
// -c NumberOfColumns (Modifies the number of columns per page)
// -p NumberOfPages (Modifies the nimber of pages )
// --use-imageview ( Use ImageView instead of ImageActor )
// --use-mesh ( Use new renderer API (as ImageView) but shares renderers between actors when possible )

//
class Benchmark : public ConnectionTracker
{
public:

  Benchmark( Application& application )
: mApplication( application ),
  mRowsPerPage( gRowsPerPage ),
  mColumnsPerPage( gColumnsPerPage ),
  mPageCount( gPageCount )
{
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &Benchmark::Create );
}

  ~Benchmark()
  {
    // Nothing to do here;
  }

  // The Init signal is received once (only) during the Application lifetime
  void Create( Application& application )
  {
    // Get a handle to the stage
    Stage stage = Stage::GetCurrent();
    stage.SetBackgroundColor( Color::WHITE );
    Vector2 stageSize = stage.GetSize();

    stage.GetRootLayer().SetDepthTestDisabled(true);

    mSize = Vector3( stageSize.x / mColumnsPerPage, stageSize.y / mRowsPerPage, 0.0f );

    // Respond to a click anywhere on the stage
    stage.GetRootLayer().TouchedSignal().Connect( this, &Benchmark::OnTouch );

    if( gUseMesh )
    {
      CreateMeshActors();
    }
    else if( gUseImageView )
    {
      CreateImageViews();
    }
    else
    {
      CreateImageActors();
    }

    ShowAnimation();
  }

  bool OnTouch( Actor actor, const TouchEvent& touch )
  {
    // quit the application
    mApplication.Quit();
    return true;
  }

  void CreateImageActors()
  {
    Stage stage = Stage::GetCurrent();
    unsigned int actorCount(mRowsPerPage*mColumnsPerPage * mPageCount);
    mActor.resize(actorCount);

    for( size_t i(0); i<actorCount; ++i )
    {
      Image image = ResourceImage::New(IMAGE_PATH[i % NUM_IMAGES]);
      mActor[i] = ImageActor::New(image);
      mActor[i].SetSize(Vector3(0.0f,0.0f,0.0f));
      mActor[i].SetResizePolicy( ResizePolicy::FIXED, Dimension::ALL_DIMENSIONS );
      stage.Add(mActor[i]);
    }
  }

  void CreateImageViews()
  {
    Stage stage = Stage::GetCurrent();
    unsigned int actorCount(mRowsPerPage*mColumnsPerPage * mPageCount);
    mImageView.resize(actorCount);

    for( size_t i(0); i<actorCount; ++i )
    {
      Image image = ResourceImage::New(IMAGE_PATH[i % NUM_IMAGES]);
      mImageView[i] = ImageView::New(image);
      mImageView[i].SetSize(Vector3(0.0f,0.0f,0.0f));
      mImageView[i].SetResizePolicy( ResizePolicy::FIXED, Dimension::ALL_DIMENSIONS );
      stage.Add(mImageView[i]);
    }
  }

  void CreateMeshActors()
  {
    Stage stage = Stage::GetCurrent();
    unsigned int actorCount(mRowsPerPage*mColumnsPerPage * mPageCount);
    mActor.resize(actorCount);
    for( size_t i(0); i<actorCount; ++i )
    {
      mActor[i] = CreateMeshActor(i % NUM_IMAGES);
      mActor[i].SetScale(mSize);
      mActor[i].SetSize(1.0f,1.0f,1.0f);

      stage.Add(mActor[i]);
    }
  }

  void OnAnimationEnd( Animation& source )
  {
    if( source == mShow )
    {
      ScrollAnimation();
    }
    else if( source == mScroll )
    {
      HideAnimation();
    }
    else
    {
      mApplication.Quit();
    }
  }

  void ShowAnimation()
  {
    Stage stage = Stage::GetCurrent();
    Vector3 initialPosition( stage.GetSize().x * 0.5f, stage.GetSize().y*0.5f, 1000.0f );

    unsigned int totalColumns = mColumnsPerPage * mPageCount;

    size_t count(0);
    float xpos, ypos;
    mShow = Animation::New(0.0f);

    float totalDuration( 10.0f );
    float durationPerActor( 0.5f );
    float delayBetweenActors = ( totalDuration - durationPerActor) / (mRowsPerPage*mColumnsPerPage);
    for( size_t i(0); i<totalColumns; ++i )
    {
      xpos = mSize.x * i;

      for( size_t j(0);j<mRowsPerPage;++j)
      {

        ypos = mSize.y * j;

        float delay = 0.0f;
        float duration = 0.0f;
        if( count < mRowsPerPage*mColumnsPerPage )
        {
          duration = durationPerActor;
          delay = delayBetweenActors * count;
        }
        if( gUseImageView )
        {
          mImageView[count].SetPosition( initialPosition );
          mImageView[count].SetSize( Vector3(0.0f,0.0f,0.0f) );
          mImageView[count].SetOrientation( Quaternion( Radian(0.0f),Vector3::XAXIS));
          mShow.AnimateTo( Property( mImageView[count], Actor::Property::POSITION), Vector3(xpos+mSize.x*0.5f, ypos+mSize.y*0.5f, 0.0f), AlphaFunction::EASE_OUT_BACK, TimePeriod( delay, duration ));
          mShow.AnimateTo( Property( mImageView[count], Actor::Property::SIZE), mSize, AlphaFunction::EASE_OUT_BACK, TimePeriod( delay, duration ));
        }
        else
        {
          mActor[count].SetPosition( initialPosition );
          mActor[count].SetSize( Vector3(0.0f,0.0f,0.0f) );
          mActor[count].SetOrientation( Quaternion( Radian(0.0f),Vector3::XAXIS));
          mShow.AnimateTo( Property( mActor[count], Actor::Property::POSITION), Vector3(xpos+mSize.x*0.5f, ypos+mSize.y*0.5f, 0.0f), AlphaFunction::EASE_OUT_BACK, TimePeriod( delay, duration ));
          mShow.AnimateTo( Property( mActor[count], Actor::Property::SIZE), mSize, AlphaFunction::EASE_OUT_BACK, TimePeriod( delay, duration ));
        }
        ++count;
      }
    }
    mShow.Play();
    mShow.FinishedSignal().Connect( this, &Benchmark::OnAnimationEnd );
  }

  void ScrollAnimation()
  {
    Stage stage = Stage::GetCurrent();
    Vector3 stageSize( stage.GetSize() );

    mScroll = Animation::New(10.0f);
    size_t actorCount( mRowsPerPage*mColumnsPerPage*mPageCount);
    for( size_t i(0); i<actorCount; ++i )
    {
      if( gUseImageView )
      {
        mScroll.AnimateBy( Property( mImageView[i], Actor::Property::POSITION), Vector3(-4.0f*stageSize.x,0.0f, 0.0f), AlphaFunction::EASE_OUT, TimePeriod(0.0f,3.0f));
        mScroll.AnimateBy( Property( mImageView[i], Actor::Property::POSITION), Vector3(-4.0f*stageSize.x,0.0f, 0.0f), AlphaFunction::EASE_OUT, TimePeriod(3.0f,3.0f));
        mScroll.AnimateBy( Property( mImageView[i], Actor::Property::POSITION), Vector3(-4.0f*stageSize.x,0.0f, 0.0f), AlphaFunction::EASE_OUT, TimePeriod(6.0f,2.0f));
        mScroll.AnimateBy( Property( mImageView[i], Actor::Property::POSITION), Vector3( 12.0f*stageSize.x,0.0f, 0.0f), AlphaFunction::EASE_OUT, TimePeriod(8.0f,2.0f));
      }
      else
      {
        mScroll.AnimateBy( Property( mActor[i], Actor::Property::POSITION), Vector3(-4.0f*stageSize.x,0.0f, 0.0f), AlphaFunction::EASE_OUT, TimePeriod(0.0f,3.0f));
        mScroll.AnimateBy( Property( mActor[i], Actor::Property::POSITION), Vector3(-4.0f*stageSize.x,0.0f, 0.0f), AlphaFunction::EASE_OUT, TimePeriod(3.0f,3.0f));
        mScroll.AnimateBy( Property( mActor[i], Actor::Property::POSITION), Vector3(-4.0f*stageSize.x,0.0f, 0.0f), AlphaFunction::EASE_OUT, TimePeriod(6.0f,2.0f));
        mScroll.AnimateBy( Property( mActor[i], Actor::Property::POSITION), Vector3( 12.0f*stageSize.x,0.0f, 0.0f), AlphaFunction::EASE_OUT, TimePeriod(8.0f,2.0f));
      }
    }
    mScroll.Play();
    mScroll.FinishedSignal().Connect( this, &Benchmark::OnAnimationEnd );
  }

  void HideAnimation()
  {
    size_t count(0);
    unsigned int actorsPerPage( mRowsPerPage*mColumnsPerPage );
    mHide = Animation::New(0.0f);

    unsigned int totalColumns = mColumnsPerPage * mPageCount;

    float totalDuration( 5.0f);
    float durationPerActor( 0.5f );
    float delayBetweenActors = ( totalDuration - durationPerActor) / (mRowsPerPage*mColumnsPerPage);
    for( size_t i(0); i<mRowsPerPage; ++i )
    {
      for( size_t j(0);j<totalColumns;++j)
      {
        float delay = 0.0f;
        float duration = 0.0f;
        if( count < actorsPerPage )
        {
          duration = durationPerActor;
          delay = delayBetweenActors * count;
        }

        if( gUseImageView )
        {
          mHide.AnimateTo( Property( mImageView[count], Actor::Property::ORIENTATION),  Quaternion( Radian( Degree( 70.0f ) ), Vector3::XAXIS ), AlphaFunction::EASE_OUT, TimePeriod( delay, duration ));
          mHide.AnimateBy( Property( mImageView[count], Actor::Property::POSITION_Z), 1000.0f, AlphaFunction::EASE_OUT_BACK, TimePeriod( delay +delayBetweenActors*actorsPerPage + duration, duration ));
        }
        else
        {
          mHide.AnimateTo( Property( mActor[count], Actor::Property::ORIENTATION),  Quaternion( Radian( Degree( 70.0f ) ), Vector3::XAXIS ), AlphaFunction::EASE_OUT, TimePeriod( delay, duration ));
          mHide.AnimateBy( Property( mActor[count], Actor::Property::POSITION_Z), 1000.0f, AlphaFunction::EASE_OUT_BACK, TimePeriod( delay +delayBetweenActors*actorsPerPage + duration, duration ));
        }
        ++count;
      }
    }

    mHide.Play();
    mHide.FinishedSignal().Connect( this, &Benchmark::OnAnimationEnd );
  }

private:
  Application&  mApplication;

  std::vector<Actor>  mActor;
  std::vector<ImageView>  mImageView;

  Vector3             mSize;
  unsigned int        mRowsPerPage;
  unsigned int        mColumnsPerPage;
  unsigned int        mPageCount;

  Animation           mShow;
  Animation           mScroll;
  Animation           mHide;
};

void RunTest( Application& application )
{
  Benchmark test( application );

  application.MainLoop();
}

// Entry point for Linux & Tizen applications
//
int main( int argc, char **argv )
{
  Application application = Application::New( &argc, &argv );

  for( int i(1) ; i < argc; ++i )
  {
    std::string arg( argv[i] );
    if( arg.compare("--use-mesh") == 0)
    {
      gUseMesh = true;
    }
    else if( arg.compare("--use-imageview") == 0)
    {
      gUseImageView = true;
    }
    else if( arg.compare(0, 2, "-r" ) == 0)
    {
      gRowsPerPage = atoi( arg.substr( 2, arg.size()).c_str());
    }
    else if( arg.compare(0, 2, "-c" ) == 0)
    {
      gColumnsPerPage = atoi( arg.substr( 2, arg.size()).c_str());
    }
    else if( arg.compare(0, 2, "-p" ) == 0)
    {
      gPageCount = atoi( arg.substr( 2, arg.size()).c_str());
    }


  }

  RunTest( application );

  return 0;
}
