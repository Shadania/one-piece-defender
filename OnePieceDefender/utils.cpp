#include "stdafx.h"
#define _USE_MATH_DEFINES
#include "utils.h"
#include <SDL_opengl.h>
#include <cmath>
#include <string>
#include <iostream>

// OpenGL libs
#pragma comment (lib,"opengl32.lib")
#pragma comment (lib,"Glu32.lib")


namespace utils
{
#pragma region drawing functions
	void DrawMultipleSquares(int amount, Rectf rect)
	{
		float interLine{ rect.width / (2 * amount) }; //distance between two lines of the squares

		for (int k{ 0 }; k < amount; k++)
		{
			glBegin(GL_LINE_LOOP);
			glColor3f(.0f, .0f, .0f);
			glVertex2f(rect.left + interLine * k, rect.bottom + interLine * k);
			glVertex2f(rect.left + rect.width - interLine * k, rect.bottom + interLine * k);
			glVertex2f(rect.left + rect.width - interLine * k, rect.bottom + rect.height - interLine * k);
			glVertex2f(rect.left + interLine * k, rect.bottom + rect.height - interLine * k);
			glEnd();
		}

	}
	void FillEllipse(Point2f center, float radiusX, float radiusY, Color4f color)
	{
		const float pi{ float(3.1415) };
		float deltaAngle{ pi / radiusX };
		if (radiusX < radiusY)
		{
			deltaAngle = pi / radiusY;
		}

		glBegin(GL_TRIANGLE_FAN);
		glColor4f(color.r, color.g, color.b, color.a);
		glVertex2f(center.x, center.y);
		for (float angle{}; angle <= 2 * pi + (deltaAngle / 2); angle += deltaAngle)
		{
			glVertex2f(cos(angle)*radiusX + center.x, sin(angle)*radiusY + center.y);
		}
		glEnd();
	}
	void DrawEllipse(Point2f center, float radiusX, float radiusY, Color4f color, float lineWidth)
	{
		const float pi{ float(3.1415) };
		float deltaAngle{ pi / radiusX };
		if (radiusX < radiusY)
		{
			deltaAngle = pi / radiusY;
		}

		glLineWidth(lineWidth);
		glBegin(GL_LINE_LOOP);
		glColor4f(color.r, color.g, color.b, color.a);
		for (float angle{}; angle <= 2 * pi + (deltaAngle / 2); angle += deltaAngle)
		{
			glVertex2f(cos(angle)*radiusX + center.x, sin(angle)*radiusY + center.y);
		}
		glEnd();
	}
	void DrawPentagram(Point2f center, float radius, Color4f color)
	{
		float angle{ float(M_PI * 2 / 5) };

		glBegin(GL_LINE_LOOP);
		glColor4f(color.r, color.g, color.b, color.a);
		glVertex2f(cos(angle * 0) * radius + center.x, sin(angle * 0) * radius + center.y);
		glVertex2f(cos(angle * 2) * radius + center.x, sin(angle * 2) * radius + center.y);
		glVertex2f(cos(angle * 4) * radius + center.x, sin(angle * 4) * radius + center.y);
		glVertex2f(cos(angle * 1) * radius + center.x, sin(angle * 1) * radius + center.y);
		glVertex2f(cos(angle * 3) * radius + center.x, sin(angle * 3) * radius + center.y);
		glEnd();
	}
	void DrawEquilateralTriangle(Point2f leftBotPos, float sideLength, bool filled, Color4f color)
	{
		Point2f rightBotPos{ leftBotPos.x + sideLength, leftBotPos.y };
		float angle{ float(M_PI / 3) };
		Point2f topPos{ leftBotPos.x + cos(angle) * sideLength, leftBotPos.y + sin(angle) * sideLength };

		switch (filled)
		{
		case true:
			glBegin(GL_POLYGON);
			break;
		case false:
			glBegin(GL_LINE_LOOP);
		}

		glColor4f(color.r, color.g, color.b, color.a);
		glVertex2f(leftBotPos.x, leftBotPos.y);
		glVertex2f(rightBotPos.x, leftBotPos.y);
		glVertex2f(topPos.x, topPos.y);
		glEnd();
	}
	void DrawQuadrangle(Rectf rect)
	{
		glBegin(GL_LINE_LOOP);
		glColor3f(.0f, .0f, .0f);
		glVertex2f(rect.left, rect.bottom);
		glVertex2f(rect.left + rect.width, rect.bottom);
		glVertex2f(rect.left + rect.width, rect.bottom + rect.height);
		glVertex2f(rect.left, rect.bottom + rect.height);
		glEnd();

		float topY{ rect.bottom + rect.height };
		float midY{ rect.bottom + rect.height / 2 };
		float botY{ rect.bottom };
		float leftX{ rect.left };
		float midX{ rect.left + rect.width / 2 };
		float rightX{ rect.left + rect.width };

		glBegin(GL_LINE_LOOP);
		glVertex2f(leftX, midY);
		glVertex2f(midX, botY);
		glVertex2f(rightX, midY);
		glVertex2f(midX, topY);
		glEnd();
	}
	void FillRectangle(Rectf rect, Color4f color)
	{
		glColor4f(color.r, color.g, color.b, color.a);
		glRectf(rect.left, rect.bottom, rect.left + rect.width, rect.bottom + rect.height);
	}
#pragma endregion drawing functions

#pragma region rects and circles overlapping
	float GetDistance(Point2f pointA, Point2f pointB)
	{
		float result{ sqrt(pow(pointA.x - pointB.x, 2) + pow(pointB.y - pointA.y,2)) };
		return result;
	}

	bool IsPointInCircle(Point2f point, Circlef circle)
	{
		if (GetDistance(point, circle.center) > circle.radius) return false;
		else return true;
	}

	bool IsPointInRect(Point2f point, Rectf rect)
	{
		bool xRight{ false };
		bool yRight{ false };

		if (point.x >= rect.left && point.x <= (rect.left + rect.width)) xRight = true;
		if (point.y >= rect.bottom && point.y <= (rect.bottom + rect.height)) yRight = true;

		if (xRight && yRight) return true;
		else return false;
	}

	bool IsOverlapping(Circlef circleA, Circlef circleB)
	{
		if (GetDistance(circleA.center, circleB.center) <= (circleA.radius + circleB.radius)) return true;
		else return false;
	}

	bool IsOverlapping(Rectf rectA, Rectf rectB) 
	{
		bool a{ true };
		bool b{ true };
		bool c{ true };
		bool d{ true };

		if ((rectA.left + rectA.width) < rectB.left) a = false;
		if (rectA.left > (rectB.left + rectB.width)) b = false;
		if ((rectA.bottom + rectA.height) < rectB.bottom) c = false;
		if (rectA.bottom > (rectB.bottom + rectB.height)) d = false;

		if (a && b && c && d) return true;
		else return false;
	}

	void DrawRectangle(Rectf rect, Color4f color, float lineWidth)
	{
		glLineWidth(lineWidth);
		glColor4f(color.r, color.g, color.b, color.a);
		glBegin(GL_LINE_LOOP);
		glVertex2f(rect.left, rect.bottom);
		glVertex2f(rect.left, rect.bottom + rect.height);
		glVertex2f(rect.left + rect.width, rect.bottom + rect.height);
		glVertex2f(rect.left + rect.width, rect.bottom);
		glEnd();
	}
#pragma endregion rects and circles overlapping

#pragma region vectors
	void DrawVector(Vector2f vector, Point2f start, Color4f color)
	{
		float deltaAngle{float(M_PI / 6)};
		float angle{ atan2(vector.y /*- start.y*/, vector.x /*- start.x*/) };

		glColor4f(color.r, color.g, color.b, color.a);
		glBegin(GL_LINES);
		glVertex2f(start.x, start.y);
		glVertex2f(vector.x + start.x, vector.y + start.y);

		glVertex2f(vector.x + start.x, vector.y + start.y);
		glVertex2f(vector.x  + start.x - cos(deltaAngle - angle) * 10.0f, vector.y + start.y + sin(deltaAngle - angle) * 10.0f);

		glVertex2f(vector.x + start.x, vector.y + start.y);
		glVertex2f(vector.x + start.x - cos(-deltaAngle - angle) * 10.0f, vector.y + start.y + sin(-deltaAngle - angle) * 10.0f);
		glEnd();
	}
	std::string ToString(Vector2f vector)
	{
		std::string vect{ "" };
		vect = vect + "[" + std::to_string(vector.x) + ", " + std::to_string(vector.y) + "]";
		return vect;
	}
	Vector2f Add(Vector2f vectA, Vector2f vectB)
	{
		Vector2f result{};
		result.x = vectA.x + vectB.x;
		result.y = vectA.y + vectB.y;
		return result;
	}
	Vector2f Subtract(Vector2f vectA, Vector2f vectB)
	{
		Vector2f result{};
		result.x = vectA.x - vectB.x;
		result.y = vectA.y - vectB.y;
		return result;
	}
	float DotProduct(Vector2f vectA, Vector2f vectB)
	{
		float result{};
		result = GetDistance(Point2f{ 0.0f,0.0f }, Point2f{ vectA.x, vectA.y }) * GetDistance(Point2f{ 0.0f,0.0f }, Point2f{ vectB.x, vectB.y }); //lengths
		result = result * cos(atan2(vectA.y, vectA.x) - atan2(vectB.y, vectB.x)); //angle
		return result;
	}
	float CrossProduct(Vector2f vectA, Vector2f vectB)
	{
		return 0;
	}
#pragma endregion vectors

#pragma region arrays
	void PrintElements(int* pNumbers, int size)
	{
		for (int i{}; i < size; i++)
		{
			std::cout << "Element " << i << ": " << pNumbers[i] << ".\n";
		}
	}

	void PrintElements(int* pNumbers, int startIdx, int endIdx)
	{
		for (int i{}; i < (endIdx - startIdx); i++)
		{
			std::cout << pNumbers[i + startIdx] << " ";
		}
	}

	int GetIndex(const int rowIdx, const int colIdx, const int cols)
	{
		int value{ rowIdx * cols + colIdx };

		return value;
	}
	int Count(const int *pArray, const int arraySize, const int counter)
	{
		int count{ 0 };
		for (int i{}; i < arraySize; i++)
		{
			std::cout << pArray[i] << " ";
			if (pArray[i] == counter) count++;
		}
		std::cout << std::endl;
		return count;
	}
	int MinElement(const int *pArray, const int arraySize)
	{
		int min{ 0 };
		for (int i{}; i < arraySize; i++)
		{
			std::cout << pArray[i] << " ";
			if (pArray[i] < min) min = pArray[i];
		}
		std::cout << std::endl;
		return min;
	}
	int MaxElement(const int *pArray, const int arraySize)
	{
		int max{ 0 };
		for (int i{}; i < arraySize; i++)
		{
			std::cout << pArray[i] << " ";
			if (pArray[i] > max) max = pArray[i];
		}
		std::cout << std::endl;
		return max;
	}
	void SwapArrayElements(int *pArray, const int arraySize, int idx1, int idx2)
	{
		int temp;
		temp = pArray[idx1];
		pArray[idx1] = pArray[idx2];
		pArray[idx2] = temp;

	}
	void ShuffleArray(int *pArray, const int arraySize, const int swaps)
	{
		//std::cout << "\nBefore: ";
		//PrintArray(pArray, arraySize);
		for (int i{}; i < swaps; i++)
		{
			int idx1{ rand() % arraySize };
			int idx2{ rand() % arraySize };
			SwapArrayElements(pArray, arraySize, idx1, idx2);
		}
		//std::cout << "\nAfter: ";
		//PrintArray(pArray, arraySize);
	}
	void BubbleSort(int *pArray, const int arraySize)
	{
		for (int k{}; k < (arraySize - 1); k++)
		{
			for (int i{}; i < (arraySize - 1); i++)
			{
				//i have the first two elements. these need to be compared, and the first one needs to be the min, the second one the max.
				int min, max;
				if (pArray[i] > pArray[i + 1])
				{
					min = pArray[i + 1];
					max = pArray[i];
				}
				else
				{
					min = pArray[i];
					max = pArray[i + 1];
				}
				pArray[i] = min;
				pArray[i + 1] = max;
			}
		}
	}
	void PrintArray(const int *pArray, const int arraySize)
	{
		for (int i{}; i < arraySize; i++)
		{
			std::cout << pArray[i] << " ";
		}
	}
#pragma endregion arrays
}