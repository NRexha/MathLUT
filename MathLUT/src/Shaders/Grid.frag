#version 430 core

out vec4 FragColor;

uniform vec2 uViewport;
uniform vec2 uCenter;
uniform float uPixelsPerUnit;
uniform float uGridStep;

float GridLine(float value, float stepSize)
{
    float d = abs(fract(value / stepSize + 0.5) - 0.5) * stepSize * uPixelsPerUnit;
    float aa = max(fwidth(d), 0.75);
    return 1.0 - smoothstep(0.7 - aa, 0.7 + aa, d);
}

float Line(float distancePx, float halfWidth)
{
    float aa = max(fwidth(distancePx), 0.75);
    return 1.0 - smoothstep(halfWidth - aa, halfWidth + aa, distancePx);
}

void main()
{
    vec2 graph = uCenter + (gl_FragCoord.xy - uViewport * 0.5) / uPixelsPerUnit;
    vec3 color = vec3(0.075, 0.082, 0.095);

    float minorGrid = max(GridLine(graph.x, uGridStep), GridLine(graph.y, uGridStep));
    float majorGrid = max(GridLine(graph.x, uGridStep * 5.0), GridLine(graph.y, uGridStep * 5.0));

    color = mix(color, vec3(0.13, 0.14, 0.16), minorGrid * 0.45);
    color = mix(color, vec3(0.19, 0.20, 0.23), majorGrid * 0.65);

    float axisX = Line(abs(graph.x) * uPixelsPerUnit, 0.8);
    float axisY = Line(abs(graph.y) * uPixelsPerUnit, 0.8);
    color = mix(color, vec3(0.48, 0.50, 0.54), max(axisX, axisY));

    FragColor = vec4(color, 1.0);
}
