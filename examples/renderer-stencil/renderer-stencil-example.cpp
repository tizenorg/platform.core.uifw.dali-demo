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
#include "shared/view.h"
#include "shared/utility.h"
//todor
#include <iostream>

using namespace Dali;

namespace
{

// Constants:

// Texture filenames:
const char * const CUBE_TEXTURE  = DEMO_IMAGE_DIR "people-medium-1.jpg";
const char * const FLOOR_TEXTURE = DEMO_IMAGE_DIR "wood.png";

//todor these values must be percentages of stage
// The width (and height + depth) of the main and reflection cubes.
const float   CUBE_WIDTH         = 150.0f;
// The width and height of the floor object.
const Vector2 FLOOR_DIMENSIONS   = Vector2( 320.0f, 8.0f );

// Base colors for the objects:
const Vector4 CUBE_COLOR         = Color::WHITE;
const Vector4 FLOOR_COLOR        = Color::WHITE;
const Vector4 REFLECTION_COLOR   = Vector4( 0.6f,  0.6f, 0.6f, 0.6f );

//todordel
#if 0
const Vector4 CUBE_COLOR         = Vector4( 1.0f,  1.0f,  1.0f, 1.0f );
const Vector4 FLOOR_COLOR        = Vector4( 0.8f,  0.3f,  0.6f, 1.0f );
const Vector4 REFLECTION_COLOR   = Vector4( 0.6f,  0.3f, 0.5f, 0.5f );
#endif

// Shader properties:
const char * const COLOR_UNIFORM_NAME( "uColor" );
const char * const OBJECT_DIMENSIONS_UNIFORM_NAME( "uObjectDimensions" );
const char * const STAGE_SIZE_UNIFORM_NAME( "uStageSize" );
const char * const LIGHT_POSITION_UNIFORM_NAME = "uLightPosition";
const char * const POSITION( "aPosition");
const char * const NORMAL( "aNormal" );
const char * const TEXTURE( "aTexCoord" );

//todor put in another file
// Shader for todor (vertex)
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

} // Anonymous namespace

/**
 * @brief This example shows how to manipulate stencil and depth buffer properties within the Renderer API.
 */
class RendererStencilExample : public ConnectionTracker
{
public:

  RendererStencilExample( Application& application )
  : mApplication( application )
  {
    // Connect to the Application's Init signal.
    mApplication.InitSignal().Connect( this, &RendererStencilExample::Create );
  }

  ~RendererStencilExample()
  {
  }

  /**
   * @brief Enum to facilitate more readable use of the cube array.
   */
  enum CubeType
  {
    MAIN_CUBE,
    REFLECTION_CUBE
  };

  /**
   * @brief Struct to store the position, normal and texture coordinates of a single vertex.
   */
  struct TexturedVertex
  {
    Vector3 position;
    Vector3 normal;
    Vector2 textureCoord;
  };

  /**
   * @brief This is the main scene setup method for this demo.
   * This is called via the Init signal which is received once (only) during the Application lifetime.
   * @param[in] application The DALi application object
   */
  void Create( Application& application )
  {
    Stage stage = Stage::GetCurrent();

    // Add a label as a heading for the demo.
    Toolkit::TextLabel textLabel = Toolkit::TextLabel::New( "Renderer Stencil API Demo" );
    textLabel.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::ALL_DIMENSIONS );
    textLabel.SetProperty( Toolkit::TextLabel::Property::TEXT_COLOR, Color::WHITE );
    textLabel.SetAnchorPoint( AnchorPoint::TOP_CENTER );
    // Set the parent origin to a percentage from the top (so the demo will scale for different resolutions).
    textLabel.SetParentOrigin( Vector3( 0.5f, 0.08f, 0.5f ) );
    textLabel.SetName( "infoLabel" );
    stage.Add( textLabel );

    // Layer to hold the 3D scene.
    Layer layer = Layer::New();
    layer.SetAnchorPoint( AnchorPoint::CENTER );
    // Set the parent origin to a small percentage below the center (so the demo will scale for different resolutions).
    layer.SetParentOrigin( Vector3( 0.5f, 0.58f, 0.5f ) );
    layer.SetBehavior( Layer::LAYER_2D );
    layer.SetDepthTestDisabled( false );
    stage.Add( layer );

    // Main cube:
    Vector3 cubeSize( CUBE_WIDTH, CUBE_WIDTH, CUBE_WIDTH );
    // Create the geometry for the cube, and the texture.
    Geometry cubeGeometry = CreateCubeVertices( Vector3::ONE, false );
    TextureSet cubeTextureSet = LoadTexture( CUBE_TEXTURE );
    // Create the cube object and add it.
    // Note: The cube is anchored around its base for animation purposes, so the position can be zero.
    mCubes[ MAIN_CUBE ] = CreateMainCubeObject( cubeGeometry, cubeSize, cubeTextureSet );
    layer.Add( mCubes[ MAIN_CUBE ] );

    // Floor:
    Vector3 floorSize( FLOOR_DIMENSIONS.x, FLOOR_DIMENSIONS.y, FLOOR_DIMENSIONS.x );
    // Create the floor object using the cube geometry with a new size, and add it.
    Actor floorObject( CreateFloorObject( cubeGeometry, floorSize ) );
    layer.Add( floorObject );

    // Stencil:
    Vector3 planeSize( FLOOR_DIMENSIONS.x, FLOOR_DIMENSIONS.x, 0.0f );
    // Create the stencil plane object, and add it.
    Actor stencilPlaneObject( CreateStencilPlaneObject( planeSize ) );
    layer.Add( stencilPlaneObject );

    // Reflection cube:
    // Create the reflection cube object and add it.
    // Note: The cube is anchored around its base for animation purposes, so the position can be zero.
    mCubes[ REFLECTION_CUBE ] = CreateReflectionCubeObject( cubeSize, cubeTextureSet );
    layer.Add( mCubes[ REFLECTION_CUBE ] );

    // Rotate the layer so we can see some of the top of the cube for a more 3D effect.
    layer.SetProperty( Actor::Property::ORIENTATION, Quaternion( Degree( -24.0f ), Degree( 0.0f ), Degree( 0.0f ) ) );

    // Set up the rotation on the Y axis.
    mRotationAnimation = Animation::New( 10.0f );
    float fullRotation = 360.0f;
    mRotationAnimation.AnimateBy( Property( mCubes[ MAIN_CUBE ], Actor::Property::ORIENTATION ),
                                 Quaternion( Degree( 0.0f ), Degree( fullRotation ), Degree( 0.0f ) ) );
    mRotationAnimation.AnimateBy( Property( floorObject, Actor::Property::ORIENTATION ),
                                 Quaternion( Degree( 0.0f ), Degree( fullRotation ), Degree( 0.0f ) ) );
    // Note the stencil is pre-rotated by 90 degrees on X, so we rotate relatively on its Z axis for an equivalent Y rotation.
    mRotationAnimation.AnimateBy( Property( stencilPlaneObject, Actor::Property::ORIENTATION ),
                                 Quaternion( Degree( 0.0f ), Degree( 0.0f ), Degree( fullRotation ) ) );
    mRotationAnimation.AnimateBy( Property( mCubes[ REFLECTION_CUBE ], Actor::Property::ORIENTATION ),
                                 Quaternion( Degree( 0.0f ), Degree( fullRotation ), Degree( 0.0f ) ) );
    mRotationAnimation.SetLooping( true );

    // Set up the cube bouncing animation.
    float totalTime = 1.6f;
    float halfTime = totalTime / 2.0f;
    float deformationAmount = 0.2f;
    float deformationTime = 0.4f;
    float halfDeformationTime = deformationTime / 2.0f;
    float heightChange = cubeSize.y / 2.5f;

    // First position the cubes at the top of the animation cycle.
    mCubes[ MAIN_CUBE ].SetProperty(       Actor::Property::POSITION_Y, -heightChange );
    mCubes[ REFLECTION_CUBE ].SetProperty( Actor::Property::POSITION_Y,  heightChange );

    mBounceAnimation = Animation::New( totalTime );

    // The animations for the main and reflected cubes are almost identical, so we combine the code to do both.
    for( int cube = 0; cube < 2; ++cube )
    {
      if( cube == 1 )
      {
        heightChange = -heightChange;
      }

      // 1st TimePeriod: Start moving down with increasing speed, until it is time to distort the cube due to impact.
      mBounceAnimation.AnimateBy( Property( mCubes[ cube ], Actor::Property::POSITION_Y ),  heightChange, AlphaFunction::EASE_IN_SQUARE, TimePeriod( 0.0f, halfTime - halfDeformationTime ) );

      // 2nd TimePeriod: The cube is touching the floor, start deforming it - then un-deform it again.
      mBounceAnimation.AnimateBy( Property( mCubes[ cube ], Actor::Property::SCALE_X ),  deformationAmount, AlphaFunction::BOUNCE, TimePeriod( halfTime - halfDeformationTime, deformationTime ) );
      mBounceAnimation.AnimateBy( Property( mCubes[ cube ], Actor::Property::SCALE_Z ),  deformationAmount, AlphaFunction::BOUNCE, TimePeriod( halfTime - halfDeformationTime, deformationTime ) );
      mBounceAnimation.AnimateBy( Property( mCubes[ cube ], Actor::Property::SCALE_Y ), -deformationAmount, AlphaFunction::BOUNCE, TimePeriod( halfTime - halfDeformationTime, deformationTime ) );

      // 3rd TimePeriod: Start moving up with decreasing speed, until at the apex of the animation.
      mBounceAnimation.AnimateBy( Property( mCubes[ cube ], Actor::Property::POSITION_Y ), -heightChange, AlphaFunction::EASE_OUT_SQUARE, TimePeriod( halfTime + halfDeformationTime, halfTime - halfDeformationTime ) );
    }

    mBounceAnimation.SetLooping( true );

    // Start the animations.
    mRotationAnimation.Play();
    mBounceAnimation.Play();

    // Respond to a click anywhere on the stage
    stage.GetRootLayer().TouchSignal().Connect( this, &RendererStencilExample::OnTouch );
    // Connect signals to allow Back and Escape to exit.
    stage.KeyEventSignal().Connect( this, &RendererStencilExample::OnKeyEvent );
  }

private:

  // Methods to setup each component of the 3D scene:

  Actor CreateMainCubeObject( Geometry& geometry, Vector3 size, TextureSet& textureSet )
  {
    // Main cube object. todor: move to doxy above
    Toolkit::Control container = Toolkit::Control::New();
    container.SetAnchorPoint( AnchorPoint::BOTTOM_CENTER );
    container.SetParentOrigin( ParentOrigin::BOTTOM_CENTER );
    container.SetSize( size );
    container.SetResizePolicy( ResizePolicy::FIXED, Dimension::ALL_DIMENSIONS );

    // Create a renderer from the geometry and add the texture.
    Renderer renderer = CreateRenderer( geometry, size, true, CUBE_COLOR );
    renderer.SetTextures( textureSet );

    // Setup the renderer properties:
    renderer.SetProperty( Renderer::Property::WRITE_TO_COLOR_BUFFER, true );
    renderer.SetProperty( Renderer::Property::FACE_CULLING_MODE, FaceCullingMode::BACK );

    renderer.SetProperty( Renderer::Property::STENCIL_MODE, StencilMode::OFF );

    renderer.SetProperty( Renderer::Property::DEPTH_WRITE_MODE, DepthWriteMode::ON );
    renderer.SetProperty( Renderer::Property::DEPTH_TEST_MODE, DepthTestMode::OFF );

    container.AddRenderer( renderer );
    return container;
  }

  Actor CreateFloorObject( Geometry& geometry, Vector3 size )
  {
    // Floor object.
    Toolkit::Control container = Toolkit::Control::New();
    container.SetAnchorPoint( AnchorPoint::TOP_CENTER );
    container.SetParentOrigin( ParentOrigin::TOP_CENTER );
    container.SetSize( size );
    container.SetResizePolicy( ResizePolicy::FIXED, Dimension::ALL_DIMENSIONS );

    // Create a renderer from the geometry and add the texture.
    TextureSet planeTextureSet = LoadTexture( FLOOR_TEXTURE );
    Renderer renderer = CreateRenderer( geometry, size, true, FLOOR_COLOR );
    renderer.SetTextures( planeTextureSet );

    // Setup the renderer properties:
    renderer.SetProperty( Renderer::Property::WRITE_TO_COLOR_BUFFER, true );
    renderer.SetProperty( Renderer::Property::FACE_CULLING_MODE, FaceCullingMode::BACK );

    renderer.SetProperty( Renderer::Property::STENCIL_MODE, StencilMode::OFF );

    renderer.SetProperty( Renderer::Property::DEPTH_WRITE_MODE, DepthWriteMode::OFF );
    renderer.SetProperty( Renderer::Property::DEPTH_TEST_MODE, DepthTestMode::ON );

    container.AddRenderer( renderer );
    return container;
  }

  Actor CreateStencilPlaneObject( Vector3 size )
  {
    // Stencil plane.
    Toolkit::Control container = Toolkit::Control::New();
    container.SetAnchorPoint( AnchorPoint::CENTER );
    container.SetParentOrigin( ParentOrigin::CENTER );
    container.SetSize( size );
    container.SetResizePolicy( ResizePolicy::FIXED, Dimension::ALL_DIMENSIONS );

    //todor comment
    container.SetProperty( Actor::Property::ORIENTATION, Quaternion( Degree( -90.0f ), Degree( 0.0f ), Degree( 0.0f ) ) );

    Geometry planeGeometry = CreatePlaneVertices( Vector2::ONE );
    // Create a renderer from the geometry.
    Renderer renderer = CreateRenderer( planeGeometry, size, false, Vector4::ONE );

    // Setup the renderer properties:
    // The stencil plane is only for stencilling, so disable writing to color buffer.
    renderer.SetProperty( Renderer::Property::WRITE_TO_COLOR_BUFFER, false );

    // Enable stencil. Draw to the stencil buffer (only).
    renderer.SetProperty( Renderer::Property::STENCIL_MODE, StencilMode::ON );
    renderer.SetProperty( Renderer::Property::STENCIL_FUNCTION, StencilFunction::ALWAYS );
    renderer.SetProperty( Renderer::Property::STENCIL_FUNCTION_REFERENCE, 1 );
    renderer.SetProperty( Renderer::Property::STENCIL_FUNCTION_MASK, 0xFF );
    renderer.SetProperty( Renderer::Property::STENCIL_OPERATION_ON_FAIL, StencilOperation::KEEP );
    renderer.SetProperty( Renderer::Property::STENCIL_OPERATION_ON_Z_FAIL, StencilOperation::KEEP );
    renderer.SetProperty( Renderer::Property::STENCIL_OPERATION_ON_Z_PASS, StencilOperation::REPLACE );
    renderer.SetProperty( Renderer::Property::STENCIL_MASK, 0xFF );

    // We don't want to write to the depth buffer, as this would block the reflection being drawn.
    renderer.SetProperty( Renderer::Property::DEPTH_WRITE_MODE, DepthWriteMode::OFF );
    // We test the depth buffer as we want the stencil to only exist underneath the cube.
    renderer.SetProperty( Renderer::Property::DEPTH_TEST_MODE, DepthTestMode::ON );

    container.AddRenderer( renderer );
    return container;
  }

  Actor CreateReflectionCubeObject( Vector3 size, TextureSet& textureSet )
  {
    // Cube reflection.
    Toolkit::Control container = Toolkit::Control::New();
    container.SetAnchorPoint( AnchorPoint::TOP_CENTER );
    container.SetParentOrigin( ParentOrigin::TOP_CENTER );
    container.SetSize( size );
    container.SetResizePolicy( ResizePolicy::FIXED, Dimension::ALL_DIMENSIONS );

    // Create the cube geometry of unity size.
    // The "true" specifies we want the texture UVs flipped vertically as this is the reflection cube.
    Geometry reflectedCubeGeometry = CreateCubeVertices( Vector3::ONE, true );
    // Create a renderer from the geometry and add the texture.
    Renderer renderer = CreateRenderer( reflectedCubeGeometry, size, true, REFLECTION_COLOR );
    renderer.SetTextures( textureSet );

    // Setup the renderer properties:
    renderer.SetProperty( Renderer::Property::WRITE_TO_COLOR_BUFFER, true );              ///< Write to color buffer so reflection is visible
    renderer.SetProperty( Renderer::Property::FACE_CULLING_MODE, FaceCullingMode::BACK ); ///< We cull to skip drawing the back faces

    renderer.SetProperty( Renderer::Property::BLEND_MODE, BlendMode::ON );                ///< We use blending to blend the reflection with the floor texture
    renderer.SetProperty( Renderer::Property::BLEND_EQUATION_RGB, BlendEquation::ADD );
    renderer.SetProperty( Renderer::Property::BLEND_EQUATION_ALPHA, BlendEquation::ADD );
    renderer.SetProperty( Renderer::Property::BLEND_FACTOR_DEST_RGB, BlendFactor::ONE );

    renderer.SetProperty( Renderer::Property::STENCIL_MODE, StencilMode::ON );            ///< Enable stencil
    renderer.SetProperty( Renderer::Property::STENCIL_FUNCTION, StencilFunction::EQUAL ); ///< Only draw to areas within the stencil
    renderer.SetProperty( Renderer::Property::STENCIL_FUNCTION_REFERENCE, 1 );
    renderer.SetProperty( Renderer::Property::STENCIL_FUNCTION_MASK, 0xff );
    renderer.SetProperty( Renderer::Property::STENCIL_MASK, 0x00 );                       ///< Don't write to the stencil

    renderer.SetProperty( Renderer::Property::DEPTH_WRITE_MODE, DepthWriteMode::OFF );    ///< We don't need to write to the depth buffer, as we are culling
    renderer.SetProperty( Renderer::Property::DEPTH_TEST_MODE, DepthTestMode::ON );       ///< We need to test the depth buffer as we need the reflection to be underneath the cube

    container.AddRenderer( renderer );
    return container;
  }

  // Methods:

  Geometry CreateTexturedGeometry( Vector<TexturedVertex>& vertices, Vector<unsigned short>& indices )
  {
    // Vertices
    Property::Map vertexFormat;
    vertexFormat[POSITION] = Property::VECTOR3;
    vertexFormat[NORMAL] =   Property::VECTOR3;
    vertexFormat[TEXTURE] =  Property::VECTOR2;

    PropertyBuffer surfaceVertices = PropertyBuffer::New( vertexFormat );
    surfaceVertices.SetData( &vertices[0u], vertices.Size() );

    Geometry geometry = Geometry::New();
    geometry.AddVertexBuffer( surfaceVertices );

    // Indices for triangle formulation
    geometry.SetIndexBuffer( &indices[0u], indices.Size() );
    return geometry;
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

  // Geometry Creation:

  Geometry CreatePlaneVertices( Vector2 dimensions )
  {
    Vector<TexturedVertex> vertices;
    Vector<unsigned short> indices;
    vertices.Resize( 4u );
    indices.Resize( 6u );

    float scaledX = 0.5f * dimensions.x;
    float scaledY = 0.5f * dimensions.y;

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
      vertices[i].normal = Vector3( 0.0f, 0.0f, -1.0f );
    }

    indices[0] = 0;
    indices[1] = 1;
    indices[2] = 2;
    indices[3] = 2;
    indices[4] = 3;
    indices[5] = 0;

    return CreateTexturedGeometry( vertices, indices );
  }

  Geometry CreateCubeVertices( Vector3 dimensions, bool reflectVerticalUVs )
  {
    Vector<TexturedVertex> vertices;
    Vector<unsigned short> indices;
    int vertexIndex = 0u; // Tracks progress through vertices.
    float scaledX = 0.5f * dimensions.x;
    float scaledY = 0.5f * dimensions.y;
    float scaledZ = 0.5f * dimensions.z;
    float verticalTextureCoord = reflectVerticalUVs ? 0.0f : 1.0f;

    vertices.Resize( 4u * 6u ); // 4 vertices x 6 faces

    Vector<Vector3> positions;  // Stores vertex positions, which are shared between vertexes at the same position but with a different normal.
    positions.Resize( 8u );
    Vector<Vector3> normals;    // Stores normals, which are shared between vertexes of the same face.
    normals.Resize( 6u );

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

    // Top face, upward normals.
    for( int i = 0; i < 4; ++i, ++vertexIndex )
    {
      vertices[vertexIndex].position = positions[i];
      vertices[vertexIndex].normal = normals[0];
      // The below logic forms the correct U/V pairs for a quad when "i" goes from 0 to 3.
      vertices[vertexIndex].textureCoord = Vector2( ( i == 1 || i == 2 ) ? 1.0f : 0.0f, ( i == 2 || i == 3 ) ? 1.0f : 0.0f );
    }

    // Top face, outward normals.
    for( int i = 0; i < 4; ++i, vertexIndex += 2 )
    {
      vertices[vertexIndex].position = positions[i];
      vertices[vertexIndex].normal = normals[i + 1];

      if( i == 3 )
      {
        // End, so loop around.
        vertices[vertexIndex + 1].position = positions[0];
      }
      else
      {
        vertices[vertexIndex + 1].position = positions[i + 1];
      }
      vertices[vertexIndex + 1].normal = normals[i + 1];

      vertices[vertexIndex].textureCoord = Vector2( 0.0f, verticalTextureCoord );
      vertices[vertexIndex+1].textureCoord = Vector2( 1.0f, verticalTextureCoord );
    }

    // Flip the vertical texture coord for the UV values of the bottom points.
    verticalTextureCoord = 1.0f - verticalTextureCoord;

    // Bottom face, outward normals.
    for( int i = 0; i < 4; ++i, vertexIndex += 2 )
    {
      vertices[vertexIndex].position = positions[i + 4];
      vertices[vertexIndex].normal = normals[i + 1];

      if( i == 3 )
      {
        // End, so loop around.
        vertices[vertexIndex + 1].position = positions[4];
      }
      else
      {
        vertices[vertexIndex + 1].position = positions[i + 5];
      }
      vertices[vertexIndex + 1].normal = normals[i + 1];

      vertices[vertexIndex].textureCoord = Vector2( 0.0f, verticalTextureCoord );
      vertices[vertexIndex+1].textureCoord = Vector2( 1.0f, verticalTextureCoord );
    }

    // Bottom face, downward normals.
    for( int i = 0; i < 4; ++i, ++vertexIndex )
    {
      // Reverse positions for bottom face to keep triangles clockwise (for culling).
      vertices[vertexIndex].position = positions[ 7 - i ];
      vertices[vertexIndex].normal = normals[5];
      // The below logic forms the correct U/V pairs for a quad when "i" goes from 0 to 3.
      vertices[vertexIndex].textureCoord = Vector2( ( i == 1 || i == 2 ) ? 1.0f : 0.0f, ( i == 2 || i == 3 ) ? 1.0f : 0.0f );
    }

    // Create cube indices.
    int triangleIndex = 0u;     //Track progress through indices.
    indices.Resize( 3u * 12u ); // 3 points x 12 triangles.

    // Top face.
    indices[triangleIndex] =     0;
    indices[triangleIndex + 1] = 1;
    indices[triangleIndex + 2] = 2;
    indices[triangleIndex + 3] = 2;
    indices[triangleIndex + 4] = 3;
    indices[triangleIndex + 5] = 0;
    triangleIndex += 6;

    int topFaceStart = 4u;
    int bottomFaceStart = topFaceStart + 8u;

    // Side faces.
    for( int i = 0; i < 8; i += 2, triangleIndex += 6 )
    {
      indices[triangleIndex    ] = i + topFaceStart;
      indices[triangleIndex + 1] = i + bottomFaceStart + 1;
      indices[triangleIndex + 2] = i + topFaceStart + 1;
      indices[triangleIndex + 3] = i + topFaceStart;
      indices[triangleIndex + 4] = i + bottomFaceStart;
      indices[triangleIndex + 5] = i + bottomFaceStart + 1;
    }

    // Bottom face.
    indices[triangleIndex] =     20;
    indices[triangleIndex + 1] = 21;
    indices[triangleIndex + 2] = 22;
    indices[triangleIndex + 3] = 22;
    indices[triangleIndex + 4] = 23;
    indices[triangleIndex + 5] = 20;

    return CreateTexturedGeometry( vertices, indices );
  }

  // Signal handlers:

  bool OnTouch( Actor actor, const TouchData& touch )
  {
    // Quit the application.
    mApplication.Quit();
    return true;
  }

  void OnKeyEvent( const KeyEvent& event )
  {
    if( event.state == KeyEvent::Down )
    {
      if ( IsKey( event, Dali::DALI_KEY_ESCAPE ) || IsKey( event, Dali::DALI_KEY_BACK ) )
      {
        mApplication.Quit();
      }
    }
  }

private:

  // Member variables:

  Application&  mApplication;       ///< todor

  Animation     mRotationAnimation; ///< todor
  Animation     mBounceAnimation;   ///< todor
  Actor         mCubes[2];          ///< todor
};


/**
 * @brief todor
 * @param[in] application
 */
void RunTest( Application& application )
{
  RendererStencilExample example( application );

  application.MainLoop();
}

/**
 * @brief Entry point for Linux & Tizen applications
 * @param[in] argc todor
 * @param[in] argv
 * @return The executables exit code (0)
 */
int DALI_EXPORT_API main( int argc, char **argv )
{
  Application application = Application::New( &argc, &argv );

  RunTest( application );

  return 0;
}
