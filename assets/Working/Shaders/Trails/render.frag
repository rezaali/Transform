#version 150 core

uniform float trailFade; 
uniform float trailLength; 
uniform vec4 color; 

in float ID; 
in vec3 Pos; 
out vec4 oColor;

void main( void )
{
	oColor = color;
	oColor.a *= mix( color.a, 1.0 - fract( ID / trailLength ), trailFade ); 
}