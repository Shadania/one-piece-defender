#pragma once
#include "structs.h"
#include <string>

namespace utils
{
	//drawing
	void FillEllipse(Point2f center, float radiusX, float radiusY, Color4f color = { 1.0f,1.0f,1.0f,1.0f });
	void DrawEllipse(Point2f center, float radiusX, float radiusY, Color4f color = { 1.0f,1.0f,1.0f,1.0f }, float lineWidth = 1 );
	void FillRectangle(Rectf rect, Color4f color = { 1.0f,1.0f,1.0f,1.0f });
	void DrawRectangle(Rectf rect, Color4f color = { 1.0f,1.0f,1.0f,1.0f }, float lineWidth = 1);
	void DrawPentagram(Point2f center, float radius, Color4f color);
	void DrawEquilateralTriangle(Point2f leftBotPos, float sideLength, bool filled, Color4f color);
	void DrawQuadrangle(Rectf rect);
	void DrawMultipleSquares(int amount, Rectf rect);
	//rects and circles overlapping
	float GetDistance(Point2f pointA, Point2f pointB);
	bool IsPointInCircle(Point2f point, Circlef circle);
	bool IsPointInRect(Point2f point, Rectf rect);
	bool IsOverlapping(Circlef circleA, Circlef circleB);
	bool IsOverlapping(Rectf rectA, Rectf rectB);
	//vectors
	void DrawVector(Vector2f vector, Point2f start = { 0.0f,0.0f }, Color4f color = { 1.0f,1.0f,1.0f,1.0f });
	Vector2f Add(Vector2f vectA, Vector2f vectB);
	Vector2f Subtract(Vector2f vectA, Vector2f vectB);
	float DotProduct(Vector2f vectA, Vector2f vectB);
	float CrossProduct(Vector2f vectA, Vector2f vectB);
	std::string ToString(Vector2f vector);
	Vector2f Scale();
	float Length();
	Vector2f Normalize();
	float AngleBetween();
	bool AreEqual();
	//array functions
	void PrintArray(const int *pArray, const int arraySize);
	void PrintElements(const int* pNumbers, const int size);
	void PrintElements(const int* pNumbers, const int startIdx, const int endIdx);
	int GetIndex(const int rowIdx, const int colIdx, const int cols);
	int Count(const int *pArray, const int arraySize, const int counter);
	int MinElement(const int *pArray, const int arraySize);
	int MaxElement(const int *pArray, const int arraySize);
	void SwapArrayElements(int *pArray, const int arraySize, int idx1, int idx2);
	void ShuffleArray(int *pArray, const int arraySize, const int swaps);
	void BubbleSort(int *pArray, const int arraySize);
}