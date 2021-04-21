/*!\file blur.fs
 *
 * \brief fragment shader calculant un filtre de flou Gaussien.
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

vec4 flouGauss() {
  const float G[25] = float[]( 1.0 / 273.0, 4.0 / 273.0, 7.0 / 273.0, 4.0 / 273.0, 1.0 / 273.0,
			      4.0 / 273.0, 16.0 / 273.0, 26.0 / 273.0, 16.0 / 273.0, 4.0 / 273.0,
			      7.0 / 273.0, 26.0 / 273.0, 41.0 / 273.0, 26.0 / 273.0, 7.0 / 273.0,
			      4.0 / 273.0, 16.0 / 273.0, 26.0 / 273.0, 16.0 / 273.0, 4.0 / 273.0,
			      1.0 / 273.0, 4.0 / 273.0, 7.0 / 273.0, 4.0 / 273.0, 1.0 / 273.0 );
  vec2 offset[25] = vec2[]( vec2(-2.0 * pas.x, -2.0 * pas.y), vec2(-pas.x, -2.0 * pas.y), vec2(0, -2.0 * pas.y), vec2(pas.x, -2.0 * pas.y), vec2(2.0 * pas.x, -2.0 * pas.y),
			    vec2(-2.0 * pas.x, -1.0 * pas.y), vec2(-pas.x, -1.0 * pas.y), vec2(0, -1.0 * pas.y), vec2(pas.x, -1.0 * pas.y), vec2(2.0 * pas.x, -1.0 * pas.y),
			    vec2(-2.0 * pas.x, 0.0), vec2(-pas.x, 0.0), vec2(0, 0.0), vec2(pas.x, 0.0), vec2(2.0 * pas.x, 0.0),
			    vec2(-2.0 * pas.x, 1.0 * pas.y), vec2(-pas.x, 1.0 * pas.y), vec2(0, 1.0 * pas.y), vec2(pas.x, 1.0 * pas.y), vec2(2.0 * pas.x, 1.0 * pas.y),
			    vec2(-2.0 * pas.x, 2.0 * pas.y), vec2(-pas.x, 2.0 * pas.y), vec2(0, 2.0 * pas.y), vec2(pas.x, 2.0 * pas.y), vec2(2.0 * pas.x, 2.0 * pas.y));
  vec3 res = vec3(0.0);
  vec4 c;
  for(int i = 0; i < 25; ++i) {
    c = texture(tex, vsoTexCoord + offset[i]);
    res += c.rgb * G[i];
  }
  return vec4(res, 1.0);
}

void main(void) {
  fragColor = flouGauss();
}
