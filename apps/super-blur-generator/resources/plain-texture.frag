
precision mediump float;
uniform sampler2D sTexture;
varying vec2 vTexCoord;

void main()
{
  vec4 color = texture2D( sTexture, vTexCoord );

  gl_FragColor = vec4(color.xyz, 1.0);
}
