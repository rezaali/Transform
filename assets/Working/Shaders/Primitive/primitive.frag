#include "uniforms.glsl"
#include "pi.glsl"
#include "shapes.glsl"

in vec2	vTexCoord;
in vec4 vColor;
in vec3 vNormal;
in float vDiffuse;

out vec4 oColor;

void main( void )
{
    oColor = vColor;
    oColor.rgb = mix( iBackgroundColor.rgb, oColor.rgb, vDiffuse );
}