#include "uniforms.glsl"
#include "gamma.glsl"
#include "curlnoise.glsl"

uniform sampler2D iScreenTexture;

in vec2 vTexcoord;

out vec4 oColor;

void main(void)
{
    oColor = iBackgroundColor;
    // oColor = gamma( gamma( vec4( 1.0 - curlNoise( vec3( vTexcoord, iAnimationTime ) ), 1.0 ) ) );
    // oColor = texture( iScreenTexture, vTexcoord );
    // oColor.rgb = oColor.bgr;
}