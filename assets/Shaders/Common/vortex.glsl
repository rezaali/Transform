vec3 vortexForce( in vec3 p, in vec3 startPt, in vec3 endPt, in float radius, in float power )
{
	vec3 se = startPt - endPt; 
    float lenSe = length( se ); 
	vec3 sen = normalize( se );

	vec3 centerPt = endPt + sen*lenSe*0.5; 
	float rad = lenSe * 0.5;
   
    vec3 pe = p - endPt;; 
    float lenPe = length(pe); 
    float dotPe = dot( pe, sen );
    	    
    vec3 ptOnWire = dotPe*sen+endPt; 

    float dFC = length( centerPt - p ); 

    if( dFC < rad*radius )
    {
    	vec3 rv = p - ptOnWire; 		    	
    	float lenRv = length(rv); 
    	vec3 rvn = rv / lenRv;  		    					
    	return ( cross( sen, rv ) / pow( lenRv, power ) );
    }
    return vec3( 0.0f ); 
}