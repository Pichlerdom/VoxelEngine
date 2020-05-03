#version 330
// It was expressed that some drivers required this next line to function properly
precision highp float;
 
in vec4 ex_Color;
flat in vec3 ex_Normal;
in vec3 ex_light_pos;

out vec4 fragColor;
 
void main(void) {
     	vec3 normalized_normal = normalize(ex_Normal);
		
	fragColor = ex_Color* vec4(vec3(1.0f,1.0f,1.0f) *  dot(normalized_normal, normalize(ex_light_pos)),1.0f);
}

