#pragma once

class CMyGame : public CGame
{
	CSprite background;
	CSprite inGameBackground;
	CSprite spring2;
	CSprite base2;
	CSprite marker;
	CSprite theBall;

	CSprite leftFlipper;
	CSprite rightFlipper;

	float xstart, ystart;
	float X;
	float Y;
	float f1;
	float f2;

	int score;
	int lives;
	bool Held;
	bool springDown;

	CSpriteList theWalls;
	CSpriteList bumpers;
	CSpriteList TriBumpers;
	CSpriteList ramps;

	CSoundPlayer bounce;
	CSoundPlayer bumper;

	float k2; // spring constants (stiffness)

	string strInfo1, strInfo2, strInfo3;

	void SetupLevel();
	void PlatformCollision();
	void RampCollision ();
	void BumperCollision();
	void FlipperControl();
	void LeftFlipper();
	void RightFlipper();

	float GetMagnitude(CVector velocity);
	void BallBounce(CVector normal, float damp);
	CVector BetterHitTest(CSprite* a, CSprite* b);

public:
	CMyGame(void);
	~CMyGame(void);


	// Per-Frame Callback Funtions (must be implemented!)
	virtual void OnUpdate();
	virtual void OnDraw(CGraphics* g);

	// Game Life Cycle
	virtual void OnInitialize();
	virtual void OnDisplayMenu();
	virtual void OnStartGame();
	virtual void OnStartLevel(Sint16 nLevel);
	virtual void OnGameOver();
	virtual void OnTerminate();

	// Keyboard Event Handlers
	virtual void OnKeyDown(SDLKey sym, SDLMod mod, Uint16 unicode);
	virtual void OnKeyUp(SDLKey sym, SDLMod mod, Uint16 unicode);

	// Mouse Events Handlers
	virtual void OnMouseMove(Uint16 x,Uint16 y,Sint16 relx,Sint16 rely,bool bLeft,bool bRight,bool bMiddle);
	virtual void OnLButtonDown(Uint16 x,Uint16 y);
	virtual void OnLButtonUp(Uint16 x,Uint16 y);
	virtual void OnRButtonDown(Uint16 x,Uint16 y);
	virtual void OnRButtonUp(Uint16 x,Uint16 y);
	virtual void OnMButtonDown(Uint16 x,Uint16 y);
	virtual void OnMButtonUp(Uint16 x,Uint16 y);
};
