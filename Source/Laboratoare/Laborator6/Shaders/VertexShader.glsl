#version 330

// TODO: get vertex attributes from each location
layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_color;
layout(location = 2) in vec2 v_coord;
layout(location = 3) in vec3 v_normal;
// Uniform properties
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;
uniform float Clock;
// TODO: output values to fragment shader
out vec2 frag_coord;
out vec3 frag_normal;
out vec3 frag_color;
out vec3 frag_position;

void main()
{
	// TODO: send output to fragment shader
	frag_position = v_position;
	frag_normal = v_normal;
	frag_color = v_color * (1 - abs(sin (Clock)));
	frag_coord = v_coord;
	// TODO: compute gl_Position
	gl_Position = Projection * (View + vec3(0,(sin(Clock)))) * Model * vec4(v_position + vec3(0,(sin(Clock)),0), 1.0);
}
