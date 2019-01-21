
#include <vector>
#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <Core/Engine.h>

#include "Tema1.h"
#include "Transform2D.h"
#include "Object2D.h"

using namespace std;
int paletteX = 0;
int lives = 3;
bool gameStarted = false;
glm::ivec2 res;
int ballY = 5;
int ballX = -1;
int vx = 0;
int vy = 300;
int radians = 0;
int powerUpVY = -300;

int ballRadius = 10;
vector<vector<bool>> aliveBrick(5, vector<bool>(10, true));
vector<vector<float>> brickscaleX(5, vector<float>(10, 1));
vector<vector<float>> brickscaleY(5, vector<float>(10, 1));
vector<int> luckyNos;
double sc = 1;

int activePowerUp = 0;

class GameObject {
public:
	float x0, y0;
	float x1, y1;
	float x2, y2;
	float x3, y3;
	float width;
	float height;
	float centerX;
	float centerY;
	int i;
	int j;
	GameObject(float lcx, float lcy, float width, float height, int i, int j) {
		x0 = lcx;
		y0 = lcy;

		x1 = lcx;
		y1 = lcx + height;

		x2 = lcx + width;
		y2 = y1;

		x3 = x0 + width;
		y3 = y0;

		this->width = width;
		this->height = height;
	
		this->i = i;
		this->j = j;

		centerX = lcx + width / 2;
		centerY = lcy + height / 2;
	}
};
vector<GameObject> colidableObjects;

class PowerUp {
 public:
	float centerX;
	float centerY;
	float width;
	float height;
};
vector<PowerUp> powerups;

Tema1::Tema1()
{
}

Tema1::~Tema1()
{
}

void Tema1::Init()
{
	srand(time(NULL));
	for (int i = 0; i < 5; i++) {
		luckyNos.push_back(rand() % 10);
	}

	glm::ivec2 resolution = window->GetResolution();
	auto camera = GetSceneCamera();
	camera->SetOrthographic(0, (float)resolution.x, 0, (float)resolution.y, 0.01f, 400);
	camera->SetPosition(glm::vec3(0, 0, 50));
	camera->SetRotation(glm::vec3(0, 0, 0));
	camera->Update();
	GetCameraInput()->SetActive(false);

	glm::vec3 corner = glm::vec3(0, 0, 0);
	float squareSide = 100;

	glm::vec2 windowSize =  window->GetResolution();

	scaleX = 1;
	scaleY = 1;

	float verticalWallWidth = 10;
	float verticalWallHeight = windowSize.y * 0.9;
	Mesh* verticalWall = Object2D::CreateRectangle("verticalWall", corner, verticalWallWidth, verticalWallHeight, glm::vec3(1, 0,0), true);
	AddMeshToList(verticalWall);

	float horizontalWallWidth = windowSize.x;
	float horizontalWallHeight = 10;
	Mesh* horizontalWall = Object2D::CreateRectangle("horizontalWall", corner, horizontalWallWidth, horizontalWallHeight, glm::vec3(1,0,0), true);
	AddMeshToList(horizontalWall);

	float paletteWidth = windowSize.x * 0.2;
	float paletteHeight = 15;
	Mesh* palette = Object2D::CreateRectangle("palette", corner, paletteWidth, paletteHeight, glm::vec3(0,1,0), true);
	AddMeshToList(palette);

	Mesh* ball = Object2D::CreateCircle("ball", corner, 10, 1000, glm::vec3(1,1,1), true);
	AddMeshToList(ball);
	Mesh* powerBall = Object2D::CreateCircle("powerball", corner, 10, 1000, glm::vec3(0, 0, 1), true);
	AddMeshToList(powerBall);

	Mesh* life = Object2D::CreateCircle("life", corner, 10, 1000, glm::vec3(1, 1, 1), true);
	AddMeshToList(life);

	float brickWidth = windowSize.x * 0.05;
	float brickHeight = windowSize.y * 0.05;
	Mesh* brick = Object2D::CreateRectangle("brick", corner, brickWidth, brickHeight, glm::vec3(1, 0, 0), true);
	AddMeshToList(brick);

	Mesh* killedBrick = Object2D::CreateRectangle("killedBrick", corner, brickWidth, brickHeight, glm::vec3(1, 0, 0), true);
	AddMeshToList(killedBrick);

	Mesh* powerup = Object2D::CreateSquare("powerup", corner, brickWidth / 3, glm::vec3(0, 0, 1), true);
	AddMeshToList(powerup);

	// add left, right and top wall to list of collidable objects
	GameObject* leftWall = new GameObject(0, window->GetResolution().y * 0.1 - 10, 10, windowSize.y * 0.9, -1, -1);
	colidableObjects.push_back(*leftWall);
	GameObject* rightWall = new GameObject(window->GetResolution().x - 10, window->GetResolution().y * 0.1 - 10, 10, windowSize.y * 0.9, -1, -1);
	colidableObjects.push_back(*rightWall);
	GameObject* topWall = new GameObject(0, window->GetResolution().y - 10, windowSize.x, 10, -1, -1);
	colidableObjects.push_back(*topWall);
	// add all bricks to list of colidbale objects
	float brickXOffset = 0.08 * window->GetResolution().x;
	float brickYOffset = 0.08 * window->GetResolution().y;
	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 10; j++) {
			GameObject* temp = new GameObject(window->GetResolution().x * 0.23 / 2 + brickXOffset * j,
				window->GetResolution().y * 0.5 + brickYOffset * i,
				brickWidth,
				brickHeight,
				i,
				j);
			colidableObjects.push_back(*temp);
		}
	}
}

void Tema1::FrameStart()
{
	// clears the color buffer (using the previously set color) and depth buffer
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::ivec2 resolution = window->GetResolution();
	res = resolution;
	// sets the screen area where to draw
	glViewport(0, 0, resolution.x, resolution.y);
}

void Tema1::Update(float deltaTimeSeconds)
{
	// check for win
	bool win = true;
	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 10; j++) {
			if (aliveBrick[i][j]) {
				win = false;
			}
		}
	}
	if (win) {
		gameStarted = false;
		lives = 3;
		colidableObjects.erase(colidableObjects.begin() + 3, colidableObjects.end());
		float brickWidth = window->GetResolution().x * 0.05;
		float brickHeight = window->GetResolution().y * 0.05;
		float brickXOffset = 0.08 * window->GetResolution().x;
		float brickYOffset = 0.08 * window->GetResolution().y;
		for (int i = 0; i < 5; i++) {
			for (int j = 0; j < 10; j++) {
				aliveBrick[i][j] = true;
				GameObject* temp = new GameObject(window->GetResolution().x * 0.23 / 2 + brickXOffset * j,
					window->GetResolution().y * 0.5 + brickYOffset * i,
					brickWidth,
					brickHeight,
					i,
					j);
				colidableObjects.push_back(*temp);
				brickscaleX[i][j] = 1;
				brickscaleY[i][j] = 1;
			}
		}

		for (int i = 0; i < powerups.size(); i++) {
			powerups.erase(powerups.begin() + i);
		}
	}

	// always render walls
	modelMatrix = glm::mat3(1);
	modelMatrix *= Transform2D::Translate(0, window->GetResolution().y * 0.1 - 10);
	RenderMesh2D(meshes["verticalWall"], shaders["VertexColor"], modelMatrix);

	modelMatrix = glm::mat3(1);
	modelMatrix *= Transform2D::Translate(window->GetResolution().x - 10, window->GetResolution().y * 0.1 - 10);
	RenderMesh2D(meshes["verticalWall"], shaders["VertexColor"], modelMatrix);

	modelMatrix = glm::mat3(1);
	modelMatrix *= Transform2D::Translate(0, window->GetResolution().y - 10);
	RenderMesh2D(meshes["horizontalWall"], shaders["VertexColor"], modelMatrix);

	// always render palette
	modelMatrix = glm::mat3(1);
	modelMatrix *= Transform2D::Translate(paletteX, 5);
	RenderMesh2D(meshes["palette"], shaders["VertexColor"], modelMatrix);

	// render alive bricks
	float brickXOffset = 0.08 * window->GetResolution().x;
	float brickYOffset = 0.08 * window->GetResolution().y;
	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 10; j++) {
			if (aliveBrick[i][j]) {
				modelMatrix = glm::mat3(1);
				modelMatrix *= Transform2D::Translate(window->GetResolution().x * 0.23 / 2 + brickXOffset * j,
					window->GetResolution().y * 0.5 + brickYOffset * i);
				RenderMesh2D(meshes["brick"], shaders["VertexColor"], modelMatrix);
			}
			else {
				modelMatrix = glm::mat3(1);
				modelMatrix *= Transform2D::Translate(window->GetResolution().x * 0.23 / 2 + brickXOffset * j + window->GetResolution().x * 0.025,
					window->GetResolution().y * 0.5 + brickYOffset * i + window->GetResolution().y * 0.025);
				modelMatrix *= Transform2D::Scale(brickscaleX[i][j], brickscaleY[i][j]);
				modelMatrix *= Transform2D::Translate(0 - window->GetResolution().x * 0.025, 0 - window->GetResolution().y * 0.025);
				brickscaleX[i][j] -= deltaTimeSeconds * sc;
				brickscaleY[i][j] -= deltaTimeSeconds * sc;

				if (brickscaleX[i][j] > 0.75)
					RenderMesh2D(meshes["killedBrick"], shaders["VertexColor"], modelMatrix);
			}
		}
	}

	// always render lives
	for (int i = 0; i < lives; i++) {
		modelMatrix = glm::mat3(1);
		modelMatrix *= Transform2D::Translate(75 + i * 50, 60);
		RenderMesh2D(meshes["life"], shaders["VertexColor"], modelMatrix);
	}

	// render ball at position
	modelMatrix = glm::mat3(1);
	if (!gameStarted) {
		ballX = paletteX + 0.1 * window->GetResolution().x;
		modelMatrix *= Transform2D::Translate(ballX, 35);
		RenderMesh2D(meshes["ball"], shaders["VertexColor"], modelMatrix);
	}
	else {
		// check for colisions with all elements
		int objCounter = 0;
		for (auto colidableObject : colidableObjects) {
			float distanceX = glm::abs(colidableObject.centerX - ballX);
			float distanceY = glm::abs(colidableObject.centerY - ballY);

			float minDistanceX = ballRadius + colidableObject.width / 2;
			float minDistanceY = ballRadius + colidableObject.height / 2;

			if (distanceX <= minDistanceX && distanceY <= minDistanceY) {

				if (ballX <= colidableObject.centerX - colidableObject.width / 2) {  // ball came from left
					if (!activePowerUp || (activePowerUp && objCounter < 3)) {
						vx *= -1;
						ballX--;
					}
				} else
				if (ballX >= colidableObject.centerX + colidableObject.width / 2) {  // ball came from right
					if (!activePowerUp || (activePowerUp && objCounter < 3)) {
						vx *= -1;
						ballX++;
					}
				} else

				if (ballY <= colidableObject.centerY - colidableObject.height / 2) {  // ball came from below
					if (!activePowerUp || (activePowerUp && objCounter < 3)) {
						vy *= -1;
						ballY--;
					}
				} else 

				if (ballY >= colidableObject.centerY + colidableObject.height / 2) {  // ball came from above
					if (!activePowerUp || (activePowerUp && objCounter < 3)) {
						vy *= -1;
						ballY++;
					}
				}

				if (colidableObject.i != -1) {
					aliveBrick[colidableObject.i][colidableObject.j] = false;
					colidableObjects.erase(colidableObjects.begin() + objCounter);

					if (luckyNos[colidableObject.i] == colidableObject.j) {  // powerup was hit
						// add powerup to list of objects to be rendered
						PowerUp * newPowerUp = new PowerUp();
						newPowerUp->centerX = colidableObject.centerX;
						newPowerUp->centerY = colidableObject.centerY;
						newPowerUp->height = window->GetResolution().x * 0.05 / 3;
						newPowerUp->width = newPowerUp->height;
						powerups.push_back(*newPowerUp);
					}
				}
			}
			objCounter++;
		}

		// check for platform colisions 
		if (ballY <= 25 || ballX < 0 || ballX > window->GetResolution().x) {
			if (paletteX < ballX && ballX < paletteX + 0.2 * window->GetResolution().x) {
				vy = glm::abs(vy);
				if (activePowerUp) {
					activePowerUp--;
				}
				if (paletteX + 0.1 * window->GetResolution().x != ballX) {
					float mulFactor = 2 * (ballX - paletteX) / (0.2 * window->GetResolution().x) - 1;
					vx = vy * mulFactor;
				}
			}
			else {
				gameStarted = false;
				activePowerUp = 0;
				vx = 0;
				vy = 300;
				if (lives > 0) {
					lives--;
				}
				else {
					lives = 3;
					colidableObjects.erase(colidableObjects.begin() + 3, colidableObjects.end());
					float brickWidth = window->GetResolution().x * 0.05;
					float brickHeight = window->GetResolution().y * 0.05;
					for (int i = 0; i < 5; i++) {
						for (int j = 0; j < 10; j++) {
							aliveBrick[i][j] = true;
							GameObject* temp = new GameObject(window->GetResolution().x * 0.23 / 2 + brickXOffset * j,
								window->GetResolution().y * 0.5 + brickYOffset * i,
								brickWidth,
								brickHeight,
								i,
								j);
							colidableObjects.push_back(*temp);
							brickscaleX[i][j] = 1;
							brickscaleY[i][j] = 1;
						}
					}

					for (int i = 0; i < powerups.size(); i++) {
						powerups.erase(powerups.begin() + i);
					}
				}
			}
		}
		// render ball
		ballX += vx * deltaTimeSeconds;
		ballY += vy * deltaTimeSeconds;
		modelMatrix *= Transform2D::Translate(ballX, ballY);
	}
	if (activePowerUp) {
		RenderMesh2D(meshes["powerball"], shaders["VertexColor"], modelMatrix);
	}
	else {
		RenderMesh2D(meshes["ball"], shaders["VertexColor"], modelMatrix);
	}

	for (int i = 0; i < powerups.size(); i++) {
		modelMatrix = glm::mat3(1);
		if (powerups[i].centerY > 20) {
			powerups[i].centerY += deltaTimeSeconds * powerUpVY;
			radians += deltaTimeSeconds * 80;

			modelMatrix *= Transform2D::Translate(powerups[i].centerX + window->GetResolution().x * 0.05 / 3 / 2, powerups[i].centerY + window->GetResolution().x * 0.05 / 3 / 2);
			modelMatrix *= Transform2D::Rotate(radians);
			modelMatrix *= Transform2D::Translate(0 - window->GetResolution().x * 0.05 / 3 / 2, 0 - window->GetResolution().x * 0.05 / 3 / 2);

			RenderMesh2D(meshes["powerup"], shaders["VertexColor"], modelMatrix);
		}
		else {
			if (paletteX < ballX && ballX < paletteX + 0.2 * window->GetResolution().x) {
				// powerup fell on palette
				activePowerUp++;
				powerups.erase(powerups.begin() + i);
			}
		}
	}
}
void Tema1::FrameEnd()
{

}

void Tema1::OnInputUpdate(float deltaTime, int mods)
{

}

void Tema1::OnKeyPress(int key, int mods)
{

}

void Tema1::OnKeyRelease(int key, int mods)
{
	// add key release event
}

void Tema1::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
	paletteX = mouseX - window->GetResolution().x * 0.1; // take into account half palette width offset
}

void Tema1::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
	// add mouse button press event
	if (button == 1 && !gameStarted) {
		gameStarted = true;
	}
}

void Tema1::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
	// add mouse button release event
}

void Tema1::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}

void Tema1::OnWindowResize(int width, int height)
{
}