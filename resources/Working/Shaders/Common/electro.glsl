vec3 electroForce( in vec4 posid, in vec4 velmass, int total, in samplerBuffer positionid, in samplerBuffer velocitymass ) {
    float count = 1.0;
    vec3 result = vec3( 0.0 );
    int id = int( posid.w );
    vec3 pos = posid.xyz;
    float mass = velmass.w;
    for ( int i = 0; i < total; i++ ) {
        if( i != id ) {
            vec3 otherPos = texelFetch( positionid, i ).xyz;
            vec3 direction = otherPos - pos;
            float otherMass = texelFetch( velocitymass, i ).w;

            float dist = length( direction );
            result -= otherMass * mass * ( direction / ( dist * dist ) );
            count++;
        }
    }
    return result / count;
}

vec3 electroForceInvMass( in vec4 posid, in vec4 velmass, int total, in samplerBuffer positionid, in samplerBuffer velocitymass ) {
    float count = 1.0;
    vec3 result = vec3( 0.0 );
    int id = int( posid.w );
    vec3 pos = posid.xyz;
    float mass = velmass.w;
    for ( int i = 0; i < total; i++ ) {
        if( i != id ) {
            vec3 otherPos = texelFetch( positionid, i ).xyz;
            vec3 direction = otherPos - pos;
            float otherMass = texelFetch( velocitymass, i ).w;

            float dist = length( direction );
            result -= mass * ( direction / ( dist * dist ) );
            count++;
        }
    }
    return result / count;
}


vec3 electroForceNoMass( in vec4 posid, in vec4 velmass, int total, in samplerBuffer positionid, in samplerBuffer velocitymass ) {
    float count = 1.0;
    vec3 result = vec3( 0.0 );
    int id = int( posid.w );
    vec3 pos = posid.xyz;
    for ( int i = 0; i < total; i++ ) {
        if( i != id ) {
            vec3 otherPos = texelFetch( positionid, i ).xyz;
            vec3 direction = otherPos - pos;
            float dist = length( direction );
            result -= ( direction / ( dist * dist ) );
            count++;
        }
    }
    return result / count;
}