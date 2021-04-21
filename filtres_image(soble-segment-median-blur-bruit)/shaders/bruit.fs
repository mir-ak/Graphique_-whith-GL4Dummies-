/*!\file bruit.fs
 *
 * \brief fragment shader multipliant deux textures.
 * \author Farès BELHADJ, amsi@up8.edu
 * \date March 03 2021
 */
/* Version GLSL 3.30 */
#version 330
in  vec4 vsoColor;
in  vec2 vsoTexCoord;

out vec4 fragColor;
uniform vec2 pas;
uniform sampler2D tex;
uniform sampler2D rand;

vec4 bruit(){
  vec2 offset[25] = vec2[]( vec2(-2.0 * pas.x, -2.0 * pas.y), vec2(-pas.x, -2.0 * pas.y), vec2(0, -2.0 * pas.y), vec2(pas.x, -2.0 * pas.y), vec2(2.0 * pas.x, -2.0 * pas.y),
			    vec2(-2.0 * pas.x, -1.0 * pas.y), vec2(-pas.x, -1.0 * pas.y), vec2(0, -1.0 * pas.y), vec2(pas.x, -1.0 * pas.y), vec2(2.0 * pas.x, -1.0 * pas.y),
			    vec2(-2.0 * pas.x, 0.0), vec2(-pas.x, 0.0), vec2(0, 0.0), vec2(pas.x, 0.0), vec2(2.0 * pas.x, 0.0),
			    vec2(-2.0 * pas.x, 1.0 * pas.y), vec2(-pas.x, 1.0 * pas.y), vec2(0, 1.0 * pas.y), vec2(pas.x, 1.0 * pas.y), vec2(2.0 * pas.x, 1.0 * pas.y),
			    vec2(-2.0 * pas.x, 2.0 * pas.y), vec2(-pas.x, 2.0 * pas.y), vec2(0, 2.0 * pas.y), vec2(pas.x, 2.0 * pas.y), vec2(2.0 * pas.x, 2.0 * pas.y));

  vec4 number = texture(rand, vsoTexCoord);
  int truc = int(number.r * 255 + 0.5);
  vec4 c = texture(tex, vsoTexCoord + offset[truc]);

  return vec4(c.r, c.g, c.b, 1.0);
}

void main(void) {
  fragColor = bruit();
}
