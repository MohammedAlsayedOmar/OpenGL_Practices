#shader vertex
#version 330 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec3 color;

out vec3 Color;

void main()
{
	Color = color;
	gl_Position = position;
};

#shader fragment
#version 330 core

in vec3 Color;
out vec4 outColor;
uniform vec4 uColor;

void main()
{
	//outColor = vec4(0.9, 0.6, 0.2, 1.0);
	outColor = vec4(Color,1);
};