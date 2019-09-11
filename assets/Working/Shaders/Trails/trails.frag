#version 400

uniform vec4 color; 
uniform sampler2D colorTexture; 
uniform float trailFade = 1.0; 
uniform float palette = 0.0; 
uniform float rangeLow = 0.0; 
uniform float rangeHigh = 1.0; 
uniform float offsetWeight = 1.0; 
uniform float offset = 1.0; 

out vec4 oColor;

in vec3 gPos;
in vec2 gTexcoord; 
in float gOffset; 

float map( float value, float inMin, float inMax, float outMin, float outMax ) {
	return ( (value - inMin) / ( inMax - inMin ) * ( outMax - outMin ) ) + outMin; 
}

void main () {		
	oColor = texture( colorTexture, 
		vec2( map( gTexcoord.x, 0.0, 1.0, rangeLow, rangeHigh ) + offsetWeight*gOffset + offset, palette ) ); 	 
	oColor.a *= mix( 1.0, 1.0 - gTexcoord.x, trailFade ) * color.a;
}