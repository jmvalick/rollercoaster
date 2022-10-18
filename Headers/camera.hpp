#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include <heightmap.hpp>
#include <track.hpp>
#include <vector>

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT,
	UP,
	DOWN
};

// Default camera values
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 5.0f;
const float SENSITIVTY = 0.1f;
const float ZOOM = 45.0f;


// An abstract camera class that processes input and calculates the corresponding Eular Angles, Vectors and Matrices for use in OpenGL
class Camera
{
public:
	// Camera Attributes
	glm::vec3 Position;
	glm::vec3 Front;
	glm::vec3 Up;
	glm::vec3 Right;
	glm::vec3 carPosition; //holds car position
	glm::mat4 carRotationMat; //holds car rotation
	glm::vec3 WorldUp;
	// Eular Angles
	float Yaw;
	float Pitch;
	// Camera options
	float MovementSpeed;
	float MouseSensitivity;
	float Zoom;
	// Track movement parameters
	float s = 2.0f;  // Position you are on the track
	bool onTrack = true; // Whether or not you are following the track
	bool tPressed; //bool to check if T was pressed
	bool tCurPressed; //bool to check if T is currently press

	// Constructor with vectors
	Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVTY), Zoom(ZOOM)
	{
		Position = position;
		WorldUp = up;
		Yaw = yaw;
		Pitch = pitch;
		updateCameraVectors();
	}
	// Constructor with scalar values
	Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVTY), Zoom(ZOOM)
	{
		Position = glm::vec3(posX, posY, posZ);
		WorldUp = glm::vec3(upX, upY, upZ);
		Yaw = yaw;
		Pitch = pitch;
		updateCameraVectors();
	}

	// Returns the view matrix calculated using Eular Angles and the LookAt Matrix
	glm::mat4 GetViewMatrix()
	{
		return glm::lookAt(Position, Position + Front, Up);
	}

	// Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
	void ProcessKeyboard(Camera_Movement direction, float deltaTime)
	{
		float velocity = MovementSpeed * deltaTime;
		if (direction == FORWARD)
			Position += Front * velocity;
		if (direction == BACKWARD)
			Position -= Front * velocity;
		if (direction == LEFT)
			Position -= Right * velocity;
		if (direction == RIGHT)
			Position += Right * velocity;
		if (direction == UP)
			Position += WorldUp * velocity;
		if (direction == DOWN)
			Position -= WorldUp * velocity;
	}

	//  Find the next camera position based on the amount of passed time, the track, and the track position s (defined in this class).  You can just use your code from the track function. 
	void ProcessTrackMovement(float deltaTime, Track &track, GLFWwindow* window)
	{
		//position variables
		glm::vec3 cameraOffset = Up / 3.75f;
		glm::vec3 currentPos = Position - cameraOffset;
		glm::vec3 nextPos;
		
		//helper variables
		float passed_time = 0.0f;
		float distance = 0.0f;

		//velocity equation variables
		float velocity;
		float g = 2.0f;    //gravity
		float hmax = 0.0f; //find hmax by getting the highest point of the track
		for (int hCount = 0; hCount < track.orientations.size(); hCount++) {
			if (track.orientations[hCount].origin.y > hmax)
				hmax = track.orientations[hCount].origin.y + 0.5f;
		}
		float h = currentPos.y ; //current height

		velocity = sqrt(2*g*(hmax - h));

		//Figure out how far along the track you go
		while (passed_time < deltaTime) {

			tCurPressed = glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS;
			if (!tPressed && tCurPressed) {	//toggle off rail if T is pressed
				onTrack = false;
				tPressed = tCurPressed;
				return;
			}
			tPressed = tCurPressed;

			// Increment s by a small amount
			s += 0.001f;
			// Prevent array out of bounds
			if (s >= float(track.controlPoints.size()) + 2.0)
				s = 2.0;

			// Get next location
			nextPos = track.get_point(s);
			// Compute passed time to move that distance
			distance = glm::distance(currentPos, nextPos);
			passed_time += distance / velocity;
			// Update location
			currentPos = nextPos;
		}

		// Get amount for linear interpolation
		int index = int(floor((s- 2.0) * 10.0f));
		float blend = (s- 2.0) * 10.0f - float(index);

		// Get orientations to blend between
		Orientation ori_prev = track.orientations.at(index);
		Orientation ori_next = track.orientations.at((index + 1) % track.orientations.size());

		// Now actually blend the orientations
		Front = ori_prev.Front * (1.0f - blend) + ori_next.Front * blend;
		Up = ori_prev.Up * (1.0f - blend) + ori_next.Up * blend;
		Right = ori_prev.Right * (1.0f - blend) + ori_next.Right * blend;

		Position = currentPos + cameraOffset; //update camera position with vertical offset
		carPosition = Position - cameraOffset + Up/5.0f;  //update car position
		//take inverse of camera rotation for car rotation
		carRotationMat = glm::inverse(glm::mat4(glm::mat3(glm::lookAt(Position, Position + Front, Up)))); 
	}

	// Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
	void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
	{
		xoffset *= MouseSensitivity;
		yoffset *= MouseSensitivity;

		Yaw += xoffset;
		Pitch += yoffset;

		// Make sure that when pitch is out of bounds, screen doesn't get flipped
		if (constrainPitch)
		{
			if (Pitch > 89.0f)
				Pitch = 89.0f;
			if (Pitch < -89.0f)
				Pitch = -89.0f;
		}

		// Update Front, Right and Up Vectors using the updated Euler angles
		updateCameraVectors();
	}

	// Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
	//    Not really necessary, you can use this for something else if you like
	void ProcessMouseScroll(float yoffset)
	{
		if (Zoom >= 1.0f && Zoom <= 45.0f)
			Zoom -= yoffset;
		if (Zoom <= 1.0f)
			Zoom = 1.0f;
		if (Zoom >= 45.0f)
			Zoom = 45.0f;
	}

private:
	// Calculates the front vector from the Camera's (updated) Eular Angles
	void updateCameraVectors()
	{
		// Calculate the new Front vector
		glm::vec3 front;
		front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		front.y = sin(glm::radians(Pitch));
		front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		Front = glm::normalize(front);
		// Also re-calculate the Right and Up vector
		Right = glm::normalize(glm::cross(Front, WorldUp));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
		Up = glm::normalize(glm::cross(Right, Front));
	}
};
#endif
