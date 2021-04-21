/*!\file basic.fs
 *
 * \brief .
 * \author Farès BELHADJ, amsi@up8.edu
 * \date June 5, 2020
 */
/* Version GLSL 3.30 */
#version 330
uniform sampler1D tex;
uniform int nbmob;
uniform sampler1D siteCoords, siteColors;
uniform vec2 dim;
out vec4 fragColor;
uniform int voronoi;
uniform int vrn;
in vec2 vsoTexCoord;

/* sortie du frament shader : une couleur */
float dist(vec2 a, vec2 b) {
  float dx = a.x - b.x, dy = a.y - b.y;
  return dx * dx + dy * dy;
}

vec4 mobile(void) {
  float pas = 1.0 / (2.0 * float(nbmob));
  float i;
  vec4 texel0, texel1;
  for(i = pas / 2.0; i <= 1.0; i += 2.0 * pas) {
    texel0 = texture(tex, i);
    texel1 = texture(tex, i + pas);
    if(length(texel0.xy - vsoTexCoord.xy) < 0.01)
      return vec4(texel1);
  }
  return vec4(0);
}

vec4 voronoi_simple_couleur(void) {
  vec2 c = gl_FragCoord.xy / dim;
  float pas = 1.0 / (2.0 * float(nbmob));
  float l,i, distanceEntreMoiEtLeMobileI;
  float mini = 0, minl = dist(c, texture(siteCoords, mini).rg);
  vec4 texel0, texel1;
  for(i = pas / 2.0 ; i <= 1.0; i += 2.0 * pas) {
    texel0 = texture(tex, i);
    texel1 = texture(tex, i +=pas);
    distanceEntreMoiEtLeMobileI = length(texel0.xy - vsoTexCoord.xy);
    if (minl > distanceEntreMoiEtLeMobileI) {
      minl = distanceEntreMoiEtLeMobileI;
      mini = i;
    }
    
  }
  if(minl > 0.0)
    return texture(siteColors, mini) + vec4(0, 0, 0, 1);
  return vec4(0,0,0,1);
}
vec4 voronoi_simple_couleur_point(void) {
  vec2 c = gl_FragCoord.xy / dim;
  float pas = 1.0 / (2.0 * float(nbmob-1));
  float l,i, distanceEntreMoiEtLeMobileI;
  float mini = 0, minl = dist(c, texture(siteCoords, mini).rg);
  vec4 texel0, texel1;
  for(i = pas / 2.0 ; i <= 1.0; i += 2.0 * pas) {
    texel0 = texture(tex, i);
    texel1 = texture(tex, i += pas);
    distanceEntreMoiEtLeMobileI = length(texel0.xy - vsoTexCoord.xy);
    if (minl > distanceEntreMoiEtLeMobileI) {
      minl = distanceEntreMoiEtLeMobileI;
      mini = i;
    }
    
  }
  if(minl > 0.004)
    return texture(siteColors, mini);
  return vec4(1,1,1,250);
}
vec4 voronoi_couleur_progressive(void) {
  vec2 c = gl_FragCoord.xy / dim;
  float pas = 1.0 / (4.0 * float(nbmob-1));
  float i, distanceEntreMoiEtLeMobileI;
  vec4 texel0, texel1, uneCouleur;
  float mini[2] = float[2](0, pas); 
  float minl[2] = float[2](dist(c, texture(siteCoords, mini[0]).rg), 
			   dist(c, texture(siteCoords, mini[1]).rg));
  if(minl[0] > minl[1]) {
    distanceEntreMoiEtLeMobileI = minl[0];
    minl[0] = minl[1];
    minl[1] = distanceEntreMoiEtLeMobileI;
    mini[0] = pas;
    mini[1] = 0;
  }
  for(i = pas / 2.0 ; i <= 1.0; i += 2.0 * pas) {
    texel0 = texture(tex, i);
    texel1 = texture(tex, i+=pas);
    distanceEntreMoiEtLeMobileI = length(texel0.xy - vsoTexCoord.xy);
    /* il faut trouver le moyen de calculer la bonne couleur */
    if(minl[0] > distanceEntreMoiEtLeMobileI) {
      minl[1] = minl[0];
      mini[1] = mini[0];
      minl[0] = distanceEntreMoiEtLeMobileI ;
      mini[0] = i;
    } 
    /*mettre des espacees entre chaque cellule*/
   else if(minl[1] > distanceEntreMoiEtLeMobileI ) {
      minl[1] = distanceEntreMoiEtLeMobileI ;
      mini[1] = i;
    }
  }
  if(minl[0] > 0.004) 
  	return ((minl[1] - minl[0]) * texture(siteColors, mini[0]) + 2.0 * minl[0] * vec4(0, 0, 0, 1)) / (minl[1] + minl[0]);
  
}
void main(void) {
	/*si vous etes dans la version 0.0 vous clique sur 'v' pour voir la version 0.1 si c'est pas*/ 
	/*le cas vous etes dans la version 0.2 vous clique sur 'v' pour voir la version 0.1 */
	/*pour revenir par defaut si vous etes toujour dans cette version vous clique une autre fois sur 'v' */
	if (voronoi != 0 ){
		/*version couleur 0.1 */
		fragColor = voronoi_simple_couleur_point();
		
	} 
	/*si vous etes dans dans la version 0.0 vous clique sur 'b' pour voir la version 0.2 si c'est pas */
	/*le cas (vous etes dans la version 0.1 vous clique sur b puis sur 'v' il vous remène direct vers version 0.1*/
	/*pour revenir par defaut si vous etes toujour dans cette version vous clique une autre fois sur 'b' */
	else if (vrn != 0){
		/*version couleur 0.2 */
		fragColor = voronoi_couleur_progressive();
	} 
	 /* version par defaut*/
	else {
		/*version couleur 0.0 */
		fragColor = voronoi_simple_couleur();

	}
}
