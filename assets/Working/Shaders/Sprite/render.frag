#define PI 3.14159265359
#define TWOPI 6.2831853072

uniform float alpha; //UI:0.0,1.0,0.1
uniform float power; //UI:0.0,1.0,1.0
uniform float sigma; //UI:0.0,1.0,1.0

in vec4 vColor; 
in vec2 vTexcoord; 

out vec4 oColor;

void main( void )
{		
	float dist = length( vTexcoord - vec2( 0.5f ) );
	dist = pow( dist, power ); 

	float a = 1.0 / ( sigma * sqrt( TWOPI ) );
	float b = 0.0; 
	float gau = a * exp( - pow( dist - b, 2.0 ) / ( 2.0 * pow( sigma, 2.0 ) ) );
	
	oColor = vColor;
	oColor *= vec4( gau ); 
	oColor.a *= alpha; 

}
