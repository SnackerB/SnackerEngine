#pragma once

#include "Math/Vec.h"
#include "Math/Mat.h"

namespace SnackerEngine
{
	//------------------------------------------------------------------------------------------------------
	/// Class that represents a point of view in a 3D scene, can be used for easier rendering
	class Camera
	{
	public:
		/// Camera mode specifies how the viewMatrix is calculated
		enum class CameraMode {
			TARGET,		/// Camera looks at a given target
			DIRECTION,	/// Camera looks in a given direction
		};
	protected:
		/// View Matrix
		Mat4f viewMatrix;
		/// Projection matrix
		Mat4f projectionMatrix;
		/// View Matrix * Projection Matrix
		Mat4f viewProjMatrix;
		/// Position of the camera
		Vec3f position;
		/// Direction in which the camera is looking
		Vec3f direction;
		/// Vector pointing up from the perspective of the camera
		Vec3f up;
		/// Vector pointing right from the perspective of the camera
		Vec3f target;
		// Rotation around x-axis in [-pi/2, pi/2]
		float pitch;
		// Rotation around y-axis in [0, 2pi]
		float yaw;
		// Field of view along y-axis of the camera
		float fovy;
		// Aspect ration of the screen
		float aspect;
		// Near clipping plane
		float near;
		// Far clipping plane
		float far;
		/// Mode for computing the view matrix
		CameraMode cameraMode;
		/// Helper function that computes target and direction vectors from the pitch and the yaw.
		void computeDirectionAndTargetFromPitchAndYaw();
		/// Helper function that computes pitch and yaw from position and direction vectors
		void computePitchAndYawFromDirectionAndTarget();
	public:
		/// Constructor
		Camera();
		/// Destructor
		~Camera();
		/// Computes the view matrix
		void computeView();
		/// Computes the view matrix and multiplies it with the projection matrix
		void computeProjection();
		/// Getters and Setters
		virtual void setTargetVector(const Vec3f& target); /// Sets mode to TARGET
		virtual void setPosition(const Vec3f& position);
		virtual void setDirection(const Vec3f& direction); /// Sets mode to DIRECTION
		virtual void setPitch(const float& pitch);	/// Sets mode to DIRECTION
		virtual void setYaw(const float& yaw); /// Sets mode to DIRECTION
		
		void setFov(const float& fovy); 
		void setAspect(const Vec2i& dimensions);
		void setAspect(const float& aspect);
		void setNearPlane(const float& near); 
		void setFarPlane(const float& far);

		Mat4f getViewMatrix() const;
		Mat4f getProjectionMatrix() const;
		Mat4f getViewProjectionMatrix() const;
		Vec3f getPosition() const;
		float getNearPlane() const;
		float getFarPlane() const;

		/// Callback function for window resize
		void callbackWindowResize(const Vec2i& screenDims);
	};
	//------------------------------------------------------------------------------------------------------
	class FPSCamera : public Camera
	{
	protected:
		/// global up vector
		Vec3f globalUp;
		/// We now need a right vector for camera movement
		Vec3f right;
		/// How fast the camera moves
		float speed;
		/// How fast the camera is looking arounf
		float angleSpeed;
		/// How much the speed changes when using the scroll wheel
		float speedFactor;
		/// Last mouse position
		Vec2d lastMousePosition;
		/// If moving the mouse (i.e. looking around) is allowed
		bool isMouseMovement;
		/// If moving the camera position via keyboard input is allowed
		bool isMovement;
		/// Internal bools for updating the movement
		bool movingRight, movingLeft, movingUp, movingDown, movingForward, movingBackward;
		/// Helper function that computes the vector pointing right from the direction and the global up vector
		void computeRightFromDirectionAndUp();
	public:
		/// Constructor
		FPSCamera();
		/// Destructor
		~FPSCamera();
		/// Setters
		void setSpeed(const float& speed);
		void setAngleSpeed(const float& angleSpeed);
		/// Update function after delta mouse movement of (dx, dy)
		void moveMouse(const Vec2d& delta);
		/// Update function, should be called as often as possible
		void update(const double& dt);
		/// Enabling and disabling different flags
		void enableMouseMovement();
		void disableMouseMovement();
		void toggleMouseMovement();
		void enableMovement();
		void disableMovement();
		void toggleMovement();
		/// Callback function for keyboard input
		void callbackKeyboard(const int& key, const int& scancode, const int& action, const int& mods);
		/// Callback function for mouse motion
		void callbackMouseMotion(const Vec2d& position);
		/// Callback function for mouse scrolling
		void callbackMouseScroll(const Vec2d& offset);
		/// Getters for flags
		bool isEnableMovement();
		bool isEnableMouse();
		/// Getters and Setters we need to overwrite (because we need to update additional right vector)
		void setTargetVector(const Vec3f& target) override; /// Sets mode to TARGET
		void setPosition(const Vec3f& position) override;
		void setDirection(const Vec3f& direction) override; /// Sets mode to DIRECTION
		void setPitch(const float& pitch) override; /// Sets mode to DIRECTION
		void setYaw(const float& yaw) override; /// Sets mode to DIRECTION
	};
	//------------------------------------------------------------------------------------------------------
}