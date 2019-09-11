uniform vec4 uColor; 

uniform float dist; //UI:0.0,40.0,25.0
uniform float power; //UI:0.0,32.0,10.0

in float vDist; 
out vec4 oColor;

void main(void)
{	
	oColor = uColor;
	float a = ( dist - clamp( vDist, 0.0,  dist ) ) / ( dist ) ;
	oColor.a *= pow( a, power );
}