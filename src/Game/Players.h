/*
 * Players.h
 *
 *  Created on: Dec 18, 2016
 *      Author: Carl
 */

#ifndef LOCAL_PLAYERS_H_
#define LOCAL_PLAYERS_H_

#include <SDL2/SDL_ttf.h>

#include "../LWindow.h"
#include "../LTexture.h"
#include "../Particless.h"
#include "../Helper.h"
#include "Maps.h"
#include "Tiles.h"
#include "Tilec.h"
#include "Object.h"
#include "TextNFont.h"

#include "../Settings.h"

#include <vector>

//Player
class Players: public Helper, public TextNFont {
public:
	enum Result { QUIT, LOADMENU, PLAYGAME, QUITPLAYGAME };

	Settings *settings;
	void setSettings(Settings *settings);

public:
	// Joystick Sensitivity
	const int JOYSTICK_DEAD_ZONE = 8000;
	void OnKeyDown(SDL_Keycode sym );
	void OnKeyUp(SDL_Keycode sym );
	void mouseClickState(SDL_Event &e);

	// Mouse stuff
	bool leftclick;
	bool rightclick;
	bool test;

private:	// Weapons

public:	// resources
	// Local resources
	LTexture gPlayer;
	LTexture gPlayerShadow;
	LTexture gShield;
	LTexture gItems;
	SDL_RendererFlip flipW;

	/* Item types
	 *
	 * 0: Pistol
	 * 1: Rifle
	 * 2: Shotgun
	 * 3: Crowbar
	 *
	 * 4: Sword
	 * 5: SOMETHING WHITE
	 * 6: Adrenaline
	 * 7: Health
	 *
	 * 8: Vest
	 * 9: Helmet
	 * 10: Pistol/Rifle ammo
	 * 11: Shotgun ammo
	 *
	 * 12: Water
	 * 13: Soda
	 * 14: Energy Drink
	 * 15: Food
	 */
	SDL_Rect rItems[16];

	/* Item types
	 *
	 * -1: Knife
	 * 0: Pistol
	 * 1: Rifle
	 * 2: Shotgun
	 * 3: Crowbar
	 *
	 * 4: Sword
	 * 5: SOMETHING WHITE
	 * 6: Adrenaline
	 * 7: Health
	 *
	 * 8: Vest
	 * 9: Helmet
	 * 10: Pistol/Rifle ammo
	 * 11: Shotgun ammo
	 *
	 * 12: Water
	 * 13: Soda
	 * 14: Energy Drink
	 * 15: Food
	 */
	int itemIndex = 0;

	// Weapon attack speeds
	const float pistolAtkSpe = 9.85;
	const float rifleAtkSpe = 13.67;
	const float shotgunAtkSpe = 4.45;

	/*
	 *
	 *
	// Walk-down
	    // 0:
		// 1:
		// 2:
		// 3:
		// 4:

		// 5: Walk-down slash 0
		// 6: Walk-down slash (Attack 1)
		// 7: Walk-down slash (Attack 2)
		// 8: Walk-down slash (Attack 3)
		 *
	// Walk-up
		// 9:
		// 10:
		// 11:
		// 12:
		// 13:

		// 14: Walk-up slash 1
		// 15: Walk-up slash (Attack 1)
		// 16: Walk-up slash (Attack 2)
		// 17: Walk-up slash (Attack 3)
		 *
	// Walk-right
		// 18:
		// 19:
		// 20:
		// 21:
		// 22:

		// 23: Walk-right slash
		// 24: Walk-right slash (Attack 1)
		// 25: Walk-right slash (Attack 2)
		// 26: Walk-right slash (Attack 3)
		 *
	// Walk-left
		// 27:
		// 28:
		// 29:
		// 30:
		// 31:

		// 32: Walk-left slash 0
		// 33: Walk-left slash (Attack 1)
		// 34: Walk-left slash (Attack 2)
		// 35: Walk-left slash (Attack 3)

	 * Store player texture clips here
	 */
	std::vector<SDL_Rect> vPlayer;

	SDL_Rect rShield[7];

	/*
	 * 0-3: Walk
	 *
	 */
	int sprite_index;

	/*
	 * 0 = 6: starts on 1st slash
	 * 1 = 7: starts on 2nd slash combo
	 * 2 = 8: starts on 3rd slash combo
	 */
	int sprite_index_attack_1;
	int sprite_index_attack_2;
	int sprite_index_attack_3;

	/*
	 * 0: Facing down block
	 * 9: Facing up block
	 * 18: Facing right
	 * 27: Facing left
	 */
	int sprite_dir;

	/*
	 * 0 = 0: Facing Down
	 * 1 = 9: Facing Up
	 * 2 = 18: Facing Right
	 * 3 = 27: Facing Left
	 */
	int sprite_dir_list[4];

	/*
	 * 0: Keyboard
	 * 1: Keyboard + Mouse
	 */
	int ControlsPreference = 1;	// default: 0

public:	// variables

	/*
	 * Parrying small blue Bullet from Boss:		score += 1;
	 * Parrying small red Bullet from Boss:			score += 2;
	 * Parrying large red Bullet from Boss:			score += 4;
	 * Hitting Boss directly w/ Slash Projectile: 	score += 5
	 * Hitting Boss directly w/ Sword: 				score += 5
	 * Hitting Boss with counter is:				score += 10
	 */
	unsigned int score	= 0;
	unsigned int highscore = 0;

	std::string name;
	int realw = 32, realh = 32;			// object size in actual pixel size
	int w = 10, h = 10;					// object size in collision pixels
	float x = 0,
		  y = 0;
	float checkpointX, checkPointY;					// When Player enters a new area, this will be there spawn point
	float armX;							// This is where we will attach the sword to
	float armY;
	float x2, y2;						// player center
	float angle 		= 0.0,
		   radians 		= 0.0,
		   radianCos 	= 0.0,
		   radianSin 	= 0.0;
	bool camlocked 		= true;
	bool shift 			= false;
	bool deathScreen	= false;
	bool alive;

	double tempc, temps;
	SDL_Rect continueButton[3];

	// High-Score display
	std::string highList[10];
	int indx 		= 0;
	float travel 	= 0.0;
	int position	= 0;
	int position2	= 0;
	int dir 		= 1;

public: // Attack variables

	bool clash;

private:	// Abilities

	// Fire-rate
	double particleW 	= 48;
	double particleH 	= 48;

private:

	// Walking
	std::string facing;
	bool moveleft 		= false,
		 moveright 		= false,
		 moveup 		= false,
		 movedown		= false;
	bool moving 		= false;
	bool sprint			= false;
	float walkTimerVFX;
	float walkTimer;
	float vX 			= 0.0;
	float vY 			= 0.0;
	float velMax 		= 1.00;
	float velSpeed		= 0.50;

	// Invulnerability ability
	int invurnerableFrame;
	int invurnerableTick;
	int invurnerableT;
	bool invurnerable;

	// Shoot Attack
	float shootTimer	= 0;
	bool shootDelay 	= false;
	bool initialshot 	= false;

	// Attack

	/*
	 * 300 frames
	 *
	 * From 0 - 30, sprite_index = 5;
	 */
	float attackTimer;
	float atkAnimSpe = 0;
	int attackFrame = 5;
	int attackType;	// 1st Attack, 2nd attack, 3 attack lunge
	bool attack;
	bool promptContinueAttacking;	// Used for combos
	bool startCombo;				// Used for combos
	float knockBackPower;	// Knock back power
	int powerUp;			// 0: no power up, 1: 3 burst, 2: 360 burst
	float powerUpTimer;

	// Delay
	float atkSpeed;	// Player's attack speed
	float delayTimer;
	bool delay;

	// Parry ability
	bool parry;
	float parryTimer;					// Parry timer
	float parryCDTimer;					// Parry cooldown timer tracker
	const float parryCDMax = 60 * 3;	// Parry cooldown, default: 60*3

	// Dash ability
	bool dash;							// Dash ability
	float dashSpeed;					// Default: 15
	float dashLength;					// Default: 5
	float dashCounter;					// Default: 0
	float dashCoolCounter;				// Default: 0
	const float dashCooldown = 60 * 3;	// Default: 60

	// Stunned ability
	int stunTimer;
	bool stunned;

	// Official player control keys
	bool pressedEquipKey;

	// Health
	int hearts;							// Default: 3
	const int heartsMax = 3;			// Default: 99
	float health;						// Default: 100
	float healthDecay;					// Default: 100
	float healthMax = 100;				// Default: 100

	// Damage
	float damage;			// Sword damage
	float castDamage;		// Cast damage
	float damageMultipler;	// Parrying will increase damage multiplier

	// Mana
	float mana;				// Mana
	float maxMana;			// Max mana
	float manaRegenTimer;	// Timer for mana regen
	float manaRegenSpeed;	// Mana
	float manaGainOnParry;	// Gain mana if you parry successfully

	// Parry
	float parryLength;		// Parry length: 15 default

private:	// TODO [ ] - Save in file

	/* Save into player save data
	 *
	 * damage 			= 10;
	 * atkSpeed			= 4.27;
	 * heartsMax		= 3;
	 * healthMax 		= 100;
	 * castDamage 		= 100;
	 * castAtkSpe 		= 1;
	 * maxMana			= 100;
	 * manaRegenSpeed	= 8.75;
	 * manaGainOnParry	= 5.25;
	 * parryLength		= 15;
	 * dashSpeed		= 10;
	 * dashLength		= 5;
	 * alive 			= true;
	 *
	 */

private:	// Variables used in Textures

	const float xOffset = 0;
	const float yOffset = - (this->h * 0.70);

public:	// Functions to do stuff?

	void SlashAttack();

	void ActivateParry();

	void ActivateDash();


	// Move forward a little
	void MoveForward();

	// Spawn attack object
	void SpawnAttackObject(Object &obj, Object object[]);

public:	// Mutator functions

	float setVx(float value);

	float setVy(float value);

	// Move x pos
	float moveX(float value);

	// Move y pos
	float moveY(float value);

	// Applies invurnerability to Player
	void ActivateInvurnerability();

	// Stops parry but goes on cool down
	void StopParrying();

	// Stops parry but reset cool down to 0
	void ResetParry();

	// Extend parry duration, and Gain mana back
	void ExtendParryDuration();

	// Increase Hearts
	void IncreaseHearts(int value = 1);

	// Increase Health
	void IncreaseHealth(float value);

	// Increase Score
	void IncreaseScore(float value);

	// Shorten parry cool down
	void ShortenParryCD(float value);

	// Stop movement
	void StopMovement();

	// Stop Slash Attack
	void StopSlashAttack();

	// Stop dashing
	void StopDashing();

	// Reset dashing
	void ResetDashing();

	// Equip a sword
	void EquipWeapon(int weaponIndex, float damage, float slashAtkSpe);

	// Set false to equip key state
	void stopEquipState();

	// Change powerup
	void setPowerUp(int value);

public:	// Accessor functions

	// Get x pos
	float getX();

	// Get y pos
	float getY();

	// Get y offset
	float getYOffset();

	// Get width
	float getW();

	// Get height
	float getH();

	// Get width
	float getPixelW();

	// Get height
	float getPixelH();

	// Get left pos
	float getLeftSide();

	// Get right pos
	float getRightSide();

	// Get top pos
	float getTopSide();

	// Get bottom pos
	float getBottomSide();

	// Get player damage
	float getDamage();

	// Get player cast damage
	float getCastDamage();

	// Get x center of player
	float getCenterX();

	// Get y center of player
	float getCenterY();

	// Get value depending on direction facing
	float getXDir();

	// Get knockback power
	float getKnockBackPower();

	// Get status of invurnerability
	float getInvurnerableStatus();

	// Get status of parrying
	float getParryStatus();

	// Get status of dashing
	float getDashStatus();

	// Get score
	float getScore();

	// Get equip key state
	bool getEquipState();

	// Check if item in hand is this index
	int getItemEqipped(int checkThisIndex);

	// Get item index in hand
	int getItemID();

	// Return number of Silver keys for Player
	int getSilverKeys();

	// Return number of Gold keys for Player
	int getGoldKeys();

	// Return current attack type
	int getAtkType();

public:

	// Reset High Scores
	void ResetHighScore(int LevelWeLoaded);

	// Load previous high score from PlayGame.cpp
	void ApplyHighScore(float previousHighScore);

	// Save current highs-core for current Level
	void SaveHighScore(int LevelToLoad);

	// Save player stats
	void SaveData();

	// Load player stats
	void LoadData();

public:	// functions
	// Functions

	void Load(SDL_Renderer* gRenderer);

	void Free();

	void Init(float spawnX, float spawnY, std::string newName);

	// Called when player loses all their health
	void RespawnPlayer();

	// Called when player loses all their lives
	void ResetLivesAndPlayer();

	bool checkCollision(int x, int y, int w, int h, int x2, int y2, int w2, int h2);

	// Get audio files
	//void getAudio(Mix_Chunk* sLazer);

	// Player shoot
	void fire(Particle particle[], Particle &p_dummy, Mix_Chunk* sCastSFX, int mx, int my);

	void Update(Map &map,
				Particle particle[], Particle &p_dummy,
				Tile &tl, Tile tile[],
				Tilec &tlc, Tilec tilec[],
				Object &obj, Object object[],
				int mx, int my, int camx, int camy,
				float spawnX, float spawnY,
				LWindow gWindow, SDL_Renderer* gRenderer,
				SDL_Color color,
				bool &RestartLevel,
				int LevelWeLoaded, bool &ShakeCamera);

	void Render(int mx, int my, int camX, int camY, LWindow gWindow,
				SDL_Renderer* gRenderer,
				SDL_Color color, int &PARTICLES);

	void RenderUI(SDL_Renderer *gRenderer, int camX, int camY, int CurrentLevel);

	void RenderDebug(SDL_Renderer* gRenderer, int camX, int camY);

};

#endif /* LOCAL_PLAYERS_H_ */
