/*!\file basic.fs
 *
 * \brief renvoi du depth pour la shadow map. Shader vide.
 * \author Farès BELHADJ, amsi@ai.univ-paris8.fr 
 * \date March 13 2017
*/
#version 330

in vec4 mvpos;
in vec3 N;
in vec3 T;
in vec2 texCoord;
in  vec4 vsoSMCoord;
out vec4 fragColor;

uniform sampler2D smTex;
uniform vec4 scolor;
uniform vec4 lcolor;
uniform vec4 lumpos;

void main() { 

     vec3 B = cross(T,N);
     vec3 newN = normalize(normalize(N) + 0.2 * sin(texCoord.x * 50.0) * T + 0.2 * sin(texCoord.y * 50.0) * B);
     //vec3 projCoords = vsoSMCoord.xyz / vsoSMCoord.w;
     vec4 ambient = 0.15f * lcolor * scolor; /* lacolor, sacolor */
     vec3 Ld = normalize(mvpos.xyz - lumpos.xyz);
     float ild = clamp(dot(newN, -(Ld)), 0, 1);
     vec4 diffus = (ild * lcolor) * scolor; /* ldcolor, sdcolor */
     vec3 R = reflect(Ld, newN);
     vec3 Vue = vec3(0, 0, -1);
     float ils = pow(clamp(dot(R, -Vue), 0, 1), 10);
     vec4 spec = ils * lcolor; /* sscolor */
     vec3 projCoords = vsoSMCoord.xyz / vsoSMCoord.w;
     fragColor = ambient + diffus + spec;  
}
