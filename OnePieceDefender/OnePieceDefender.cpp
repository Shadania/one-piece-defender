#pragma region generalDirectives
// precompiled header file
#include "stdafx.h"

// SDL libs
#pragma comment(lib, "sdl2.lib")
#pragma comment(lib, "SDL2main.lib")

// OpenGL libs
#pragma comment (lib,"opengl32.lib")
#pragma comment (lib,"Glu32.lib")

// SDL extension libs 
#pragma comment(lib, "SDL2_image.lib") // Library to load image files
#pragma comment(lib, "SDL2_ttf.lib") // Library to use fonts

// SDL and OpenGL Includes
#include <SDL.h>
#include <SDL_opengl.h>
#include <GL\GLU.h>

#include <SDL_image.h> // png loading
#include <SDL_ttf.h> // Font
#pragma endregion generalDirectives

#include <iostream>
#include <string>
#include <ctime>
#include <chrono>

#include "structs.h"
#include "utils.h"

#pragma region windowInformation
const float g_WindowWidth{ 1280.0f };
const float g_WindowHeight{ 720.0f };
const std::string g_WindowTitle{ "One Piece Defender - Druyts, Sarah - Djeebet, Redouan - Duarte Mendes, Diogo - 1DAE07" };
bool g_IsVSyncOn{ true };
#pragma endregion windowInformation

#pragma region textureDeclarations
struct Texture
{
	GLuint id;
	float width;
	float height;
};

bool TextureFromFile(const std::string& path, Texture & texture);
bool TextureFromString(const std::string & text, TTF_Font *pFont, const Color4f & textColor, Texture & texture);
bool TextureFromString(const std::string & text, const std::string& fontPath, int ptSize, const Color4f & textColor, Texture & texture);
void TextureFromSurface(const SDL_Surface *pSurface, Texture & textureData);
void DrawTexture(const Texture & texture, const Point2f& bottomLeftVertex, const Rectf & sourceRect = {});
void DrawTexture(const Texture & texture, const Rectf & destinationRect, const Rectf & sourceRect = {});
void DeleteTexture(Texture & texture);
#pragma endregion textureDeclarations

#pragma region coreDeclarations
// Functions
void Initialize();
void Run();
void Cleanup();
void QuitOnSDLError();
void QuitOnOpenGlError();
void QuitOnImageError();
void QuitOnTtfError();

// Variables
SDL_Window* g_pWindow{ nullptr }; // The window we'll be rendering to
SDL_GLContext g_pContext; // OpenGL context
Uint32 g_MilliSeconds{};
const Uint32 g_MaxElapsedTime{ 100 };
#pragma endregion coreDeclarations

#pragma region gameDeclarations
enum class State {
	idle, running, attack1, attack2, hurt
};

enum class RobotState {
	idle, running, attack
};

struct Stats {
	float health;
	int actionPoints;
	float superCharge;
};

struct Sprite {
	Texture texture;
	State state;
	int cols;
	float frameTime;
	int currentFrame;
	float accumulatedTime;
	bool isFacingLeft;
	float accumulatedHurtTime;
	float hurtMovement;
	int gridArrayIndex;
	Stats stats;
	bool isAlive;
	Point2f pos;
	bool turnActive;
	bool hasMoved;
};

// Functions
void Update(float elapsedSec);
void Draw();

void ClearBackground();
void InitGameResources();
void FreeGameResources();

void ProcessKeyDownEvent(const SDL_KeyboardEvent  & e);
void ProcessKeyUpEvent(const SDL_KeyboardEvent  & e);
void ProcessMouseMotionEvent(const SDL_MouseMotionEvent & e);
void ProcessMouseDownEvent(const SDL_MouseButtonEvent & e);
void ProcessMouseUpEvent(const SDL_MouseButtonEvent & e);

void InitLuffy();
void UpdateSprite(float elapsedSec, Sprite &sprite);
void DrawLuffy();

void InitRobots();
void DrawRobots();

void DrawBackground();
void DrawOverlay();

void InitRobotTextures();
void InitLuffyTextures();

void InitGrid();
void DrawSelection();
void CheckSelectionGrid();

void InitGameText();
void DrawGameText();

void DrawActionPoints(float left, float bottom, float height);

void ClickMenu();
void ClickDoublePunch(float elapsedSec = 0.0f);
void ClickSuperPunch(float elapsedSec = 0.0f);
void DrawMenu();

void DisplayInfo();

void InitMenuText();

void MoveLuffy(int destCell);
void MoveSprite(Sprite &sprite, int destCell, bool isItLuffy = false, float elapsedSec = 0.0f);

void HandleEnemyTurns();
void MoveEnemy(int robotIndex);
void AttackEnemy(int robotIndex, float elapsedSec = 0.0f);
bool IsLuffyInRange(int robotIndex);

int GetRandGridPos();

void DealDamageToEnemy(int gridIndex, int damage);

// ----Variables----

const int g_BackgroundRows = 9;
const int g_BackgroundCols = 20;

const float g_BoxHeight = g_WindowHeight / 11;
const float g_BoxWidth = g_WindowWidth / 20;

Texture g_Background{};

// sprites
Sprite g_Luffy{};
const int g_RobotsArrayLength{ 6 };
Sprite g_Robots[g_RobotsArrayLength]{};

// textures
const int g_LuffyTexturesArrayLength{ 10 };
Texture g_LuffyTextures[g_LuffyTexturesArrayLength]{};
const int g_RobotTexturesArrayLength{ 8 };
Texture g_RobotTextures[g_RobotTexturesArrayLength]{};

// selection grid
const int g_GridArrayLength{ 180 };
bool g_GridArray[g_GridArrayLength]{};
Point2f g_MousePos{};
int g_GridSelectedIdx{};

// bottom menu text
const int g_GameTextArrayLength{ 8 };
Texture g_GameText[g_GameTextArrayLength]{};

// movement
bool g_IsItMyTurn{ true };
bool g_IsLuffyMoving{ false };
float g_TotalMovementTime{ 0.0f };
float g_NeededMovementTime{ 1.0f };
int g_MovementDestCell{};
const int g_TotalActionPoints{ 10 };
int g_RobotMovementDestCell{};

// menu
bool g_IsMenuUp{ false };
const int g_MenuTextArrayLength{ 3 };
Texture g_MenuText[g_MenuTextArrayLength]{};
bool g_QuitFromMenu{ false };

#pragma endregion gameDeclarations


int main(int argc, char* args[])
{
	// seed the pseudo random number generator
	srand(unsigned int(time(nullptr)));

	std::cout << "Press <I> for information about the game.\n";

	// Initialize SDL and OpenGL
	Initialize();

	// Event loop
	Run();

	// Clean up SDL and OpenGL
	Cleanup();

	return 0;
}

#pragma region gameImplementations
void InitGameResources()
{
	TextureFromFile("Resources/background.png", g_Background); // background

	InitRobotTextures();
	InitLuffyTextures();
	InitGameText();
	InitMenuText();

	InitGrid();
	InitLuffy();
	InitRobots();
}
void FreeGameResources()
{
	DeleteTexture(g_Background);

	for (int i{}; i < g_LuffyTexturesArrayLength; i++)
	{
		DeleteTexture(g_LuffyTextures[i]);
	}
	for (int i{}; i < g_RobotTexturesArrayLength; i++)
	{
		DeleteTexture(g_RobotTextures[i]);
	}
	for (int i{}; i < g_GameTextArrayLength; i++)
	{
		DeleteTexture(g_GameText[i]);
	}
	for (int i{}; i < g_MenuTextArrayLength; i++)
	{
		DeleteTexture(g_MenuText[i]);
	}
}

void ProcessKeyDownEvent(const SDL_KeyboardEvent  & e)
{
	switch (e.keysym.sym)
	{
	case SDLK_h: // for testing purposes
		g_Luffy.state = State::hurt;
		break;
	case SDLK_i:
		DisplayInfo();
		break;
	case SDLK_ESCAPE:
		if (g_IsMenuUp) g_IsMenuUp = false;
		else g_IsMenuUp = true;
		break;
	case SDLK_l:
		g_IsItMyTurn = true;
		break;
	case SDLK_s:
		g_Luffy.stats.superCharge = 100;
		break;		
	}
}
void ProcessKeyUpEvent(const SDL_KeyboardEvent  & e)
{

}
void ProcessMouseMotionEvent(const SDL_MouseMotionEvent & e)
{
	g_MousePos.x = float(e.x);
	g_MousePos.y = g_WindowHeight - e.y;
}
void ProcessMouseDownEvent(const SDL_MouseButtonEvent & e)
{
	switch (e.button)
	{
	case SDL_BUTTON_LEFT:

		float border{ 5.0f }; // regular bottom interface menu clicking
		float height{ (g_BoxHeight * 2 - 6 * border) / 3 };
		float width{ (g_WindowWidth - 5 * border) / 2 };
		float scale{ 1.0f };

		Rectf destRect{ border * 2, border * 2, g_GameText[2].width * scale, g_GameText[2].height * scale }; // menu button
		if (utils::IsPointInRect(g_MousePos, destRect)) ClickMenu();

		destRect.bottom = height * 2 + border * 3; // double punch
		destRect.left = width + border * 3;
		destRect.width = width;
		if (utils::IsPointInRect(g_MousePos, destRect))
		{
			ClickDoublePunch();
			g_Luffy.currentFrame = 0;
		}

		destRect.bottom = destRect.bottom - border - height; // super punch
		if (utils::IsPointInRect(g_MousePos, destRect))
		{
			ClickSuperPunch();
			g_Luffy.currentFrame = 0;
		}


		if (g_IsItMyTurn && !g_IsMenuUp) // movement
		{
			for (int i{}; i < g_BackgroundRows; i++) // loops over every row
			{
				for (int j{}; j < g_BackgroundCols; j++) // loops over every column, every time it loops over a row
				{
					Rectf rect{ 0.0f,0.0f, g_BoxWidth, g_BoxHeight }; // gets the cell position
					rect.left = j * g_BoxWidth;
					rect.bottom = g_WindowHeight - ((i + 1) * g_BoxHeight);

					if (utils::IsPointInRect(g_MousePos, rect))
					{
						g_MovementDestCell = g_GridSelectedIdx;
						std::cout << g_MovementDestCell << std::endl;
						MoveLuffy(g_MovementDestCell);
						break;
					}
				}
			}
		}

		if (g_IsMenuUp) // fancy menu clicking
		{
			float width{ 150.0f };
			float height{ 100.0f };
			destRect = { g_WindowWidth / 2 - width, g_WindowHeight / 2 - height, width * 2, height * 2 };
			width = 200.0f;
			height = 50.0f;
			float vertBorder{ 20 / 3.0f };
			float horBorder{ 100 / 2.0f };
			destRect = { destRect.left + horBorder, destRect.bottom + vertBorder, width, height };
			if (utils::IsPointInRect(g_MousePos, destRect)) g_IsMenuUp = false;

			destRect.bottom = destRect.bottom + height + vertBorder;
			if (utils::IsPointInRect(g_MousePos, destRect)) DisplayInfo();

			destRect.bottom = destRect.bottom + height + vertBorder;
			if (utils::IsPointInRect(g_MousePos, destRect)) g_QuitFromMenu = true;
		}
		break;
	}
}
void ProcessMouseUpEvent(const SDL_MouseButtonEvent & e)
{

}

void Update(float elapsedSec)
{
	UpdateSprite(elapsedSec, g_Luffy);
	for (int i{}; i < g_RobotsArrayLength; i++)
	{
		UpdateSprite(elapsedSec, g_Robots[i]);
	}

	CheckSelectionGrid();
	
	if (g_Luffy.state == State::attack1) ClickDoublePunch(elapsedSec);
	if (g_Luffy.state == State::attack2) ClickSuperPunch(elapsedSec);

	for (int i{}; i < g_RobotsArrayLength; i++)
	{
		if (g_Robots[i].state == State::running) MoveSprite(g_Robots[i], g_RobotMovementDestCell, false, elapsedSec);
	}

	for (int i{}; i < g_RobotsArrayLength; i++)
	{
		if (g_Robots[i].state == State::attack1) AttackEnemy(i, elapsedSec);
	}
	
	if (!g_IsItMyTurn && g_TotalMovementTime <= 0.00001f) HandleEnemyTurns();
}
void Draw()
{
	ClearBackground();
	DrawBackground();
	DrawLuffy();
	DrawSelection();
	DrawRobots();


	DrawOverlay();
	DrawGameText();
	if (g_IsMenuUp) DrawMenu();
}
void ClearBackground()
{
	glClearColor(185.0f / 255.0f, 211.0f / 255.0f, 238.0f / 255.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}

void InitLuffy()
{
	g_Luffy.frameTime = 1 / 10.0f;
	g_Luffy.state = State::idle;
	g_Luffy.cols = 7;
	g_Luffy.currentFrame = 0;
	g_Luffy.isFacingLeft = false;
	g_Luffy.accumulatedHurtTime = 0.0f;
	g_Luffy.hurtMovement = 0.0f;
	g_Luffy.gridArrayIndex = 89;
	g_Luffy.stats.health = 100;
	g_Luffy.stats.actionPoints = 10;
	g_Luffy.stats.superCharge = 100;

	g_GridArray[g_Luffy.gridArrayIndex] = true;
}
void UpdateSprite(float elapsedSec, Sprite &sprite)
{
	// sprite change
	sprite.accumulatedTime += elapsedSec;
	if (sprite.accumulatedTime >= sprite.frameTime)
	{
		sprite.accumulatedTime -= sprite.frameTime;
		sprite.currentFrame += 1;
		sprite.currentFrame = sprite.currentFrame % sprite.cols;
	}

	// handle state to animation and time
	switch (sprite.state)
	{
	case State::running:
		MoveSprite(g_Luffy, g_MovementDestCell, true, elapsedSec);
		break;
	case State::hurt:
		float hurtTimeMax{ 0.3f };
		sprite.accumulatedHurtTime += elapsedSec;

		float direction{ -1.0f };
		if (sprite.isFacingLeft) direction = 1.0f;

		float maxHurtMovement{ 5.0f };
		sprite.hurtMovement = (maxHurtMovement * sin(sprite.accumulatedHurtTime / hurtTimeMax  * float(M_PI))) * direction;

		if (sprite.accumulatedHurtTime >= hurtTimeMax)
		{
			sprite.accumulatedHurtTime = 0.0f;
			sprite.state = State::idle;
			sprite.hurtMovement = 0.0f;
		}
		break;
	}
}
void DrawLuffy()
{
	Rectf sourceRect{}, destRect{};
	int texIdx{ 0 };

	// pick texture based on state
	switch (g_Luffy.state)
	{
	case State::idle:
		g_Luffy.cols = 7;
		if (g_Luffy.isFacingLeft) texIdx = 0;
		else texIdx = 1;
		break;

	case State::running:
		g_Luffy.cols = 6;
		if (g_Luffy.isFacingLeft) texIdx = 2;
		else texIdx = 3;
		break;

	case State::attack1:
		g_Luffy.cols = 7;
		if (g_Luffy.isFacingLeft) texIdx = 4;
		else texIdx = 5;
		break;

	case State::attack2:
		g_Luffy.cols = 11;
		if (g_Luffy.isFacingLeft) texIdx = 6;
		else texIdx = 7;
		break;

	case State::hurt:
		g_Luffy.cols = 7;
		if (g_Luffy.isFacingLeft) texIdx = 8;
		else texIdx = 9;
		break;
	}

	sourceRect.bottom = g_LuffyTextures[texIdx].height; // get sourceRect
	sourceRect.height = g_LuffyTextures[texIdx].height;
	sourceRect.width = g_LuffyTextures[texIdx].width / g_Luffy.cols;
	sourceRect.left = g_Luffy.currentFrame * sourceRect.width;

	int row{ g_Luffy.gridArrayIndex / g_BackgroundCols };
	int col{ g_Luffy.gridArrayIndex % g_BackgroundCols };
	Point2f pos{ col * g_BoxWidth, g_WindowHeight - g_BoxHeight * (row + 1) };

	destRect.height = g_Background.height / g_BackgroundRows; // get destRect for drawing
	destRect.width = (sourceRect.width * destRect.height / sourceRect.height);
	destRect.left = pos.x + g_Luffy.hurtMovement + g_Luffy.pos.x; // pos from box, pos from hurt, pos from smooth movement
	destRect.bottom = pos.y +  + g_Luffy.pos.y;

	DrawTexture(g_LuffyTextures[texIdx], destRect, sourceRect);
}

void DrawBackground()
{
	float bottom{ g_WindowHeight / 11 * 2 };
	DrawTexture(g_Background, Rectf{ 0.0f, bottom, g_WindowWidth, g_WindowHeight }); //background grid: 11 rows, 20 cols
}
void DrawOverlay()
{
	float top{ g_BoxHeight * 2 };
	utils::FillRectangle(Rectf{ 0.0f, 0.0f, g_WindowWidth, top }, Color4f{ .7f, .4f,.1f,1.0f });
	utils::FillRectangle(Rectf{ 5.0f, 5.0f, g_WindowWidth - 10.0f, top - 10.0f }, Color4f{ .8f, .5f,.2f,1.0f });
}

void InitRobotTextures()
{
	TextureFromFile("Resources/Robot1/idleLeft.png", g_RobotTextures[0]);
	TextureFromFile("Resources/Robot1/idleRight.png", g_RobotTextures[1]);
	TextureFromFile("Resources/Robot1/walkLeft.png", g_RobotTextures[2]);
	TextureFromFile("Resources/Robot1/walkRight.png", g_RobotTextures[3]);
	TextureFromFile("Resources/Robot1/attackLeft.png", g_RobotTextures[4]);
	TextureFromFile("Resources/Robot1/attackRight.png", g_RobotTextures[5]);
	TextureFromFile("Resources/Robot1/idleLeftHurt.png", g_RobotTextures[6]);
	TextureFromFile("Resources/Robot1/idleRightHurt.png", g_RobotTextures[7]);
}
void InitLuffyTextures()
{
	TextureFromFile("Resources/Luffy/idleLeft.png", g_LuffyTextures[0]);
	TextureFromFile("Resources/Luffy/idleRight.png", g_LuffyTextures[1]);
	TextureFromFile("Resources/Luffy/runLeft.png", g_LuffyTextures[2]);
	TextureFromFile("Resources/Luffy/runRight.png", g_LuffyTextures[3]);
	TextureFromFile("Resources/Luffy/doublePunchLeft.png", g_LuffyTextures[4]);
	TextureFromFile("Resources/Luffy/doublePunchRight.png", g_LuffyTextures[5]);
	TextureFromFile("Resources/Luffy/superPunchLeft.png", g_LuffyTextures[6]);
	TextureFromFile("Resources/Luffy/superPunchRight.png", g_LuffyTextures[7]);
	TextureFromFile("Resources/Luffy/idleLeftHurt.png", g_LuffyTextures[8]);
	TextureFromFile("Resources/Luffy/idleRightHurt.png", g_LuffyTextures[9]);
}

void InitRobots()
{
	for (int i{}; i < g_RobotsArrayLength; i++)
	{
		g_Robots[i].state = State::idle;
		g_Robots[i].cols = 4;
		g_Robots[i].frameTime = 0.7f;
		g_Robots[i].currentFrame = 0;
		g_Robots[i].isFacingLeft = true;
		g_Robots[i].stats.health = 100;
		g_Robots[i].stats.actionPoints = 2;
		g_Robots[i].turnActive = true;
		g_Robots[i].gridArrayIndex = GetRandGridPos();
		g_Robots[i].hasMoved = false;
	    
		while (g_GridArray[g_Robots[i].gridArrayIndex])
		{
			g_Robots[i].gridArrayIndex = GetRandGridPos();
		}
		g_GridArray[g_Robots[i].gridArrayIndex] = true;
		std::cout << g_Robots[i].gridArrayIndex << '\n';
	}
}
void DrawRobots()
{
	for (int i{}; i < g_RobotsArrayLength; i++)
	{
		Rectf sourceRect{}, destRect{};
		int texIdx{ 0 };

		switch (g_Robots[i].state)
		{
		case State::idle:
			g_Robots[i].cols = 4;
			if (g_Robots[i].isFacingLeft) texIdx = 0;
			else texIdx = 1;
			break;

		case State::running:
			g_Robots[i].cols = 8;
			if (g_Robots[i].isFacingLeft) texIdx = 2;
			else texIdx = 3;
			break;

		case State::attack1:
			g_Robots[i].cols = 7;
			if (g_Robots[i].isFacingLeft) texIdx = 4;
			else texIdx = 5;
			break;

		case State::hurt:
			if (g_Robots[i].isFacingLeft) texIdx = 8;
			else texIdx = 9;
			break;
		}

		sourceRect.bottom = g_RobotTextures[texIdx].height;
		sourceRect.height = g_RobotTextures[texIdx].height;
		sourceRect.width = g_RobotTextures[texIdx].width / g_Robots[i].cols;
		sourceRect.left = g_Robots[i].currentFrame * sourceRect.width;

		int row{ g_Robots[i].gridArrayIndex / g_BackgroundCols };
		int col{ g_Robots[i].gridArrayIndex % g_BackgroundCols };
		Point2f pos{ col * g_BoxWidth, g_WindowHeight - (row + 1) * g_BoxHeight };

		destRect.height = g_Background.height / g_BackgroundRows;
		destRect.width = (sourceRect.width * destRect.height / sourceRect.height);
		destRect.left = pos.x + g_Robots[i].hurtMovement + g_Robots[i].pos.x;
		destRect.bottom = pos.y + g_Robots[i].pos.y;

		DrawTexture(g_RobotTextures[texIdx], destRect, sourceRect);
	}
}

void InitGrid()
{
	int indexArray[]{ 92, 93, 94, 95, 112, 113, 114, 115, 132, 133, 134, 135, 47, 48, 27, 28, 67, 98, 175, 143, 144, 146, 147, 31, 32, 33, 34, 35, 37, 38 };

	for (int i{}; i < 30; i++)
	{
		g_GridArray[indexArray[i]] = true;
	}
}
void DrawSelection()
{
	int row{ g_GridSelectedIdx / g_BackgroundCols };
	int col{ g_GridSelectedIdx % g_BackgroundCols };

	Rectf destRect{ col * g_BoxWidth, g_WindowHeight - ((row + 1) * g_BoxHeight), g_BoxWidth, g_BoxHeight };

	if (g_GridArray[g_GridSelectedIdx])
	{
		glLineWidth(7);
		glBegin(GL_LINES);
		glColor3f(1.0f, .0f, .0f);
		glVertex2f(destRect.left, destRect.bottom);
		glVertex2f(destRect.left + destRect.width, destRect.bottom + destRect.height);

		glVertex2f(destRect.left + destRect.width, destRect.bottom);
		glVertex2f(destRect.left, destRect.bottom + destRect.height);
		glEnd();
	}

	utils::DrawRectangle(destRect, Color4f{ 1.0f,1.0f,1.0f,1.0f }, 5);
}
void CheckSelectionGrid()
{
	for (int i{}; i < g_BackgroundRows; i++) // loops over every row
	{
		for (int j{}; j < g_BackgroundCols; j++) // loops over every column, every time it loops over a row
		{
			Rectf rect{ 0.0f,0.0f, g_BoxWidth, g_BoxHeight };
			rect.left = j * g_BoxWidth;
			rect.bottom = g_WindowHeight - ((i + 1) * g_BoxHeight);

			if (utils::IsPointInRect(g_MousePos, rect))
			{
				g_GridSelectedIdx = utils::GetIndex(i, j, g_BackgroundCols);
				break;
			}
		}
	}
}

void InitGameText()
{
	TextureFromString("HP: ", "Resources/VCR_OSD_MONO_1.001.ttf", 40, Color4f{ .6f, .3f,.1f,1.0f }, g_GameText[0]);
	TextureFromString("AP: ", "Resources/VCR_OSD_MONO_1.001.ttf", 40, Color4f{ .6f, .3f,.1f,1.0f }, g_GameText[1]);
	TextureFromString("MENU", "Resources/VCR_OSD_MONO_1.001.ttf", 40, Color4f{ .6f, .3f,.1f,1.0f }, g_GameText[2]);
	TextureFromString("DOUBLE PUNCH", "Resources/VCR_OSD_MONO_1.001.ttf", 40, Color4f{ .6f, .3f,.1f,1.0f }, g_GameText[3]);
	TextureFromString("SUPER PUNCH", "Resources/VCR_OSD_MONO_1.001.ttf", 40, Color4f{ .6f, .3f,.1f,1.0f }, g_GameText[4]);
	TextureFromString("SUPER PUNCH CHARGE", "Resources/VCR_OSD_MONO_1.001.ttf", 40, Color4f{ .1f,.8f,1.0f,1.0f }, g_GameText[5]);
	TextureFromString("Your Turn", "Resources/VCR_OSD_MONO_1.001.ttf", 40, Color4f{ .6f, .3f,.1f,1.0f }, g_GameText[6]);
	TextureFromString("Enemy Turn", "Resources/VCR_OSD_MONO_1.001.ttf", 40, Color4f{ .6f, .3f,.1f,1.0f }, g_GameText[7]);
}
void DrawGameText()
{
	float border{ 5.0f };
	float height{ (g_BoxHeight * 2 - 6 * border) / 3 };
	float width{ (g_WindowWidth - 5 * border) / 2 };

	float scale{ 1.0f };

	// HP bar
	Rectf destRect{ border * 2, 3 * border + 2 * height, g_GameText[0].width * scale, g_GameText[0].height * scale };
	DrawTexture(g_GameText[0], destRect);

	destRect.left += destRect.width; // back black bar
	destRect.width = width - destRect.width;
	utils::FillRectangle(destRect, Color4f{ .0f,.0f,.0f,.8f });

	destRect.width = destRect.width * g_Luffy.stats.health / 100;
	utils::FillRectangle(destRect, Color4f{ 1.0f,.0f,.0f,1.0f });
	utils::DrawRectangle(destRect, Color4f{ .0f,.0f,.0f,1.0f }, 3);

	// AP dots
	destRect.bottom = destRect.bottom - border - height;
	destRect.left = border * 2;
	destRect.width = g_GameText[1].width;
	DrawTexture(g_GameText[1], destRect);
	DrawActionPoints(destRect.left + destRect.width, destRect.bottom + destRect.height / 2, destRect.height);

	// Menu button
	destRect.bottom = 2 * border;
	destRect.left = 2 * border;
	destRect.width = g_GameText[2].width;
	Color4f color = { .4f, .2f,.1f,1.0f };
	if (utils::IsPointInRect(g_MousePos, destRect)) color = { .3f, .15f,.1f,1.0f };
	utils::FillRectangle(destRect, color);
	DrawTexture(g_GameText[2], destRect);

	// Double Punch Button
	destRect.bottom = height * 2 + border * 3;
	destRect.left = width + border * 3;
	destRect.width = width;
	color = { .4f, .2f,.1f,1.0f };
	if (utils::IsPointInRect(g_MousePos, destRect)) color = { .3f, .15f,.1f,1.0f };
	utils::FillRectangle(destRect, color);
	DrawTexture(g_GameText[3], destRect);

	// Super Punch Button
	destRect.bottom = destRect.bottom - border - height;
	color = { .4f, .2f,.1f,1.0f };
	if (utils::IsPointInRect(g_MousePos, destRect)) color = { .3f, .15f,.1f,1.0f };
	utils::FillRectangle(destRect, color);
	DrawTexture(g_GameText[4], destRect);

	destRect.bottom = destRect.bottom - border - height;
	utils::FillRectangle(destRect, Color4f{ .1f,.5f,.8f,1.0f });
	DrawTexture(g_GameText[5], destRect);
	destRect.left += g_Luffy.stats.superCharge / 100 * width;
	destRect.width -= g_Luffy.stats.superCharge / 100 * width;
	utils::FillRectangle(destRect, Color4f{ .0f,.0f,.0f,.7f });

	// Enemy Turn
	destRect.left = border;
	destRect.bottom = g_WindowHeight - border - height;
	destRect.height = height;
	destRect.width = g_GameText[7].width;
	utils::FillRectangle(destRect, Color4f{ .3f, .15f,.1f,1.0f });
	DrawTexture(g_GameText[7], destRect);
	if (g_IsItMyTurn) // Your Turn
	{
		utils::FillRectangle(destRect, Color4f{ .4f, .2f,.1f,1.0f });
		DrawTexture(g_GameText[6], destRect);
	}
}
void DrawActionPoints(float left, float bottom, float height)
{
	for (int i{}; i < g_TotalActionPoints; i++)
	{
		Point2f center{ left + i * (height + 5.0f), bottom };
		utils::FillEllipse(center, height / 2 - 2.5f, height / 2 - 2.5f, Color4f{ .0f,.0f,.0f,.8f });
	}

	for (int i{}; i < g_Luffy.stats.actionPoints; i++)
	{
		Point2f center{ left + i * (height + 5.0f), bottom };
		utils::FillEllipse(center, height / 2 - 5.0f, height / 2 - 5.0f, Color4f{ .3f,.3f,.8f,1.f });
	}
}

void ClickMenu()
{
	std::cout << "Clicked on Menu button\n";
	g_IsMenuUp = true;
}
void ClickDoublePunch(float elapsedSec)
{	
	float doublePunchTime{ (g_Luffy.cols) * g_Luffy.frameTime };
	g_Luffy.state = State::attack1;
	g_TotalMovementTime += elapsedSec;

	if (g_TotalMovementTime >= doublePunchTime)
	{
		g_Luffy.state = State::idle;
		g_TotalMovementTime = 0.0f;
		g_IsItMyTurn = false;
	}
	
}
void ClickSuperPunch(float elapsedSec)
{
	// std::cout << "Clicked on Super Punch button\n";
	if (g_Luffy.stats.superCharge >= 100)
	{
		g_IsItMyTurn = false;
		float superPunchTime{ g_Luffy.cols * g_Luffy.frameTime };
		g_Luffy.state = State::attack2;

		g_TotalMovementTime += elapsedSec;
		if (g_TotalMovementTime >= superPunchTime)
		{
			g_Luffy.state = State::idle;
			g_TotalMovementTime = 0.0f;
			g_Luffy.stats.superCharge = 0;
		}
	}
	else std::cout << "You don't have enough charge for that!\n";
}
void DrawMenu()
{
	utils::FillRectangle(Rectf{ 0.0f,0.0f,g_WindowWidth, g_WindowHeight }, Color4f{ .0f,.0f,.0f,.4f });
	float width{ 150.0f };
	float height{ 100.0f };

	Rectf destRect{ g_WindowWidth / 2 - width, g_WindowHeight / 2 - height, width * 2, height * 2 };
	utils::FillRectangle(destRect, Color4f{ .7f, .4f,.1f,1.0f });

	destRect = { destRect.left + 5.0f, destRect.bottom + 5.0f, destRect.width - 10.0f, destRect.height - 10.0f };
	utils::FillRectangle(destRect, Color4f{ .8f, .5f,.2f,1.0f });

	width = 200.0f;
	height = 50.0f;
	float vertBorder{ 20 / 3.0f };
	float horBorder{ 100 / 2.0f };
	Color4f color{};

	destRect = { destRect.left - 5.0f + horBorder, destRect.bottom - 5.0f + vertBorder, width, height };
	color = { .4f, .2f,.1f,1.0f };
	if (utils::IsPointInRect(g_MousePos, destRect)) color = { .3f, .15f,.1f,1.0f };
	utils::FillRectangle(destRect, color);
	DrawTexture(g_MenuText[0], destRect);

	destRect.bottom = destRect.bottom + height + vertBorder;
	color = { .4f, .2f,.1f,1.0f };
	if (utils::IsPointInRect(g_MousePos, destRect)) color = { .3f, .15f,.1f,1.0f };
	utils::FillRectangle(destRect, color);
	DrawTexture(g_MenuText[1], destRect);

	destRect.bottom = destRect.bottom + height + vertBorder;
	color = { .4f, .2f,.1f,1.0f };
	if (utils::IsPointInRect(g_MousePos, destRect)) color = { .3f, .15f,.1f,1.0f };
	utils::FillRectangle(destRect, color);
	DrawTexture(g_MenuText[2], destRect);
}
void InitMenuText()
{
	TextureFromString("Exit Menu", "Resources/VCR_OSD_MONO_1.001.ttf", 40, Color4f{ .6f, .3f,.1f,1.0f }, g_MenuText[0]);
	TextureFromString("View Info", "Resources/VCR_OSD_MONO_1.001.ttf", 40, Color4f{ .6f, .3f,.1f,1.0f }, g_MenuText[1]);
	TextureFromString("Close Game", "Resources/VCR_OSD_MONO_1.001.ttf", 40, Color4f{ .6f, .3f,.1f,1.0f }, g_MenuText[2]);
}
void DisplayInfo()
{
	std::cout << "Punch all the robots to death!\nClick on the ground to move around, you are limited to 5 moves up/down and 5 moves left/right per turn.\n";
	std::cout << "Dealing and receiving damage will charge your Super Punch. Use it to deal massive damage.\n";
	std::cout << "You get ten Action Points per turn. Walking costs 1 AP per block, double-punch costs 2 AP, and the super punch costs 5 AP.\n";
}

void MoveLuffy(int destCell) // gets called once, from a mouseclick
{
	int rowSelect{ destCell / g_BackgroundCols }; // get cols and rows and difference between
	int colSelect{ destCell % g_BackgroundCols };
	int rowLuffy{ g_Luffy.gridArrayIndex / g_BackgroundCols };
	int colLuffy{ g_Luffy.gridArrayIndex % g_BackgroundCols };
	int rowDifference{ rowLuffy - rowSelect };
	int colDifference{ colLuffy - colSelect };

	if ( abs(rowDifference) + abs(colDifference) == 1 && !g_GridArray[g_MovementDestCell]) // You can move
	{
		MoveSprite(g_Luffy, g_MovementDestCell, true);
	}
	else if (!g_IsMenuUp)
	{
		std::cout << "You can't go there!\n";
	}
}
void MoveSprite(Sprite &sprite, int destCell, bool isItLuffy, float elapsedSec) // make sure to input differences of 1 else they gon teleport
{
	int colSelect{ destCell % g_BackgroundCols }; // getting cols
	int colOriginal{ sprite.gridArrayIndex % g_BackgroundCols };
	int rowSelect{ destCell / g_BackgroundCols }; // getting rows
	int rowOriginal{ sprite.gridArrayIndex / g_BackgroundCols };

	if (colSelect > colOriginal) sprite.isFacingLeft = false; // putting characters facing in the right direction
	else if (colSelect < colOriginal) sprite.isFacingLeft = true;

	sprite.state = State::running; // as long as the sprite is "running" this function will be called from the Update function

	int direction{ 1 };
	g_TotalMovementTime += elapsedSec; // for smooth movement progress
	float timeToCrossACell{ 1.0f };
	if (isItLuffy) timeToCrossACell = .3f; // cause luffy gotta go fast

	if (g_TotalMovementTime < timeToCrossACell) // has not yet reached destination
	{
		if (colSelect == colOriginal) // if the difference is in the rows
		{
			if (rowSelect > rowOriginal) direction = -1;
			sprite.pos.y += direction * elapsedSec / timeToCrossACell * g_BoxHeight;
		}
		else // if it is not -> its in the cols
		{
			if (colSelect < colOriginal) direction = -1;
			sprite.pos.x += direction * elapsedSec / timeToCrossACell * g_BoxWidth;
		}
	}

	else // destination reached
	{
		std::cout << "Moved!\n";
		g_TotalMovementTime = 0.0f; // resetting this var cause its used for literally anything that needs to be smooth instead of instant
		sprite.pos.x = .0f; // resetting sprite pos
		sprite.pos.y = .0f;
		sprite.state = State::idle; // resetting state so this function wont be called anymore from Update() and texture resets from UpdateSprite(sprite)
		g_GridArray[sprite.gridArrayIndex] = false; // old cell gets freed
		g_GridArray[destCell] = true; // new cell gets occupied
		sprite.gridArrayIndex = destCell; //setting cell idx to newest cell
		sprite.stats.actionPoints -= 1;
		if (sprite.stats.actionPoints == 0 && isItLuffy) g_IsItMyTurn = false;
		if (!isItLuffy)
		{
			sprite.turnActive = false;
			sprite.hasMoved = true;
		}
	}
}

void HandleEnemyTurns()
{
	// need to figure out if, and if so how, this can be stuffed in a for-loop
	
	if (g_Robots[0].turnActive)
	{
		if (!IsLuffyInRange(0)) //if luffy isnt in range, move
		{
			MoveEnemy(0); // has a turn = false at the end
		}
		else if (g_TotalMovementTime <= 0.00001f) // if he is and no other animations are going on atm, attack
		{
			std::cout << "Robot 0 wants to attack\n";
			g_Robots[0].currentFrame = 0;
			AttackEnemy(0); // at the end of this function there's a turn = false too
		}
		else g_Robots[0].turnActive = false;
	}
	else if (g_Robots[1].turnActive)
	{
		if (!IsLuffyInRange(1))
		{
			MoveEnemy(1);
		}
		else if (g_TotalMovementTime <= 0.000001f)
		{
			std::cout << "Robot 1 wants to attack\n";
			g_Robots[1].currentFrame = 0;
			AttackEnemy(1);
		}
		else g_Robots[1].turnActive = false;
	}
	else if (g_Robots[2].turnActive)
	{
		if (abs(!IsLuffyInRange(2)))
		{
			MoveEnemy(2);
		}
		else if (g_TotalMovementTime <= 0.000001f)
		{
			std::cout << "Robot 2 wants to attack\n";
			g_Robots[2].currentFrame = 0;
			AttackEnemy(2);
		}
		else g_Robots[2].turnActive = false;
	}
	else if (g_Robots[3].turnActive)
	{
		if (!IsLuffyInRange(3))
		{
			MoveEnemy(3);
		}
		else if (g_TotalMovementTime <= 0.000001f)
		{
			std::cout << "Robot 3 wants to attack\n";
			g_Robots[3].currentFrame = 0;
			AttackEnemy(3);
		}
		else g_Robots[3].turnActive = false;
	}
	else if (g_Robots[4].turnActive)
	{
		if (!IsLuffyInRange(4))
		{
			MoveEnemy(4);
		}
		else if (g_TotalMovementTime <= 0.000001f)
		{
			std::cout << "Robot 4 wants to attack\n";
			g_Robots[4].currentFrame = 0;
			AttackEnemy(4);
		}
		else g_Robots[4].turnActive = false;
	}
	else if (g_Robots[5].turnActive)
	{
		if (!IsLuffyInRange(5))
		{
			MoveEnemy(5);
		}
		else if (g_TotalMovementTime <= 0.000001f)
		{
			std::cout << "Robot 5 wants to attack\n";
			g_Robots[5].currentFrame = 0;
			AttackEnemy(5);
		}
		else g_Robots[5].turnActive = false;
	}
	else // if none of the robots has active turns its logically the players turn again
	{
		g_IsItMyTurn = true;
		g_Luffy.stats.actionPoints = 10;
		for (int i{}; i < g_RobotsArrayLength; i++)
		{
			g_Robots[i].turnActive = true; // preparing for next turn
		}
	}
}
void MoveEnemy(int robotIndex)
{
	std::cout << "Moving robot " << robotIndex << " from pos " << g_Robots[robotIndex].gridArrayIndex;
	int luffyCol{ g_Luffy.gridArrayIndex % g_BackgroundCols };
	int luffyRow{ g_Luffy.gridArrayIndex / g_BackgroundCols };
	int robotCol{ g_Robots[robotIndex].gridArrayIndex % g_BackgroundCols };
	int robotRow{ g_Robots[robotIndex].gridArrayIndex / g_BackgroundCols };

	int rowDifference{ luffyRow - robotRow }; // if positive, luffy's row is greater than robot's row -> luffy is below the robot
	int colDifference{ luffyCol - robotCol }; // if positive, luffy's col is greater than robot's col -> luffy is to the right of the robot

	int rowGoal{ g_Robots[robotIndex].gridArrayIndex };
	int colGoal{ g_Robots[robotIndex].gridArrayIndex };

	int backupRowGoal{ g_Robots[robotIndex].gridArrayIndex };
	int backupColGoal{ g_Robots[robotIndex].gridArrayIndex };

	bool canRobotMoveVertical{ true };
	bool canRobotMoveHorizontal{ true };

	if (rowDifference > 0 && !g_GridArray[rowGoal + g_BackgroundCols]) // space below bot has to be free
	{
		rowGoal += g_BackgroundCols; // luffy is below the bot -> bot goes down
	}
	else if (rowDifference < 0 && !g_GridArray[rowGoal - g_BackgroundCols]) // space above bot has to be free
	{
		rowGoal -= g_BackgroundCols; // luffy is above the bot -> bot goes up
	}
	else // space below and above bot are occupied thus bot cannot move vertically
	{
		canRobotMoveVertical = false;
		if (rowDifference > 0) backupRowGoal -= g_BackgroundCols; //preparing backup for switch case direction == 3 a bit below
		else backupRowGoal += g_BackgroundCols;
	}

	if (colDifference > 0 && !g_GridArray[colGoal + 1]) 
	{
		colGoal += 1; // luffy is to the right of the bot -> bot goes right
	}
	else if (colDifference < 0 && !g_GridArray[colGoal - 1])
	{
		colGoal -= 1; // luffy is to the left of the bot -> bot goes to the left
	}
	else // space to the left and to the right are occupied so bot cannot move horizontally
	{
		canRobotMoveHorizontal = false;
		if (colDifference > 0) backupColGoal -= 1;
		else backupColGoal += 1;
	}
	
	int direction{ rand() % 2 };

	if (direction == 0) { // try to go horizontal
		if (!canRobotMoveHorizontal) direction = 1;
		else {
			g_RobotMovementDestCell = colGoal;
			MoveSprite(g_Robots[robotIndex], colGoal);
			std::cout << " to pos " << colGoal << '\n';
		}
	}
	if (direction == 1) { // try to go vertical (if horizontal didnt work)
		if (!canRobotMoveVertical) direction = 2;
		else {
			g_RobotMovementDestCell = rowGoal;
			MoveSprite(g_Robots[robotIndex], rowGoal);
			std::cout << " to pos " << rowGoal << '\n';
		}
	}
	if (direction == 2) { // we tried to go vertical but it didnt work so lets go horizontal
		if (!canRobotMoveHorizontal) direction = 3;
		else {
			g_RobotMovementDestCell = colGoal;
			MoveSprite(g_Robots[robotIndex], colGoal);
			std::cout << " to pos " << colGoal << '\n';
		}
	}
	if (direction == 3) { // we tried to go vertical, didnt work, then we tried horizontal, which also didnt work. fuck it lets try to avoid this obstacle
		if (!g_GridArray[backupRowGoal] && backupRowGoal > 0 && backupRowGoal < 180) {
			g_RobotMovementDestCell = backupRowGoal;
			MoveSprite(g_Robots[robotIndex], backupRowGoal);
			std::cout << " to pos " << backupRowGoal << '\n';
		}
		else if (!g_GridArray[backupColGoal]) {
			g_RobotMovementDestCell = backupColGoal;
			MoveSprite(g_Robots[robotIndex], backupColGoal);
			std::cout << " to pos " << backupColGoal << '\n';
		}
	}
}
void AttackEnemy(int robotIndex, float elapsedSec)
{
	float attackTime{ g_Robots[robotIndex].frameTime * g_Robots[robotIndex].cols };
	int damage{ rand() % 5 + 5 };

	// deciding what direction both will be looking in
	if ((g_Luffy.gridArrayIndex % g_BackgroundCols) > (g_Robots[robotIndex].gridArrayIndex % g_BackgroundCols))
	{
		g_Luffy.isFacingLeft = true;
		g_Robots[robotIndex].isFacingLeft = false;
	}
	else if ((g_Luffy.gridArrayIndex % g_BackgroundCols) < (g_Robots[robotIndex].gridArrayIndex % g_BackgroundCols))
	{
		g_Luffy.isFacingLeft = false;
		g_Robots[robotIndex].isFacingLeft = true;
	}

	g_TotalMovementTime += elapsedSec;
	g_Robots[robotIndex].state = State::attack1;
	
	if (g_TotalMovementTime > attackTime) {
		g_TotalMovementTime = 0.0f;
		g_Robots[robotIndex].frameTime = .7f;
		g_Robots[robotIndex].state = State::idle;
		std::cout << "Robot " << robotIndex << " attacked!\n";
		g_Robots[robotIndex].turnActive = false;

		g_Luffy.stats.health -= damage;
		g_Luffy.stats.superCharge += damage;
		g_Luffy.state = State::hurt;
	}
}
bool IsLuffyInRange(int robotIndex)
{
	bool result{ false };

	int luffyCol{ g_Luffy.gridArrayIndex % g_BackgroundCols };
	int luffyRow{ g_Luffy.gridArrayIndex / g_BackgroundCols };
	int robotCol{ g_Robots[robotIndex].gridArrayIndex % g_BackgroundCols };
	int robotRow{ g_Robots[robotIndex].gridArrayIndex / g_BackgroundCols };

	if (abs(luffyCol - robotCol) + abs(luffyRow - robotRow) < 2) result = true;

	return result;
}

int GetRandGridPos()
{
	int result{ rand() % 180 };
	return result;
}

void DealDamageToEnemy(int gridIndex, int damage)
{
	int robotIndex{g_RobotsArrayLength + 1};
	
	for (int i{}; i < g_RobotsArrayLength; i++)
	{
		if (g_Robots[i].gridArrayIndex == gridIndex) robotIndex = i;
	}

	if (robotIndex != g_RobotsArrayLength + 1) g_Robots[robotIndex].state = State::hurt;
}

#pragma endregion gameImplementations

#pragma region coreImplementations
void Initialize()
{
	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		QuitOnSDLError();
	}

	//Use OpenGL 2.1
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

	//Create window
	g_pWindow = SDL_CreateWindow(
		g_WindowTitle.c_str(),
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		int(g_WindowWidth),
		int(g_WindowHeight),
		SDL_WINDOW_OPENGL);

	if (g_pWindow == nullptr)
	{
		QuitOnSDLError();
	}

	// Create an opengl context and attach it to the window 
	g_pContext = SDL_GL_CreateContext(g_pWindow);
	if (g_pContext == nullptr)
	{
		QuitOnSDLError();
	}

	if (g_IsVSyncOn)
	{
		// Synchronize buffer swap with the monitor's vertical refresh
		if (SDL_GL_SetSwapInterval(1) < 0)
		{
			QuitOnSDLError();
		}
	}
	else
	{
		SDL_GL_SetSwapInterval(0);
	}

	// Initialize Projection matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	// Set the clipping (viewing) area's left, right, bottom and top
	gluOrtho2D(0, g_WindowWidth, 0, g_WindowHeight);

	// The viewport is the rectangular region of the window where the image is drawn.
	// Set it to the entire client area of the created window
	glViewport(0, 0, int(g_WindowWidth), int(g_WindowHeight));

	//Initialize Modelview matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Enable color blending and use alpha blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//Initialize PNG loading
	int imgFlags = IMG_INIT_PNG;
	if (!(IMG_Init(imgFlags) & imgFlags))
	{
		QuitOnImageError();
	}

	//Initialize SDL_ttf
	if (TTF_Init() == -1)
	{
		QuitOnTtfError();
	}

}
void Run()
{
	//Main loop flag
	bool quit{ false };

	// Set start time
	std::chrono::steady_clock::time_point t1 = std::chrono::steady_clock::now();

	InitGameResources();

	//The event loop
	SDL_Event e{};
	while (!quit)
	{
		if (g_QuitFromMenu) quit = true;

		// Poll next event from queue
		while (SDL_PollEvent(&e) != 0)
		{
			// Handle the polled event
			switch (e.type)
			{
			case SDL_QUIT:
				//std::cout << "\nSDL_QUIT\n";
				quit = true;
				break;
			case SDL_KEYDOWN:
				ProcessKeyDownEvent(e.key);
				break;
			case SDL_KEYUP:
				ProcessKeyUpEvent(e.key);
				break;
			case SDL_MOUSEMOTION:
				ProcessMouseMotionEvent(e.motion);
				break;
			case SDL_MOUSEBUTTONDOWN:
				ProcessMouseDownEvent(e.button);
				break;
			case SDL_MOUSEBUTTONUP:
				ProcessMouseUpEvent(e.button);
				break;
			default:
				//std::cout << "\nSome other event\n";
				break;
			}
		}

		if (!quit)
		{

			// Calculate elapsed time
			// Get current time
			std::chrono::steady_clock::time_point t2 = std::chrono::steady_clock::now();
			// Calculate elapsed time
			float elapsedSeconds = std::chrono::duration<float>(t2 - t1).count();
			// Update current time
			t1 = t2;
			// Prevent jumps in time caused by break points
			if (elapsedSeconds > g_MaxElapsedTime)
			{
				elapsedSeconds = g_MaxElapsedTime;
			}

			// Call update function, using time in seconds (!)
			Update(elapsedSeconds);

			// Draw in the back buffer
			Draw();

			// Update screen: swap back and front buffer
			SDL_GL_SwapWindow(g_pWindow);
		}
	}
	FreeGameResources();
}

void Cleanup()
{
	SDL_GL_DeleteContext(g_pContext);

	SDL_DestroyWindow(g_pWindow);
	g_pWindow = nullptr;

	SDL_Quit();
}

void QuitOnSDLError()
{
	std::cout << "Problem during SDL initialization: ";
	std::cout << SDL_GetError();
	std::cout << std::endl;
	Cleanup();
	exit(-1);
}

void QuitOnOpenGlError()
{
	std::cout << "Problem during OpenGL initialization: ";
	std::cout << SDL_GetError();
	std::cout << std::endl;
	Cleanup();
	exit(-1);
}

void QuitOnImageError()
{
	std::cout << "Problem during SDL_image initialization: ";
	std::cout << IMG_GetError();
	std::cout << std::endl;
	Cleanup();
	exit(-1);
}

void QuitOnTtfError()
{
	std::cout << "Problem during SDL_ttf initialization: ";
	std::cout << TTF_GetError();
	std::cout << std::endl;
	Cleanup();
	exit(-1);
}
#pragma endregion coreImplementations

#pragma region textureImplementations

bool TextureFromFile(const std::string& path, Texture & texture)
{
	//Load file for use as an image in a new surface.
	SDL_Surface* pLoadedSurface = IMG_Load(path.c_str());
	if (pLoadedSurface == nullptr)
	{
		std::cerr << "TextureFromFile: SDL Error when calling IMG_Load: " << SDL_GetError() << std::endl;
		return false;
	}

	TextureFromSurface(pLoadedSurface, texture);

	//Free loaded surface
	SDL_FreeSurface(pLoadedSurface);

	return true;
}

bool TextureFromString(const std::string & text, const std::string& fontPath, int ptSize, const Color4f & textColor, Texture & texture)
{
	// Create font
	TTF_Font *pFont{};
	pFont = TTF_OpenFont(fontPath.c_str(), ptSize);
	if (pFont == nullptr)
	{
		std::cout << "TextureFromString: Failed to load font! SDL_ttf Error: " << TTF_GetError();
		std::cin.get();
		return false;
	}

	// Create texture using this fontand close font afterwards
	bool textureOk = TextureFromString(text, pFont, textColor, texture);
	TTF_CloseFont(pFont);

	return textureOk;
}

bool TextureFromString(const std::string & text, TTF_Font *pFont, const Color4f & color, Texture & texture)
{
	//Render text surface
	SDL_Color textColor{};
	textColor.r = Uint8(color.r * 255);
	textColor.g = Uint8(color.g * 255);
	textColor.b = Uint8(color.b * 255);
	textColor.a = Uint8(color.a * 255);

	SDL_Surface* pLoadedSurface = TTF_RenderText_Blended(pFont, text.c_str(), textColor);
	//SDL_Surface* pLoadedSurface = TTF_RenderText_Solid(pFont, textureText.c_str(), textColor);
	if (pLoadedSurface == nullptr)
	{
		std::cerr << "TextureFromString: Unable to render text surface! SDL_ttf Error: " << TTF_GetError() << '\n';
		return false;
	}

	// copy to video memory
	TextureFromSurface(pLoadedSurface, texture);

	//Free loaded surface
	SDL_FreeSurface(pLoadedSurface);

	return true;
}

void TextureFromSurface(const SDL_Surface *pSurface, Texture & texture)
{
	//Get image dimensions
	texture.width = float(pSurface->w);
	texture.height = float(pSurface->h);

	// Get pixel format information and translate to OpenGl format
	GLenum pixelFormat{ GL_RGB };
	switch (pSurface->format->BytesPerPixel)
	{
	case 3:
		if (pSurface->format->Rmask == 0x000000ff)
		{
			pixelFormat = GL_RGB;
		}
		else
		{
			pixelFormat = GL_BGR;
		}
		break;
	case 4:
		if (pSurface->format->Rmask == 0x000000ff)
		{
			pixelFormat = GL_RGBA;
		}
		else
		{
			pixelFormat = GL_BGRA;
		}
		break;
	default:
		std::cerr << "TextureFromSurface error: Unknow pixel format, BytesPerPixel: " << pSurface->format->BytesPerPixel << "\nUse 32 bit or 24 bit images.\n";;
		texture.width = 0;
		texture.height = 0;
		return;
	}

	//Generate an array of textures.  We only want one texture (one element array), so trick
	//it by treating "texture" as array of length one.
	glGenTextures(1, &texture.id);

	//Select (bind) the texture we just generated as the current 2D texture OpenGL is using/modifying.
	//All subsequent changes to OpenGL's texturing state for 2D textures will affect this texture.
	glBindTexture(GL_TEXTURE_2D, texture.id);

	// check for errors.
	GLenum e = glGetError();
	if (e != GL_NO_ERROR)
	{
		std::cerr << "TextureFromSurface, error binding textures, Error id = " << e << '\n';
		texture.width = 0;
		texture.height = 0;
		return;
	}

	//Specify the texture's data.  This function is a bit tricky, and it's hard to find helpful documentation.  A summary:
	//   GL_TEXTURE_2D:    The currently bound 2D texture (i.e. the one we just made)
	//               0:    The mipmap level.  0, since we want to update the base level mipmap image (i.e., the image itself,
	//                         not cached smaller copies)
	//         GL_RGBA:    Specifies the number of color components in the texture.
	//                     This is how OpenGL will store the texture internally (kinda)--
	//                     It's essentially the texture's type.
	//      surface->w:    The width of the texture
	//      surface->h:    The height of the texture
	//               0:    The border.  Don't worry about this if you're just starting.
	//     pixelFormat:    The format that the *data* is in--NOT the texture! 
	//GL_UNSIGNED_BYTE:    The type the data is in.  In SDL, the data is stored as an array of bytes, with each channel
	//                         getting one byte.  This is fairly typical--it means that the image can store, for each channel,
	//                         any value that fits in one byte (so 0 through 255).  These values are to be interpreted as
	//                         *unsigned* values (since 0x00 should be dark and 0xFF should be bright).
	// surface->pixels:    The actual data.  As above, SDL's array of bytes.
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, pSurface->w, pSurface->h, 0, pixelFormat, GL_UNSIGNED_BYTE, pSurface->pixels);

	//Set the minification and magnification filters.  In this case, when the texture is minified (i.e., the texture's pixels (texels) are
	//*smaller* than the screen pixels you're seeing them on, linearly filter them (i.e. blend them together).  This blends four texels for
	//each sample--which is not very much.  Mipmapping can give better results.  Find a texturing tutorial that discusses these issues
	//further.  Conversely, when the texture is magnified (i.e., the texture's texels are *larger* than the screen pixels you're seeing
	//them on), linearly filter them.  Qualitatively, this causes "blown up" (overmagnified) textures to look blurry instead of blocky.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

void DeleteTexture(Texture & texture)
{
	glDeleteTextures(1, &texture.id);
}

void DrawTexture(const Texture & texture, const Point2f& bottomLeftVertex, const Rectf & sourceRect)
{
	Rectf destinationRect{ bottomLeftVertex.x, bottomLeftVertex.y, sourceRect.width, sourceRect.height };
	DrawTexture(texture, destinationRect, sourceRect);
}

void DrawTexture(const Texture & texture, const Rectf & destinationRect, const Rectf & sourceRect)
{
	// Determine texture coordinates, default values = draw complete texture
	float textLeft{};
	float textRight{ 1.0f };
	float textTop{};
	float textBottom{ 1.0f };
	if (sourceRect.width > 0.0f && sourceRect.height > 0.0f) // Clip specified, convert them to the range [0.0, 1.0]
	{
		textLeft = sourceRect.left / texture.width;
		textRight = (sourceRect.left + sourceRect.width) / texture.width;
		textTop = (sourceRect.bottom - sourceRect.height) / texture.height;
		textBottom = sourceRect.bottom / texture.height;
	}

	// Determine vertex coordinates
	float vertexLeft{ destinationRect.left };
	float vertexBottom{ destinationRect.bottom };
	float vertexRight{};
	float vertexTop{};
	if (!(destinationRect.width > 0.0f && destinationRect.height > 0.0f)) // If no size specified use size of texture
	{
		vertexRight = vertexLeft + texture.width;
		vertexTop = vertexBottom + texture.height;
	}
	else
	{
		vertexRight = vertexLeft + destinationRect.width;
		vertexTop = vertexBottom + destinationRect.height;

	}

	// Tell opengl which texture we will use
	glBindTexture(GL_TEXTURE_2D, texture.id);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	// Draw
	glEnable(GL_TEXTURE_2D);
	{
		glBegin(GL_QUADS);
		{
			glTexCoord2f(textLeft, textBottom);
			glVertex2f(vertexLeft, vertexBottom);

			glTexCoord2f(textLeft, textTop);
			glVertex2f(vertexLeft, vertexTop);

			glTexCoord2f(textRight, textTop);
			glVertex2f(vertexRight, vertexTop);

			glTexCoord2f(textRight, textBottom);
			glVertex2f(vertexRight, vertexBottom);
		}
		glEnd();
	}
	glDisable(GL_TEXTURE_2D);

}
#pragma endregion textureImplementations