/*!\file basic.fs
 *
 * \brief fragment shader calculant un filtre de flou Gaussien.
 * \author Farès BELHADJ, amsi@up8.edu
 * \date March 03 2021
 */
/* Version GLSL 3.30 */
#version 330
uniform sampler2D myTexture;
uniform vec4 couleur;
in  vec2 vsoTexCoord;
out vec4 fragColor;

void main(void) {
  fragColor = couleur * texture(myTexture, vsoTexCoord.st);
}
