uniform vec3   uLightPosition; // in view-space
uniform float  uAmbientLight;
varying vec3  vEyeSpacePosition;
varying float vLightDistance;
varying highp vec3 vSurfaceLocalLightPosition;
varying highp vec3 vSurfaceLocalEyePosition;

void main()
{
  vec4 eyeSpaceVertex = uModelView * vec4( aPosition, 1.0 );

  vec3 n = normalize( uNormalMatrix * vec3(0.0, 0.0, 1.0) );
  vec3 t = normalize( uNormalMatrix * vec3(1.0, 0.0, 0.0) );
  vec3 b = normalize( cross(n,t) );

  // transform the eye position to surface-local-space
  vec3 eyeSpaceFromVertToLite = uLightPosition - eyeSpaceVertex.xyz;

  vSurfaceLocalLightPosition.x = dot( eyeSpaceFromVertToLite, t );
  vSurfaceLocalLightPosition.y = dot( eyeSpaceFromVertToLite, b );
  vSurfaceLocalLightPosition.z = dot( eyeSpaceFromVertToLite, n );

  vEyeSpacePosition = eyeSpaceVertex.xyz;
  vTexCoord = aTexCoord;
  gl_Position = uMvpMatrix * vec4(aPosition, 1.0);

}
