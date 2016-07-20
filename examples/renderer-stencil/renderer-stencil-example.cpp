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

#include <dali-toolkit/dali-toolkit.h>
#include <dali/devel-api/adaptor-framework/bitmap-loader.h>
//todor
#include "shared/view.h"
#include "shared/utility.h"
#include <iostream>
#include <dali/public-api/rendering/renderer.h>
#include <stdio.h>
#include <sstream>
#include <cstring>

using namespace Dali;
using Dali::Toolkit::TextLabel;

namespace
{

const char * const CUBE_TEXTURE  = DEMO_IMAGE_DIR "people-medium-1.jpg";
const char * const FLOOR_TEXTURE = DEMO_IMAGE_DIR "people-medium-2.jpg";

#if 0
const Vector4 CUBE_COLOR       = Vector4( 1.0f,  1.0f,  1.0f, 1.0f );
const Vector4 FLOOR_COLOR      = Vector4( 0.8f,  0.3f,  0.6f, 1.0f );
const Vector4 REFLECTION_COLOR = Vector4( 0.6f,  0.3f, 0.5f, 0.5f );
#else
const Vector4 CUBE_COLOR       = Vector4( 1.0f,  1.0f,  1.0f, 1.0f );
const Vector4 FLOOR_COLOR      = Vector4::ONE;
const Vector4 REFLECTION_COLOR = Vector4( 0.6f,  0.6f, 0.6f, 0.6f );
#endif

// Shader properties
const char * const COLOR_UNIFORM_NAME( "uColor" );
const char * const OBJECT_DIMENSIONS_UNIFORM_NAME( "uObjectDimensions" );
const char * const STAGE_SIZE_UNIFORM_NAME( "uStageSize" );
const char * const LIGHT_POSITION_UNIFORM_NAME = "uLightPosition";
const char * const POSITION( "aPosition");
const char * const NORMAL( "aNormal" );
const char * const TEXTURE( "aTexCoord" );


const char* VERTEX_SHADER = DALI_COMPOSE_SHADER(
  attribute mediump vec3  aPosition;
  attribute highp   vec3  aNormal;
  attribute highp   vec2  aTexCoord;

  varying   mediump vec2  vTexCoord;
  uniform   mediump mat4  uMvpMatrix;
  uniform   mediump vec3  uSize;
  uniform   mediump vec3  uObjectDimensions;
  varying   mediump vec3  vIllumination;
  uniform   mediump mat4  uModelView;
  uniform   mediump mat4  uViewMatrix;
  uniform   mediump mat3  uNormalMatrix;
  uniform   mediump vec3  uLightPosition;

  void main()
  {
    mediump vec4 vertexPosition = vec4( aPosition * uObjectDimensions, 1.0 );
    vertexPosition = uMvpMatrix * vertexPosition;

    vec4 mvVertexPosition = uModelView * vertexPosition;

    vec3 vectorToLight = normalize( mat3( uViewMatrix ) * uLightPosition - mvVertexPosition.xyz );

    vec3 normal = uNormalMatrix * aNormal;
    float lightDiffuse = max( dot( vectorToLight, normal ), 0.0 );
    vIllumination = vec3( lightDiffuse * 0.5 + 0.5 );

    gl_Position = vertexPosition;
  }
);

const char* FRAGMENT_SHADER = DALI_COMPOSE_SHADER(
  varying mediump vec2  vTexCoord;
  varying mediump vec3  vIllumination;
  uniform lowp    vec4  uColor;
  uniform sampler2D     sTexture;

  void main()
  {
    gl_FragColor = vec4( vIllumination.rgb * uColor.rgb, uColor.a );
  }
);

const char* VERTEX_SHADER_TEXTURED = DALI_COMPOSE_SHADER(
  attribute mediump vec3  aPosition;
  attribute highp   vec3  aNormal;
  attribute highp   vec2  aTexCoord;

  varying   mediump vec2  vTexCoord;
  uniform   mediump mat4  uMvpMatrix;
  uniform   mediump vec3  uSize;
  uniform   mediump vec3  uObjectDimensions;
  varying   mediump vec3  vIllumination;
  uniform   mediump mat4  uModelView;
  uniform   mediump mat4  uViewMatrix;
  uniform   mediump mat3  uNormalMatrix;
  uniform   mediump vec3  uLightPosition;

  void main()
  {
    mediump vec4 vertexPosition = vec4( aPosition * uObjectDimensions, 1.0 );
    vertexPosition = uMvpMatrix * vertexPosition;

    vec4 mvVertexPosition = uModelView * vertexPosition;

    vec3 vectorToLight = normalize( mat3( uViewMatrix ) * uLightPosition - mvVertexPosition.xyz );

    vec3 normal = uNormalMatrix * aNormal;
    float lightDiffuse = max( dot( vectorToLight, normal ), 0.0 );
    vIllumination = vec3( lightDiffuse * 0.5 + 0.5 );

    vTexCoord = aTexCoord;
    gl_Position = vertexPosition;
  }
);

const char* FRAGMENT_SHADER_TEXTURED = DALI_COMPOSE_SHADER(
  varying mediump vec2  vTexCoord;
  varying mediump vec3  vIllumination;
  uniform lowp    vec4  uColor;
  uniform sampler2D     sTexture;

  void main()
  {
    gl_FragColor = vec4( texture2D( sTexture, vTexCoord ).rgb * vIllumination.rgb * uColor.rgb, uColor.a );
  }
);

/**
 * @brief Loads a texture from a file
 * @param[in] imageUrl The url of the file
 * @param[in] generateMipmaps Indicates whether to generate mipmaps for the texture
 * @return A texture if loading succeeds, an empty handle otherwise
 */
Texture LoadTexture( const char* imageUrl, bool generateMipmaps )
{
  Texture texture;
  Dali::BitmapLoader loader = Dali::BitmapLoader::New( imageUrl );
  loader.Load();
  PixelData pixelData = loader.GetPixelData();
  if( pixelData )
  {
    texture = Texture::New( TextureType::TEXTURE_2D, pixelData.GetPixelFormat(), pixelData.GetWidth(), pixelData.GetHeight() );
    texture.Upload( pixelData );

    if( generateMipmaps )
    {
      texture.GenerateMipmaps();
    }
  }

  return texture;
}

} // Anonymous namespace

// This example shows how to create and display Hello World! using a simple TextActor
//
class HelloWorldController : public ConnectionTracker
{
public:

  HelloWorldController( Application& application )
  : mApplication( application )
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &HelloWorldController::Create );
  }

  ~HelloWorldController()
  {
    // Nothing to do here;
  }

  //Simple struct to store the position and normal of a single vertex.
  struct Vertex
  {
    Vertex()
    {}

    Vertex( const Vector3& position, const Vector3& normal )
    : position( position ), normal( normal )
    {}

    Vector3 position;
    Vector3 normal;
  };

  //Simple struct to store the position and normal of a single vertex.
  struct TexturedVertex
  {
    TexturedVertex()
    {}

    TexturedVertex( const Vector3& position, const Vector3& normal, const Vector2& textureCoord )
    : position( position ), normal( normal ), textureCoord( textureCoord )
    {}

    Vector3 position;
    Vector3 normal;
    Vector2 textureCoord;
  };

  Geometry CreateGeometry( Vector<Vertex>& vertices, Vector<unsigned short>& indices )
  {
    // Vertices
    Property::Map vertexFormat;
    vertexFormat[POSITION] = Property::VECTOR3;
    vertexFormat[NORMAL] = Property::VECTOR3;

    PropertyBuffer surfaceVertices = PropertyBuffer::New( vertexFormat );
    surfaceVertices.SetData( &vertices[0], vertices.Size() );

    Geometry geometry = Geometry::New();
    geometry.AddVertexBuffer( surfaceVertices );

    // Indices for triangle formulation
    geometry.SetIndexBuffer( &indices[0], indices.Size() );
    return geometry;
  }

  Geometry CreateTexturedGeometry( Vector<TexturedVertex>& vertices, Vector<unsigned short>& indices )
  {
    // Vertices
    Property::Map vertexFormat;
    vertexFormat[POSITION] = Property::VECTOR3;
    vertexFormat[NORMAL] = Property::VECTOR3;
    vertexFormat[TEXTURE] = Property::VECTOR2;

    PropertyBuffer surfaceVertices = PropertyBuffer::New( vertexFormat );
    surfaceVertices.SetData( &vertices[0], vertices.Size() );

    Geometry geometry = Geometry::New();
    geometry.AddVertexBuffer( surfaceVertices );

    // Indices for triangle formulation
    geometry.SetIndexBuffer( &indices[0], indices.Size() );
    return geometry;
  }

  Geometry CreatePlaneVertices( Vector2 dimensions )
  {
    Vector<TexturedVertex> vertices;
    Vector<unsigned short> indices;
    vertices.Resize( 4 );
    indices.Resize( 6 );

    float scaledX = 0.5 * dimensions.x;
    float scaledY = 0.5 * dimensions.y;

    vertices[0].position     = Vector3( -scaledX, -scaledY, 0.0f );
    vertices[0].textureCoord = Vector2( 0.0, 0.0f );
    vertices[1].position     = Vector3(  scaledX, -scaledY, 0.0f );
    vertices[1].textureCoord = Vector2( 1.0, 0.0f );
    vertices[2].position     = Vector3(  scaledX,  scaledY, 0.0f );
    vertices[2].textureCoord = Vector2( 1.0, 1.0f );
    vertices[3].position     = Vector3( -scaledX,  scaledY, 0.0f );
    vertices[3].textureCoord = Vector2( 0.0, 1.0f );

    for( int i = 0; i < 4; ++i )
    {
      vertices[i].normal = Vector3( 0, 0, -1 );
    }

    indices[0] = 0;
    indices[1] = 1;
    indices[2] = 2;
    indices[3] = 2;
    indices[4] = 3;
    indices[5] = 0;

    return CreateTexturedGeometry( vertices, indices );
  }

  Renderer CreateRenderer( Geometry geometry, Vector3 dimensions, bool textured, Vector4 color )
  {
    Stage stage = Stage::GetCurrent();
    Shader shader;

    if( textured )
    {
      shader = Shader::New( VERTEX_SHADER_TEXTURED, FRAGMENT_SHADER_TEXTURED );
    }
    else
    {
      shader = Shader::New( VERTEX_SHADER, FRAGMENT_SHADER );
    }

    shader.RegisterProperty( LIGHT_POSITION_UNIFORM_NAME, Vector3( -stage.GetSize().width / 2.0f, -stage.GetSize().width / 2.0f, 1000.0f ) );
    shader.RegisterProperty( COLOR_UNIFORM_NAME, color );
    shader.RegisterProperty( OBJECT_DIMENSIONS_UNIFORM_NAME, dimensions );

    return Renderer::New( geometry, shader );
  }

  Geometry CreateCubeVertices( Vector3 dimensions, bool openCube = false )
  {
    Vector<TexturedVertex> vertices;
    Vector<unsigned short> indices;
    int vertexIndex = 0; //Tracks progress through vertices.
    float scaledX = 0.5 * dimensions.x;
    float scaledY = -0.5 * dimensions.y; // Flip vertically
    float scaledZ = 0.5 * dimensions.z;

    vertices.Resize( 4 * ( openCube ? 4u : 6u ) );

    Vector<Vector3> positions; //Stores vertex positions, which are shared between vertexes at the same position but with a different normal.
    positions.Resize( 8 );
    Vector<Vector3> normals; //Stores normals, which are shared between vertexes of the same face.
    normals.Resize( 6 );

    positions[0] = Vector3( -scaledX,  scaledY, -scaledZ );
    positions[1] = Vector3(  scaledX,  scaledY, -scaledZ );
    positions[2] = Vector3(  scaledX,  scaledY,  scaledZ );
    positions[3] = Vector3( -scaledX,  scaledY,  scaledZ );
    positions[4] = Vector3( -scaledX, -scaledY, -scaledZ );
    positions[5] = Vector3(  scaledX, -scaledY, -scaledZ );
    positions[6] = Vector3(  scaledX, -scaledY,  scaledZ );
    positions[7] = Vector3( -scaledX, -scaledY,  scaledZ );

    normals[0] = Vector3(  0,  1,  0 );
    normals[1] = Vector3(  0,  0, -1 );
    normals[2] = Vector3(  1,  0,  0 );
    normals[3] = Vector3(  0,  0,  1 );
    normals[4] = Vector3( -1,  0,  0 );
    normals[5] = Vector3(  0, -1,  0 );

    if( !openCube )
    {
      //Top face, upward normals.
      for( int i = 0; i < 4; i++, vertexIndex++ )
      {
        vertices[vertexIndex].position = positions[i];
        vertices[vertexIndex].normal = normals[0];
        // The below logic forms the correct U/V pairs for a quad when "i" goes from 0 to 3.
        vertices[vertexIndex].textureCoord = Vector2( ( i == 1 || i == 2 ) ? 1.0f : 0.0f, ( i == 2 || i == 3 ) ? 1.0f : 0.0f );
      }
    }

    //Top face, outward normals.
    for( int i = 0; i < 4; i++, vertexIndex += 2 )
    {
      vertices[vertexIndex].position = positions[i];
      vertices[vertexIndex].normal = normals[i + 1];

      if( i == 3 )
      {
        //End, so loop around.
        vertices[vertexIndex + 1].position = positions[0];
      }
      else
      {
        vertices[vertexIndex + 1].position = positions[i + 1];
      }
      vertices[vertexIndex + 1].normal = normals[i + 1];

      vertices[vertexIndex].textureCoord = Vector2( 0.0f, 0.0f );
      vertices[vertexIndex+1].textureCoord = Vector2( 1.0f, 0.0f );
    }

    //Bottom face, outward normals.
    for( int i = 0; i < 4; i++, vertexIndex += 2 )
    {
      vertices[vertexIndex].position = positions[i + 4];
      vertices[vertexIndex].normal = normals[i + 1];

      if( i == 3 )
      {
        //End, so loop around.
        vertices[vertexIndex + 1].position = positions[4];
      }
      else
      {
        vertices[vertexIndex + 1].position = positions[i + 5];
      }
      vertices[vertexIndex + 1].normal = normals[i + 1];

      vertices[vertexIndex].textureCoord = Vector2( 0.0f, 1.0f );
      vertices[vertexIndex+1].textureCoord = Vector2( 1.0f, 1.0f );
    }

    if( !openCube )
    {
      //Bottom face, downward normals.
      for( int i = 0; i < 4; i++, vertexIndex++ )
      {
        vertices[vertexIndex].position = positions[i + 4];
        vertices[vertexIndex].normal = normals[5];
        // The below logic forms the correct U/V pairs for a quad when "i" goes from 0 to 3.
        vertices[vertexIndex].textureCoord = Vector2( ( i == 1 || i == 2 ) ? 1.0f : 0.0f, ( i == 2 || i == 3 ) ? 1.0f : 0.0f );
      }
    }

    // Create cube indices.
    int numTriangles = openCube ? 8u : 12u;
    int triangleIndex = 0;  //Track progress through indices.

    indices.Resize( 3 * numTriangles );

    if( !openCube )
    {
      //Top face.
      indices[triangleIndex] =     0;
      indices[triangleIndex + 1] = 1;
      indices[triangleIndex + 2] = 2;
      indices[triangleIndex + 3] = 2;
      indices[triangleIndex + 4] = 3;
      indices[triangleIndex + 5] = 0;
      triangleIndex += 6;
    }

    int topFaceStart = openCube ? 0u : 4u;
    int bottomFaceStart = topFaceStart + 8u;

    //Side faces.
    for( int i = 0; i < 8; i += 2, triangleIndex += 6 )
    {
      indices[triangleIndex    ] = i + topFaceStart;
      indices[triangleIndex + 1] = i + bottomFaceStart + 1;
      indices[triangleIndex + 2] = i + topFaceStart + 1;
      indices[triangleIndex + 3] = i + topFaceStart;
      indices[triangleIndex + 4] = i + bottomFaceStart;
      indices[triangleIndex + 5] = i + bottomFaceStart + 1;
    }

    if( !openCube )
    {
      //Bottom face.
      indices[triangleIndex] =     20;
      indices[triangleIndex + 1] = 21;
      indices[triangleIndex + 2] = 22;
      indices[triangleIndex + 3] = 22;
      indices[triangleIndex + 4] = 23;
      indices[triangleIndex + 5] = 20;
    }

    return CreateTexturedGeometry( vertices, indices );
  }

  TextureSet LoadTexture( const char* url )
  {
    TextureSet textureSet = TextureSet::New();

    if( textureSet )
    {
      Texture texture = DemoHelper::LoadTexture( url );
      if( texture )
      {
        textureSet.SetTexture( 0u, texture );
      }
    }

    return textureSet;
  }

  // The Init signal is received once (only) during the Application lifetime
  void Create( Application& application )
  {
    // Get a handle to the stage
    Stage stage = Stage::GetCurrent();
    //stage.SetBackgroundColor( Color::WHITE );

#if 0
    TextLabel textLabel = TextLabel::New( "Hello World" );
    textLabel.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    textLabel.SetName( "helloWorldLabel" );
    stage.Add( textLabel );
#endif

    Layer layer = Layer::New();
    layer.SetAnchorPoint( AnchorPoint::CENTER );
    layer.SetParentOrigin( ParentOrigin::CENTER );
    layer.SetBehavior( Layer::LAYER_2D );
    layer.SetDepthTestDisabled( false );
    stage.Add( layer );

    float cubeHeight = 0.0f;

    float cubeActorWidth = 170.0f;
    Vector3 cubeActorSize( cubeActorWidth, cubeActorWidth, cubeActorWidth );

    Toolkit::Control container = Toolkit::Control::New();
    container.SetAnchorPoint( AnchorPoint::CENTER );
    container.SetParentOrigin( ParentOrigin::CENTER );
    container.SetSize( cubeActorSize );
    container.SetResizePolicy( ResizePolicy::FIXED, Dimension::ALL_DIMENSIONS );
    container.SetPosition( 0.0f, 0.0f - cubeHeight, 0.0f ); //todorkeep

    Vector3 unitySize( Vector3::ONE );
    Geometry cubeGeometry = CreateCubeVertices( unitySize );
    TextureSet cubeTextureSet = LoadTexture( CUBE_TEXTURE );
    Renderer renderer = CreateRenderer( cubeGeometry, cubeActorSize, true, CUBE_COLOR ); //todor works
    renderer.SetTextures( cubeTextureSet );

    renderer.SetProperty( Renderer::Property::FACE_CULLING_MODE, FaceCullingMode::FRONT );
    renderer.SetProperty( Renderer::Property::WRITE_TO_COLOR_BUFFER, true );

    renderer.SetProperty( Renderer::Property::STENCIL_MODE, StencilMode::OFF );
    renderer.SetProperty( Renderer::Property::DEPTH_WRITE_MODE, DepthWriteMode::AUTO );
    renderer.SetProperty( Renderer::Property::DEPTH_TEST_MODE, DepthWriteMode::AUTO );

    container.AddRenderer( renderer );
    layer.Add( container );


#if 1
    Vector3 planeSize( 320.0f, 320.0f, 0.0f );
    Toolkit::Control container2 = Toolkit::Control::New();
    container2.SetAnchorPoint( AnchorPoint::CENTER );
    container2.SetParentOrigin( ParentOrigin::CENTER );
    container2.SetSize( planeSize );
    container2.SetResizePolicy( ResizePolicy::FIXED, Dimension::ALL_DIMENSIONS );
    container2.SetPosition( 0.0f, cubeActorSize.y / 2.0f, 0.0f );
    container2.SetProperty( Actor::Property::ORIENTATION, Quaternion( Degree( -90.0f ), Degree( 0.0f ), Degree( 0.0f ) ) );

    Geometry planeGeometry = CreatePlaneVertices( unitySize.GetVectorXY() );
    Renderer renderer2 = CreateRenderer( planeGeometry, planeSize, false, Vector4::ONE );

    renderer2.SetProperty( Renderer::Property::STENCIL_MODE, StencilMode::ON );
    renderer2.SetProperty( Renderer::Property::STENCIL_FUNCTION, StencilFunction::ALWAYS );
    renderer2.SetProperty( Renderer::Property::STENCIL_FUNCTION_REFERENCE, 1 );
    renderer2.SetProperty( Renderer::Property::STENCIL_FUNCTION_MASK, 0xFF );
    renderer2.SetProperty( Renderer::Property::STENCIL_OPERATION_ON_FAIL, StencilOperation::KEEP );
    renderer2.SetProperty( Renderer::Property::STENCIL_OPERATION_ON_Z_FAIL, StencilOperation::KEEP );
    renderer2.SetProperty( Renderer::Property::STENCIL_OPERATION_ON_Z_PASS, StencilOperation::REPLACE );
    renderer2.SetProperty( Renderer::Property::STENCIL_MASK, 0xFF );

    renderer2.SetProperty( Renderer::Property::DEPTH_WRITE_MODE, DepthWriteMode::OFF );
    renderer2.SetProperty( Renderer::Property::DEPTH_TEST_MODE, DepthWriteMode::ON );

    container2.AddRenderer( renderer2 );
    layer.Add( container2 );
#endif


#if 1
    Toolkit::Control container2b = Toolkit::Control::New();
    container2b.SetAnchorPoint( AnchorPoint::CENTER );
    container2b.SetParentOrigin( ParentOrigin::CENTER );
    container2b.SetSize( planeSize );
    container2b.SetResizePolicy( ResizePolicy::FIXED, Dimension::ALL_DIMENSIONS );
    container2b.SetPosition( 0.0f, cubeActorSize.y / 2.0f, 0.0f );
    container2b.SetProperty( Actor::Property::ORIENTATION, Quaternion( Degree( -90.0f ), Degree( 0.0f ), Degree( 0.0f ) ) );

    TextureSet planeTextureSet = LoadTexture( FLOOR_TEXTURE );
    Renderer renderer2b = CreateRenderer( planeGeometry, planeSize, true, FLOOR_COLOR );
    renderer2b.SetTextures( planeTextureSet );

    renderer2b.SetProperty( Renderer::Property::DEPTH_WRITE_MODE, DepthWriteMode::OFF );
    renderer2b.SetProperty( Renderer::Property::DEPTH_TEST_MODE, DepthWriteMode::ON );

    container2b.AddRenderer( renderer2b );
    layer.Add( container2b );
#endif


#if 1
    Toolkit::Control container3 = Toolkit::Control::New();
    container3.SetAnchorPoint( AnchorPoint::CENTER );
    container3.SetParentOrigin( ParentOrigin::CENTER );
    container3.SetSize( cubeActorSize );
    container3.SetResizePolicy( ResizePolicy::FIXED, Dimension::ALL_DIMENSIONS );
    container3.SetPosition( 0.0f, cubeActorSize.y + cubeHeight, 0.0f );//todor 200.0f
    container3.SetProperty( Actor::Property::ORIENTATION, Quaternion( Degree( 180.0f ), Degree( 0.0f ), Degree( 0.0f ) ) );

    Renderer renderer3 = CreateRenderer( cubeGeometry, cubeActorSize, true, REFLECTION_COLOR );
    renderer3.SetTextures( cubeTextureSet );

    renderer3.SetProperty( Renderer::Property::FACE_CULLING_MODE, FaceCullingMode::FRONT );

    renderer3.SetProperty( Renderer::Property::BLEND_MODE, BlendMode::ON );
    renderer3.SetProperty( Renderer::Property::BLEND_EQUATION_RGB, BlendEquation::ADD );
    renderer3.SetProperty( Renderer::Property::BLEND_EQUATION_ALPHA, BlendEquation::ADD );
    renderer3.SetProperty( Renderer::Property::BLEND_FACTOR_DEST_RGB, BlendFactor::ONE );

    renderer3.SetProperty( Renderer::Property::STENCIL_MODE, StencilMode::ON );
    renderer3.SetProperty( Renderer::Property::STENCIL_FUNCTION, StencilFunction::EQUAL );
    renderer3.SetProperty( Renderer::Property::STENCIL_FUNCTION_REFERENCE, 1 );
    renderer3.SetProperty( Renderer::Property::STENCIL_FUNCTION_MASK, 0xFF );

    renderer3.SetProperty( Renderer::Property::DEPTH_WRITE_MODE, DepthWriteMode::ON );
    renderer3.SetProperty( Renderer::Property::DEPTH_TEST_MODE, DepthWriteMode::ON );

    container3.AddRenderer( renderer3 );
    layer.Add( container3 );
#endif


#if 1
    Vector3 openCubeSize( 320.0f, 8.0f, 320.0f );
    Toolkit::Control container4 = Toolkit::Control::New();
    container4.SetAnchorPoint( AnchorPoint::CENTER );
    container4.SetParentOrigin( ParentOrigin::CENTER );
    container4.SetSize( openCubeSize );
    container4.SetResizePolicy( ResizePolicy::FIXED, Dimension::ALL_DIMENSIONS );
    container4.SetPosition( 0.0f, ( cubeActorSize.y / 2.0f ) + ( openCubeSize.y / 2.0f ), 0.0f );

    // Create an open cube (a cube without the top and bottom faces).
    Geometry openCubeGeometry = CreateCubeVertices( unitySize, true );
    Renderer renderer4 = CreateRenderer( openCubeGeometry, openCubeSize, false, FLOOR_COLOR );

    renderer4.SetProperty( Renderer::Property::FACE_CULLING_MODE, FaceCullingMode::FRONT );

    renderer4.SetProperty( Renderer::Property::STENCIL_MODE, StencilMode::OFF );

    renderer4.SetProperty( Renderer::Property::DEPTH_WRITE_MODE, DepthWriteMode::ON );
    renderer4.SetProperty( Renderer::Property::DEPTH_TEST_MODE, DepthWriteMode::ON );


    container4.AddRenderer( renderer4 );
    layer.Add( container4 );
#endif




    layer.SetProperty( Actor::Property::ORIENTATION, Quaternion( Degree( -24.0f ), Degree( 0.0f ), Degree( 0.0f ) ) );

    mRotationAnimation = Animation::New( 8.0f );
    //mRotationAnimation.AnimateBy( Property( container, Actor::Property::ORIENTATION ),
    //                             Quaternion( Degree( 270.0f ), Degree( 360.0f ), Degree( 190.0f ) ) );
#if 1
    float rotang = 360.0f;
    mRotationAnimation.AnimateBy( Property( container, Actor::Property::ORIENTATION ),
                                 Quaternion( Degree( 0.0f ), Degree( rotang ), Degree( 0.0f ) ) );
    //mRotationAnimation.AnimateBy( Property( container, Actor::Property::ORIENTATION ),
    //                             Quaternion( Degree( 0.0f ), Degree( rotang ), Degree( 0.0f ) ) );


#if 1
    mRotationAnimation.AnimateBy( Property( container2, Actor::Property::ORIENTATION ),
                                 Quaternion( Degree( 0.0f ), Degree( 0.0f ), Degree( rotang ) ) );
#endif

#if 1
    mRotationAnimation.AnimateBy( Property( container2b, Actor::Property::ORIENTATION ),
                                 Quaternion( Degree( 0.0f ), Degree( 0.0f ), Degree( rotang ) ) );
#endif
#if 1
    // The reflection is rotated upside-down, so we rotate it the opposite direction on Y to match the cube above.
    mRotationAnimation.AnimateBy( Property( container3, Actor::Property::ORIENTATION ),
                                 Quaternion( Degree( 0.0f ), Degree( -rotang ), Degree( 0.0f ) ) );
#endif
#if 1
    mRotationAnimation.AnimateBy( Property( container4, Actor::Property::ORIENTATION ),
                                 Quaternion( Degree( 0.0f ), Degree( rotang ), Degree( 0.0f ) ) );
#endif


#if 0
    mRotationAnimation.AnimateBy( Property( container,  Actor::Property::SIZE_HEIGHT ), -cubeActorSize.y );
    mRotationAnimation.AnimateBy( Property( container,  Actor::Property::POSITION_Y ),   cubeActorSize.y / 2.0f );

    mRotationAnimation.AnimateBy( Property( container3, Actor::Property::SIZE_HEIGHT ), -cubeActorSize.y );
    mRotationAnimation.AnimateBy( Property( container3, Actor::Property::POSITION_Y ),  -cubeActorSize.y / 2.0f );
#endif




#endif
    mRotationAnimation.SetLooping( true );
    mRotationAnimation.Play();





    // Respond to a click anywhere on the stage
    stage.GetRootLayer().TouchSignal().Connect( this, &HelloWorldController::OnTouch );
  }

  bool OnTouch( Actor actor, const TouchData& touch )
  {
    // quit the application
    mApplication.Quit();
    return true;
  }

private:
  Application&  mApplication;

  Animation mRotationAnimation;

};

void RunTest( Application& application )
{
  HelloWorldController test( application );

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
