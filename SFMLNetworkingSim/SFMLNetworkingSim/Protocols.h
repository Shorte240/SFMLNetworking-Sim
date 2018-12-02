// Protocols
// All info types the server/client need

#pragma once

#include "BoidManager.h"
#include "ObstacleManager.h"

enum Messages : int
{
	Connect,
	BoidCount,
	ObstacleCount,
	ObstaclePositionData,
	Disconnect
};

struct BaseMessage
{
	int messageType;

	BaseMessage() :
		messageType(0)
	{
	}
};

struct NewConnection : BaseMessage
{
	float time;
	float totalTime;
	int playerID;

	NewConnection()
	{
	}

	NewConnection(float time_, float totalTime_, int ID) :
		time(time_),
		totalTime(totalTime_),
		playerID(ID)
	{
		messageType = Messages::Connect;
	}
};

struct NumBoids : BaseMessage
{
	int numberOfBoids;

	NumBoids(int boidCount) :
		numberOfBoids(boidCount)
	{
		messageType = Messages::BoidCount;
	}
};

struct BoidData
{
	float positionX;
	float positionY;
	float velocityX;
	float velocityY;

	BoidData(float posX, float posY, float velX, float velY) :
		positionX(posX),
		positionY(posY),
		velocityX(velX),
		velocityY(velY)
	{
	}
};

struct NumObstacles : BaseMessage
{
	int numberOfObstacles;

	NumObstacles(int obstacleCount) :
		numberOfObstacles(obstacleCount)
	{
		messageType = Messages::ObstacleCount;
	}
};

struct ObstacleData : BaseMessage
{
	float positionX;
	float positionY;

	ObstacleData(float posX, float posY) :
		positionX(posX),
		positionY(posY)
	{
		messageType = Messages::ObstaclePositionData;
	}
};

struct CloseConnection : BaseMessage
{
	CloseConnection()
	{
		messageType = Messages::Disconnect;
	}
};
