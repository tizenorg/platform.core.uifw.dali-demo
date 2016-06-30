/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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
#include <dali/devel-api/rendering/renderer.h>
#include <dali/devel-api/adaptor-framework/bitmap-loader.h>

#include <dali-toolkit/dali-toolkit.h>

using namespace Dali;
using Dali::Toolkit::TextLabel;

namespace
{

const char* IMAGE_FILENAME_ETC         =        DEMO_IMAGE_DIR "tx-etc1.ktx";
const char* IMAGE_FILENAME_ASTC_LINEAR =        DEMO_IMAGE_DIR "tx-astc-4x4-linear.ktx";
const char* IMAGE_FILENAME_ASTC_LINEAR_NATIVE = DEMO_IMAGE_DIR "tx-astc-4x4-linear-native.astc";

/**
 * @brief Create a renderer to render an image and adds it to an actor
 * @param[in] actor The actor that will be used to render the image
 * @param[in] imagePath The path where the image file is located
 */
void AddImage( Actor& actor, const char*imagePath )
{
  //Create the shader
  static Shader shader;
  if( !shader )
  {
    static const char* VERTEX_SHADER_TEXTURE = DALI_COMPOSE_SHADER(
        attribute mediump vec2 aPosition;\n
        attribute mediump vec2 aTexCoord;\n
        uniform mediump mat4 uMvpMatrix;\n
        uniform mediump vec3 uSize;\n
        varying mediump vec2 vTexCoord;\n
        void main()\n
        {\n
          vec4 position = vec4(aPosition,0.0,1.0)*vec4(uSize,1.0);\n
          gl_Position = uMvpMatrix * position;\n
          vTexCoord = aTexCoord;\n
        }\n
    );

    static const char* FRAGMENT_SHADER_TEXTURE = DALI_COMPOSE_SHADER(
        uniform lowp vec4 uColor;\n
        uniform sampler2D sTexture;\n
        varying mediump vec2 vTexCoord;\n

        void main()\n
        {\n
          gl_FragColor = texture2D( sTexture, vTexCoord ) * uColor;\n
        }\n
    );

    shader = Shader::New(VERTEX_SHADER_TEXTURE, FRAGMENT_SHADER_TEXTURE);
  }

  //Create the geometry
  static Geometry geometry;
  if( !geometry )
  {
    struct Vertex
    {
      Vector2 position;
      Vector2 texCoord;
    };

    static const Vertex gQuadWithTexture[] = {{ Vector2( -0.5f, -0.5f ), Vector2( 0.0f, 0.0f ) },
                                              { Vector2(  0.5f, -0.5f ), Vector2( 1.0f, 0.0f ) },
                                              { Vector2( -0.5f,  0.5f ), Vector2( 0.0f, 1.0f ) },
                                              { Vector2(  0.5f,  0.5f ), Vector2( 1.0f, 1.0f ) }};
    PropertyBuffer vertexBuffer;
    Property::Map vertexFormat;
    vertexFormat["aPosition"] = Property::VECTOR2;
    vertexFormat["aTexCoord"] = Property::VECTOR2;

    //Create a vertex buffer for vertex positions and texture coordinates
    vertexBuffer = PropertyBuffer::New( vertexFormat );
    vertexBuffer.SetData( gQuadWithTexture, 4u );

    //Create the geometry
    geometry = Geometry::New();
    geometry.AddVertexBuffer( vertexBuffer );
    geometry.SetGeometryType(Geometry::TRIANGLE_STRIP );
  }

  //Load the texture
  Dali::BitmapLoader loader = Dali::BitmapLoader::New( imagePath );
  loader.Load();
  PixelData pixelData = loader.GetPixelData();
  Texture texture = Texture::New( TextureType::TEXTURE_2D, pixelData.GetPixelFormat(), pixelData.GetWidth(), pixelData.GetHeight() );
  texture.Upload( pixelData );
  TextureSet textureSet = TextureSet::New();
  textureSet.SetTexture( 0u, texture );

  //Create the renderer
  Renderer renderer = Renderer::New( geometry, shader );
  renderer.SetTextures( textureSet );

  //Set actor size and add the renderer
  actor.SetSize( texture.GetWidth(), texture.GetHeight() );
  actor.AddRenderer( renderer );
}

}
/**
 * @brief This example shows 3 images, each of a different compressed texture type.
 * If built and run on a OpenGL ES 3.1 compatable target, then all 3 images will display.
 * Otherwise, the top image will display and the other 2 will appear as black squares.
 */
class CompressedTextureFormatsController : public ConnectionTracker
{
public:

  CompressedTextureFormatsController( Application& application )
  : mApplication( application )
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &CompressedTextureFormatsController::Create );
  }

  ~CompressedTextureFormatsController()
  {
    // Nothing to do here;
  }

  // The Init signal is received once (only) during the Application lifetime
  void Create( Application& application )
  {
    // Get a handle to the stage
    Stage stage = Stage::GetCurrent();
    stage.SetBackgroundColor( Color::WHITE );

    // Setup a TableView to hold a grid of images and labels.
    Toolkit::TableView table = Toolkit::TableView::New( 3u, 2u );
    table.SetAnchorPoint( AnchorPoint::CENTER );
    table.SetParentOrigin( ParentOrigin::CENTER );
    table.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
    table.SetRelativeWidth( 0u, 0.5f );
    table.SetRelativeWidth( 1u, 0.5f );
    table.SetRelativeHeight( 0u, 1.0f / 3.0f );
    table.SetRelativeHeight( 1u, 1.0f / 3.0f );
    table.SetRelativeHeight( 2u, 1.0f / 3.0f );


    // Add text labels.
    TextLabel textLabel = TextLabel::New( "ETC1 (KTX):" );
    textLabel.SetAnchorPoint( AnchorPoint::CENTER );
    textLabel.SetParentOrigin( ParentOrigin::CENTER );
    textLabel.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
    textLabel.SetProperty( Toolkit::TextLabel::Property::MULTI_LINE, true );
    table.AddChild( textLabel, Toolkit::TableView::CellPosition( 0u, 0u ) );
    table.SetCellAlignment( Toolkit::TableView::CellPosition( 0u, 0u ), HorizontalAlignment::LEFT, VerticalAlignment::CENTER );

    textLabel = TextLabel::New( "ASTC (KTX) 4x4 linear:" );
    textLabel.SetAnchorPoint( AnchorPoint::CENTER );
    textLabel.SetParentOrigin( ParentOrigin::CENTER );
    textLabel.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
    textLabel.SetProperty( Toolkit::TextLabel::Property::MULTI_LINE, true );
    table.AddChild( textLabel, Toolkit::TableView::CellPosition( 1u, 0u ) );
    table.SetCellAlignment( Toolkit::TableView::CellPosition( 1u, 0u ), HorizontalAlignment::LEFT, VerticalAlignment::CENTER );

    textLabel = TextLabel::New( "ASTC (Native) 4x4 linear:" );
    textLabel.SetAnchorPoint( AnchorPoint::CENTER );
    textLabel.SetParentOrigin( ParentOrigin::CENTER );
    textLabel.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
    textLabel.SetProperty( Toolkit::TextLabel::Property::MULTI_LINE, true );
    table.AddChild( textLabel, Toolkit::TableView::CellPosition( 2u, 0u ) );
    table.SetCellAlignment( Toolkit::TableView::CellPosition( 2u, 0u ), HorizontalAlignment::LEFT, VerticalAlignment::CENTER );

    // Add images.
    Actor actor = Actor::New();
    actor.SetAnchorPoint( AnchorPoint::CENTER );
    actor.SetParentOrigin( ParentOrigin::CENTER );
    AddImage( actor, IMAGE_FILENAME_ETC );
    table.AddChild( actor, Toolkit::TableView::CellPosition( 0u, 1u ) );
    table.SetCellAlignment( Toolkit::TableView::CellPosition( 0u, 1u ), HorizontalAlignment::CENTER, VerticalAlignment::CENTER );

    actor = Actor::New();
    actor.SetAnchorPoint( AnchorPoint::CENTER );
    actor.SetParentOrigin( ParentOrigin::CENTER );
    AddImage( actor, IMAGE_FILENAME_ASTC_LINEAR );
    table.AddChild( actor, Toolkit::TableView::CellPosition( 1u, 1u ) );
    table.SetCellAlignment( Toolkit::TableView::CellPosition( 1u, 1u ), HorizontalAlignment::CENTER, VerticalAlignment::CENTER );

    actor = Actor::New();
    actor.SetAnchorPoint( AnchorPoint::CENTER );
    actor.SetParentOrigin( ParentOrigin::CENTER );
    AddImage( actor, IMAGE_FILENAME_ASTC_LINEAR_NATIVE );
    table.AddChild( actor, Toolkit::TableView::CellPosition( 2u, 1u ) );
    table.SetCellAlignment( Toolkit::TableView::CellPosition( 2u, 1u ), HorizontalAlignment::CENTER, VerticalAlignment::CENTER );

    stage.Add( table );

    // Respond to a click anywhere on the stage
    stage.GetRootLayer().TouchSignal().Connect( this, &CompressedTextureFormatsController::OnTouch );
  }

  bool OnTouch( Actor actor, const TouchData& touch )
  {
    // quit the application
    mApplication.Quit();
    return true;
  }

private:
  Application&  mApplication;
};

void RunTest( Application& application )
{
  CompressedTextureFormatsController test( application );

  application.MainLoop();
}

// Entry point for Linux & Tizen applications
//
int DALI_EXPORT_API main( int argc, char **argv )
{
  Application application = Application::New( &argc, &argv );

  RunTest( application );

  return 0;
}
