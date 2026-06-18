#version 330

out vec4 finalColor;

uniform vec2 resolution;
uniform float time;

void main()
{
    vec2 uv = gl_FragCoord.xy / resolution.xy;
    
    // Invert Y because gl_FragCoord origin is bottom-left
    uv.y = 1.0 - uv.y;
    
    vec2 center = vec2(0.5, 0.5);
    float dist = distance(uv, center);
    
    float pulse = sin(time * 8.0) * 0.05 + 0.05;
    float vignette = smoothstep(0.45 - pulse, 0.8 - pulse, dist);
    
    finalColor = vec4(0.6, 0.0, 0.0, vignette * 0.65);
}
