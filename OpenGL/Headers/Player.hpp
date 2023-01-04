#pragma once
#include <LowRenderer/Camera.hpp>
#include <DataStructure/GameObject.hpp>

using namespace Core::Maths;


struct PlayerInputs
{
	bool moveForward = false;
	bool moveBackward = false;
	bool moveRight = false;
	bool moveLeft = false;
	bool moveJump = false;
};

struct KeyBoardInput
{
	int moveForward = GLFW_KEY_W;
	int moveBackward = GLFW_KEY_S;
	int moveRight = GLFW_KEY_D;
	int moveLeft = GLFW_KEY_A;
	int moveJump = GLFW_KEY_SPACE;
};

class Player
{
public:
	DataStructure::GameObject* Object;
	PlayerInputs PlayerInput;
	KeyBoardInput keyBoardInput;
	bool CanJump = false;

private:
	float mSpeed = 5.f;
	float mJumpHeight = 7.5f;

public:
	void Update(const float deltaTime, const LowRenderer::Camera& cam);
	void InitPos();
};


