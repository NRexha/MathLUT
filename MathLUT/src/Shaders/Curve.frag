#version 430 core

out vec4 FragColor;

uniform vec2 uViewport;
uniform vec2 uCenter;
uniform float uPixelsPerUnit;
uniform vec4 uVisibleChannels;
uniform sampler1D uSamples;
uniform vec2 uViewportOrigin;

float CurveLine(float y, float sampleY)
{
    float slope = dFdx(sampleY) * uPixelsPerUnit;
    float distancePx = abs(y - sampleY) * uPixelsPerUnit / sqrt(1.0 + slope * slope);
    float aa = max(fwidth(distancePx), 0.75);
    return 1.0 - smoothstep(1.0 - aa, 1.0 + aa, distancePx);
}

void main()
{
    vec2 localPosition = gl_FragCoord.xy - uViewportOrigin;
    vec2 graph = uCenter + (localPosition - uViewport * 0.5) / uPixelsPerUnit;

    float x = graph.x;
    float inDomain = step(0.0, x) * step(x, 1.0);
    vec4 sampleValue = texture(uSamples, clamp(x, 0.0, 1.0));

    float red = CurveLine(graph.y, sampleValue.r) * inDomain * uVisibleChannels.r;
    float green = CurveLine(graph.y, sampleValue.g) * inDomain * uVisibleChannels.g;
    float blue = CurveLine(graph.y, sampleValue.b) * inDomain * uVisibleChannels.b;
    float alpha = CurveLine(graph.y, sampleValue.a) * inDomain * uVisibleChannels.a;

    float total = red + green + blue + alpha;

    vec3 curveColor = (
        vec3(1.0, 0.24, 0.28) * red +
        vec3(0.25, 0.90, 0.43) * green +
        vec3(0.25, 0.58, 1.0) * blue +
        vec3(0.88, 0.88, 0.90) * alpha
    ) / max(total, 0.0001);

    FragColor = vec4(curveColor, clamp(total, 0.0, 1.0));
}
