#include <Player.hpp>

void Player::Update(const float deltaTime, const LowRenderer::Camera& cam)
{
    if (Object->Velocity.y < -20.f)
        Object->Velocity.y = -20.f;
    Object->Velocity = Vec3(0.f, Object->Velocity.y, 0.f);

    Vec2 forward = Vec2(cam.Front.x, cam.Front.z).Normalize();

    if (PlayerInput.moveForward)
        Object->Velocity -= Vec3(forward.x, 0.f, forward.y) * mSpeed;
    if (PlayerInput.moveBackward)
        Object->Velocity += Vec3(forward.x, 0.f, forward.y) * mSpeed;

    if (PlayerInput.moveLeft)
        Object->Velocity += Vec3(cam.Right.x, 0.f, cam.Right.z) * mSpeed;
    if (PlayerInput.moveRight)
        Object->Velocity -= Vec3(cam.Right.x, 0.f, cam.Right.z) * mSpeed;
    
    if (CanJump && PlayerInput.moveJump)
    {
        Object->Velocity.y = mJumpHeight;
        CanJump = false;
    }

    Object->Transform.Position += Object->Velocity * deltaTime;

    if (Object->Transform.Position.y < -5)
        InitPos();
}

void Player::InitPos()
{
    Object->Velocity = 0;
    Object->Transform.Position = Vec3(0.0f, 5.0f, 0.0f);
}