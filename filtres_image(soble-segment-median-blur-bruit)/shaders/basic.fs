/*!\file basic.fs
 *
 * \brief fragment shader calculant un flou moyenné.
 * \author Farès BELHADJ, amsi@up8.edu
 * \date February 17 2021
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

void main(void) {
  /* la couleur de sortie est le vsoColor linéairement interpolé au
     fragment (voir attribut flat pour désactiver l'interpolation). */
  /* fragColor = vsoColor; */
  /* la couleur de sortie représente les coordonnées de texture. */
  /* fragColor = vec4(vsoTexCoord, 0, 1); */
  /* la couleur de sortie est piochée dans une texture aux coordonnées de texture transmises. */
  /* fragColor = texture(tex, vsoTexCoord); */
  /* la couleur est la moyenne des r*r texels autour de la position du fragment. */
  int n = 0 ;
  float r = 15.0;
  vec4 c = vec4(0);
  for(float y = -r * pas.y; y <= r * pas.y; y += pas.y)
    for(float x = -r * pas.x; x <= r * pas.x; x += pas.x) {
      c += texture(tex, vec2(vsoTexCoord.x + x, vsoTexCoord.y + y));
      n++;
    }
  fragColor = c / float(n);
}
