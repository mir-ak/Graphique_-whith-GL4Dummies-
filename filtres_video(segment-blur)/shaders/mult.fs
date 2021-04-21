/*!\file mult.fs
 *
 * \brief fragment shader multipliant deux textures.
 * \author Farès BELHADJ, amsi@up8.edu
 * \date March 03 2021
 */
/* Version GLSL 3.30 */
#version 330
/* Le fragment shader est directement en relation avec les sorties du vertex shader */
in  vec4 vsoColor;
in  vec2 vsoTexCoord;
/* sortie du frament shader : une couleur */
out vec4 fragColor;
uniform vec2 pas;
uniform sampler2D tex0, tex1, tex2;

void main(void) {
  fragColor = texture(tex0, vsoTexCoord) * texture(tex1, vsoTexCoord) * texture(tex2, vsoTexCoord);
}
