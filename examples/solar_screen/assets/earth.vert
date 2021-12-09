#version 410

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

uniform vec4 lightDirWorldSpace;

out vec3 fragV;
out vec3 fragL;
out vec3 fragN;
out vec2 fragTexCoord;
out vec3 fragPObj;
out vec3 fragNObj;

void main() {
  vec3 lightPosition = vec3(0.0, 0.0, 0.0);
  vec3 worldPosition = (modelMatrix*vec4(inPosition, 1.0)).xyz;
  /* 
  Com essa equacao, a direcao da luz passa a ter origem no Sol e se espalha
  pelo espaco
  */
  vec3 lightDirection = normalize(lightPosition - worldPosition);

  vec3 L = lightDirection;
  mat3 normalMatrix = inverse(transpose(mat3(viewMatrix*modelMatrix)));
  vec3 P = (viewMatrix * modelMatrix * vec4(inPosition, 1.0)).xyz;
  vec3 N = normalMatrix * inNormal;

  fragL = L;
  fragV = -P;
  fragN = N;
  fragTexCoord = inTexCoord;
  fragPObj = inPosition;
  fragNObj = inNormal;

  gl_Position = projMatrix * vec4(P, 1.0);
}