#version 410

in vec4 fragColor;

out vec4 outColor;

uniform float time;
uniform float alphaMin;

void main() {
    float alphaMax = 1.0;
    float frequency = 800.0;
    
    float alpha = ((alphaMax - alphaMin) * 0.5) * sin(time*frequency) + (alphaMax - alphaMin) * 0.5 + alphaMin;
    //fragColor.rgb
    outColor = vec4(fragColor.rgb , fragColor.a*alpha); //usar xyzw
}