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

#include <fstream>

#include <dali-toolkit/dali-toolkit.h>

namespace
{
  const char* TEST_BITMAP = DALI_IMAGE_DIR "test.tga";
  const char* TEST_BITMAP2 = DALI_IMAGE_DIR "test2.tga";
}

using namespace Dali;
using namespace Dali::Toolkit;

class ShaderRGB : public ShaderEffect
{
public:

  ShaderRGB()
  {
  }

  static ShaderRGB New()
  {
    std::string vertexSource;
    std::string fragmentSource;

    vertexSource = "void main()                                          \n"
                   "{\n"
                   "  vTexCoord = aTexCoord;\n"
                   "  gl_Position = uMvpMatrix * vec4( aPosition, 1.0 ); \n"

                   "}\n";

    fragmentSource = "precision lowp float;                                   \n"
                     "void main()                                                \n"
                     "{\n"
                     "  vec3 color = texture2D( sTexture, vTexCoord ).rgb;\n"
                     "  gl_FragColor = vec4( color.rgb, 1.0 );\n"
                     "}\n";

    ShaderEffect shader = ShaderEffect::New( vertexSource,
                                             fragmentSource,
                                             GeometryType( GEOMETRY_TYPE_TEXTURED_MESH ),
                                             ShaderEffect::GeometryHints( ShaderEffect::HINT_NONE ) );

    ShaderRGB handle( shader );
    return handle;
  }

  /**
   * Virtual destructor.
   */
  virtual ~ShaderRGB()
  {
  }

private:

  /**
   * Helper for New()
   */
  ShaderRGB( ShaderEffect handle )
  : ShaderEffect( handle )
  {
  }

};


// This example shows how to create and display Hello World! using a simple TextActor
//
class TextureAtlasExample : public ConnectionTracker
{
public:

  typedef uint32_t SizeType;

  TextureAtlasExample( Application& application )
  : mApplication( application ),
    mTimerSlot( this ),
    mFlash( false )
  {
    std::cout << "TextureAtlasExample::TextureAtlasExample" << std::endl;

    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &TextureAtlasExample::Create );
  }

  ~TextureAtlasExample()
  {
    // Nothing to do here;
  }

  // The Init signal is received once (only) during the Application lifetime
  void Create( Application& application )
  {

    mAtlasManager = AtlasManager::Get();

    TextAbstraction::FontClient fontClient = TextAbstraction::FontClient::Get();
    fontClient.SetDpi( 96, 96 );

    TextLabel label1 = TextLabel::New();
    label1.SetRenderer( Text::AtlasRenderer::New() );
    label1.SetProperty( TextLabel::PROPERTY_TEXT, "L8 Text Renderering using Atlas" );
    label1.SetParentOrigin( Vector3( 0.68f, 1.2f, 0.5f ) );

    TextLabel label2 = TextLabel::New();
    label2.SetRenderer( Text::AtlasRenderer::New() );
    label2.SetProperty( TextLabel::PROPERTY_TEXT, "RGB Images Rendered using Atlas" );
    label2.SetParentOrigin( Vector3( 0.7f, 0.52f, 0.5f ) );

    TextLabel label3 = TextLabel::New();
    label3.SetRenderer( Text::AtlasRenderer::New() );
    label3.SetProperty( TextLabel::PROPERTY_TEXT, "Testing Reference Counting" );
    label3.SetParentOrigin( Vector3( 0.72f, 1.38f, 0.5f ) );

    Stage stage = Stage::GetCurrent();
    stage.Add( label1 );
    stage.Add( label2 );
    stage.Add( label3 );

    mBuffer1 = ( LoadFile( TEST_BITMAP ) );
    mImage = ConvertTGA( mBuffer1, 256, 256 );

    mBuffer2 = ( LoadFile( TEST_BITMAP2 ) );
    mImage2 = ConvertTGA( mBuffer2, 128, 128 );

    AtlasManager::AtlasSlot slot1;
    AtlasManager::AtlasSlot slot2;

    // Create a specific atlas for RGB images
    SizeType bitmapAtlas = mAtlasManager.CreateAtlas( 512u, 512u, 256u, 256u, Pixel::RGB888 );

    // Add images to the new atlas
    mAtlasManager.Add( mImage, slot1, bitmapAtlas );
    mAtlasManager.Add( mImage2, slot2, bitmapAtlas );

    MeshData firstMesh;
    MeshData secondMesh;
    mAtlasManager.GenerateMeshData( slot1.mImageId, Vector2( -140.0f, -160.0f ), firstMesh );
    mAtlasManager.GenerateMeshData( slot2.mImageId, Vector2( -140.0f, -320.0f ), secondMesh );

    mShader = ShaderRGB::New ();

    MeshActor actor = MeshActor::New( Mesh::New( firstMesh ) );
    actor.SetParentOrigin( ParentOrigin::CENTER );
    actor.SetAffectedByLighting( false );
    actor.SetShaderEffect( mShader );
    stage.Add( actor );

    MeshActor actor2 = MeshActor::New( Mesh::New( secondMesh ) );
    actor2.SetParentOrigin( ParentOrigin::CENTER );
    actor2.SetAffectedByLighting( false );
    actor2.SetShaderEffect( mShader );
    stage.Add( actor2 );

    mTimer = Dali::Timer::New( 400.0f );
    mTimer.TickSignal().Connect( mTimerSlot, &TextureAtlasExample::OnTick );
    mTimer.Start();
  }

  BitmapImage ConvertTGA( char* buffer, uint32_t width, uint32_t height )
  {
    for ( uint32_t i = 0; i < ( width * height ); ++i )
    {
      char temp = buffer[ ( i * 3 ) + 18 ];
      buffer[ ( i * 3 ) + 18 ] = buffer[ ( i * 3 ) + 20 ];
      buffer[ ( i * 3 ) + 20 ] = temp;
    }
    return BitmapImage::New( reinterpret_cast< PixelBuffer* >( &buffer[18] ), width, height, Pixel::RGB888 );
  }

  bool OnTouch( Actor actor, const TouchEvent& touch )
  {
    delete[] mBuffer1;
    delete[] mBuffer2;

    // quit the application
    mApplication.Quit();
    return true;
  }

  bool OnTick()
  {
    Stage stage = Stage::GetCurrent();
    if ( !mFlash )
    {
      mLabel = TextLabel::New();
      mLabel.SetRenderer( Text::AtlasRenderer::New() );
      mLabel.SetProperty( TextLabel::PROPERTY_TEXT, "***Flashing Text:123***" );
      mLabel.SetParentOrigin( Vector3( 0.75f, 1.4f, 0.5f ) );
      stage.Add( mLabel );
    }
    else
    {
      stage.Remove( mLabel );
      mLabel.Reset();
    }
    mFlash ^= 1;
    return true;
  }

  char* LoadFile( const char* fileName )
  {
    char* data;
    std::fstream in;
    unsigned long fileSize;

    in.open( fileName, std::ios_base::binary | std::ios_base::in );
    if ( in.fail() )
    {
      return NULL;
    }

    in.seekg( 0, std::ios_base::end );
    fileSize = in.tellg();
    in.seekg( 0, std::ios_base::beg );

    data = new char[ fileSize ];
    if ( !data )
    {
      return NULL;
    }

    in.read( data, fileSize );
    in.close();

    if ( in.fail() )
    {
      delete[] data;
      return NULL;
    }

    return data;
  };

private:
  Application&  mApplication;
  TextLabel mLabel;
  SlotDelegate< TextureAtlasExample > mTimerSlot;
  Dali::Timer mTimer;
  bool mFlash;
  AtlasManager mAtlasManager;
  char* mBuffer1;
  char* mBuffer2;
  BitmapImage mImage;
  BitmapImage mImage2;
  ShaderRGB mShader;
};

void RunTest( Application& application )
{
  TextureAtlasExample test( application );

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
