/*!\file segment.fs
 *
 * \brief fragment shader calculant un filtre Sobel.
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
uniform sampler2D tex;

vec4 segment() {
  float eps = 0.0001;
  const vec2 G[9] = vec2[]( vec2(-1, -1), vec2(0, -2), vec2(1, -1),
			    vec2(-2, 0),  vec2(0, 0),  vec2(2, 0),
			    vec2(-1, 1),  vec2(0, 2),  vec2(1, 1) );
  float r,g,b;
  vec4 c;
  c = texture(tex, vsoTexCoord);
  r = (float(int(4*c.r)-eps))/(4-1.0);
  g = (float(int(4*c.g)-eps))/(4-1.0);
  b = (float(int(4*c.b)-eps))/(4-1.0);
  return vec4(r,g,b,1.0);
}

void main(void) {
  fragColor = segment();
}
