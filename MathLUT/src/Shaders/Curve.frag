#version 430 core

out vec4 FragColor;

uniform vec2 uViewport;
uniform vec2 uCenter;
uniform float uPixelsPerUnit;
uniform vec4 uVisibleChannels;
uniform sampler1D uSamples;
uniform vec2 uViewportOrigin;

float Line(float distancePx, float halfWidth)
{
    float aa = max(fwidth(distancePx), 0.75);
    return 1.0 - smoothstep(halfWidth - aa, halfWidth + aa, distancePx);
}

void main()
{
    vec2 localPosition = gl_FragCoord.xy - uViewportOrigin;
    vec2 graph = uCenter + (localPosition - uViewport * 0.5) / uPixelsPerUnit;
    float x = graph.x;
    float y = graph.y;
    float inDomain = step(0.0, x) * step(x, 1.0);

    vec4 sampleValue = texture(uSamples, clamp(x, 0.0, 1.0));

    float red = Line(abs(y - sampleValue.r) * uPixelsPerUnit, 1.5) * inDomain * uVisibleChannels.r;
    float green = Line(abs(y - sampleValue.g) * uPixelsPerUnit, 1.5) * inDomain * uVisibleChannels.g;
    float blue = Line(abs(y - sampleValue.b) * uPixelsPerUnit, 1.5) * inDomain * uVisibleChannels.b;
    float alpha = Line(abs(y - sampleValue.a) * uPixelsPerUnit, 1.5) * inDomain * uVisibleChannels.a;

    float total = red + green + blue + alpha;
    vec3 curveColor = (
        vec3(1.0, 0.24, 0.28) * red +
        vec3(0.25, 0.90, 0.43) * green +
        vec3(0.25, 0.58, 1.0) * blue +
        vec3(0.88, 0.88, 0.90) * alpha
    ) / max(total, 0.0001);

    FragColor = vec4(curveColor, clamp(total, 0.0, 1.0));
}
