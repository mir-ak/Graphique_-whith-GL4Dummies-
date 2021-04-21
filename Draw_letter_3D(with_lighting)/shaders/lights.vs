/*!\file basic.fs
 *
 * \brief renvoi du depth pour la shadow map. Shader vide.
 * \author Farès BELHADJ, amsi@ai.univ-paris8.fr 
 * \date March 13 2017
*/
#version 330

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 tC;

uniform mat4 proj;
uniform mat4 mod;
uniform mat4 view;
uniform mat4 lightViewMatrix;
uniform mat4 lightProjectionMatrix;

out vec4 mvpos;
out vec4 vsoSMCoord;
out vec3 N;
out vec3 T;
out vec2 texCoord;
void main() {
      const mat4 bias = mat4( 0.5, 0.0, 0.0, 0.0,
			  0.0, 0.5, 0.0, 0.0,
			  0.0, 0.0, 0.5, 0.0,
			  0.5, 0.5, 0.5, 1.0 );
     N = normalize(inverse(transpose(mod)) * vec4(normal, 0)).xyz;
     T.x = normal.z;
     T.y = normal.x;
     T.z = normal.y;
     T = normalize((inverse(transpose(mod)) * vec4(T,0)).xyz);
     texCoord = vec2(tC.x * mod[0][0], tC.y * mod[1][1]);
     mvpos = view * mod * vec4(pos, 1.0);
     gl_Position = proj * mvpos;
     vsoSMCoord  = bias * lightProjectionMatrix * lightViewMatrix * mod * vec4(pos, 1.0);
    
}
