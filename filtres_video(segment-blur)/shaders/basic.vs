/*!\file basic.vs
 *
 * \brief fragment shader calculant un filtre de flou Gaussien.
 * \author Farès BELHADJ, amsi@up8.edu
 * \date March 03 2021
 */
/* Version GLSL 3.30 */
#version 330

layout (location = 0) in vec3 vsiPosition;
layout (location = 1) in vec2 vsiTexCoord;
uniform mat4 projectionMatrix, modelviewMatrix;
out vec2 vsoTexCoord;

void main(void) {
  gl_Position = projectionMatrix * modelviewMatrix * vec4(vsiPosition, 1.0);
  vsoTexCoord = vec2(vsiTexCoord.s, vsiTexCoord.t);
}
