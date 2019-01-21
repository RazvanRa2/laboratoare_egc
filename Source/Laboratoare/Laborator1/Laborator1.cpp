#include "Laborator1.h"

#include <vector>
#include <iostream>

#include <Core/Engine.h>

#define RADIUS 5
using namespace std;
int ok = 0;
int moveObject = 0;
int x = 0;
int y = 0;
int z = 0;
int angle = 0;
// Order of function calling can be seen in "Source/Core/World.cpp::LoopUpdate()"
// https://github.com/UPB-Graphics/Framework-EGC/blob/master/Source/Core/World.cpp

Laborator1::Laborator1()
{
}

Laborator1::~Laborator1()
{
}

void Laborator1::Init()
{
	// Load a mesh from file into GPU memory
	{
		Mesh* mesh = new Mesh("box");
		mesh->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "box.obj");
		meshes[mesh->GetMeshID()] = mesh;

		Mesh* archerMesh = new Mesh("archer");
		archerMesh->LoadMesh(RESOURCE_PATH::MODELS + "Characters/Archer", "Archer.fbx");
		meshes[archerMesh->GetMeshID()] = archerMesh;

		Mesh* mesh2 = new Mesh("teapot");
		mesh2->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "teapot.obj");
		meshes[mesh2->GetMeshID()] = mesh2;
	}
}

void Laborator1::FrameStart()
{

}

void Laborator1::Update(float deltaTimeSeconds)
{
	glm::ivec2 resolution = window->props.resolution;

	// sets the clear color for the color buffer
	//glClearColor(0, 0, 0, 1);

	// clears the color buffer (using the previously set color) and depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// sets the screen area where to draw
	glViewport(0, 0, resolution.x, resolution.y);

	// render the object
	RenderMesh(meshes["box"], glm::vec3(cos(angle), sin(angle), 0), glm::vec3(0.5f));
	angle++;
	// render the object again but with different properties
	RenderMesh(meshes["box"], glm::vec3(-1 + x, 0.5f + y, 0 + z));
	if (window->KeyHold(GLFW_KEY_Q)) {
		x += 200 * deltaTimeSeconds;
	}
	if (window->KeyHold(GLFW_KEY_A)) {
		x -= 200 * deltaTimeSeconds;
	}
	if (window->KeyHold(GLFW_KEY_W)) {
		y += 200 * deltaTimeSeconds;
	}
	if (window->KeyHold(GLFW_KEY_S)) {
		y -= 200 * deltaTimeSeconds;
	}
	if (window->KeyHold(GLFW_KEY_E)) {
		z += 200 * deltaTimeSeconds;
	}
	if (window->KeyHold(GLFW_KEY_D)) {
		z -= 200 * deltaTimeSeconds;
	}
	// render archer
	RenderMesh(meshes["archer"], glm::vec3(5, 5, 0), glm::vec3(0.05f));
	if (ok % 3 == 0) {
		RenderMesh(meshes["archer"], glm::vec3(-5, 0.5f, 0), glm::vec3(0.05f));
	}
	else {
		if (ok % 3 == 1) {
			RenderMesh(meshes["box"], glm::vec3(-5, 0.5f, 0), glm::vec3(0.05f));
		}
		else {
			RenderMesh(meshes["teapot"], glm::vec3(-5, 0.5f, 0), glm::vec3(0.05f));
		}
	}
}

void Laborator1::FrameEnd()
{
	DrawCoordinatSystem();
}

// Read the documentation of the following functions in: "Source/Core/Window/InputController.h" or
// https://github.com/UPB-Graphics/Framework-EGC/blob/master/Source/Core/Window/InputController.h

void Laborator1::OnInputUpdate(float deltaTime, int mods)
{
	// treat continuous update based on input
};

void Laborator1::OnKeyPress(int key, int mods)
{
	// add key press event
	if (key == GLFW_KEY_F) {
		glClearColor(0.5, 0.3, 0.3, 1);
	}
	if (key == GLFW_KEY_G) {
		ok++;
	}

	if (key == GLFW_KEY_P) {
		moveObject = 1;
	}

	if (moveObject != 0) {
		if (key == GLFW_KEY_Q) {
			//x++;
		}
		if (key == GLFW_KEY_A) {
			//x--;
		}
		if (key == GLFW_KEY_W) {
			//z++;
		}
		if (key == GLFW_KEY_S) {
			//z--;
		}
		if (key == GLFW_KEY_E) {
			//y++;
		}
		if (key == GLFW_KEY_D) {
			//y--;
		}
	}
};

void Laborator1::OnKeyRelease(int key, int mods)
{
	// add key release event
	if (key == GLFW_KEY_F) {
		glClearColor(0, 0, 0, 1);
	}

	if (key == GLFW_KEY_P) {
		moveObject = 0;
	}
};

void Laborator1::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
	// add mouse move event
};

void Laborator1::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
	// add mouse button press event
};

void Laborator1::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
	// add mouse button release event
}

void Laborator1::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
	// treat mouse scroll event
}

void Laborator1::OnWindowResize(int width, int height)
{
	// treat window resize event
}
