precision mediump float;
uniform vec3 uLightPosition; // in view-space
uniform float uAmbientLight;
varying vec3 vEyeSpacePosition;
varying highp vec3 vSurfaceLocalLightPosition;
varying highp vec3 vSurfaceLocalEyePosition;

void main()
{
  lowp vec4 color   = texture2D( sTexture, vTexCoord );
  highp vec4 normal = texture2D( sEffect, vTexCoord );

  // falloff
  highp vec3 vecToLite = uLightPosition - vEyeSpacePosition.xyz;
  highp float dist = length(vecToLite);
  highp float distanceAttenuation = 1.0 / (2.0 * dist * dist);

  highp vec3 surfLocalNormal = normal.xyz * 2.0 - 1.0; // expand (0 - 1) to (-1 - 1)
  surfLocalNormal = normalize( surfLocalNormal * vec3( 1.0, 1.0, 0.25 ) ); // accentuate

  highp vec3 lightVector = normalize( vSurfaceLocalLightPosition );
  lowp vec3 lightDiffuse = vec3( distanceAttenuation *
                                 max( 0.0, dot( lightVector, surfLocalNormal ) ) );

  gl_FragColor = vec4( color.rgb * ( vec3(uAmbientLight) + lightDiffuse ), 1.0 );
}
