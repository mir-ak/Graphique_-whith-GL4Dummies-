/*!\file median.fs
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

float luminance(vec3 rgb) {
  vec3 irgb = vec3(0.299, 0.587, 0.114);
  return dot(rgb, irgb);
}

vec4 median(){
  vec2 offset[25] = vec2[]( vec2(-2.0 * pas.x, -2.0 * pas.y), vec2(-pas.x, -2.0 * pas.y), vec2(0, -2.0 * pas.y), vec2(pas.x, -2.0 * pas.y), vec2(2.0 * pas.x, -2.0 * pas.y),
			    vec2(-2.0 * pas.x, -1.0 * pas.y), vec2(-pas.x, -1.0 * pas.y), vec2(0, -1.0 * pas.y), vec2(pas.x, -1.0 * pas.y), vec2(2.0 * pas.x, -1.0 * pas.y),
			    vec2(-2.0 * pas.x, 0.0), vec2(-pas.x, 0.0), vec2(0, 0.0), vec2(pas.x, 0.0), vec2(2.0 * pas.x, 0.0),
			    vec2(-2.0 * pas.x, 1.0 * pas.y), vec2(-pas.x, 1.0 * pas.y), vec2(0, 1.0 * pas.y), vec2(pas.x, 1.0 * pas.y), vec2(2.0 * pas.x, 1.0 * pas.y),
			    vec2(-2.0 * pas.x, 2.0 * pas.y), vec2(-pas.x, 2.0 * pas.y), vec2(0, 2.0 * pas.y), vec2(pas.x, 2.0 * pas.y), vec2(2.0 * pas.x, 2.0 * pas.y));
  vec4 c;
  float nb[25];
  int i, j;
  float tmp;
  vec2 tmpVec;

  for(i=0; i < 25; ++i){
    c = texture(tex, vsoTexCoord + offset[i]);
    nb[i] = luminance(c.rgb);
  }

  for (i=1 ; i <= 8; i++) {
    j = i;
    while (j > 0 && nb[j-1] > nb[j]) {
      tmp = nb[j];
      nb[j] = nb[j-1];
      nb[j-1] = tmp;
      tmpVec = offset[j];
      offset[j] = offset[j-1];
      offset[j-1] = tmpVec;
      j--;
    }
  }

  c = texture(tex, vsoTexCoord + offset[4]);

  return vec4(c);
}

void main(void) {
  fragColor = median();
}
