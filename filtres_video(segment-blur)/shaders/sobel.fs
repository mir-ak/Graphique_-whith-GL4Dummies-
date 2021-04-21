/*!\file sobel.fs
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

float luminance(vec3 rgb) {
  vec3 irgb = vec3(0.299, 0.587, 0.114);
  return dot(rgb, irgb);
}

vec4 sobel() {
  const vec2 G[9] = vec2[]( vec2(-1, -1), vec2(0, -2), vec2(1, -1),
			    vec2(-2, 0),  vec2(0, 0),  vec2(2, 0),
			    vec2(-1, 1),  vec2(0, 2),  vec2(1, 1) );
  vec2 offset[9] = vec2[]( vec2(-pas.x, -pas.y), vec2(0, -pas.y), vec2(pas.x, -pas.y),
			   vec2(-pas.x, 0), vec2(0, 0), vec2(pas.x, 0),
			   vec2(-pas.x, pas.y), vec2(0, pas.y), vec2(pas.x, pas.y) );
  vec2 r = vec2(0, 0), g = vec2(0, 0), b = vec2(0, 0);
  vec4 c;
  for(int i = 0; i < 9; ++i) {
    c = texture(tex, vsoTexCoord + offset[i]);
    r += c.r * G[i];
    g += c.g * G[i];
    b += c.b * G[i];
  }
  //float l = 1.0 - luminance(vec3(length(r), length(g), length(b)));
  float l = luminance(vec3(length(r), length(g), length(b))) > 0.3 ? 0.0 : 1.0;
  return vec4(l, l, l, 1.0);
}

void main(void) {
  fragColor = sobel();
}
