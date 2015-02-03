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

#define MAKE_STRING(A)#A

using namespace Dali;

namespace
{
const char* BACKGROUND_IMAGE( DALI_IMAGE_DIR "background-gradient.jpg" );
const char* CONTACTS_IMAGE( DALI_IMAGE_DIR "music-libray-main-screen.png" );
const char* LOGO_IMAGE( DALI_IMAGE_DIR "spot_button_on.png" );

std::string sPopupVertexShader = MAKE_STRING(
  uniform vec3 uPoints[5];\n
  void main()\n
  {\n
    int vertexId = int(aNormal.z);\n
    gl_Position = uMvpMatrix * vec4(uPoints[vertexId], 1.0);\n
    vNormal = aNormal;\n
  }\n
);

std::string sPopupFragmentShader = MAKE_STRING(
  void main()\n
  {\n
    highp float C = (vNormal.x*vNormal.x-vNormal.y);\n
      if( C > 0.0 )\n
        discard;\n
      else\n
        gl_FragColor = vec4(1.0, 1.0 ,1.0, 1.0 );\n
  }\n
);


std::string sCircleVertexShader = MAKE_STRING(
  uniform highp vec3 uPoints[7];\n
  highp mat4 m3 = mat4( 1.0, -3.0, 3.0, -1.0, 0.0, 3.0, -6.0, 3.0, 0.0, 0.0, 3.0, -3.0, 0.0, 0.0, 0.0, 1.0 );\n
  highp mat4 m3Inverse = mat4( 1.0, 1.0, 1.0, 1.0, 0.0, 1.0/3.0, 2.0/3.0, 1.0, 0.0, 0.0, 1.0/3.0, 1.0, 0.0, 0.0, 0.0, 1.0 );\n
  highp mat4 OrientationReverse = mat4( -1.0, 0.0, 0.0, 0.0, 0.0, -1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0 );\n

  struct SPoint\n
  {\n
    highp vec3 position;\n
    highp vec3 coefficients;\n
  };\n

  //Compute determinant of a 3x3 matrix
  float Determinant( in float c00, in float c01, in float c02,in float c10, in float c11, in float c12,in float c20, in float c21, in float c22)\n
  {\n
    return c00*c11*c22 + c10*c21*c02 + c20*c01*c12 - c02*c11-c20 - c12*c21*c00 - c22*c10*c01;\n
  }\n

  void ComputeCoefficients( inout SPoint points[4] )\n
  {\n
    highp mat4 controlPoints = mat4( points[0].position.x, points[1].position.x, points[2].position.x, points[3].position.x, \n
                               points[0].position.y, points[1].position.y, points[2].position.y, points[3].position.y, \n
                               points[0].position.z, points[1].position.z, points[2].position.z, points[3].position.z, \n
                                  1.0,          1.0,          1.0,          1.0      );\n

    highp mat4 C = m3 * controlPoints;\n
    highp float d1 = -Determinant( C[0][3],C[1][3],C[3][3],C[0][2],C[1][2],C[3][2],C[0][0],C[1][0],C[3][0] )*0.00001;\n
    highp float d2 =  Determinant( C[0][3],C[1][3],C[3][3],C[0][1],C[1][1],C[3][1],C[0][0],C[1][0],C[3][0] )*0.00001;\n
    highp float d3 = -Determinant( C[0][2],C[1][2],C[3][2],C[0][1],C[1][1],C[3][1],C[0][0],C[1][0],C[3][0] )*0.00001;\n

    highp mat4 result; \n
    highp float discriminant = 3.0*d2*d2 - 4.0*d1*d3;\n
    if( d1 != 0.0 && discriminant >= 0.0 )\n  //Serpentine or Cusp at infinity\n
    {\n
      highp float s0 = (1.0/sqrt(3.0))*sqrt(3.0*d2*d2-4.0*d1*d3);\n
      highp float lt = d2 + s0;\n
      highp float ls = 2.0*d1;\n
      highp float mt = d2 - s0;\n
      highp float ms = 2.0*d1;\n

      highp mat4 F = mat4(   lt*mt,   (-(ms*lt)-(ls*mt)),     ls*ms,          0.0,    \n
                     lt*lt*lt,  (-(3.0*ls*lt*lt)),  3.0*ls*ls*lt,  (-(ls*ls*ls)), \n
                     mt*mt*mt,  -(3.0*ms*mt*mt),   3.0*ms*ms*mt, -(ms*ms*ms), \n
                       1.0,          0.0,             0.0,            0.0 );  \n

      if( d1 < 0.0 )\n
        F = F * OrientationReverse;\n

      result = m3Inverse * F; //Change points to bernstein basis\n
    }
    else if( d1 != 0.0 && discriminant < 0.0 ) //Loop\n
    {\n

      highp float td = d2 + sqrt(4.0*d1*d3 - 3.0*d2*d2);\n
      highp float sd = 2.0*d1;\n
      highp float te = d2 - sqrt(4.0*d1*d3 - 3.0*d2*d2);\n
      highp float se = 2.0*d1;\n

      highp mat4 F = mat4(   td*te,          -(se*td)-(sd*te),              sd*se,            0.0,
                 td*td*te,  -(se*td*td)-(2.0*sd*te*td),  te*sd*sd+2.0*se*td*sd, -(sd*sd)*se,
                 td*te*te, -(sd*te*te)-(2.0*se*td*te), td*se*se + 2.0*sd*te*se, -sd*se*se,
                    1.0,             0.0,                      0.0,                0.0 );\n

      highp float h0 = 36.0 * ( d3*d1 - d2*d2);\n
      highp float h1 = 36.0 * ( ( d3*d1 - d2*d2) + d1*d2 - d1 );\n
      if( ( d1*h0 > 0.0 ) || (d1*h1 > 0.0 ) )\n
      {\n
        F = F * OrientationReverse;\n
      }\n

      result = m3Inverse * F;\n //Change points to bernstein basis\n
    }\n
    else if( d1 != 0.0 && d2 != 0.0 )  //Cusp with cusp at infinity\n
    {\n
      highp float tl = d3;\n
      highp float sl = 3.0*d2;\n
      highp mat4 F = mat4( tl,-sl,0.0,0.0,  tl*tl*tl,-(3.0*sl*tl*tl),3.0*sl*sl*tl,-(sl*sl*sl),  1.0,0.0,0.0,0.0,  1.0,0.0,0.0,0.0 );\n
      result = m3Inverse * F;\n //Change points to bernstein basis\n
    }\n
    else if( d1==0.0 && d2==0.0 && d3!=0.0 ) //Cuadratic
    {
      result = mat4(0.0, 1.0/3.0, 2.0/3.0, 1.0, 0.0, 0.0, 1.0/3.0, 1.0, 0.0, 1.0/3.0, 2.0/3.0, 1.0, 0.0, 0.0, 0.0, 0.0 );\n
      if( d3 < 0.0 )\n
        result = result * OrientationReverse;
    }

     points[0].coefficients = vec3( result[0][0],result[1][0],result[2][0]);\n
     points[1].coefficients = vec3( result[0][1],result[1][1],result[2][1]);\n
     points[2].coefficients = vec3( result[0][2],result[1][2],result[2][2]);\n
     points[3].coefficients = vec3( result[0][3],result[1][3],result[2][3]);\n
  }

  int ComputeIndex( in int index, in SPoint points[4] )\n
  {\n
    if( index == 0 )\n
      return 0;\n

    highp vec3 A = points[1].position - points[0].position;\n
    highp vec3 B = points[2].position - points[0].position;\n
    highp vec3 C = points[3].position - points[0].position;\n

    highp vec3 A0 = vec3(-A.y, A.x, A.z );\n  //Perpendicular to A
    highp vec3 B0 = vec3(-B.y, B.x, B.z );\n  //Perpendicular to B
    highp vec3 C0 = vec3(-C.y, C.x, C.z );\n  //Perpendicular to C

    ivec3 orderedIndex = ivec3(3,2,1)\n;
    if( sign(dot(A0,B)) != sign(dot(A0,C) ) )\n
    {\n
      orderedIndex = ivec3(2,3,1);\n
    }\n
    else if( sign(dot(B0,A)) != sign(dot(B0,C) ) )\n
    {\n
      orderedIndex = ivec3(1,3,2);\n
    }\n
    else if( sign(dot(C0,A)) != sign(dot(C0,B) ) )\n
    {\n
      orderedIndex = ivec3(1,2,3);\n
    }\n

    return orderedIndex[index-1]\n;
  }\n

  void SplitCurve( in float t, in mat4 bezierControlPoints, out SPoint points[4] )\n
  {\n
    highp vec3 p0 = vec3(bezierControlPoints[0][0],bezierControlPoints[1][0],bezierControlPoints[2][0] );\n
    highp vec3 p1 = vec3(bezierControlPoints[0][1],bezierControlPoints[1][1],bezierControlPoints[2][1] );\n
    highp vec3 p2 = vec3(bezierControlPoints[0][2],bezierControlPoints[1][2],bezierControlPoints[2][2] );\n
    highp vec3 p3 = vec3(bezierControlPoints[0][3],bezierControlPoints[1][3],bezierControlPoints[2][3] );\n

    highp vec3 p4 = mix( p0,p1, t );\n
    highp vec3 p5 = mix( p1,p2, t );\n
    highp vec3 p6 = mix( p2,p3, t );\n
    highp vec3 p7 = mix( p4, p5, t );\n
    highp vec3 p8 = mix( p5, p6, t );\n
    highp vec3 p9 = mix( p7, p8, t );\n   //Point where the curve is splitted

    int vertexID = int(aNormal.x);
    int segment = vertexID / 8;
    int vertex = vertexID - 8*segment;
    if( vertex < 4 )\n
    {\n
      points[0].position = p0;\n
      points[1].position = p4;\n
      points[2].position = p7;\n
      points[3].position = p9;\n
    }\n
    else\n
    {\n
      points[0].position = p9;\n
      points[1].position = p8;\n
      points[2].position = p6;\n
      points[3].position = p3;\n
    }\n
  }\n

  void ComputeBezierPoints( in mat4 bezierControlPoints, out SPoint points[4] )\n
  {\n
    highp mat4 C = m3 * bezierControlPoints;\n
    highp float d1 = -Determinant( C[0][3],C[1][3],C[3][3],C[0][2],C[1][2],C[3][2],C[0][0],C[1][0],C[3][0] );\n
    highp float d2 =  Determinant( C[0][3],C[1][3],C[3][3],C[0][1],C[1][1],C[3][1],C[0][0],C[1][0],C[3][0] );\n
    highp float d3 = -Determinant( C[0][2],C[1][2],C[3][2],C[0][1],C[1][1],C[3][1],C[0][0],C[1][0],C[3][0] );\n

    highp float tSplit = 0.0;
    if( d1 != 0.0 &&  3.0*d2*d2 - 4.0*d1*d3 < 0.0 ) //Loop\n
    {
      highp float td = d2 + sqrt(4.0*d1*d3 - 3.0*d2*d2);\n
      highp float sd = 2.0*d1;\n
      highp float te = d2 - sqrt(4.0*d1*d3 - 3.0*d2*d2);\n
      highp float se = 2.0*d1;\n

      highp float d = td/sd;
      highp float e = te/se;

      tSplit = d*( step( 0.0, d) - step(1.0, d ) ) + e*( step( 0.0, e ) - step(1.0, e ) );
    }\n

    SplitCurve( tSplit, bezierControlPoints, points );\n
    ComputeCoefficients( points );\n
  }\n

  void main()\n
  {\n
    int segment = int(aNormal.x) / 8;

    int baseIndex = 3 * segment;
    highp mat4 controlPoints = mat4( uPoints[baseIndex].x, uPoints[baseIndex+1].x, uPoints[baseIndex+2].x, uPoints[baseIndex+3].x,
                               uPoints[baseIndex].y, uPoints[baseIndex+1].y, uPoints[baseIndex+2].y, uPoints[baseIndex+3].y,
                               uPoints[baseIndex].z, uPoints[baseIndex+1].z, uPoints[baseIndex+2].z, uPoints[baseIndex+3].z,
                               1.0, 1.0, 1.0, 1.0 );\n

    SPoint points[4];\n
    ComputeBezierPoints( controlPoints, points  );\n

    int vertexInSegment = int(aNormal.x) - 8*segment;\n
    int vertexID = vertexInSegment - int( ( 4.0 * step( 3.0,float(vertexInSegment) ) ) );
    int index = ComputeIndex( vertexID, points );

    gl_Position = uMvpMatrix * vec4(points[index].position, 1.0);\n
    vNormal = points[index].coefficients;\n
  }\n
);

std::string sCircleFragmentShader = MAKE_STRING(
  uniform mediump float uAlpha;
  uniform mediump float uLinewWidth;\n

  void main()\n
  {\n
    highp float C = (vNormal.x*vNormal.x*vNormal.x - vNormal.y*vNormal.z);\n
    highp float Cdx = dFdx(C);\n
    highp float Cdy = dFdy(C);\n

    highp float distance = abs(float(C / sqrt(Cdx*Cdx + Cdy*Cdy)));\n
    gl_FragColor = vec4( 1.0,1.0,1.0, uAlpha )*(uLinewWidth-distance);\n
    return;
  }\n
);
}

// This example shows resolution independent rendering and animation of curves using the gpu.
//
class CurvesExample : public ConnectionTracker
{
public:

  CurvesExample( Application& application )
: mApplication( application )
{
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &CurvesExample::Create );
}

  ~CurvesExample()
  {
    // Nothing to do here;
  }

  // The Init signal is received once (only) during the Application lifetime
  void Create( Application& application )
  {
    // Get a handle to the stage
    Stage stage = Stage::GetCurrent();

    //Create a view
    mView = Dali::Toolkit::View::New();
    stage.Add( mView );

    //Set background image for the view
    Dali::Image backgroundImage = Dali::Image::New( BACKGROUND_IMAGE );
    Dali::ImageActor backgroundImageActor = Dali::ImageActor::New( backgroundImage );
    mView.SetBackground( backgroundImageActor );

    //Create the popup window
    CreatePopup();

    //Create the circles effect
    CreateCirclesEffect();

    // Respond to a click anywhere on the stage
    stage.GetRootLayer().TouchedSignal().Connect( this, &CurvesExample::OnTouch );
    stage.KeyEventSignal().Connect(this, &CurvesExample::OnKeyEvent);
  }

  /*
   * Creates the popup window
   */
  void CreatePopup()
  {
    //Create the shader effect
    ShaderEffect shader = ShaderEffect::New( sPopupVertexShader, sPopupFragmentShader, GEOMETRY_TYPE_UNTEXTURED_MESH );
    Stage stage =  Stage::GetCurrent();
    Vector2 stageSize = stage.GetSize();
    float offset = std::max( stage.GetSize().x,  stage.GetSize().y ) * 0.1f;
    shader.SetUniform( "uPoints[0]",Vector3(0.0f,stageSize.y+offset, 0.0f ), ShaderEffect::COORDINATE_TYPE_DEFAULT );
    shader.SetUniform( "uPoints[1]",Vector3(stageSize.x*0.5f, stageSize.y, 0.0f) , ShaderEffect::COORDINATE_TYPE_DEFAULT );
    shader.SetUniform( "uPoints[2]",Vector3(stageSize.x,stageSize.y+offset,0.0f), ShaderEffect::COORDINATE_TYPE_DEFAULT );
    shader.SetUniform( "uPoints[3]",Vector3(stageSize.x,stageSize.y,0.0f), ShaderEffect::COORDINATE_TYPE_DEFAULT );
    shader.SetUniform( "uPoints[4]",Vector3(0.0f,stageSize.y,0.0f), ShaderEffect::COORDINATE_TYPE_DEFAULT );

    Layer layer = Layer::New();
    layer.SetAnchorPoint(AnchorPoint::TOP_LEFT );
    stage.Add( layer );

    ////Generate the mesh
    Dali::MeshData::VertexContainer vertices;
    vertices.push_back( MeshData::Vertex( Vector3(100.0f,400.0f, 0.0f ), Vector2::ZERO, Vector3::ZERO) );
    vertices.push_back(  MeshData::Vertex( Vector3::ZERO, Vector2::ZERO, Vector3(0.5f,0.0f,1.0f) ) );
    vertices.push_back( MeshData::Vertex( Vector3::ZERO, Vector2::ZERO, Vector3(1.0f,1.0f,2.0f)  ) );
    vertices.push_back( MeshData::Vertex( Vector3::ZERO, Vector2::ZERO, Vector3(0.0f,0.0f,3.0f)  ) );
    vertices.push_back( MeshData::Vertex( Vector3::ZERO, Vector2::ZERO, Vector3(0.0f,0.0f,4.0f)  ) );

    short unsigned int indexArray[] = { 0,2,1,0,4,2,2,4,3};
    Dali::MeshData::FaceIndices index( indexArray, indexArray + sizeof(indexArray)/sizeof(short unsigned int) );

    //Material
    Dali::Material material = Material::New("Material");
    material.SetDiffuseColor( Vector4(1.0f,1.0f,1.0f,1.0f));

    //Create the Mesh object
    Dali::MeshData data;
    data.SetVertices(vertices);
    data.SetFaceIndices( index );
    data.SetMaterial( material );
    data.SetHasNormals( true );
    Mesh mesh = Mesh::New( data );

    //Create the mesh actor
    MeshActor meshActor = MeshActor::New(mesh);
    meshActor.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    meshActor.SetParentOrigin( ParentOrigin::TOP_LEFT );
    meshActor.SetAffectedByLighting(false);
    meshActor.SetCullFace(CullNone);
    meshActor.SetShaderEffect(shader);
    meshActor.SetDrawMode(DrawMode::STENCIL);
    layer.Add( meshActor );

    Image image = Image::New( CONTACTS_IMAGE );
    ImageActor imageActor = ImageActor::New( image );
    imageActor.SetAnchorPoint(AnchorPoint::TOP_LEFT );
    imageActor.SetPosition(0.0f,stageSize.y,0.0f);
    imageActor.SetSize( stageSize );
    layer.Add( imageActor );

    //Animation
    Animation animation = Animation::New(2.0f);
    animation.AnimateBy( Property(shader, "uPoints[0]"),Vector3(0.0f,-stageSize.y*0.4f-offset, 0.0f),AlphaFunctions::EaseInOutSine,TimePeriod(0.0f,1.0f));
    animation.AnimateBy( Property(shader, "uPoints[1]"),Vector3(0.0f,-stageSize.y*0.4f, 0.0f),AlphaFunctions::EaseInOutSine, TimePeriod(0.0f,1.0f));
    animation.AnimateBy( Property(shader, "uPoints[2]"),Vector3(0.0f,-stageSize.y*0.4f-offset, 0.0f),AlphaFunctions::EaseInOutSine,TimePeriod(0.0f,1.0f));
    animation.AnimateBy( Property(imageActor, Actor::POSITION_Y), -stageSize.y*0.4f,AlphaFunctions::EaseInOutSine,TimePeriod(0.0f,1.0f));
    animation.SetLooping(true);
    animation.Play();
  }

  /*
   * Create a single circle
   * @param[in] center Center of the circle
   * @param[in] radius Radius of the circle
   * @param[in] animation The animation object which will animate the circle
   * @param[in] period Delay and duration in seconds of the animation for the circle
   * @param[in] rotation Initial rotation for the circle
   */
  void CreateCircle( Vector3 center,float radius, Animation animation, TimePeriod period, Degree rotation )
  {
    //Create the shader effect
    float offset = radius*4.0f/3.0f;
    ShaderEffect shader = ShaderEffect::NewWithPrefix( "",sCircleVertexShader, "#extension GL_OES_standard_derivatives : enable\n", sCircleFragmentShader, GEOMETRY_TYPE_UNTEXTURED_MESH );
    shader.SetUniform( "uPoints[0]",Vector3(-radius,0.0f,0.0f ), ShaderEffect::COORDINATE_TYPE_DEFAULT );
    shader.SetUniform( "uPoints[1]",Vector3(-radius,-offset,0.0f) , ShaderEffect::COORDINATE_TYPE_DEFAULT );
    shader.SetUniform( "uPoints[2]",Vector3(radius,-offset,0.0f), ShaderEffect::COORDINATE_TYPE_DEFAULT );
    shader.SetUniform( "uPoints[3]",Vector3(radius,0.0,0.0f), ShaderEffect::COORDINATE_TYPE_DEFAULT );
    shader.SetUniform( "uPoints[4]",Vector3(radius,offset,0.0f), ShaderEffect::COORDINATE_TYPE_DEFAULT );
    shader.SetUniform( "uPoints[5]",Vector3(-radius,offset,0.0f), ShaderEffect::COORDINATE_TYPE_DEFAULT );
    shader.SetUniform( "uPoints[6]",Vector3(-radius,0.0f,0.0f), ShaderEffect::COORDINATE_TYPE_DEFAULT );
    shader.SetUniform( "uAlpha",1.0f, ShaderEffect::COORDINATE_TYPE_DEFAULT );
    shader.SetUniform( "uLinewWidth",1.5f, ShaderEffect::COORDINATE_TYPE_DEFAULT );

    ////Generate the mesh
    Dali::MeshData::VertexContainer vertices;
    for( unsigned int i(0); i!=16; ++i )
      vertices.push_back( MeshData::Vertex( Vector3::ZERO, Vector2::ZERO, Vector3((float)i,0.0f,0.0f) ) );

    short unsigned int indexArray[] = {0,2,1,2,3,1,4,6,5,6,7,5,8,10,9,10,11,9,12,14,13,14,15,13};
    Dali::MeshData::FaceIndices index( indexArray, indexArray + 24 );

    //Material
    Dali::Material material0 = Material::New("Material");
    material0.SetDiffuseColor( Vector4(0.0f,0.0f,0.0f,0.0f));

    //Create the Mesh Actor
    Dali::MeshData data;
    data.SetVertices(vertices);
    data.SetFaceIndices( index );
    data.SetMaterial( material0 );
    data.SetHasNormals( true );
    Mesh mesh0 = Mesh::New( data );

    MeshActor meshActor = MeshActor::New(mesh0);
    meshActor.SetAnchorPoint( AnchorPoint::CENTER );
    meshActor.SetParentOrigin( ParentOrigin::CENTER );
    meshActor.SetAffectedByLighting(false);
    meshActor.SetCullFace(CullNone);
    Stage stage = Stage::GetCurrent();
    meshActor.SetPosition( Vector3( center.x - stage.GetSize().x * 0.5f,center.y - stage.GetSize().y * 0.5f, 0.0f) );
    meshActor.SetRotation(rotation, Vector3::ZAXIS);
    meshActor.SetShaderEffect(shader);

    ///Animation
    float maxRadius = radius * 0.9f;

    //Point 0 & 2
    KeyFrames k0 = KeyFrames::New();
    k0.Add( 0.0f, Vector3(-radius,0.0f, 0.0f ));
    k0.Add( 1.0f, Vector3(-radius-maxRadius,0.0f, 0.0f ));
    animation.AnimateBetween( Property(shader, "uPoints[0]"),k0,AlphaFunctions::EaseInOutSine, period);
    animation.AnimateBetween( Property(shader, "uPoints[6]"),k0,AlphaFunctions::EaseInOutSine, period);

    //Point 1
    k0 = KeyFrames::New();
    k0.Add( 0.0f, Vector3(radius,0.0f,0.0f));
    k0.Add( 1.0f, Vector3(radius+maxRadius,0.0f,0.0f));
    animation.AnimateBetween( Property(shader, "uPoints[3]"),k0,AlphaFunctions::EaseInOutSine, period);

    //CP 1
    k0 = KeyFrames::New();
    k0.Add( 0.0f, Vector3(-radius,-offset,0.0f));
    k0.Add( 0.5f, Vector3((-radius-maxRadius)*0.6f , (-offset-maxRadius)*0.6f, 0.0f));
    k0.Add( 1.0f, Vector3(-radius-maxRadius,-offset-maxRadius, 0.0f));
    animation.AnimateBetween( Property(shader, "uPoints[1]"),k0,AlphaFunctions::EaseInOutSine, period);

    //CP2
    k0 = KeyFrames::New();
    k0.Add( 0.0f, Vector3(radius,-offset,0.0f));
    k0.Add( 0.5f, Vector3((radius+maxRadius)*0.7f,(-offset-maxRadius)*0.7f,0.0f));
    k0.Add( 1.0f, Vector3(radius+maxRadius,-offset-maxRadius,0.0f));
    animation.AnimateBetween( Property(shader, "uPoints[2]"),k0,AlphaFunctions::EaseInOutSine, period);

    //CP3
    k0 = KeyFrames::New();
    k0.Add( 0.0f, Vector3(radius,offset,0.0f));
    k0.Add( 1.0f, Vector3(radius+maxRadius,offset+maxRadius,0.0f));
    animation.AnimateBetween( Property(shader, "uPoints[4]"),k0,AlphaFunctions::EaseInOutSine, period);

    //CP4
    k0 = KeyFrames::New();
    k0.Add( 0.0f, Vector3(-radius,offset, 0.0f));
    k0.Add( 1.0f, Vector3(-radius-maxRadius, offset+maxRadius, 0.0f));
    animation.AnimateBetween( Property(shader, "uPoints[5]"),k0,AlphaFunctions::EaseInOutSine, period);

    animation.AnimateTo( Property(shader, "uAlpha"),0.0f,AlphaFunctions::EaseInOutSine, period);
    animation.RotateBy( meshActor,rotation,Vector3::ZAXIS, AlphaFunctions::EaseInOutSine, period.delaySeconds, period.durationSeconds );

    //Add to stage
    mView.Add( meshActor );
  }

  /**
   * Creates the circles effect
   */
  void CreateCirclesEffect()
  {
    //Create animation for the circles
    Animation animation = Dali::Animation::New(2.0f);
    animation.SetLooping(true);
    animation.Play();

    //Create four circles
    Stage stage = Stage::GetCurrent();
    Vector2 stageSize = stage.GetSize();
    float radius = std::max( stage.GetSize().x,  stage.GetSize().y ) * 0.075f;
    Vector3 center = Vector3( stageSize.x * 0.5f, stageSize.y * 0.25f, 0.0f );
    CreateCircle( center, radius, animation, TimePeriod(0.0f,1.0f), Degree(-50.0f));
    CreateCircle( center, radius, animation, TimePeriod(0.3f,1.3f), Degree(50.0f));
    CreateCircle( center, radius, animation, TimePeriod(0.6f,1.0f), Degree(-90.0f));
    CreateCircle( center, radius, animation, TimePeriod(0.8f,1.2f), Degree(-45.0f));

    //Add image actor in the center of the circles
    Image image = Image::New( LOGO_IMAGE );
    ImageActor imageActor = ImageActor::New( image );
    float size = std::max(stageSize.x,  stageSize.y )*0.2f;
    imageActor.SetSize( size, size );
    imageActor.SetPosition(stageSize.x * 0.5f, stageSize.y * 0.25f );
    mView.Add( imageActor );
  }

  /**
   * Touch event handler
   */
  bool OnTouch( Actor actor, const TouchEvent& touch )
  {
    mApplication.Quit();
    return true;
  }

  /**
   * Main key event handler
   */
  void OnKeyEvent(const KeyEvent& event)
  {
    if( IsKey( event, DALI_KEY_ESCAPE) || IsKey( event, DALI_KEY_BACK ) )
    {
      //Quit the application
      mApplication.Quit();
    }
  }

private:
  Application&        mApplication;
  Toolkit::View       mView;
};

void RunTest( Application& application )
{
  CurvesExample test( application );
  application.MainLoop();
}

int main( int argc, char **argv )
{
  Application application = Application::New( &argc, &argv );
  RunTest( application );

  return 0;
}
