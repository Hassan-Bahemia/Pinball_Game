#include "stdafx.h"
#include "MyGame.h"
#include <math.h>
#include <fstream>

CMyGame::CMyGame(void) :
	base2(570, 10, "base.bmp", CColor::Blue(), 0),
	spring2(400, 165, "spring.bmp", CColor::Blue(), 0),
	marker(570, 305, "marker.bmp", CColor::Blue(), 0),
	theBall(CVector(200, 400), "Ball.bmp", CColor::Blue(), GetTime()),
	leftFlipper(250, 105, "LeftFlipper.bmp", CColor::Blue(), 0),
	rightFlipper(350, 107, "RightFlipper.bmp", CColor::Blue(), 0),
	background(300, 450, "Border.bmp", CColor::Blue(), 0),
	inGameBackground(600, 450, "inGameBackground.bmp", CColor::Blue(), 0)

{
	xstart = 0;
	ystart = 0;
	k2 = 1.25;
	spring2.SetPivot(400, 25);
}

CMyGame::~CMyGame(void)
{
	// TODO: add destruction code here
}

/////////////////////////////////////////////////////
// Per-Frame Callback Funtions (must be implemented!)

void CMyGame::OnUpdate()
{
	Uint32 t = GetTime();
	CVector g (0, -3);
	auto velocity = BetterHitTest(&theBall, &marker);

	if (velocity != CVector(0,0))
	{
		theBall.SetVelocity(0, 0);
		// user input
		if (IsKeyDown(SDLK_SPACE))
		{
			springDown = true;
			marker.SetVelocity(marker.GetVelocity().m_x, 0);
			marker.Move(0, -1);
		}
		else
		{
			if (springDown)
			{
				theBall.SetVelocity(0, 600);
			}
			springDown = false;
			float y = marker.GetY() - 305;
			marker.Accelerate(0, -k2 * y);
			marker.SetSpeed(marker.GetSpeed() * 0.95);
		}
	}
	else
	{
		float y = marker.GetY() - 305;
		marker.Accelerate(0, -k2 * y);
		marker.SetSpeed(marker.GetSpeed() * 0.95);
	}
	marker.Update(t);


	theBall.Update(t);
	PlatformCollision();
	RampCollision();
	BumperCollision();


	for (CSprite* pBumpers : bumpers) pBumpers->Update(t);
	for (CSprite* pWall : theWalls) pWall->Update(t);

	theBall.Accelerate(g);

	// animate the spring2
	base2.SetX(marker.GetX());
	spring2.SetX(marker.GetX());
	spring2.SetSize(32,-32+marker.GetY());


	leftFlipper.Update(t);
	rightFlipper.Update(t);

	if (theBall.GetY() <= -100 && lives > 0)
	{
		lives--;
		marker.SetVelocity(marker.GetVelocity().m_x, 0);
		float y = marker.GetY() - 305;
		marker.Accelerate(0, -k2 * y);
		marker.SetSpeed(marker.GetSpeed() * 0.95);
		theBall.SetPosition(xstart, ystart);
		theBall.SetVelocity(0,0);
	}

	if (lives == 0)
	{
		GameOver();
	}

	FlipperControl();

}

void CMyGame::OnDraw(CGraphics* g)
{
	inGameBackground.Draw(g);
	background.Draw(g);
	for each (CSprite * pWall in theWalls) pWall->Draw(g);
	for each (CSprite * pSlopedWalls in ramps) pSlopedWalls->Draw(g);
	for each (CSprite* pBumpers in bumpers) pBumpers->Draw(g);
	base2.Draw(g);
	spring2.Draw(g);
	marker.Draw(g);
	theBall.Draw(g);
	leftFlipper.Draw(g);
	rightFlipper.Draw(g);
	if (IsGameOver())
	{
		*g << font(36) << color(CColor::Green()) << xy(225, 450)<< "  Game Over!" << endl;
		*g << font(36) << color(CColor::Green()) << xy(185, 375) << "Your Final score: " << score << endl;
	}
	else
	{
		*g << font(18) << color(CColor::Green()) <<xy (635, 870 )<< strInfo1 << endl;
		*g << font(19) << color(CColor::Green()) <<xy (640, 850) << strInfo2 << endl;
		*g << font(22) << color(CColor::Green()) << xy(615, 35) << strInfo3 << endl;
		*g << font(22) << color(CColor::Green()) << xy(600, 725) << " Score: " << score << endl;
		*g << font(22) << color(CColor::Green()) << xy(640, 525) << " Controls" << endl;
		*g << font(19) << color(CColor::Green()) << xy(600, 475) << " A - Left Flipper Move" << endl;
		*g << font(19) << color(CColor::Green()) << xy(600, 425) << " D - Right Flipper Move" << endl;
		*g << font(19) << color(CColor::Green()) << xy(600, 375) << " Space - Pull Spring" << endl;
		*g << font(19) << color(CColor::Green()) << xy(600, 325) << " F2 - Restart Game" << endl;
		*g << font(19) << color(CColor::Green()) << xy(600, 275) << " P - Pause Game" << endl;
		*g << font(19) << color(CColor::Green()) << xy(600, 600) << " Balls Remaining: " << lives << endl;
		
	}
}

/////////////////////////////////////////////////////
// Game Life Cycle

// one time initialisation
void CMyGame::OnInitialize()
{
}

// called at the start of a new game - display menu here
void CMyGame::OnDisplayMenu()
{
	SetupLevel();	// this allows to start the game immediately
}

// called when Game Mode entered
void CMyGame::OnStartGame()
{
}

// called when a new level started - first call for nLevel = 1
void CMyGame::OnStartLevel(Sint16 nLevel)
{
}

void CMyGame::SetupLevel()
{
	for each (CSprite * pWall in theWalls) delete pWall;
	theWalls.clear();
	for each (CSprite * pLRamps in ramps) delete pLRamps;
	ramps.clear();
	for each (CSprite * pRRamps in ramps) delete pRRamps;
	ramps.clear();
	for each (CSprite * pBumpers in bumpers) delete pBumpers;
	bumpers.clear();

	score = 0;
	lives = 3;

	theBall.SetVelocity(0, 0);

	theWalls.push_back(new CSprite(CRectangle(0, 890, 600, 10), "WallHorizontal.bmp", GetTime()));
	theWalls.push_back(new CSprite(CRectangle(600, 790, 200, 10), "WallHorizontal.bmp", GetTime()));
	theWalls.push_back(new CSprite(CRectangle(600, 650, 200, 10), "WallHorizontal.bmp", GetTime()));
	theWalls.push_back(new CSprite(CRectangle(600, 890, 200, 10), "WallHorizontal.bmp", GetTime()));
	theWalls.push_back(new CSprite(CRectangle(600, 550, 200, 10), "WallHorizontal.bmp", GetTime()));
	theWalls.push_back(new CSprite(CRectangle(600, 0, 200, 10), "WallHorizontal.bmp", GetTime()));
	theWalls.push_back(new CSprite(CRectangle(600, 65, 200, 10), "WallHorizontal.bmp", GetTime()));
	theWalls.push_back(new CSprite(CRectangle(0, 0, 10, 900), "WallVertical.bmp", GetTime()));
	theWalls.push_back(new CSprite(CRectangle(591, 0, 10, 900), "WallVertical.bmp", GetTime()));

	strInfo1 = "Octotastic Pinball";
	strInfo2 = "Arcade Edition";
	strInfo3 = "Made by Hassan";

	int type, x, y, w, h, rot;

	// declaring new file for reading in data
	fstream myfile;
	// opening file for reading
	myfile.open("level.txt", ios_base::in);


	// reading while the end of file has not been reached
	bool neof; // not end of file
	do
	{
		myfile >> type >> x >> y >> w >> h >> rot;
		neof = myfile.good();

		if (type == 1)
		{
			CSprite* pWall = new CSprite(CRectangle(x, y, w, h), "WallHorizontal.bmp", CColor::Blue(), GetTime());
			pWall->SetRotation(rot);
			theWalls.push_back(pWall);
		}
		if (type == 2)
		{
			CSprite* pWall = new CSprite(CRectangle(x, y, w, h), "WallVertical.bmp", CColor::Blue(), GetTime());
			pWall->SetRotation(rot);
			ramps.push_back(pWall);
		}
		if (type == 3)
		{
			CSprite* pBumpers = new CSprite(CVector(x, y), "Bumper.bmp", CColor::Blue(), GetTime());
			bumpers.push_back(pBumpers);
		}

		xstart = 570;
		ystart = 450;
		theBall.SetPosition(xstart, ystart);


	} while (neof);
	myfile.close();
}

void CMyGame::PlatformCollision()
{
	const float r = 12;
	const float damp = 0.65;

	Uint32 dt = GetDeltaTime();

	for (CSprite* pWalls : theWalls)
	{
		CVector v = theBall.GetVelocity() * dt / 1000;
		CVector t = pWalls->GetPos() - theBall.GetPos();

		Y = pWalls->GetHeight() / 2;
		X = pWalls->GetWidth() / 2;

		//bottom
		if (v.m_y < 0)
		{
			f1 = (t.m_y + Y + r) / v.m_y;
			f2 = (t.m_x - v.m_x * f1) / (X + r);

			if (f1 >= 0 && f1 <= 1 && f2 >= -1 && f2 <= 1)
			{
				BallBounce(CVector(0,1), damp);
				score = score + rand() % 45;
				bounce.Play("bounce.wav");

			}
		}


		//top
		if (v.m_y > 0)
		{
			f1 = (t.m_y - Y - r) / v.m_y;
			f2 = (t.m_x - v.m_x * f1) / (X + r);

			if (f1 >= 0 && f1 <= 1 && f2 >= -1 && f2 <= 1)
			{
				BallBounce(CVector(0,1), damp);
				score = score + rand() % 45;
				bounce.Play("bounce.wav");
			}
		}


		//right
		if (v.m_x > 0)
		{
			f1 = (t.m_x - X - r) / v.m_x;
			f2 = (t.m_y + v.m_y * f1) / (Y + r);

			if (f1 >= 0 && f1 <= 1 && f2 >= -1 && f2 <= 1)
			{
				BallBounce(CVector(1,0), damp);
				score = score + rand() % 45;
				bounce.Play("bounce.wav");
			}
		}

		//left
		if (v.m_x < 0)
		{
			f1 = (t.m_x + X + r) / v.m_x;
			f2 = (t.m_y - v.m_y * f1) / (Y + r);

			if (f1 >= 0 && f1 <= 1 && f2 >= -1 && f2 <= 1)
			{
				BallBounce(CVector(1,0), damp);
				score = score + rand() % 45;
				bounce.Play("bounce.wav");
			}
		}
	}
}

void CMyGame::RampCollision()
{

	const float r = 12;
	const float damp = 0.65;

	Uint32 dt = GetDeltaTime();

	for (CSprite* pRamps : ramps)
	{
		// R = radius ofthe ball
		// X = half of the width of the box
		float X = pRamps->GetWidth() / 2;
		//Y = half the height of the box
		float Y = pRamps->GetHeight() / 2;
		float rot = pRamps->GetRotation();
		float alpha = DEG2RAD(rot);
		CVector v = theBall.GetVelocity() * dt / 1000;
		float xt = theBall.GetX() - pRamps->GetX();
		float yt = theBall.GetY() - pRamps->GetY();
		CVector t = -CVector(xt, yt);
		CVector n = CVector(sin(alpha), cos(alpha));
		if (Dot(v, n) < 0)
		{
			// Perpendicular component (oncoming)
			float vy = Dot(v, n);
			// velocity component
			CVector d = t + (Y + r) * n;
			// distance vector between edges
			float dy = Dot(d, n);
			// perpendicular space between
			float f1 = dy / vy;
			// Parallel component (breadth control)
			float vx = Cross(v, n);
			// velocity component
			float tx = Cross(t, n);
			// distance between centres
			float f2 = (tx + vx * f1) / (X + r);

			if (f1 >= 0 && f1 <= 1 && f2 >= -1 && f2 <= 1)
			{
				BallBounce(n, damp);
				bounce.Play("bounce.wav");
				score = score + rand() % 45;
			}
		}

		//controls the bottom part
		if (Dot(v, n) > 0)
		{
			// Perpendicular component (oncoming)
			float vy = Dot(v, -n);
			// velocity component
			CVector d = t + (Y + r) * -n;
			// distance vector between edges
			float dy = Dot(d, -n);
			// perpendicular space between
			float f1 = dy / vy;
			// Parallel component (breadth control)
			float vx = Cross(v, -n);
			// velocity component
			float tx = Cross(t, -n);
			// distance between centres
			float f2 = (tx - vx * f1) / (X + r);

			if (f1 >= 0 && f1 <= 1 && f2 >= -1 && f2 <= 1)
			{
				BallBounce(-n, damp);
				bounce.Play("bounce.wav");
				score = score + rand() % 45;
			}
		}
	}
}

void CMyGame::BumperCollision()
{
	for (CSprite* pBumpers : bumpers)
	{
		auto velocity = BetterHitTest(&theBall, pBumpers);

		if (velocity != CVector(0,0))
		{
			BallBounce(velocity.Normalise(), 1.25);
			bumper.Play("bumper.wav");
		}

	}
}

void CMyGame::FlipperControl()
{
	LeftFlipper();
	RightFlipper();
}

void CMyGame::LeftFlipper()
{
	const float r = 12;
	const float damp = 1.15;

	auto velocity = BetterHitTest(&theBall, &leftFlipper);

	theBall.HitTest

	if (velocity != CVector(0,0))
	{
		BallBounce(velocity.Normalize(), damp);
	}

	if (IsKeyDown(SDLK_a))
	{
		leftFlipper.SetRotation(-45);
	}
	else
	{
		leftFlipper.SetRotation(0);
	}
}


void CMyGame::RightFlipper()
{
	const float r = 12;
	const float damp = 1.15;

	auto velocity = BetterHitTest(&theBall, &rightFlipper);

	if (velocity != CVector(0,0))
	{
		BallBounce(velocity.Normalize(), damp);
	}

	if (IsKeyDown(SDLK_d))
	{
		rightFlipper.SetRotation(45);
	}
	else
	{
		rightFlipper.SetRotation(0);
	}
}

float CMyGame::GetMagnitude(CVector velocity)
{
	double value = velocity.m_x* velocity.m_x + velocity.m_y * velocity.m_y;
	return sqrt(value);
}

void CMyGame::BallBounce(CVector normal, float damp)
{
	float minVelocity = 250;
	cout << GetMagnitude(theBall.GetVelocity()) << endl;
	CVector newVelocity = theBall.GetVelocity() * damp;
	if (GetMagnitude(theBall.GetVelocity()) < minVelocity)
	{
		newVelocity = theBall.GetVelocity().Normalize() * damp * minVelocity;
	}
	theBall.SetVelocity(Reflect(newVelocity, normal));
}

CVector CMyGame::BetterHitTest(CSprite* a, CSprite* b)
{
	if (a->HitTest(b))
	{
		return a->GetPosition() - b->GetPosition();
	}
	return CVector(0, 0);
}

// called when Game is Over
void CMyGame::OnGameOver()
{
}

// one time termination code
void CMyGame::OnTerminate()
{
}

/////////////////////////////////////////////////////
// Keyboard Event Handlers

void CMyGame::OnKeyDown(SDLKey sym, SDLMod mod, Uint16 unicode)
{
	if (sym == SDLK_F4 && (mod & (KMOD_LALT | KMOD_RALT)))
		StopGame();
	if (sym == SDLK_p)
		PauseGame();
	if (sym == SDLK_F2)
		NewGame();

}

void CMyGame::OnKeyUp(SDLKey sym, SDLMod mod, Uint16 unicode)
{
}


/////////////////////////////////////////////////////
// Mouse Events Handlers

void CMyGame::OnMouseMove(Uint16 x,Uint16 y,Sint16 relx,Sint16 rely,bool bLeft,bool bRight,bool bMiddle)
{
}

void CMyGame::OnLButtonDown(Uint16 x,Uint16 y)
{
}

void CMyGame::OnLButtonUp(Uint16 x,Uint16 y)
{
}

void CMyGame::OnRButtonDown(Uint16 x,Uint16 y)
{
}

void CMyGame::OnRButtonUp(Uint16 x,Uint16 y)
{
}

void CMyGame::OnMButtonDown(Uint16 x,Uint16 y)
{
}

void CMyGame::OnMButtonUp(Uint16 x,Uint16 y)
{
}
