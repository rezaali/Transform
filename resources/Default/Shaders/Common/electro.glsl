vec3 electroForce( in vec4 pm, int id, int total, in samplerBuffer positionmass ) {
    float count = 1.0;       
    vec3 result = vec3( 0.0 );        
    for ( int i = 0; i < total; i++ ) {                    
        if( i != id ) {
            vec4 other = texelFetch( positionmass, i );                
            vec3 direction = other.xyz - pm.xyz;              

            float dist = length( direction );            
            result -= other.w * pm.w * ( direction / ( dist * dist ) );                                    
            count++;                                                                 
        }
    }
    return result / count;     
}