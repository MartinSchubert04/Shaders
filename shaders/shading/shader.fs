#version 330 

uniform vec3 iResolution; 
uniform float iTime; 

in vec2 fragCoord;

out vec4 fragColor;


mat2 rot2D(float angle) {
    float s = sin(angle);
    float c = cos(angle);
    return mat2(c, -s, s, c);
}

vec3 rot3D(vec3 p, vec3 axis, float angle) {
    return mix(dot(axis, p) * axis, p, cos(angle)) + cross(axis, p) * sin(angle);
}

float smin( float a, float b, float k )
{
    float h = max(k-abs(a-b),0.0);
    return min(a, b) - h*h*0.25/k;
}

float sdBox(vec3 p, vec3 b) {
    vec3 q = abs(p) - b;
    return length(max(q,0.0)) - min(max(q.x,max(q.y,q.z)),0.0);
}

float sdSphere(vec3 p, float r) {
    return length(p) - r;
}

float map(vec3 p) {
    vec3 sPos = vec3(sin(iTime)*3.,0,0);
    float sphere = sdSphere(p - sPos, 1.);
    
    vec3 q = rot3D(p, vec3(1,0,0), iTime);

    float box = sdBox(q, vec3(.80));
    
    float ground = p.y + .75;
    
    return smin(ground, smin(sphere, box, 1.),.2); // distance to a sphere of radius 1
}

void main() {
    vec2 uv = (gl_FragCoord.xy * 2. - iResolution.xy) / iResolution.y;

    vec3 ro = vec3(0,0,-3.);
    vec3 rd = normalize(vec3(uv, 1.));
    vec3 col = vec3(0);               // final pixel color

    int maxSteps = 80;

    float t = 0.; // total distance travelled

    for (int i = 0; i < maxSteps; i++) {
        vec3 p = ro + rd * t;     // position along the ray

        float d = map(p);         // current distance to the scene

        t += d;                   // "march" the ray

        if (d < .001) break;      // early stop if close enough
        if (t > 100.) break;      // early stop if too far
    }

    col = vec3(t * .2);

    fragColor = vec4(col, 1.0);
}