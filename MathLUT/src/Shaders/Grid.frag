#version 430 core

out vec4 FragColor;

uniform vec2 uViewport;
uniform vec2 uCenter;
uniform float uPixelsPerUnit;
uniform float uGridStep;
uniform vec2 uViewportOrigin;

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
    vec2 localPosition = gl_FragCoord.xy - uViewportOrigin;
    vec2 graph = uCenter + (localPosition - uViewport * 0.5) / uPixelsPerUnit;

    vec3 color = vec3(0.094, 0.102, 0.125);

    float minorGrid = max(GridLine(graph.x, uGridStep), GridLine(graph.y, uGridStep));
    float majorGrid = max(GridLine(graph.x, uGridStep * 5.0), GridLine(graph.y, uGridStep * 5.0));

    color = mix(color, vec3(0.125, 0.133, 0.157), minorGrid * 0.40);
    color = mix(color, vec3(0.176, 0.192, 0.231), majorGrid * 0.65);

    float axisX = Line(abs(graph.x) * uPixelsPerUnit, 0.8);
    float axisY = Line(abs(graph.y) * uPixelsPerUnit, 0.8);
    color = mix(color, vec3(0.40, 0.65, 0.92), max(axisX, axisY));

    FragColor = vec4(color, 1.0);
}
