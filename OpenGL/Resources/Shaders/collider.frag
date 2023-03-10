#version 330 core

out vec4 FragColor;

uniform bool isColliding;

void main(){
	if(isColliding)
		FragColor = vec4(1.0, 0.0, 0.0, 0.0);
	else
		FragColor = vec4(0.0, 1.0, 0.0, 0.0);
}