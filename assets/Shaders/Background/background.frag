#include "uniforms.glsl"

uniform sampler2D iScreenTexture; 

in vec2 vTexcoord;

out vec4 oColor;

void main(void)
{
	oColor = iBackgroundColor;   
}