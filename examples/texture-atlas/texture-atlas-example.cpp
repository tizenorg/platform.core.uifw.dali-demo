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
#include <dali-toolkit/dali-toolkit.h>
#include <fstream>
#include <string>
#include <stdint.h>

#include <dali-toolkit/public-api/atlas-manager/atlas-manager.h>
#include <dali/public-api/text-abstraction/text-abstraction.h>

using namespace Dali;
using namespace Dali::Toolkit;

/**
 * Custom shader
 */
class TextShaderRGB : public ShaderEffect
{
public:

  TextShaderRGB()
  {
  }

  static TextShaderRGB New()
  {
    std::string vertexSource;
    std::string fragmentSource;

    vertexSource = "void main()                                          \n"
                   "{\n"
                   "  vTexCoord = aTexCoord;\n"
                   "  gl_Position = uMvpMatrix * vec4( aPosition, 1.0 ); \n"

                   "}\n";

    fragmentSource = "precision lowp float;                                   \n"
                     " const vec3 zero = vec3( 0.0 );\n"
                     " const vec3 one = vec3( 1.0 );\n"
                      "const vec3 colour = vec3( 0.2, 0.8, 1.0 );\n"
                     "void main()                                                \n"
                     "{\n"
                     "  vec3 color = texture2D( sTexture, vTexCoord ).rgb;\n"
                     "  if ( color == zero )\n"
                     "  {\n"
                     "    discard;\n"
                     "  }\n"
                     "  else\n"
                     "  {\n"
                     "    gl_FragColor = vec4( color.rgb * colour, 1.0 );\n"
                     "  }\n"
                     "}\n";

    ShaderEffect shader = ShaderEffect::New( vertexSource,
                                             fragmentSource,
                                             GeometryType( GEOMETRY_TYPE_TEXTURED_MESH ),
                                             ShaderEffect::GeometryHints( ShaderEffect::HINT_NONE ) );

    TextShaderRGB handle( shader );
    return handle;
  }

  /**
   * Virtual destructor.
   */
  virtual ~TextShaderRGB()
  {
  }

private:

  /**
   * Helper for New()
   */
  TextShaderRGB( ShaderEffect handle )
  : ShaderEffect( handle )
  {
  }

};

const uint32_t test[] = { 0x41, 0x42, 0x43, 0x44,
                          //0x1f607, 0x1f608, 0x1f609, 0x1f60a,
                          0
                        };

// This example shows how to create and display Hello World! using a simple TextActor
//
class TextureAtlasExample : public ConnectionTracker
{
public:

  typedef uint32_t TextHandle;

  struct AtlasRecord
  {
    uint32_t mCharacter;
    uint32_t mImageID;
    float mAdvance;
    float mXBearing;
    float mYBearing;
  };

  struct MeshRecord
  {
    uint32_t mAtlasID;
    MeshData mMeshData;
  };

  struct CacheList
  {
    TextAbstraction::FontId mFont;
    Dali::Vector< AtlasRecord > mRecords;
  };

  struct TextRenderer
  {
    std::vector< MeshActor > mMeshActors;
    Dali::Vector< uint32_t > mImageIDs;
  };

  struct TextFormat
  {
    float mX;
    float mY;
    float mSpace;
    float mPointSize;
    uint32_t mCache;
    TextAbstraction::GlyphInfo mGlyphInfo;
    std::vector< MeshRecord > mMeshContainer;
  };

  TextureAtlasExample( Application& application )
  : mApplication( application ),
    mTimerSlot( this )
  {

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

    // Get a handle to the stage
    mStage = Stage::GetCurrent();
    mStageSize = mStage.GetSize();

    mDpi = Vector2( 96.0f, 96.0f );

    // Respond to a click anywhere on the stage
    mStage.GetRootLayer().TouchedSignal().Connect( this, &TextureAtlasExample::OnTouch );

    mTextShaderRGB = TextShaderRGB::New ();
    mAtlasManager = AtlasManager::New();

    mFontClient = mFontClient.Get();
    mFontClient.SetDpi( mDpi.x, mDpi.y );

    TextAbstraction::FontId header = mFontClient.GetFontId( "/usr/share/fonts/truetype/oxygen-fonts/Oxygen-Sans.ttf", 48.0*64 );
    TextAbstraction::FontId italics = mFontClient.GetFontId( "/usr/share/fonts/truetype/ttf-dejavu/DejaVuSerif-BoldItalic.ttf", 20.0*64 );
    TextAbstraction::FontId normal = mFontClient.GetFontId( "/usr/share/fonts/truetype/ubuntu-font-family/UbuntuMono-R.ttf", 18.0*64 );
    mItalics = mFontClient.GetFontId( "/usr/share/fonts/truetype/ttf-dejavu/DejaVuSerif-BoldItalic.ttf", 14.0*64 );
    mEmoticons = mFontClient.GetFontId( "/home/r.underhill/Documents/FruityGirl.ttf", 32.0 *64 );
    //mEmoticons = mFontClient.GetFontId( "/usr/share/fonts/truetype/ttf-dejavu/DejaVuSans-Oblique.ttf", 32 );

    RenderText( 120, 64, header, "Atlas and Text Print Example", 48.0f );
    RenderText( 140, 128, italics, "The Quick Brown Fox Jumps Over The Lazy Dog!", 20.0f );
    mTextToRemove = RenderText( 100, 160, italics, "!!This TEXT will DISAPPEAR ALMOST IMMEDIATELY!!", 20.0f );
    RenderText( 100, 224, normal, "One bright day in the middle of the night,", 18.0f );
    RenderText( 100, 256, normal, "Two dead boys got up to fight.", 18.0f);
    RenderText( 100, 288, normal, "Back to back they faced each other,", 18.0f );
    RenderText( 100, 320, normal, "Drew their swords and shot each other.", 18.0f );

    //RenderUnicodeText( 100, 1000, mEmoticons, test );

    ImageActor actor = ImageActor::New( mAtlasManager.GetAtlasContainer( 1u ) );
    actor.SetParentOrigin( Vector3( 0.5f, 0.68f, 0.5f ) );
    actor.SetAnchorPoint( AnchorPoint::CENTER );
    actor.SetSize( 512.0f, 512.0f );
    mStage.Add( actor );

    mTimer = Dali::Timer::New( 500.0f );
    mTimer.TickSignal().Connect( mTimerSlot, &TextureAtlasExample::OnTick );
    mTimer.Start();
    mAdd = true;
    mCleared = false;
  }

  TextHandle RenderText( uint32_t x, uint32_t y, const TextAbstraction::FontId& font, const char* text, float pointSize )
  {
    TextFormat format;
    TextRenderer renderer;
    PrepareText( x, y, font, format );
    //format.mPointSize = mFontClient.GetPointSize( font );
    format.mPointSize = pointSize;

    uint32_t index = 0;
    while ( text[ index ] )
    {
      if ( 0x20 != text[ index ] )
      {
        uint32_t character = static_cast< uint32_t >( text[ index ] );
        RenderUnicodeCharacter( character, format, renderer );
      }
      else
      {
        format.mX += format.mSpace;
      }
      index++;
    }

    CreateMeshActors( format, renderer );
    mTextLabels.push_back( renderer );
    return ( mTextLabels.size() - 1u );
  }

  void PrepareText( uint32_t x, uint32_t y, const TextAbstraction::FontId& font, TextFormat& format )
  {
    format.mX = static_cast< float >( x ) - ( mStageSize.x * 0.5f );
    format.mY = static_cast< float >( y ) - ( mStageSize.y * 0.5f );
    format.mGlyphInfo.fontId = font;

    // Get the metrics for a space in this font
    format.mGlyphInfo.index = mFontClient.GetGlyphIndex( font, 0x20 );
    mFontClient.GetGlyphMetrics( &format.mGlyphInfo, 1 );
    format.mSpace = format.mGlyphInfo.advance;

    // Get the correct cache list for this font ( or create a new one if it doesn't exist )
    bool foundCache = false;
    for ( uint32_t i = 0; i < mCacheList.size(); ++i )
    {
      if ( font == mCacheList[ i ].mFont )
      {
        foundCache = true;
        format.mCache = i;
        return;
      }
    }
    if ( !foundCache )
    {
      CacheList cacheList;
      cacheList.mFont = font;
      mCacheList.push_back( cacheList );
      format.mCache = mCacheList.size() - 1u;
    }
  }

  TextHandle RenderUnicodeText( uint32_t x, uint32_t y, const TextAbstraction::FontId& font, const uint32_t* text )
  {
    TextFormat format;
    TextRenderer renderer;
    PrepareText( x, y, font, format );
    format.mPointSize = 96.0f;

    uint32_t index = 0;
    while ( text[ index ] )
    {
      if ( 0x20 != text[ index ] )
      {
        RenderUnicodeCharacter( text[ index ], format, renderer );
      }
      else
      {
        format.mX += format.mSpace;
      }
      index++;
    }

    CreateMeshActors( format, renderer );
    mTextLabels.push_back( renderer );
    return ( mTextLabels.size() - 1u );
  }


  void CreateMeshActors( const TextFormat& format, TextRenderer& textRenderer )
  {
    // For each MeshData object, create a MeshActor
    for ( uint32_t i = 0; i < format.mMeshContainer.size(); ++i )
    {
      Mesh mesh = Mesh::New( format.mMeshContainer[ i ].mMeshData );
      MeshActor meshActor = MeshActor::New( mesh );
      meshActor.SetPositionInheritanceMode( Dali::DONT_INHERIT_POSITION );
      meshActor.SetPosition( 0.0f, 0.0f, 0.0f );
      meshActor.SetScale( 1.0f );
      meshActor.SetAffectedByLighting( false );
      meshActor.SetShaderEffect( mTextShaderRGB );
      mStage.Add( meshActor );
      textRenderer.mMeshActors.push_back( meshActor );
    }
  }

  void RenderUnicodeCharacter( uint32_t character, TextFormat& format, TextRenderer& renderer )
  {
    MeshData newMeshData;
    AtlasRecord record;
    AtlasManager::AtlasSlot slot;
    uint32_t cache = format.mCache;
    float pointSize = format.mPointSize;

    // Does the character already exist?
    bool characterCached = false;

    for ( uint32_t i = 0; i < mCacheList[ cache ].mRecords.Size(); ++i )
    {
      if ( mCacheList[ cache ].mRecords[ i ].mCharacter == character )
      {
        mAtlasManager.GenerateMeshData( mCacheList[ cache ].mRecords[ i ].mImageID, mCacheList[ cache ].mRecords[ i ].mXBearing,
                                        mCacheList[ cache ].mRecords[ i ].mYBearing, newMeshData );

        StitchTextMesh( format.mMeshContainer, newMeshData, Vector3( format.mX, format.mY, 0.0f ), slot );
        characterCached = true;
        format.mX += mCacheList[ cache ].mRecords[ i ].mAdvance;
        renderer.mImageIDs.PushBack( mCacheList[ cache ].mRecords[ i ].mImageID );
        break;
      }
    }

    if ( !characterCached  )
    {
      record.mCharacter = character;
      format.mGlyphInfo.index = mFontClient.GetGlyphIndex( format.mGlyphInfo.fontId, character );

      bool gotMetrics = mFontClient.GetGlyphMetrics( &format.mGlyphInfo, 1 );
      if ( gotMetrics )
      {
        BitmapImage bitmap = mFontClient.CreateBitmap( format.mGlyphInfo.fontId, format.mGlyphInfo.index );
        mAtlasManager.Add( bitmap, Vector2( pointSize, pointSize ), slot );

        record.mAdvance = format.mGlyphInfo.advance;
        record.mXBearing = format.mGlyphInfo.xBearing;
        record.mYBearing = format.mGlyphInfo.yBearing;
        record.mImageID = slot.mImage;

        mAtlasManager.GenerateMeshData( slot.mImage, record.mXBearing, record.mYBearing, newMeshData );
        StitchTextMesh( format.mMeshContainer, newMeshData, Vector3( format.mX, format.mY, 0.0f ), slot );

        format.mX += record.mAdvance;
        mCacheList[ cache ].mRecords.PushBack( record );
        renderer.mImageIDs.PushBack( slot.mImage );
      }
    }
  }


  void StitchTextMesh( std::vector< MeshRecord >& meshContainer, MeshData& newMeshData, const Vector3& offset, const AtlasManager::AtlasSlot& slot )
  {
    // Make sure that stitched meshes will be in the same atlas
    bool stitched = false;
    for ( uint32_t i = 0; i < meshContainer.size(); ++i )
    {
      if ( slot.mAtlas == meshContainer[ i ].mAtlasID )
      {
        // Stitch the mesh to the existing mesh
        stitched = true;
        mAtlasManager.StitchMesh( meshContainer[ i ].mMeshData, newMeshData, offset, meshContainer[ i ].mMeshData );
        break;
      }
    }

    // If the mesh was not stitched then create a new meshData object
    if ( !stitched )
    {
      MeshData emptyMesh;
      MeshRecord meshRecord;
      meshRecord.mAtlasID = slot.mAtlas;
      mAtlasManager.StitchMesh( emptyMesh, newMeshData, offset, emptyMesh );
      meshRecord.mMeshData = emptyMesh;
      meshContainer.push_back( meshRecord );
    }
  }

  void RemoveText( TextRenderer& textRenderer )
  {
    for ( uint32_t i = 0; i < textRenderer.mImageIDs.Size(); ++i )
    {
      if ( mAtlasManager.Remove( textRenderer.mImageIDs[ i ] ) )
      {
        for ( uint32_t c = 0; c < mCacheList.size(); ++c )
        {
          for ( uint32_t d = 0; d < mCacheList[ c ].mRecords.Size(); ++d )
          {
            if ( mCacheList[ c ].mRecords[ d ].mImageID == textRenderer.mImageIDs[ i ] )
            {
              mCacheList[ c ].mRecords.Remove( mCacheList[ c ].mRecords.Begin() + d );
            }
          }
        }
      }
    }

    for ( uint32_t i = 0; i < textRenderer.mMeshActors.size(); ++i )
    {
      mStage.Remove( textRenderer.mMeshActors[ i ] );
    }
  }


  bool OnTick()
  {
    if ( !mCleared )
    {
      RemoveText( mTextLabels[ mTextToRemove ] );
      mCleared = true;
    }
    if ( mAdd )
    {
      mFlashingText = RenderText( 64, 380, mItalics, "*****This Text Will Be Added and Removed Continuously to Test Reference Counting!*****", 14.0f );
    }
    else
    {
      RemoveText( mTextLabels[ mFlashingText ] );
      mTextLabels.erase( mTextLabels.begin() + mFlashingText );
    }
    mAdd ^= 1u;
    return true;

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
  Vector2 mDpi;
  Vector2 mStageSize;
  AtlasManager mAtlasManager;
  std::vector< CacheList > mCacheList;
  std::vector< TextRenderer > mTextLabels;
  TextShaderRGB mTextShaderRGB;
  TextAbstraction::FontClient mFontClient;
  TextHandle mTextToRemove;
  TextHandle mFlashingText;
  ImageActor mGrayAtlas;
  SlotDelegate< TextureAtlasExample > mTimerSlot;
  Dali::Timer mTimer;
  bool mAdd;
  bool mCleared;
  TextAbstraction::FontId mItalics;
  TextAbstraction::FontId mEmoticons;
};

void RunTest( Application& application )
{
  TextureAtlasExample test( application );

  application.MainLoop();
}

// slot point for Linux & SLP applications
//
int main( int argc, char **argv )
{
  Application application = Application::New( &argc, &argv );

  RunTest( application );

  return 0;
}
