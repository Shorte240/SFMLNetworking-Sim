// Protocols
// All info types the server/client need
// Application layer types

#pragma once

enum Messages : int
{
	Connect,
	BoidCount,
	ObstacleCount,
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
	int ID = -1;
	float positionX;
	float positionY;
	float velocityX;
	float velocityY;
	int redValue;
	int greenValue;
	int blueValue;
	int alphaValue;
	float time;

	BoidData(int id, float posX, float posY, float velX, float velY, int r, int g, int b, int a, float t) :
		ID(id),
		positionX(posX),
		positionY(posY),
		velocityX(velX),
		velocityY(velY),
		redValue(r),
		greenValue(g),
		blueValue(b),
		alphaValue(a),
		time(t)
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

struct ObstacleData
{
	int ID;
	float positionX;
	float positionY;

	ObstacleData(int id, float posX, float posY) :
		ID(id),
		positionX(posX),
		positionY(posY)
	{
	}
};

struct CloseConnection : BaseMessage
{
	CloseConnection()
	{
		messageType = Messages::Disconnect;
	}
};
