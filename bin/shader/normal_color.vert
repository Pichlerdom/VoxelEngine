#version 330

// in_Position was bound to attribute index 0 and in_Color was bound to attribute index 1

in vec3 in_Position;
in vec4 in_Color;
in vec3 in_Normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform mat4 normal_mat;
uniform vec3 light_pos;

// We output the ex_Color variable to the next shader in the chain
out vec4 ex_Color;
flat out vec3 ex_Normal;
out vec3 ex_light_pos;

void main(void) {
	gl_Position =  projection * view * model * vec4(in_Position, 1.0);

	ex_Normal = mat3(normal_mat) * in_Normal;
	
	ex_Color = in_Color;
	
	ex_light_pos = mat3(view * model) * light_pos;	
}
