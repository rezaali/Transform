#version 150 core

uniform float trailFade; //slider
uniform float trailLength; 
uniform vec4 color; //color

in float ID; 
in vec3 Pos; 
out vec4 oColor;

void main( void )
{
	oColor = color;
	oColor.a *= mix( color.a, pow( 1.0 - fract( ID / trailLength ), 4.0 ), trailFade ); 
}