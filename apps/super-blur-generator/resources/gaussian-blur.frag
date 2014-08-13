#define NUM_SAMPLES 16

precision mediump float;

uniform sampler2D sTexture;

uniform vec2 uSampleOffsets[NUM_SAMPLES];
uniform float uSampleWeights[NUM_SAMPLES];

varying vec2 vTexCoord;

void main()
{
  mediump vec4 color = vec4(0.0, 0.0, 0.0, 0.0 );

  for ( int i=0; i<NUM_SAMPLES; ++i )
  {
    color += vec4( texture2D( sTexture, vTexCoord + uSampleOffsets[i] ) * uSampleWeights[i] );
  }

  gl_FragColor = vec4(color.xyz, 1.0);
}
