#version 330 core
layout (points) in;
in vec4 colorv[];

layout (triangle_strip, max_vertices = 4) out;
out vec4 color;

uniform vec2 resolution;

// Main entry point
void main()
{
	vec2 size = 1 / resolution;

    vec2 offsetX = vec2(size.x, 0) * 2;
    vec2 offsetY = vec2(0, size.y) * 2;

	color = colorv[0];

    gl_Position.xy = gl_in[0].gl_Position.xy;
    EmitVertex();

    gl_Position.xy = gl_in[0].gl_Position.xy + offsetX;
    EmitVertex();

    gl_Position.xy = gl_in[0].gl_Position.xy + offsetY;
    EmitVertex();

    gl_Position.xy = gl_in[0].gl_Position.xy + offsetX + offsetY;
    EmitVertex();

    EndPrimitive();
}