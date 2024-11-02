#include "Graphics/Camera.h"
#include "Math/Utility.h"
#include "Graphics/Renderer.h"
#include "Core/Engine.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace SnackerEngine
{
	//------------------------------------------------------------------------------------------------------
	static float computeAspect(Vec2i dimensions)
	{
		return static_cast<float>(dimensions.x) / static_cast<float>(dimensions.y);
	}
	//------------------------------------------------------------------------------------------------------
	Camera::Camera() 
		: viewMatrix(0.0f), projectionMatrix(0.0f), viewProjMatrix(0.0f), position(0.0f), direction(0.0f, 0.0f, 1.0f), up(0.0f, 1.0f, 0.0f), target(0.0f, 0.0f, 1.0f),
		pitch(0.0f), yaw(degToRad(180.0f)), fovy(degToRad(45.0f)), aspect(computeAspect(Renderer::getScreenDimensions())), near(0.1f), far(100.0f), cameraMode(CameraMode::DIRECTION)
	{
		computePitchAndYawFromDirectionAndTarget();
		computeProjection();
		computeView();
		viewProjMatrix = viewMatrix * projectionMatrix;
	}
	//------------------------------------------------------------------------------------------------------
	void Camera::computeDirectionAndTargetFromPitchAndYaw()
	{
		float cosPitch = cos(pitch);
		float sinPitch = sin(pitch);
		float cosYaw = cos(yaw);
		float sinYaw = sin(yaw);
		up = { sinYaw * sinPitch, cosPitch, cosYaw * sinPitch }; // y-axis
		direction = { -sinYaw * cosPitch, sinPitch, -cosPitch * cosYaw }; // z-axis
		target = position + direction;
	}
	//------------------------------------------------------------------------------------------------------
	void Camera::computePitchAndYawFromDirectionAndTarget()
	{
		// yeeted from https://stackoverflow.com/questions/52228411/compute-target-roll-pitch-and-yaw-angles-given-initial-and-target-positions-and
		pitch = asin(direction.y / direction.magnitude());
		yaw = atan2(direction.x, -direction.z);
	}
	//------------------------------------------------------------------------------------------------------
	Camera::~Camera() {}
	//------------------------------------------------------------------------------------------------------
	void Camera::computeView()
	{
		switch (cameraMode)
		{
		case SnackerEngine::Camera::CameraMode::TARGET: 
		{
			viewMatrix = Mat4f::ViewLookAt(position, target, up);
			break;
		}
		case SnackerEngine::Camera::CameraMode::DIRECTION:
		{
			viewMatrix = Mat4f::ViewPositionDirection(position, direction, up);
			break;
		}
		default:
			break;
		}
		viewProjMatrix = viewMatrix * projectionMatrix;
	}
	//------------------------------------------------------------------------------------------------------
	void Camera::computeProjection()
	{
		projectionMatrix = Mat4f::PerspectiveProjection(fovy, aspect, near, far);
		viewProjMatrix = viewMatrix * projectionMatrix;
	}
	//------------------------------------------------------------------------------------------------------
	void Camera::setTargetVector(const Vec3f& target)
	{
		cameraMode = CameraMode::TARGET;
		this->target = target;
		direction = target - position;
		direction.normalize();
		Vec3f right = cross(up, direction);	// The "right" vector.
		right.normalize();
		up = cross(direction, right);	// The "up" vector.
		computeView();
	}
	//------------------------------------------------------------------------------------------------------
	void Camera::setPosition(const Vec3f& position)
	{
		this->position = position;
		switch (cameraMode)
		{
		case SnackerEngine::Camera::CameraMode::TARGET: 
		{
			direction = target - position;
			break;
		}
		case SnackerEngine::Camera::CameraMode::DIRECTION:
		{
			target = position + direction;
			break;
		}
		default:
			break;
		}
	}
	//------------------------------------------------------------------------------------------------------
	void Camera::setDirection(const Vec3f& direction)
	{
		cameraMode = CameraMode::DIRECTION;
		this->direction = direction;
		this->direction.normalize();
		target = position + direction;
		Vec3f right = cross(up, direction);	// The "right" vector.
		right.normalize();
		up = cross(direction, right);	// The "up" vector.
		computeView();
	}
	//------------------------------------------------------------------------------------------------------
	void Camera::setPitch(const float& pitch)
	{
		cameraMode = CameraMode::DIRECTION;
		this->pitch = pitch;
		computeDirectionAndTargetFromPitchAndYaw();
	}
	//------------------------------------------------------------------------------------------------------
	void Camera::setYaw(const float& yaw)
	{
		cameraMode = CameraMode::DIRECTION;
		this->yaw = yaw;
		computeDirectionAndTargetFromPitchAndYaw();
	}
	//------------------------------------------------------------------------------------------------------
	void Camera::setFov(const float& fovy)
	{
		this->fovy = fovy;
		computeProjection();
	}
	//------------------------------------------------------------------------------------------------------
	void Camera::setAspect(const Vec2i& dimensions)
	{
		this->aspect = computeAspect(dimensions);
		computeProjection();
	}
	//------------------------------------------------------------------------------------------------------
	void Camera::setAspect(const float& aspect)
	{
		this->aspect = aspect;
		computeProjection();
	}
	//------------------------------------------------------------------------------------------------------
	void Camera::setNearPlane(const float& near)
	{
		this->near = near;
		computeProjection();
	}
	//------------------------------------------------------------------------------------------------------
	void Camera::setFarPlane(const float& far)
	{
		this->far = far;
		computeProjection();
	}
	//------------------------------------------------------------------------------------------------------
	Mat4f Camera::getViewMatrix() const
	{
		return viewMatrix;
	}
	//------------------------------------------------------------------------------------------------------
	Mat4f Camera::getProjectionMatrix() const
	{
		return projectionMatrix;
	}
	//------------------------------------------------------------------------------------------------------
	Mat4f Camera::getViewProjectionMatrix() const
	{
		return viewProjMatrix;
	}
	//------------------------------------------------------------------------------------------------------
	Vec3f Camera::getPosition() const
	{
		return position;
	}
	//------------------------------------------------------------------------------------------------------
	float Camera::getNearPlane() const
	{
		return near;
	}
	//------------------------------------------------------------------------------------------------------
	float Camera::getFarPlane() const
	{
		return far;
	}
	//------------------------------------------------------------------------------------------------------
	void Camera::callbackWindowResize(const Vec2i& dimensions)
	{
		setAspect(dimensions);
	}
	//------------------------------------------------------------------------------------------------------
	void FPSCamera::computeRightFromDirectionAndUp()
	{
		right = cross(up, direction); // The "right" vector.
		right.normalize();
	}
	//------------------------------------------------------------------------------------------------------
	FPSCamera::FPSCamera()
		: Camera(), right(-1.0f, 0.0f, 0.0f), globalUp(0.0f, 1.0f, 0.0f), speed(5.0f), angleSpeed(0.05f), speedFactor(1.01f), lastMousePosition(0.0f), isMouseMovement(false), isMovement(false),
		movingRight(false), movingLeft(false), movingUp(false), movingDown(false), movingForward(false), movingBackward(false)
	{
	}
	//------------------------------------------------------------------------------------------------------
	FPSCamera::~FPSCamera()
	{
	}
	//------------------------------------------------------------------------------------------------------
	void FPSCamera::setSpeed(const float& speed)
	{
		this->speed = speed;
	}
	//------------------------------------------------------------------------------------------------------
	void FPSCamera::setAngleSpeed(const float& angleSpeed)
	{
		this->angleSpeed = angleSpeed;
	}
	//------------------------------------------------------------------------------------------------------
	void FPSCamera::moveMouse(const Vec2d& delta)
	{
		yaw = fmodf(yaw + angleSpeed * static_cast<float>(delta.x) * angleSpeed, 2 * static_cast<float>(M_PI));
		pitch -= angleSpeed * static_cast<float>(delta.y) * angleSpeed;
		// Clip pitch
		if (pitch < -static_cast<float>(M_PI_2)) pitch = -static_cast<float>(M_PI_2);
		else if (pitch > static_cast<float>(M_PI_2)) pitch = static_cast<float>(M_PI_2);
		computeDirectionAndTargetFromPitchAndYaw();
		computeRightFromDirectionAndUp();
	}
	//------------------------------------------------------------------------------------------------------
	void FPSCamera::update(const double& dt)
	{
		Vec3f velocity(0.0f);
		if (movingForward) velocity += direction;
		else if (movingBackward) velocity -= direction;
		if (movingLeft) velocity -= right;
		else if (movingRight) velocity += right;
		if (movingUp) velocity += globalUp;
		else if (movingDown) velocity -= globalUp;
		velocity.normalize();
		velocity *= speed;
		position += velocity * static_cast<float>(dt);
	}
	//------------------------------------------------------------------------------------------------------
	void FPSCamera::enableMouseMovement()
	{
		lastMousePosition = static_cast<Vec2d>(Engine::constrainMouseCenter(true));
		Renderer::hideCursor();
		isMouseMovement = true;
	}
	//------------------------------------------------------------------------------------------------------
	void FPSCamera::disableMouseMovement()
	{
		Engine::constrainMouseCenter(false);
		Renderer::unhideCursor();
		isMouseMovement = false;
	}
	//------------------------------------------------------------------------------------------------------
	void FPSCamera::toggleMouseMovement()
	{
		if (isMouseMovement)
		{
			disableMouseMovement();
		}
		else
		{
			enableMouseMovement();
		}
	}
	//------------------------------------------------------------------------------------------------------
	void FPSCamera::enableMovement()
	{
		isMovement = true;
	}
	//------------------------------------------------------------------------------------------------------
	void FPSCamera::disableMovement()
	{
		isMovement = false;
		movingBackward = false;
		movingDown = false;
		movingForward = false;
		movingLeft = false;
		movingRight = false;
		movingUp = false;
	}
	//------------------------------------------------------------------------------------------------------
	void FPSCamera::toggleMovement()
	{
		if (isMovement)
		{
			disableMovement();
		}
		else
		{
			enableMovement();
		}
	}
	//------------------------------------------------------------------------------------------------------
	void FPSCamera::callbackKeyboard(const int& key, const int& scancode, const int& action, const int& mods)
	{
		if (!isMovement) return;
		if (action == GLFW_PRESS)
		{
			switch (key)
			{
			case GLFW_KEY_W: movingForward = true; movingBackward = false; break;
			case GLFW_KEY_S: movingBackward = true; movingForward = false; break;
			case GLFW_KEY_A: movingLeft = true; movingRight = false; break;
			case GLFW_KEY_D: movingRight = true; movingLeft = false; break;
			case GLFW_KEY_E: movingUp = true; movingDown = false; break;
			case GLFW_KEY_Q: movingDown = true;  movingUp = false; break;
			default: break;
			}
		}
		else if (action == GLFW_RELEASE)
		{
			switch (key)
			{
			case GLFW_KEY_W: movingForward = false; break;
			case GLFW_KEY_S: movingBackward = false; break;
			case GLFW_KEY_A: movingLeft = false; break;
			case GLFW_KEY_D: movingRight = false; break;
			case GLFW_KEY_E: movingUp = false; break;
			case GLFW_KEY_Q: movingDown = false; break;
			default: break;
			}
		}
	}
	//------------------------------------------------------------------------------------------------------
	void FPSCamera::callbackMouseMotion(const Vec2d& position)
	{
		if (isMouseMovement)
		{
			moveMouse(position - lastMousePosition);
		}
	}
	//------------------------------------------------------------------------------------------------------
	void FPSCamera::callbackMouseScroll(const Vec2d& offset)
	{
		if (isMouseMovement)
		{
			if (offset.y > 0)
			{
				speed *= (static_cast<float>(offset.y) * speedFactor);
			}
			else
			{
				speed /= (-static_cast<float>(offset.y) * (speedFactor));
			}
		}
	}
	//------------------------------------------------------------------------------------------------------
	bool FPSCamera::isEnableMovement()
	{
		return isMovement;
	}
	//------------------------------------------------------------------------------------------------------
	bool FPSCamera::isEnableMouse()
	{
		return isMouseMovement;
	}
	//------------------------------------------------------------------------------------------------------
	void FPSCamera::setTargetVector(const Vec3f& target)
	{
		Camera::setTargetVector(target);
		computeRightFromDirectionAndUp();
	}
	//------------------------------------------------------------------------------------------------------
	void FPSCamera::setPosition(const Vec3f& position)
	{
		Camera::setPosition(position);
		computeRightFromDirectionAndUp();
	}
	//------------------------------------------------------------------------------------------------------
	void FPSCamera::setDirection(const Vec3f& direction)
	{
		Camera::setDirection(direction);
		computeRightFromDirectionAndUp();
	}
	//------------------------------------------------------------------------------------------------------
	void FPSCamera::setPitch(const float& pitch)
	{
		Camera::setPitch(pitch);
		computeRightFromDirectionAndUp();
	}
	//------------------------------------------------------------------------------------------------------
	void FPSCamera::setYaw(const float& yaw)
	{
		Camera::setYaw(yaw);
		computeRightFromDirectionAndUp();
	}
	//------------------------------------------------------------------------------------------------------
}