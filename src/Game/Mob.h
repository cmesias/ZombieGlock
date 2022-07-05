/*
 * Mob.h
 *
 *  Created on: Jul 21, 2017
 *      Author: Carl
 */

#ifndef GAME_MOB_H_
#define GAME_MOB_H_

#include "Object.h"

#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>

#include "../LWindow.h"
#include "../LTexture.h"

#include "../Helper.h"
#include "Monster.h"
#include "Maps.h"
#include "TextNFont.h"
#include "../Particless.h"

#include "../Settings.h"

class Mob: public Monster, public Helper, public TextNFont {
public:
	int count;
	const int max = 100;

	// Jump attack
	bool jumpAttack;
	float jumpCD;

	// Hurt animation variables
	float hurtTimer;
	float hurtSpe;

	// Idle animation variables
	float idleTimer;
	float idleSpe;

	// Walk animation variables
	float walkTimer;
	float walkSpe;

	// Attack animation variables
	float atkAnimTimer;
	float atkAnimSpe;
	bool playSlashSFX;	// DO NOT DELETE, we need this because being on an AttackFrame takes more than one frame to complete

	// Death animation variables
	float deathTimer;
	float deathSpe;

	// Clip to render from texture
	int ClipIndex;

	/* First column index
	 * 0: Skeleton
	 * 1: Goblin
	 * 2: Mushroom
	 * 3: Flying eye
	 *
	 * Second column index
	 * 0: Idle
	 * 1: Walk
	 * 2: Attack
	 * 3: Hurt
	 * 4: Death
	 */
	LTexture gTexture[4][5];

	/*
	 * 0: Idle
	 * 1: Walk
	 * 2: Attack
	 * 3: Hurt
	 * 4: Death
	 */
	int sprite_index;

	// Input the most number clips from one of the textures, we will be using it
	SDL_Rect rClips[8];

	LTexture gMobShadow;
	SDL_RendererFlip flip;
	std::string facing;

	Settings *settings;
	void setSettings(Settings *settings);


	// Actual pixel size and pixel coordinates
	float xR,yR,wR,hR;

	/*	Variable: type
	 *	0: Skeleton
	 *
	 */

public: // variables

public:	// Core Functions
	void Init(Mob mob[]);
	void Load(SDL_Renderer *gRenderer);
	void Free();
	void RemoveAll(Mob mob[]);
	void Remove(Mob mob[]);
	void Spawn(Mob mob[], float x, float y, float angle, float speed);

	void Update(Mob mob[],
			Object &obj, Object object[],
			Particle particle[], Particle &p_dummy,
			Map &map, int mex, int mey,
			int camx, int camy, bool playerAlive);

	void UpdateEditor(Mob mob[], int mex, int mey, int camx, int camy);

	void Move(Mob mob[], std::string direction);

	void RenderShadow(SDL_Renderer *gRenderer, Mob mob[], int camx, int camy);

	void RenderBack(SDL_Renderer *gRenderer, Mob mob[], int camx, int camy);

	void RenderFront(SDL_Renderer *gRenderer, Mob mob[], int camx, int camy);

	void RenderUI(SDL_Renderer *gRenderer, Mob mob[], int camx, int camy);

	void RenderDebug(SDL_Renderer *gRenderer, Mob mob[], int camx, int camy);

	void RenderHand(SDL_Renderer *gRenderer, Mob mob[], int newMx, int newMy,
						  int mex, int mey, int camx, int camy);

	void Stun(Mob mob[], int i);

	void Hurt(Mob mob[], int i);

	void DoIdleAnim(Mob mob[], int i);

	void DoWalkAnim(Mob mob[], int i);

	// This will ovveride when idle, and when on cooldown state (animState -1 & 0)
	void DoHurtAnim(Mob mob[], int i);

	void ResetDeathAnim(Mob mob[], int i);
	void DoDeathAnim(Mob mob[], int i);
	void DoMushRoomVFX(Mob mob[], int i, Particle particle[], Particle &p_dummy);

	bool startHurtAnim;
	bool startDeathAnim;
	float delayRemoveMobTimer;

public:	// Getter functions

	// Get x pos
	float getX(Mob mob[], int i);

	// Get y pos
	float getY(Mob mob[], int i);

	// Get left pos
	float getLeftSide(Mob mob[], int i);

	// Get right pos
	float getRightSide(Mob mob[], int i);

	// Get top pos
	float getTopSide(Mob mob[], int i);

	// Get bottom pos
	float getBottomSide(Mob mob[], int i);

	// Get x center
	float getCenterX(Mob mob[], int i);

	// Get y center
	float getCenterY(Mob mob[], int i);

public:	// Setter functions

	void setStatsBasedOnType(Mob mob[], int i);

public:	// Functions that work with other classes

	void GetDistanceOfPlayer(Mob mob[], float targetX, float targetY,
				float targetW, float targetH,
				float *xFollow, float *yFollow);

public:	// Getter functions

public:	// Animations

	// This will be more specific later, just like our Item.cpp,
	// it has different sizes based on clipped size
	int pixelSizeW = 150;
	int pixelSizeH = 150;
	int	shadowSize = 75;

	// These are different per mob
	// Used for textures
	float yOffset;
	float moveSpe;

	// Mob will use this to lock on to the player when the bullet shoots out
	float *xFollow;
	float *yFollow;

	// Target center
	float bmx, bmy;

	// Self center
	float bmx2, bmy2;

	// Target center
	float targetDistanceX;
	float targetDistanceY;

	bool renderInFront;
	bool mouse;
	bool flash;				// flash mobes texture on getting hit by players attacks

	/*
	 * 0: Attack 1
	 * 1: Attack 2
	 */
	int randomAttack;	// Depracated, get rid of this



	/*
	 * -2: 	Stunned state (long cooldown before attacking)
	 * -1: 	Cooldown state (can't attack)
	 * 0: 	Walking around
	 * 1: 	Attack
	 * 2: 	Death animation
	 * 3: 	Projectile following attack
	 */
	int animState;
	std::string atkDir;					// Attack direction
	float atkXOffset;					// Attack offset
	bool chargingAttack;
	float chargeTime;					// Count down to 0
	float chargeTimeSpe;				// Speed of charge attack
	int attackFrame;					// The frame index that the Mob will attack
	const float chargeTimeStart = 60;	// Change this to change the charge-attack animation start time

	float constantFiringTimer;

	float coolDownTimer;
	float coolDownTimeStart = 60;
	const float coolDownTimeStartOnParry = 60 * 5;	// Mob attacked but User parried the attack
	float atkSpe;

public: // Saving & loading functions

	void LoadData(Mob mob[], std::fstream &fileTileDataL);		// Load data

	std::string SaveData(Mob mob[]);		// Save data

public: // Other classes: Objects


};

#endif /* GAME_BOSS_H_ */
