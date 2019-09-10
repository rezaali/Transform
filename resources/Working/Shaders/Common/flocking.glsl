uniform float zoneRadius; //slider:0,10,1
uniform vec2 flockThreshold; //range:0,1,0.5,0.75
uniform float flockRepelForce; //slider
uniform float flockAlignForce; //slider
uniform float flockAttractForce; //slider
uniform float flockCohesionForce; //slider

vec3 flockingForce( in vec4 posid, in vec4 velmass, int total, in samplerBuffer positionid, in samplerBuffer velocitymass ) {
	float numNeighbors = 0.0;
    vec3 neighborCentroid = vec3( 0.0 );
    vec3 flockCentroid = vec3( 0.0 );
    vec3 FF = vec3( 0.0 );
    float ipm = 1.0 / velmass.w;
    vec3 pos = posid.xyz;
    int id = int( posid.w );
    for ( int i = 0; i < total; i++ ) {
		if( i != id ) {
			vec4 oinfop = texelFetch( positionid, i );
			vec4 oinfov = texelFetch( velocitymass, i );
			vec3 otherPos = oinfop.xyz;
			vec3 otherVel = oinfov.xyz;

			vec3 direction = otherPos - pos;
            float dist = length( direction );

            if( dist < zoneRadius ) {
                float percent = dist / zoneRadius;
                neighborCentroid += otherPos;
                numNeighbors += 1.0;

                if( percent < flockThreshold.x ) {
                    float rf = ( flockThreshold.x / percent - 1.0 ) * flockRepelForce;
                    direction = normalize(direction);
                    direction *= rf;
                    FF -= direction;
                } else if( percent < flockThreshold.y) {
                    float threshDelta = flockThreshold.y - flockThreshold.x;
                    float adjustedPercent = ( percent - flockThreshold.x ) / threshDelta;
                    float af = ( 0.5 - cos( adjustedPercent * PI * 2.0 ) * 0.5 + 0.5 ) * flockAlignForce;
                    FF += normalize(otherVel)*af;
                }
                // else if( same ) {
                    float threshDelta = 1.0 - flockThreshold.y;
                    float adjustedPercent = ( percent - flockThreshold.y ) / threshDelta;
                    float af = ( 1.0 - ( cos( adjustedPercent * TWO_PI ) * -0.5 + 0.5 ) ) * flockAttractForce;
                    direction = normalize(direction);
                    direction *= af;
                    FF += direction;
                // }
            }
            flockCentroid += otherPos;
        }
    }

    if( numNeighbors > 0.0 )
    {
         vec3 neighborAveragePos = ( neighborCentroid / numNeighbors );
         FF += ( neighborAveragePos - pos ) * flockCohesionForce;
    }
    else
    {
        flockCentroid = flockCentroid / ( float( total ) - 1.0 );
        FF += ( flockCentroid - pos );
    }

    return ( ipm * FF / numNeighbors );
}