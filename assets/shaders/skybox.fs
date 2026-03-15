#version 330
in vec3 fragPosition;
uniform samplerCube environmentMap;
uniform bool vflipped;
uniform bool doGamma;
out vec4 finalColor;

void main() {
    vec3 dir = fragPosition;
    if (vflipped) dir.y = -dir.y;
    vec3 color = texture(environmentMap, dir).rgb;
    if (doGamma) color = pow(color, vec3(1.0/2.2));
    finalColor = vec4(color, 1.0);
}