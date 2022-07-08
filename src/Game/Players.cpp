/*
 * Players.cpp
 *
 *  Created on: Dec 18, 2016
 *      Author: Carl
 */

#include <iostream>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sstream>
#include <vector>
#include <fstream>

#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_mouse.h>

//#include "Input.h"

#include "Players.h"

void Players::setSettings(Settings *settings) {
	this->settings = settings;
}

// Reset game
void Players::Init(float spawnX, float spawnY, std::string newName){
	this->x 				= spawnX;
	this->y 				= spawnY;
	this->name				= newName;

	this->sprite_index 		= 0;
	this->clash 			= false;
	this->score 			= 0;

	// Screen prompts
	this->deathScreen 		= false;
	this->alive 				= true;

	// Always reset these

	// Walking
	this->facing			= "down";
	this->moving			= false;
	this->moveleft 			= false;
	this->moveright 		= false;
	this->moveup 			= false;
	this->movedown			= false;
	this->walkTimer 		= 0;
	this->walkTimerVFX 		= 0;
	this->vX 				= 0.0;
	this->vY 				= 0.0;

	// Invulnerability ability
	this->invurnerableFrame	= 1;
	this->invurnerableTick	= 0;
	this->invurnerableT		= 300;
	this->invurnerable		= true;

	// Shoot Attack
	this->shootTimer 		= 0;
	this->shootDelay 		= false;
	this->initialshot 		= false;

	// Weapon
	this->itemIndex 		= 0;

	// Attack
	//this->powerUp			= 0;
	setPowerUp(0);
	this->powerUpTimer		= 0;

	// Attack
	this->attackTimer 		= 0;
	this->attackFrame 		= 5;
	this->attack 			= false;
	this->attackType		= -1;
	this->promptContinueAttacking = false;
	this->startCombo		 = false;

	// Delay
	this->delayTimer		= 0;
	this->delay				= false;

	// Parry ability
	this->parry 			= false;
	this->parryTimer 		= 0;
	this->parryCDTimer 		= 0;

	// Dash ability
	this->dash 				= false;
	this->dashSpeed 		= 10;
	this->dashLength 		= 5;
	this->dashCounter 		= 0;
	this->dashCoolCounter 	= 0;

	// Stunned ability
	this->stunned 			= false;
	this->stunTimer 		= 0;

	// Official player control keys
	this->pressedEquipKey 	= false;

	// TODO (3-9-2022) [ ] - Save these player stats in a file. This will be the players save data.

	// Inventory

	// Health
	this->hearts 				= 3;
	this->healthDecay 			= 100;
	this->health				= 100;
	this->healthMax				= 100;

	// Damage
	this->castDamage			= 15;
	this->damageMultipler		= 1;
	this->atkSpeed 				= 20;

	// Mana
	this->maxMana				= 100;
	this->mana					= this->maxMana;
	this->manaRegenTimer		= 0;
	this->manaRegenSpeed		= 8.75;
	this->manaGainOnParry		= 5.25;

	// Parry
	this->parryLength			= 15;

	// Other stats
	this->knockBackPower		= 1.58;

	// Weapons
	this->ammoPistol 			= 90;
	this->ammoRifle 			= 120;
	this->ammoShotgun 			= 45;

	// Weapons
	this->magazinePistol 		= 12;
	this->magazineRifle 		= 24;
	this->magazineShotgun 		= 5;

	// Reloading
	this->reloadTimer 			= 0.0;
	this->pistolReloadSpe 		= 15.0;
	this->rifleReloadSpe 		= 9.86;
	this->shotgunReloadSpe 		= 4.0;

	// Set default sword: fists
	ResetLivesAndPlayer();

	// Load Player save data
	this->LoadData();
}

void Players::RespawnPlayer() {
	this->ActivateInvurnerability();
	this->StopMovement();
	this->StopSlashAttack();
	this->StopDashing();
	this->ResetDashing();
	this->health				= this->healthMax;
	this->walkTimer 			= 0;
	this->walkTimerVFX 			= 0;
}

void Players::ResetLivesAndPlayer() {

	// Name
	std::string newName;
	newName="AAA";

	// Set default sword: fists
	this->EquipWeapon(0, 26, 6.75);

	// Reset score and lives, and turn player alive
	this->score 				= 0;

	this->LoadData();
}



// Check collision between 2 objects
bool Players::checkCollision(int x, int y, int w, int h, int x2, int y2, int w2, int h2){
	bool collide;
	if (x+w > x2 && x < x2 + w2 && y+h > y2 && y < y2 + h2){
		collide = true;
	}else{
		collide = false;
	}
	return collide;
}

// Load asteroid resources
void Players::Load(SDL_Renderer* gRenderer)
{
    // load textures
	gPlayer.loadFromFile(gRenderer, "resource/gfx/player/player.png");
	gPlayerShadow.loadFromFile(gRenderer, "resource/gfx/player/player_shadow.png");
	gShield.loadFromFile(gRenderer, "resource/gfx/player/shield.png");
	gItems.loadFromFile(gRenderer, "resource/gfx/ZombieRLItems.png");

	// Clip player texture

	// Direciton
	sprite_dir_list[0] = 0;
	sprite_dir_list[1] = 9;
	sprite_dir_list[2] = 18;
	sprite_dir_list[3] = 27;

	// Down clips
	for (int i=0; i<9; i++)
		vPlayer.push_back({64*i, 0, 64, 64});

	// Up clips
	for (int i=0; i<9; i++)
		vPlayer.push_back({64*i, 64, 64, 64});

	// Right clips
	for (int i=0; i<9; i++)
		vPlayer.push_back({64*i, 64*2, 64, 64});

	// Left clips
	for (int i=0; i<9; i++)
		vPlayer.push_back({64*i, 64*3, 64, 64});

	// Start index for attacks
	sprite_index_attack_1 = 6;
	sprite_index_attack_2 = 7;
	sprite_index_attack_3 = 8;

	// Shield clips
	for (int i=0; i<7; i++){setClips(rShield[i], i*48, 0, 48, 48);}

	// Clip swords texture
	{

		// ------------- 1st row
		// Pistol
		rItems[0] = {0+16*0,0,16,16};

		// Rifle
		rItems[1] = {0+16*1,0,16,16};

		// Shotgun
		rItems[2] = {0+16*2,0,16,16};

		// Crowbar
		rItems[3] = {0+16*3,0,16,16};

		// ------------- 2nd row
		// Sword
		rItems[4] = {0+16*0,16,16,16};

		// SOMETHING WHITE
		rItems[5] = {0+16*1,16,16,16};

		// Adrenaline
		rItems[6] = {0+16*2,16,16,16};

		// Health
		rItems[7] = {0+16*3,16,16,16};

		// ------------- 3rd row
		// Vest
		rItems[8] = {0+16*0,32,16,16};

		// Helmet
		rItems[9] = {0+16*1,32,16,16};

		// Pistol/Rifle ammo
		rItems[10] = {0+16*2,32,16,16};

		// Shotgun ammo
		rItems[11] = {0+16*3,32,16,16};

		// ------------- 4th row
		// Water
		rItems[12] = {0+16*0,48,16,16};

		// Soda
		rItems[13] = {0+16*1,48,16,16};

		// Energy Drink
		rItems[14] = {0+16*2,48,16,16};

		// Food
		rItems[15] = {0+16*3,48,16,16};

	}

	// Other classes
	LoadFonts();
}

// Free asteroid resources
void Players::Free(){

    // Free textures
	gPlayer.free();
    gPlayerShadow.free();
    gShield.free();
    gItems.free();

	// Other classes
	FreeFonts();
}

// Player shoot
void Players::fire(Particle particle[], Particle &p_dummy, Mix_Chunk *sCastSFX, int mx, int my){

	// Get player angle based on mouse coordinates
	if (this->ControlsPreference == 1) {
		angle = atan2(my - y-h/2,mx - x-w/2);
		angle = angle * (180 / 3.1416);
	}
	//Set player angle max limits
	if (angle < 0) {
		angle = 360 - (-angle);
	}

	if (angle > 270 || angle < 90) {
		flipW = SDL_FLIP_NONE;
	} else {
		flipW = SDL_FLIP_VERTICAL;
	}

	// Get radiant from angle
	radians = (3.1415926536/180)*(angle);
	radianCos = floor(cos(radians)*10+0.5)/10;
	radianSin = floor(sin(radians)*10+0.5)/10;


	// Handle Pistol shots
	if (itemIndex == 0) {

		// Shoot particle
		if (initialshot)
		{
			// Stop trigger
			initialshot = false;

			// If we have enough bullets
			if (magazinePistol > 0) {

				// Start delay before each shot
				if (!shootDelay)
				{
					shootDelay = true;

					// Set bullet shadow offset
					float particleShadowOffset = 14;

					// spawn: Pistol bullet Particle
					p_dummy.spawnPistolBullet(particle, 0,
							getCenterX() -4, getCenterY(),
							4, 4,
							this->angle, this->damage, 9);

					// play audio
					Mix_PlayChannel(1, settings->sPistol, 0);

					// decrease magazine size
					magazinePistol--;
				}
			}

			// Not enough bullets, play SFX for trigger
			else {

				// play audio
				Mix_PlayChannel(1, settings->sTrigger, 0);
			}
		}
	}

	// Handle Rifle shots
	if (itemIndex == 1) {

		// Shoot particle
		if (initialshot)
		{
			// If we have enough bullets
			if (magazineRifle > 0) {

				// Start delay before each shot
				if (!shootDelay)
				{
					shootDelay = true;

					// Set bullet shadow offset
					float particleShadowOffset = 14;

					// spawn: Pistol bullet Particle
					p_dummy.spawnPistolBullet(particle, 0,
							getCenterX() -4, getCenterY(),
							4, 4,
							this->angle, this->damage, 9);

					// play audio
					Mix_PlayChannel(1, settings->sCast, 0);

					// decrease magazine size
					magazineRifle--;
				}
			}

			// If not enough bullets
			else {

				// Stop trigger
				initialshot = false;

				// play audio
				Mix_PlayChannel(1, settings->sTrigger, 0);
			}
		}
	}

	// Handle Shotgun shots
	if (itemIndex == 2) {

		// Shoot particle
		if (initialshot)
		{
			// Stop trigger
			initialshot = false;

			// If we have enough bullets
			if (magazineShotgun > 0) {

				// Start delay before each shot
				if (!shootDelay)
				{
					shootDelay = true;

					// Set bullet shadow offset
					float particleShadowOffset = 14;

					// spawn: Pistol bullet Particle
					p_dummy.spawnPistolBullet(particle, 0,
							getCenterX() -4, getCenterY(),
							4, 4,
							this->angle-5, this->damage, 9);

					// spawn: Pistol bullet Particle
					p_dummy.spawnPistolBullet(particle, 0,
							getCenterX() -4, getCenterY(),
							4, 4,
							this->angle, this->damage, 9);

					// spawn: Pistol bullet Particle
					p_dummy.spawnPistolBullet(particle, 0,
							getCenterX() -4, getCenterY(),
							4, 4,
							this->angle+5, this->damage, 9);

					// play audio
					Mix_PlayChannel(1, settings->sSlashHitBoss, 0);

					// decrease magazine size
					magazineShotgun--;
				}
			}

			// If not enough bullets
			else {

				// play audio
				Mix_PlayChannel(1, settings->sTrigger, 0);
			}
		}
	}

	// Handle reloading
	UpdateReloading();

	// Shoot delay
	if (shootDelay) {

		// Start timer
		shootTimer += atkSpeed;

		// After 1 second
		if (shootTimer > 60) {

			// Stop delay
			shootDelay = false;

			// Reset shoot timer
			shootTimer = 0;
		}
	}

	// Handle power up timer
	/*if (powerUp == 0) {
		powerUpTimer += 1;

		// If power up down
		if (powerUpTimer > 60 * 1) {

			// Stop power up
			powerUpTimer = 0;

			// Set to default attack
			powerUp = 0;
		}
	}

	// Handle power up timer
	if (powerUp == 1) {
		powerUpTimer += 1;

		// If power up down
		if (powerUpTimer > 60 * 1) {

			// Stop power up
			powerUpTimer = 0;

			// Set to default attack
			powerUp = 0;
		}
	}

	// Handle power up timer
	if (powerUp == 2) {
		powerUpTimer += 1;

		// If power up down
		if (powerUpTimer > 60 * 1) {

			// Stop power up
			powerUpTimer = 0;

			// Set to default attack
			powerUp = 0;
		}
	}*/
}

void Players::UpdateReloading() {

	// Handle reloading
	if (reload) {
		reloadTimer += equippedWeaponReloadSpeed;
		if (reloadTimer > 60) {
			reloadTimer = 0;

			// If Pistol equipped
			if (itemIndex == 0) {

				// Check if magazine is full
				if (magazinePistol < 12) {

					// Decrease ammo for current weapon
					if (ammoPistol > 0) {
						ammoPistol--;

						// Increase magazine clip for current weapon
						magazinePistol++;

						// Play reload SFX
						Mix_PlayChannel(1, settings->sPistolReload, 0);
					}

					// No more ammo, stop reloading
					else {
						reload = false;
						reloadTimer = 0;
					}
				}

				// Magazine clip is full, stop reload, reset timer
				else {
					reload = false;
					reloadTimer = 0;
				}
			}

			// If Rifle equipped
			if (itemIndex == 1) {

				// Check if magazine is full
				if (magazineRifle < 30) {

					// Decrease ammo for current weapon
					if (ammoPistol > 0) {
						ammoPistol--;

						// Increase magazine clip for current weapon
						magazineRifle++;

						// Play reload SFX
						Mix_PlayChannel(1, settings->sPistolReload, 0);
					}

					// No more ammo, stop reloading
					else {
						reload = false;
						reloadTimer = 0;
					}
				}

				// Magazine clip is full, stop reload, reset timer
				else {
					reload = false;
					reloadTimer = 0;
				}
			}

			// If Shotgun equipped
			if (itemIndex == 2) {

				// Check if magazine is full
				if (magazineShotgun < 5) {

					// Decrease ammo for current weapon
					if (ammoShotgun > 0) {
						ammoShotgun--;

						// Increase magazine clip for current weapon
						magazineShotgun++;

						// Play reload SFX
						Mix_PlayChannel(1, settings->sPistolReload, 0);
					}

					// No more ammo, stop reloading
					else {
						reload = false;
						reloadTimer = 0;
					}
				}

				// Magazine clip is full, stop reload, reset timer
				else {
					reload = false;
					reloadTimer = 0;
				}
			}
		}
	}
}

// Update Player
void Players::Update(Map &map,
					Particle particle[], Particle &p_dummy,
					Tile &tl, Tile tile[],
					Tilec &tlc, Tilec tilec[],
					Object &obj, Object object[],
					int mex, int mey, int camx, int camy,
					float spawnX, float spawnY,
					LWindow gWindow, SDL_Renderer* gRenderer,
					SDL_Color color,
					bool &RestartLevel,
					int LevelWeLoaded, bool &ShakeCamera)
{
	// Player center
	this->x2 = this->x+this->w/2;
	this->y2 = this->y+this->h/2;

	// Set arm location
	this->armX = this->x + 18;
	this->armY = this->y;

	// Player alive
	if (this->alive)
	{

		// Player health decay
		if (this->healthDecay > this->health) {
			this->healthDecay -= 0.25;
		}
		if (this->healthDecay < this->health) {
			this->healthDecay += 0.25;
		}

		////////////////////////////////////////////////////////////////////////////////////
		//--------------------------------------------------------------------------------//
		//----------------------------------- Movement -----------------------------------//
		{
			// If sprinting, change max movement speed
			if (this->sprint) {
				velMax = 1.50;
				velSpeed = 0.5;
			} else {
				velMax = 1.00;
				velSpeed = 0.50;
			}

			// X Axis movement
			{
				// Move left
				if (this->moveleft && !this->attack && !this->delay && !this->parry && !this->dash) {
					if (this->vX > -this->velMax) {
						this->vX -= this->velSpeed;
					}
					this->moving = true;
					if (this->ControlsPreference == 0) {
				        if (!this->shift) {
				        	this->facing = "left";
				        	this->sprite_dir = this->sprite_dir_list[3];
				        	this->angle = 180.0;
				        }
					}
				}
				// Move right
				if (this->moveright && !this->attack && !this->delay && !this->parry && !this->dash) {
					if (this->vX < this->velMax) {
						this->vX += this->velSpeed;
					}
					this->moving = true;
					if (this->ControlsPreference == 0) {
				        if (!this->shift) {
				        	this->facing = "right";
				        	this->sprite_dir = this->sprite_dir_list[2];
				        	this->angle = 0.0;
				        }
					}
				}
			}

			// Y Axis movement
			{
				// Move up
				if ((this->moveup && !this->attack && !this->delay && !this->parry && !this->dash)) {
					if (this->vY > -this->velMax) {
						this->vY -= this->velSpeed;
					}
					this->moving = true;
					if (this->ControlsPreference == 0) {
				        if (!this->shift) {
				        	this->facing = "up";
				        	this->sprite_dir = this->sprite_dir_list[1];
				        	this->angle = 270.0;
				        }
					}
				}
				// Move down
				if (this->movedown && !this->attack && !this->delay && !this->parry && !this->dash) {
					if (this->vY < this->velMax) {
						this->vY += this->velSpeed;
					}
					this->moving = true;
					if (this->ControlsPreference == 0) {
				        if (!this->shift) {
				        	this->facing = "down";
				        	this->sprite_dir = this->sprite_dir_list[0];
				        	this->angle = 90.0;
				        }
					}
				}
			}

			// If ControlsPreference is "1" then we will override the direction above ^
			if (this->ControlsPreference == 1) {
				if (mex+camx > this->x) {
		        	this->facing = "right";
				}else {
		        	this->facing = "left";
				}

				// Facing right
				if (this->angle >= 315 || this->angle < 45) {
		        	this->facing = "right";
		        	this->sprite_dir = this->sprite_dir_list[2];
				}

				// Facing down
				else if (this->angle >= 45 && this->angle < 135) {
		        	this->facing = "down";
		        	this->sprite_dir = this->sprite_dir_list[0];
				}

				// Facing left
				else if (this->angle >= 135 && this->angle < 225) {
		        	this->facing = "left";
		        	this->sprite_dir = this->sprite_dir_list[3];
				}

				// Facing up
				else if (this->angle >= 225 && this->angle < 315) {
		        	this->facing = "up";
		        	this->sprite_dir = this->sprite_dir_list[1];
				}
			}

			// Handle mouse and keyboard controls
			else {

				if (!this->shift) {

					// Top right
					if (moveup && moveright) {
						this->angle = 315.0;
					}
					// Bottom right
					else if (movedown && moveright) {
						this->angle = 45.0;
					}

					// Top left
					if (moveup && moveleft) {
						this->angle = 225.0;
					}
					// Bottom left
					else if (movedown && moveleft) {
						this->angle = 135.0;
					}
				}
			}

			// If not dashing
			if (!this->dash) {
				// Max X speed
				if (this->vX < -this->velMax) {
			        vX = vX - vX * 0.05;
				}
				if (this->vX > this->velMax) {
			        vX = vX - vX * 0.05;
				}
				// Max Y speed
				if (this->vY < -this->velMax) {
			        vY = vY - vY * 0.05;
				}
				if (this->vY > this->velMax) {
			        vY = vY - vY * 0.05;
				}
			}
		}
		//----------------------------------- Movement -----------------------------------//
		//--------------------------------------------------------------------------------//
		////////////////////////////////////////////////////////////////////////////////////

		////////////////////////////////////////////////////////////////////////////////////
		//--------------------------------------------------------------------------------//
		//--------------------------- Movement & Collision w/ Tiles ----------------------//

		// Handle movement of player, and handle collision with Tiles
		tl.checkCollisionXY(tile,
				this->x, this->y,
				this->w, this->h,
				this->vX, this->vY,
				this->dash);

		//--------------------------- Movement & Collision w/ Tiles ----------------------//
		//--------------------------------------------------------------------------------//
		////////////////////////////////////////////////////////////////////////////////////


	    ////////////////////////////////////////////////////////////////////////////
	    //------------------------------------------------------------------------//
	    //--------------------------------- Do !Parrying -------------------------//

		// Idle animation if not doing anything
		if (!this->attack && !this->dash && !this->parry) {

			// Idle animation
			if (!this->moving) {
				this->sprite_index = 0;

			// Moving animation
			} else {

				// If not attacking
				//if (!this->attack) {

					///////////////////////////////////////////////////////////////////////////
					//-----------------------------------------------------------------------//
					//----------------------------- Do walkTimer ----------------------------//
					{
						// Walk anim speed
						float walkTimerSpe;
						if (sprint)
							walkTimerSpe = 11;
						else
							walkTimerSpe = 10;

						// Increment animation timer
						this->walkTimer += walkTimerSpe;

						// Increment current animation frame
						if (this->walkTimer > 60)
						{
							// Reset timer
							this->walkTimer = 0;
							// Go to next animation frame
							this->sprite_index++;
						}

						// Reset sprite
						if (this->sprite_index > 3) {
							this->sprite_index = 0;
						}
					}
					//----------------------------- Do walkTimer ----------------------------//
					//-----------------------------------------------------------------------//
					///////////////////////////////////////////////////////////////////////////

					///////////////////////////////////////////////////////////////////////////
					//-----------------------------------------------------------------------//
					//--------------------------- Do walkTimerVFX ---------------------------//
					{
						// WalkSFXSpe
						float walkVFXSpe;
						if (sprint)
							walkVFXSpe = 5;
						else
							walkVFXSpe = 4;

						//	Increase walkTimerVFX
						walkTimerVFX += walkVFXSpe;

						// If walkTimer is at 30 frames
						if (walkTimerVFX > 60)
						{
							walkTimerVFX = 0;

							// Visual and audio effects
							{
								// Spawn particle
								int tempAngel = 0;
								float adjustX = 0;
								if (facing == "left" ) {
									tempAngel = 0;
									adjustX = 8;
								} else if (facing == "right" ) {
									tempAngel = 180;
									adjustX = -8;
								}

								// Spawn size and pos
								int randSize = rand() % 5 + 5;

								float spawnX = getX() + this->w/2 + adjustX;
								float spawnY = getY() + this->h;

								// Spawn particle effect at feet
								p_dummy.spawnParticleAngle(particle, 3, 2,
													spawnX-randSize/2,
													spawnY-randSize/2,
													randSize, randSize,
												   tempAngel, randDouble(0.1, 0.4),
												   0.0, 0, 0,
												   {255, 255, 255, 255}, 1,
												   1, 1,
												   rand() % 100 + 150, rand() % 2 + 5,
												   rand() % 50 + 90, 0,
												   true, randDouble(0.1, 0.7),
												   100, 10);

								// Play sound effect
								Mix_PlayChannel(-1, settings->sStep, 0);
							}
						}
					}
					//--------------------------- Do walkTimerVFX ---------------------------//
					//-----------------------------------------------------------------------//
					///////////////////////////////////////////////////////////////////////////
				//}
			}
		}

		// Animations
		{
			///////////////////////////////////////////////////////////////////////////
			//-----------------------------------------------------------------------//
			//------------------------------- Do Dash -------------------------------//
			if (this->dash) {

				if (dashCounter >= 0 && dashCounter < 2) {
					sprite_index = 4;
				}
				else if (dashCounter >= 2 && dashCounter < 4) {
					sprite_index = 4;
				}
				else if (dashCounter >= 4 && dashCounter < 6) {
					sprite_index = 4;
				}
				else if (dashCounter >= 6 && dashCounter < 8) {
					sprite_index = 4;
				}
				else if (dashCounter >= 8 && dashCounter < 10) {
					sprite_index = 4;
				}

				int rands = rand() % 9 + 2;
				float newX = getX() + w/2;
				float newY = getY() + h;
				p_dummy.spawnParticleAngle(particle, 3, 2,
									newX-rands/2,
									newY-rands/2,
								   rands, rands,
								   0, randDouble(0.1, 0.3),
								   0.0, 0, 0,
								   {255, 255, 255, 255}, 1,
								   1, 1,
								   rand() % 100 + 150, rand() % 2 + 5,
								   rand() % 50 + 90, 0,
								   true, 0.11,
								   rand() % 9 + 2, 1);

				// If dash counter is greater than 0
				if (dashCounter > 0) {

					// Subtract dash counter by 1 every frame
					dashCounter -= 1;
				}
				// If dash counter goes lower than 0
				else {

					// Stop player movement
					StopMovement();

					// Dash on cool down
					dash = false;

					// Start dash cool down timer
					dashCoolCounter = dashCooldown;
				}
			}

			//------------------------------- Do Dash -------------------------------//
			//-----------------------------------------------------------------------//
			///////////////////////////////////////////////////////////////////////////
			///////////////////////////////////////////////////////////////////////////
			//-----------------------------------------------------------------------//
			//----------------------------- Do Attacking ----------------------------//
			// If attacking
			else if (this->attack)
			{
				if (!this->delay)
					{
					// Stop player movement
					this->StopMovement();

					///////////////////////////////////////////////////////////////////////////////////////////
					//---------------------------------------------------------------------------------------//
					//--------------------------------------- Attack 1 --------------------------------------//
					if (this->attackType == 0)
					{
						// Increase attack timer/frames
						this->attackTimer += this->atkAnimSpe;

						// If User presses to attack within X amount of time, continue attack
						if (this->attackTimer >= 60 && this->attackTimer < 90) {
							if (promptContinueAttacking) {
								startCombo = true;
							} else {
								startCombo = false;
							}
						}

						// Before slash
						if (this->attackTimer >= 0 && this->attackTimer < 60) {
							sprite_index = 5;
						}

						// Next-slash (Attack 1)
						else if (this->attackTimer >= 60 && this->attackTimer < 90) {
							sprite_index = sprite_index_attack_1;

							// attackTimer @ 60
							if (this->attackTimer == 60)
							{
								// Move forward in direction
								MoveForward();

								// Spawn atk Obj
								this->SpawnAttackObject(obj, object);
							}
						}

						// End Attack
						else {

							// Continue to combo 2
							if (startCombo) {

								// Go to attack 2
								{
									// Stop prompt for continuing combo
									promptContinueAttacking = false;
									startCombo = false;

									// Next attack
									this->attackTimer = 0;
									this->attackType = 1;
									this->atkAnimSpe = 5;
								}
							}

							// Stop attacking
							else {

								// Reset attack-type
								attackType = -1;
								attackTimer = 0;
								attack = false;
								initialshot = false;
							}
						}
					}
					///////////////////////////////////////////////////////////////////////////////////////////
					//---------------------------------------------------------------------------------------//
					//--------------------------------------- Attack 2 --------------------------------------//

					// If attack-type: Slash Attack
					if (this->attackType == 1)
					{
						// Increase attack timer/frames
						this->attackTimer += this->atkAnimSpe;

						// If User presses to attack within X amount of time, continue attack
						if (this->attackTimer >= 60 && this->attackTimer < 90) {
							if (promptContinueAttacking) {
								startCombo = true;
							} else {
								startCombo = false;
							}
						}

						// Before slash
						if (this->attackTimer >= 0 && this->attackTimer < 60) {
							sprite_index = 5;
						}

						// (Attack 2)
						else if (this->attackTimer >= 60 && this->attackTimer < 90) {
							sprite_index = sprite_index_attack_2;

							// attackTimer @ 90
							if (this->attackTimer == 60)
							{
								// Move forward in direction
								MoveForward();

								// Spawn atk Obj
								this->SpawnAttackObject(obj, object);
							}
						}

						// End Attack
						else {

							// Continue to combo 3
							if (startCombo) {

								// Go to attack 3
								{
									// Stop prompt for continuing combo
									promptContinueAttacking = false;
									startCombo = false;

									// Next attack
									this->attackTimer = 0;
									this->attackType = 2;
									this->atkAnimSpe = 3;
								}
							}

							// Stop attacking
							else {

								// Reset attack-type
								attackType = -1;
								attackTimer = 0;
								attack = false;
								initialshot = false;
							}
						}
					}
					///////////////////////////////////////////////////////////////////////////////////////////
					//---------------------------------------------------------------------------------------//
					//--------------------------------------- Attack 3 --------------------------------------//

					// If attack-type: Slash Attack
					if (this->attackType == 2)
					{
						// Increase attack timer/frames
						this->attackTimer += this->atkAnimSpe;

						// Before slash
						if (this->attackTimer >= 0 && this->attackTimer < 60) {
							sprite_index = 5;
						}

						// (Attack 3)
						else if (this->attackTimer >= 60 && this->attackTimer < 90) {
							sprite_index = sprite_index_attack_3;

							// attackTimer @ 120
							if (this->attackTimer == 60)
							{
								// Move forward in direction
								MoveForward();

								// Spawn atk Obj
								this->SpawnAttackObject(obj, object);
							}
						}

						// End Attack
						else {

							// Stop combo
							startCombo = false;
							promptContinueAttacking = false;

							// Reset attack-type
							attackType = -1;
							attackTimer = 0;
							attack = false;
							initialshot = false;
						}
					}
				}
			}

			// Start delay after attacking
			if (this->delay)
			{
				// Start delay (the faster the attakc speed, the sooner the delay ends
				this->delayTimer += this->atkSpeed;

				// Delay over
				if (this->delayTimer > 60)
				{
					// Stop delay
					this->delayTimer = 0;
					this->delay = false;
				}
			}
			//----------------------------- Do Attacking ----------------------------//
			//-----------------------------------------------------------------------//
			///////////////////////////////////////////////////////////////////////////

			///////////////////////////////////////////////////////////////////////////
			//-----------------------------------------------------------------------//
			//------------------------------ Do Parrying ----------------------------//
			// Parrying animation
			if (this->parry)
			{
				// Stop movement
				StopMovement();

				// Determine direction
				this->sprite_index = 3;

				// Start Parrying timer
				this->parryTimer++;

				// Parry for 15 frames
				if (this->parryTimer > this->parryLength){
					this->parryTimer = 0;
					this->parry = false;
					//this->StopParrying();
				}
			// Parry cool-down
			} else if (!this->parry) {
				if (this->parryCDTimer > 0) {
					this->parryCDTimer -= 1;
				}
			}
			//------------------------------ Do Parrying ----------------------------//
			//-----------------------------------------------------------------------//
			///////////////////////////////////////////////////////////////////////////

			///////////////////////////////////////////////////////////////////////////
			//-----------------------------------------------------------------------//
			//----------------------------- Do NOT Dash -----------------------------//

			// If dash on cooldown
			if (!this->dash) {

				// Start cooldown countdown
				if (this->dashCoolCounter > 0) {
					this->dashCoolCounter -= 1;
				}
			}

			//----------------------------- Do NOT Dash -----------------------------//
			//-----------------------------------------------------------------------//
			///////////////////////////////////////////////////////////////////////////


		} // end Animations



		//------------------------------ Do Parrying ----------------------------//
		//-----------------------------------------------------------------------//
		///////////////////////////////////////////////////////////////////////////

		///////////////////////////////////////////////////////////////////////////
		//-----------------------------------------------------------------------//
		//------------------------------- Do Stunned ----------------------------//
	    // Stunned
	    /*if (this->stunned) {
	        if (this->facing == "left") {
	        	this->sprite_index = 3;
	        }
	        if (facing == "right") {
	        	this->sprite_index = 3;
	        }
	        this->stunTimer += 1;
	        if (this->stunTimer > 90) {
	        	this->stunTimer = 0;
	            this->stunned = false;
	        }
	    }*/
		//------------------------------- Do Stunned ----------------------------//
		//-----------------------------------------------------------------------//
		///////////////////////////////////////////////////////////////////////////

		////////////////////////////////////////////////////////////////////////////////////
		//--------------------------------------------------------------------------------//
		//-------------------------------- Stop Movement ---------------------------------//

		// Player not moving X
		if (!moveleft && !moveright && !dash) {
	        vX = vX - vX * 0.3;
		}

		// Player not moving Y
		if (!moveup && !movedown && !dash) {
	        vY = vY - vY * 0.3;
		}

		// Player not moving
		if (!moveup && !movedown && !moveleft && !moveright && !dash) {
			moving = false;

			// Stop sprinting
			sprint = false;
		}

		//-------------------------------- Stop Movement ---------------------------------//
		//--------------------------------------------------------------------------------//
		////////////////////////////////////////////////////////////////////////////////////

		// Player shoot
		this->fire(particle, p_dummy, settings->sCast, mex+camx, mey+camy);

		// Player shield
		if (this->invurnerable){
			this->invurnerableT-=1;
			this->invurnerableTick += 20;
			if (this->invurnerableTick>60){
				this->invurnerableTick = 0;
				this->invurnerableFrame += 1;
			}
			if (this->invurnerableFrame>7){
				this->invurnerableFrame = 1;
				this->invurnerableTick = 0;
			}
			if (this->invurnerableT<=0){
				this->invurnerableT= 300;
				this->invurnerable = false;
			}
		}

		// Check high score MAX
		if (this->score > 999999999){
			this->score = 999999999;
		}

		// Player death, lose hearts
		if (this->health <=0)
		{
			// Spawn blood VFX
			p_dummy.spawnBloodVFX(particle, this->x, this->y, this->w, this->h, {255,0,0});

			// Take away hearts
			this->hearts-= 1;

			// Reset Player
			this->RespawnPlayer();

			// Player ran out of hearts, que Death Screen
			if (this->hearts <= 0 ){

				// Bring player back to life
				this->alive 			= false;

				// Show deathscreen
				this->deathScreen 	= true;

				// Save high score
				SaveHighScore(LevelWeLoaded);

			}
		}

		// Update Player score and Wave
		if (this->score > this->highscore){
			this->highscore = this->score;
		}

		// Player level boundaries
		if( this->x < map.x ){
			this->x = map.x;
		}
		if( this->y < map.y ){
			this->vY = 0.0;
			this->y = map.y;
		}
		if( this->x+this->realw > map.w ){
			this->x = map.w-this->realw;
		}
		if( this->y+realh > map.h ){
			this->y = map.h-this->realh;
		}

		// Regain mana if not attacking
		if (!this->attack) {
			// Every frame player regenerates mana
			if (this->mana < this->maxMana)
			{
				// Start timer
				this->manaRegenTimer += 1;

				// After 1 second or 60 frames
				if (this->manaRegenTimer > 60) {

					// Reset timer
					this->manaRegenTimer = 0;

					// Increase mana based on mana regen speed
					this->mana += this->manaRegenSpeed;
				}
			}
		}

		// Check if mana spilling over max
		if (this->mana > this->maxMana)
		{
			this->mana = this->maxMana;
		}

		// Every frame reduce damage multiplier by 2%
		if (this->damageMultipler > 1.0025) {
			this->damageMultipler -= 0.0025;
		}


	}else{
		// High-Score moving
		this->travel += 0.05 * this->dir;
		if (this->travel > 10){
			this->dir = -1;
		}
		if (this->travel < -10){
			this->dir = 1;
		}

		// Continue YES or NO Screen
		if (this->deathScreen)
		{
			// Set button position
			continueButton[0] = {0 + screenWidth/2 -96/2-100, screenHeight/2-gText.getHeight()/2, 96, 33};
			continueButton[1] = {0 + screenWidth/2 -96/2+100, screenHeight/2-gText.getHeight()/2, 96, 33};
			continueButton[2] = {0 + screenWidth/2 -256/2 , screenHeight-gText.getHeight()-72, 256, 39};

			// High Score display position
			position  = 0+travel;
			position2 = continueButton[1].x+continueButton[1].w-gText.getWidth()+travel;

			// Left click
			if (leftclick)
			{
				// Option: Yes, reset everything
				if (checkCollision(mex, mey, 1, 1, continueButton[0].x, continueButton[0].y, continueButton[0].w, continueButton[0].h))
				{
					// Restores players base stats and health
					leftclick			= false;

					// Reset Player
					ResetLivesAndPlayer();
					RestartLevel = true;

					// Load last checkpoint
					this->x = this->checkpointX;
					this->y = this->checkPointY;
				}

				// Option: No, go to Main Menu
				if (checkCollision(mex, mey, 1, 1, continueButton[1].x, continueButton[1].y, continueButton[1].w, continueButton[1].h))
				{
					score = 0;
					leftclick  = false;
					deathScreen = false;
				}

				// Option: Reset high scores
				if (checkCollision(mex, mey, 1, 1, continueButton[2].x, continueButton[2].y, continueButton[2].w, continueButton[2].h))
				{
					// Reset high scores
					ResetHighScore(LevelWeLoaded);
				}
			}
		// Menu Screen
		}else{
			SDL_ShowCursor(true);
			// Set button position
			continueButton[0] = {screenWidth/2-96/2, screenHeight/2-gText.getHeight()/2, 96, 33};
			continueButton[2] = {screenWidth/2-256/2 , screenHeight-gText.getHeight()-72, 256, 39};

			// High Score display position
			position 	= continueButton[0].w/2-gText.getWidth()/2-100+travel;
			position2 = continueButton[0].x+continueButton[0].w/2-gText.getWidth()/2+100+travel;

			// Left click
			if (leftclick)
			{
				// Option: Play
				if (checkCollision(mex, mey, 1, 1, continueButton[0].x, continueButton[0].y, continueButton[0].w, continueButton[0].h))
				{
					leftclick			= false;

					// Reset Player
					ResetLivesAndPlayer();
					RestartLevel = true;
				}

				// Option: Reset high scores
				if (checkCollision(mex, mey, 1, 1, continueButton[2].x, continueButton[2].y, continueButton[2].w, continueButton[2].h))
				{
					// Reset high scores
					ResetHighScore(LevelWeLoaded);
				}
			}
		}
	}
}

// Render Player
void Players::Render(int mx, int my, int camx, int camy, LWindow gWindow, SDL_Renderer* gRenderer,
					 SDL_Color color, int &PARTICLES) {
	gText.setAlpha(255);
	// If alive
	if (alive){

		// render player
		{
			// Render shadow
			{
				/*int shadowW = 24;
				int shadowH = 10;
				gPlayerShadow.setAlpha(110);
				gPlayerShadow.render(gRenderer, x+w/2-realw/2-shadowW/2-camx,
												y+h+yOffset-shadowH/2-camy,
												shadowW, shadowH, NULL, 0.0, NULL);*/
			}

			// Render player
			//gPlayer.render(gRenderer, x+w/2 - realw/2-camx, y + yOffset-camy, realw, realh, &rPlayer[sprite_index + this->sprite_dir]);
			gPlayer.render(gRenderer,
					x+realw/2 - realw/2-camx,
					y+realh/2 - realh/2 + yOffset -camy,
					realw, realh,
					&this->vPlayer[this->sprite_index + this->sprite_dir]);

			// Render user weapon
			/*int xOffset;
			if (this->facing == "right")
				xOffset = -1;
			else
				xOffset = -12;
			gItems.render(gRenderer,
					getCenterX() + xOffset - camx,
					getCenterY() - 16 - camy,
					rItems->w, rItems->h,
					&rItems[itemIndex],
					angle, NULL, flipW);*/

		}

		// Render Shield
		if (invurnerable){
			gShield.render(gRenderer, x+realw/2 - realw/2 - camx, y+realh/2 - realh/2 - camy, realw, realh, &rShield[invurnerableFrame], 180);
		}
	}
}

void Players::RenderUI(SDL_Renderer *gRenderer, int camX, int camY, int CurrentLevel)
{
	RenderUIBars(gRenderer, camX, camY);

	RenderUIText(gRenderer, camX, camY, CurrentLevel);

	RenderUIInventory(gRenderer, camX, camY, CurrentLevel);

	RenderDeathScreen(gRenderer, camX, camY);

}

void Players::RenderUIBars(SDL_Renderer *gRenderer, int camX, int camY)
{
	// Health
	{
		const float yOffsetBar = 7;
		const float barHeight = 12;
		const float barWidth = this->w*1.75;
		float uiX = getX() + this->w/2 - barWidth/2;
		float uiY = getY() - this->h - barHeight - yOffsetBar;

		// Health Decay bar on Mobes
		{
			// Health Decay bar, bg
			RenderFillRect(gRenderer, uiX-camX, uiY-camY, (barWidth*this->healthMax)/this->healthMax, barHeight, {0, 0, 0} );

			// Render Decay health
			RenderFillRect(gRenderer, uiX-camX, uiY-camY, (barWidth*this->healthDecay)/this->healthMax, barHeight, {30, 60, 30} );
		}

		// Health bar on Mobes
		{
			// Render health
			RenderFillRect(gRenderer, uiX-camX, uiY-camY, (barWidth*this->health)/this->healthMax, barHeight, {30, 200, 30} );
		}


	}

	// Bars origin variables
	int barWidth = 55;
	int barMarginW = screenWidth * 0.004;
	int uiX = screenWidth * 0.02;
	//int uiX = screenWidth * 1.00 - barWidth - barMarginW;

	// Mana
	{
		// Mana bar
		int barSpacing = 5;
		int uiY = screenHeight * 0.02 + 8 * 1;
		//int uiY = screenHeight * 0.02 - barSpacing - 8 * 1;
		int tempN = this->dashCooldown - this->dashCoolCounter;

		//gText.loadFromRenderedText(gRenderer, "Dash CD ", {255,255,255}, gFont12);
		//gText.render(gRenderer,  uiX-gText.getWidth()-2, uiY, gText.getWidth(), gText.getHeight());

		// Render Mana, bg
		SDL_Rect tempRect = {uiX, uiY, ((barWidth*this->maxMana)/this->maxMana), 8};
		SDL_SetRenderDrawColor(gRenderer, 60, 60, 60, 255);
		SDL_RenderFillRect(gRenderer, &tempRect);

		// Render Mana
		tempRect = {uiX, uiY, ((barWidth*this->mana)/this->maxMana), 8};
		SDL_SetRenderDrawColor(gRenderer, 0, 90, 200, 255);
		SDL_RenderFillRect(gRenderer, &tempRect);

		// Render Mana, border
		tempRect = {uiX, uiY, ((barWidth*this->maxMana)/this->maxMana), 8};
		SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 255);
		SDL_RenderDrawRect(gRenderer, &tempRect);
	}

	// Dash counter
	{
		// Dash bar
		int barSpacing = 5;
		int uiY = screenHeight * 0.02 + 8 * 2;
		//int uiY = screenHeight * 0.02 - barSpacing - 8 * 2;
		int tempN = this->dashCooldown - this->dashCoolCounter;

		//gText.loadFromRenderedText(gRenderer, "Dash CD ", {255,255,255}, gFont12);
		//gText.render(gRenderer,  uiX-gText.getWidth()-2, uiY, gText.getWidth(), gText.getHeight());

		// Render dash, bg
		SDL_Rect tempRect = {uiX, uiY, ((barWidth*this->maxMana)/this->maxMana), 8};
		SDL_SetRenderDrawColor(gRenderer, 60, 60, 60, 255);
		SDL_RenderFillRect(gRenderer, &tempRect);

		// Render dash
		tempRect = {uiX, uiY, (((barWidth*tempN)/this->dashCooldown)), 8};
		SDL_SetRenderDrawColor(gRenderer, 255, 255, 0, 255);
		SDL_RenderFillRect(gRenderer, &tempRect);

		// Render dash, border
		tempRect = {uiX, uiY, ((barWidth*this->maxMana)/this->maxMana), 8};
		SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 255);
		SDL_RenderDrawRect(gRenderer, &tempRect);
	}

	// Parry CD
	{
		// Dash bar
		int barSpacing = 5;
		int uiY = screenHeight * 0.02 + 8 * 3;
		//int uiY = screenHeight * 0.02 - barSpacing - 8 * 3;
		int tempN = this->parryCDTimer - this->parryCDMax;

		//gText.loadFromRenderedText(gRenderer, "Dash CD ", {255,255,255}, gFont12);
		//gText.render(gRenderer,  uiX-gText.getWidth()-2, uiY, gText.getWidth(), gText.getHeight());

		// Render dash, bg
		SDL_Rect tempRect = {uiX, uiY, ((barWidth*this->parryCDMax)/this->parryCDMax), 8};
		SDL_SetRenderDrawColor(gRenderer, 60, 60, 60, 255);
		SDL_RenderFillRect(gRenderer, &tempRect);

		// Render dash
		tempRect = {uiX, uiY, (((barWidth*(-tempN))/this->parryCDMax)), 8};
		SDL_SetRenderDrawColor(gRenderer, 0, 220, 220, 255);
		SDL_RenderFillRect(gRenderer, &tempRect);

		// Render dash, border
		tempRect = {uiX, uiY, ((barWidth*this->parryCDMax)/this->parryCDMax), 8};
		SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 255);
		SDL_RenderDrawRect(gRenderer, &tempRect);

		// Render Parry, border
		tempRect = {uiX, uiY, (barWidth*this->parryCDMax)/this->parryCDMax, 8};
		SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);
		SDL_RenderDrawRect(gRenderer, &tempRect);
	}

	// Hearts
	{
		/*// Render number of hearts left
		int marginW = 16;
		int marginH = 16;
		int size = 32;
		for (int i=0; i<hearts; i++){
			//int tempX = screenWidth - ((1+i)*32) - marginW;
			int tempX = marginW + i*32;
			int tempY = marginH;
			gItems.render(gRenderer, tempX, tempY, size, size,
									  &rItems[24], 0, NULL);
		}*/
	}
}

void Players::RenderUIText(SDL_Renderer *gRenderer, int camX, int camY, int CurrentLevel)
{
	// Used by all 3 below
	int marginW = 32;
	int marginH = 32*2 + 4;
	int tempX = marginW;
	//int tempX = screenWidth - marginW;
	int tempY;
	std::stringstream tempsi;

	// Text UI
	{
		// Top right of screen
		{
			// Highscore text
			tempsi.str( std::string() );
			tempsi << "Highscore: " << this->highscore;
			gText.loadFromRenderedText(gRenderer, tempsi.str().c_str(), {244, 144, 20}, gFont12);
			gText.render(gRenderer, screenWidth * 1.00 - gText.getWidth(), 0, gText.getWidth(), gText.getHeight());

			tempsi.str( std::string() );
			tempsi << "Score: " << this->score;
			gText.loadFromRenderedText(gRenderer, tempsi.str().c_str(), {255, 255, 255}, gFont12);
			gText.render(gRenderer, screenWidth * 1.00 - gText.getWidth(), 0 + 12*1, gText.getWidth(), gText.getHeight());
		}

		// Bottom left of screen
		{
			tempsi.str( std::string() );
			tempsi << "Stats";
			gText.loadFromRenderedText(gRenderer, tempsi.str().c_str(), {244, 144, 44}, gFont20);
			gText.render(gRenderer, screenWidth * 0.02, screenHeight - gText.getHeight() * 5, gText.getWidth(), gText.getHeight());

			tempsi.str( std::string() );
			tempsi << "Level: " << CurrentLevel;
			gText.loadFromRenderedText(gRenderer, tempsi.str().c_str(), {255, 255, 255}, gFont12);
			gText.render(gRenderer, screenWidth * 0.02, screenHeight - gText.getHeight() * 6, gText.getWidth(), gText.getHeight());

			tempsi.str( std::string() );
			tempsi << "Damage +" << this->damage;
			gText.loadFromRenderedText(gRenderer, tempsi.str().c_str(), {255, 255, 255}, gFont12);
			gText.render(gRenderer, screenWidth * 0.02, screenHeight - gText.getHeight() * 5, gText.getWidth(), gText.getHeight());

			tempsi.str( std::string() );
			tempsi << "Cast Damage +" << this->castDamage;
			gText.loadFromRenderedText(gRenderer, tempsi.str().c_str(), {255, 255, 255}, gFont12);
			gText.render(gRenderer, screenWidth * 0.02, screenHeight - gText.getHeight() * 4, gText.getWidth(), gText.getHeight());
		}
	}
}

void Players::RenderUIInventory(SDL_Renderer *gRenderer, int camX, int camY, int CurrentLevel)
{
	// Bottom right of screen: ammo
	std::stringstream tempsi;
	float marginH = 16;

	// Pistol ammo icon
	gItems.setColor(255,255,255);
	gItems.render(gRenderer,
			screenWidth * 1.00 - 100 - 32*2,
			screenHeight * 1.00 - 32 - marginH,
			32, 32,
			&rItems[10]);

	// Rifle ammo icon
	gItems.render(gRenderer,
			screenWidth * 1.00 - 100,
			screenHeight * 1.00 - 32 - marginH,
			32, 32,
			&rItems[11]);

	// Pistol ammo text
	tempsi.str( std::string() );
	tempsi << this->ammoPistol;
	gText.loadFromRenderedText(gRenderer, tempsi.str().c_str(), {255, 255, 255}, gFont12);
	gText.render(gRenderer, screenWidth * 1.00 - 100 - 32*1,
			screenHeight * 1.00 - 32 - marginH,
			gText.getWidth(), gText.getHeight());

	// Shotgun ammo text
	tempsi.str( std::string() );
	tempsi << this->ammoShotgun;
	gText.loadFromRenderedText(gRenderer, tempsi.str().c_str(), {255, 255, 255}, gFont12);
	gText.render(gRenderer, screenWidth * 1.00 - 100 + 32,
			screenHeight * 1.00 - 32 - marginH,
			gText.getWidth(), gText.getHeight());

	// Render current weapon & amount of bullets in magazine for weapon
	{
		// Render current weapon
		gItems.render(gRenderer,
				screenWidth * 1.00 - 100 - 32*3,
				screenHeight * 1.00 - 32*2 - marginH,
				32, 32,
				&rItems[itemIndex]);

		// Determine magazine size to display
		int tempMagSize = -1;
		if (itemIndex == 0) {
			tempMagSize = magazinePistol;
		} else if (itemIndex == 1) {
			tempMagSize = magazineRifle;
		} else if (itemIndex == 2) {
			tempMagSize = magazineShotgun;
		}

		// Render text
		tempsi.str( std::string() );
		tempsi << tempMagSize;
		gText.loadFromRenderedText(gRenderer, tempsi.str().c_str(), {255, 255, 255}, gFont12);
		gText.render(gRenderer, screenWidth * 1.00 - 100 - 32*2,
				screenHeight * 1.00 - 32*2 - marginH,
				gText.getWidth(), gText.getHeight());
	}
}

void Players::RenderDeathScreen(SDL_Renderer *gRenderer, int camX, int camY)
{
	// Show death screen if not alive
	if (!this->alive){
		// Continue YES or NO Screen
		if (deathScreen)
		{
			// Render Text
			gText.loadFromRenderedText(gRenderer, "You have died. Continue?", {255,255,255}, gFont12);
			gText.render(gRenderer, screenWidth/2-gText.getWidth()/2, screenHeight/2-gText.getHeight()/2-50, gText.getWidth(), gText.getHeight());

			// Render buttons: Yes
			SDL_SetRenderDrawColor(gRenderer, 0, 255, 0, 255);
			SDL_RenderDrawRect(gRenderer, &continueButton[0]);

			// Render buttons: No
			SDL_SetRenderDrawColor(gRenderer, 255, 0, 0, 255);
			SDL_RenderDrawRect(gRenderer, &continueButton[1]);

			// Render buttons: ResetHighScore
			SDL_SetRenderDrawColor(gRenderer, 0, 255, 255, 255);
			SDL_RenderDrawRect(gRenderer, &continueButton[2]);

			// Render button texts: Yes or No
			gText.loadFromRenderedText(gRenderer, "Yes", {255,255,255}, gFont12);
			gText.render(gRenderer,  continueButton[0].x+continueButton[0].w/2-gText.getWidth()/2,
									 continueButton[0].y+continueButton[0].h/2-gText.getHeight()/2,
									 gText.getWidth(), gText.getHeight());

			gText.loadFromRenderedText(gRenderer, "No", {255,255,255}, gFont12);
			gText.render(gRenderer,  continueButton[1].x+continueButton[1].w/2-gText.getWidth()/2,
									 continueButton[1].y+continueButton[1].h/2-gText.getHeight()/2,
									 gText.getWidth(), gText.getHeight());

			// Render Text
			gText.loadFromRenderedText(gRenderer, "Reset High Scores", {255,255,255}, gFont12);
			gText.render(gRenderer, continueButton[2].x+continueButton[2].w/2-gText.getWidth()/2,
									 continueButton[2].y+continueButton[2].h/2-gText.getHeight()/2,
									 gText.getWidth(), gText.getHeight());
		// Player Menu screen
		}else{

			// Render buttons: Play
			SDL_SetRenderDrawColor(gRenderer, 0, 255, 0, 255);
			SDL_RenderDrawRect(gRenderer, &continueButton[0]);

			// Render buttons: ResetHighScore
			SDL_SetRenderDrawColor(gRenderer, 0, 255, 255, 255);
			SDL_RenderDrawRect(gRenderer, &continueButton[2]);

			// Render Text
			gText.loadFromRenderedText(gRenderer, "PLAY", {255,255,255}, gFont12);
			gText.render(gRenderer, continueButton[0].x+continueButton[0].w/2-gText.getWidth()/2,
									 continueButton[0].y+continueButton[0].h/2-gText.getHeight()/2,
									 gText.getWidth(), gText.getHeight());

			// Render Text
			//gText.loadFromRenderedText(gRenderer, "Reset High Scores", color, gFont12);
			//gText.render(gRenderer, continueButton[2].x+continueButton[2].w/2-gText.getWidth()/2,
			//						 continueButton[2].y+continueButton[2].h/2-gText.getHeight()/2,
			//						 gText.getWidth(), gText.getHeight());
		}
	}
}

void Players::RenderDebug(SDL_Renderer *gRenderer, int camX, int camY)
{
	////////////////////////////////////////////////////////////
	//--------------------- Render boxes ---------------------//
	//--------------------------------------------------------//

	// Original box, untouched
	SDL_Rect tempRect = {x+w-camX, y+h-camY, w, h};
	SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);
	SDL_RenderDrawRect(gRenderer, &tempRect);

	// Original box, untouched
	tempRect = {x-camX, y-camY, realw, realh};
	SDL_SetRenderDrawColor(gRenderer, 0, 255, 0, 255);
	SDL_RenderDrawRect(gRenderer, &tempRect);

	if (this->attack) {
		tempRect = {x-camX, y-camY, realw, realh};
		SDL_SetRenderDrawColor(gRenderer, 255, 0, 0, 255);
		SDL_RenderDrawRect(gRenderer, &tempRect);
	}

	////////////////////////////////////////////////////////////
	//---------------------- Render text ---------------------//
	//--------------------------------------------------------//

	std::stringstream tempsi;
	tempsi << "itemIndex: " << itemIndex;
	gText.loadFromRenderedText(gRenderer, tempsi.str().c_str(), {255,255,255}, gFont12);
	gText.render(gRenderer, 0, 0, gText.getWidth(), gText.getHeight());

	/*tempsi.str( std::string() );
	tempsi << "Health: " << health;
	gText.loadFromRenderedText(tempsi.str().c_str(), {0,255,0}, gFont12, gRenderer);
	gText.render(gRenderer, screenWidth-gText.getWidth()-15, 58, gText.getWidth(), gText.getHeight());*/

	// Check what keys are pressed
	/*for (int i=0; i<25; i++){
		std::std::stringstream tempss;
		tempss << "i: " << i << " -        " << SDL_JoystickGetButton(joy, i);
		gText.loadFromRenderedText(tempss.str().c_str(), {255, 255, 255, 255}, gFont, gRenderer);
		gText.render(gRenderer, 5, 5+i*15, gText.getWidth(), gText.getHeight());
	}

	std::std::stringstream tempss;
	tempss.str(std::string());
	tempss << "Axis: " << 0 << " -        " << SDL_JoystickGetAxis(joy, 0);
	gText.loadFromRenderedText(tempss.str().c_str(), {255, 255, 255, 255}, gFont, gRenderer);
	gText.render(gRenderer, 60, 5, gText.getWidth(), gText.getHeight());

	tempss.str(std::string());
	tempss << "Axis: " << 1 << " -        " << SDL_JoystickGetAxis(joy, 1);
	gText.loadFromRenderedText(tempss.str().c_str(), {255, 255, 255, 255}, gFont, gRenderer);
	gText.render(gRenderer, 60, 20, gText.getWidth(), gText.getHeight());

	tempss.str(std::string());
	tempss << "Axis: " << 2 << " -        " << SDL_JoystickGetAxis(joy, 2);
	gText.loadFromRenderedText(tempss.str().c_str(), {255, 255, 255, 255}, gFont, gRenderer);
	gText.render(gRenderer, 60, 35, gText.getWidth(), gText.getHeight());

	tempss.str(std::string());
	tempss << "Axis: " << 3 << " -        " << SDL_JoystickGetAxis(joy, 3);
	gText.loadFromRenderedText(tempss.str().c_str(), {255, 255, 255, 255}, gFont, gRenderer);
	gText.render(gRenderer, 60, 50, gText.getWidth(), gText.getHeight());

	tempss.str(std::string());
	tempss << "Axis: " << 4 << " -        " << SDL_JoystickGetAxis(joy, 4);
	gText.loadFromRenderedText(tempss.str().c_str(), {255, 255, 255, 255}, gFont, gRenderer);
	gText.render(gRenderer, 60, 65, gText.getWidth(), gText.getHeight());

	tempss.str(std::string());
	tempss << "Axis: " << 5 << " -        " << SDL_JoystickGetAxis(joy, 5);
	gText.loadFromRenderedText(tempss.str().c_str(), {255, 255, 255, 255}, gFont, gRenderer);
	gText.render(gRenderer, 60, 80, gText.getWidth(), gText.getHeight());*/
}



// Key Pressed
void Players::OnKeyDown(SDL_Keycode sym )
{
	switch (sym)
	{
	case SDLK_a:
		this->moveleft = true;
		break;
	case SDLK_d:
		this->moveright = true;
		break;
	case SDLK_w:
		this->moveup = true;
		break;
	case SDLK_s:
		this->movedown = true;
		break;
	case SDLK_e:
		//this->pressedEquipKey = true;
		break;
	case SDLK_h:
		//debug = ( !debug );
		break;
	case SDLK_y:
		this->camlocked = ( !this->camlocked );
		break;
	case SDLK_m:						// Dodge
		// Future code to dodge
	case SDLK_j:						// Attack (spell attack? This is not finalized.)

		break;
	case SDLK_LSHIFT:
		this->shift = true;
		break;
	case SDLK_n:						// Attack

		// Attack
		SlashAttack();
		break;
	case SDLK_c:						// Parry

		// Activate Parry
		ActivateParry();
		break;
	case SDLK_SPACE: 					// Dash

		// Activate Dash
		ActivateDash();
		break;
	case SDLK_r: 					// Reload

		// Pistol equipped
		if (itemIndex == 0) {
			// If magazine in pistol is less than 12
			if (magazinePistol < 12 && ammoPistol > 0) {
				if (!reload) {
					equippedWeaponReloadSpeed = pistolReloadSpe;
					reload = true;
				}
			}
		}

		// Rifle equipped
		if (itemIndex == 1) {
			// If magazine in Rifle is less than 30
			if (magazineRifle < 30 && ammoPistol > 0) {
				if (!reload) {
					equippedWeaponReloadSpeed = rifleReloadSpe;
					reload = true;
				}
			}
		}

		// Shotgun equipped
		if (itemIndex == 2) {
			// If magazine in Shotgun is less than 5
			if (magazineShotgun < 5 && ammoShotgun > 0) {
				if (!reload) {
					equippedWeaponReloadSpeed = shotgunReloadSpe;
					reload = true;
				}
			}
		}
		break;
	case SDLK_1:
		itemIndex = 0;
		break;
	case SDLK_2:
		itemIndex = 1;
		break;
	case SDLK_3:
		itemIndex = 2;
		break;
	case SDLK_LCTRL:
		sprint = true;
		break;
	}
}

// Key Released
void Players::OnKeyUp(SDL_Keycode sym )
{
	switch (sym){
	case SDLK_a:
		this->moveleft = false;
		break;
	case SDLK_d:
		this->moveright = false;
		break;
	case SDLK_w:
		this->moveup = false;
		break;
	case SDLK_s:
		this->movedown = false;
		break;
	case SDLK_e:
		this->pressedEquipKey = true;
		break;
	case SDLK_j:

		break;
	case SDLK_LSHIFT:
		this->shift 		= false;
		break;
	case SDLK_SPACE:
    	//
		break;
	}
}

// Player Mouse Pressed
void Players::mouseClickState(SDL_Event &e){
	if (e.type == SDL_MOUSEBUTTONDOWN) {
		if (e.button.button == SDL_BUTTON_LEFT) {
			this->leftclick = true;

			// If using knife
			if (this->itemIndex == -1)
			{
				// Attack
				SlashAttack();
			}

			// If using pistol
			else if (this->itemIndex >=0 && this->itemIndex <=2)
			{
				// Shoot bullet
				if (!this->reload) {
					this->initialshot = true;
				}
			}
		}
		if (e.button.button == SDL_BUTTON_RIGHT) {
			this->rightclick = true;

			// Attack
			SlashAttack();
		}
	}else if (e.type == SDL_MOUSEBUTTONUP) {
		if (e.button.button == SDL_BUTTON_LEFT) {
			this->leftclick = false;

			// Shoot bullet
			this->initialshot = false;
		}
		if (e.button.button == SDL_BUTTON_RIGHT) {
			this->rightclick = false;
		}
	}
}

void Players::SlashAttack() {

	// If attacking on certain attack, prompt to continue to a combo
	if (this->attackType == 0 || this->attackType == 1)
	{
		// For combos
		promptContinueAttacking = true;
	}

	if (!this->delay) {
		if (!this->attack && !this->stunned && !this->parry) {

			// If currently dashing
			if (this->dash)
			{
				// Stop dashing
				StopDashing();
			}

			// Set attack parameters
			this->sprite_index = 5;
			this->clash = false;
			this->attack = true;

			// Do this attack based on which attack we're currently on
			if (this->attackType == -1)
			{
				// Attack 1
				this->attackType = 0;

				// Animation Atk Speed
				this->atkAnimSpe = 3;
			}
		}
	}
}

void Players::ActivateParry() {
    if (!this->attack && !this->dash && !this->parry && this->parryCDTimer <= 0 && !this->stunned)
    {
		// Stop attacking
		//if (this->attack) {
		//	StopSlashAttack();
		//}

		// Stop dashing
		//if (this->dash) {
		//	StopDashing();
		//}

    	// enable parrying
    	this->parry = true;

    	// Have parry on CD
    	this->parryCDTimer = this->parryCDMax;

    	// Play sound effect
    	Mix_PlayChannel(-1, settings->sParry, 0);
    }
}

void Players::ActivateDash() {
	// If Dash is not on cool down
	if (this->dashCoolCounter <= 0 && this->dashCounter <=0 && !attack) {

		// Make sure we are not parrying before dashing
		// Because we dont want to stop the animation of parrying
		// if we dash.
		if (!this->parry) {

			// Stop attacking
			if (this->attack) {
				StopSlashAttack();
			}

			// Depending on which way the player is moving,
			if (this->moveleft) {
				this->vX -= this->dashSpeed;
			}else if (moveright) {
				this->vX += this->dashSpeed;
			}
			if (this->moveup) {
				this->vY -= this->dashSpeed;
			}else if (movedown) {
				this->vY += this->dashSpeed;
			}
			this->dash = true;
			this->dashCounter = this->dashLength;

			// Play dash sound effect
			Mix_PlayChannel(-1, settings->sDash, 0);
		}
	}
}

void Players::MoveForward() {
	// Move a little bit when you attack
	if (facing == "up") {
		vY -= 0.73;
	}

	if (facing == "down") {
		vY += 0.73;
	}

	if (facing == "right") {
		vX += 0.73;
	}

	if (facing == "left") {
		vX -= 0.73;
	}
}

void Players::SpawnAttackObject(Object &obj, Object object[]) {
	float spawnX = 0;
	float spawnY = 0;
	float spawnW = 64;
	float spawnH = 50;

	// Up
	if (this->sprite_dir == this->sprite_dir_list[1]) {
		spawnX = 0;
		spawnY = -realh/2;
		spawnW = 32;
		spawnH = 32;
	}
	// Down
	if (this->sprite_dir == this->sprite_dir_list[0]) {
		spawnX = 0;
		spawnY = realh/2;
		spawnW = 32;
		spawnH = 32;
	}
	// Right
	if (this->sprite_dir == this->sprite_dir_list[2]) {
		spawnX = realw/2;
		spawnY = 0;
		spawnW = 32;
		spawnH = 32;
	}
	// Left
	if (this->sprite_dir == this->sprite_dir_list[3]) {
		spawnX = -realw/2;
		spawnY = 0;
		spawnW = 32;
		spawnH = 32;
	}

	obj.spawn(object, x + spawnX,
					  y + spawnY,
					  spawnW, spawnH, 0);

	// Play slash sound effect
	Mix_PlayChannel(-1, settings->sSlash, 0);

	// Start delay
	//this->delay = true;
}

///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////// MUTATOR FUNCTIONS ///////////////////////////////


float Players::setVx(float value) {
	return this->vX += value;
}

float Players::setVy(float value) {
	return this->vY += value;
}

float Players::moveX(float value) {
	return this->x += value;
}

float Players::moveY(float value) {
	return this->y += value;
}

// Applies a shield to Player
void Players::ActivateInvurnerability(){
	invurnerableFrame			= 1;
	invurnerableTick			= 0;
	invurnerableT				= 300;
	invurnerable				= true;
}

void Players::StopParrying()
{
	// Reset parry CD
	this->parryTimer = 0;
	this->parry = false;
	this->parryCDTimer = this->parryCDMax;
}

void Players::ResetParry()
{
	// Reset parry CD
	this->parryTimer = 0;
	this->parry = false;
	this->parryCDTimer = 0;

	// Increase damage multiplier
	//this->damageMultipler += 0.1;
}

void Players::ExtendParryDuration()
{
	//  Extend parry
	this->parryTimer = 0;

	// Gain mana back
	this->mana += this->manaGainOnParry;

	// Increase damage multiplier
	//this->damageMultipler += 0.1;
}

void Players::IncreaseHearts(int value) {
	this->hearts += value;
	if (this->hearts > this->heartsMax) {
		this->hearts = this->heartsMax;
	}
}

void Players::IncreasePistolAmmo(int value) {
	this->ammoPistol += value;
	if (this->ammoPistol > this->ammoPistolMax) {
		this->ammoPistol = this->ammoPistolMax;
	}
}

void Players::IncreaseShotgunAmmo(int value) {
	this->ammoShotgun += value;
	if (this->ammoShotgun > this->ammoShotgunMax) {
		this->ammoShotgun = this->ammoShotgunMax;
	}
}

void Players::IncreaseHealth(float value) {
	this->health += value;
	if (this->health > this->healthMax) {
		this->health = this->healthMax;
	}
}

void Players::IncreaseScore(float value) {
	this->score += value;
}

void Players::ShortenParryCD(float value) {
	this->parryCDTimer -= value;

	if (this->parryCDTimer <= 0) {
		this->parryCDTimer = 0;
	}
}

void Players::StopMovement()
{
	// Stop player movement
    this->vX = 0.0;
    this->vY = 0.0;
    this->moving = false;
}

void Players::StopSlashAttack()
{
	// Reset slash attack timers
	attackType = -1;
    attackTimer = 0;
    attack = false;
}

void Players::StopDashing()
{
	// Reset dash timers
	this->dash = false;
	this->dashCounter 	= 0;
	this->dashCoolCounter = this->dashCooldown;
}

void Players::ResetDashing()
{
	// Reset dash timers
	this->dash = false;
	this->dashCounter 	= 0;
	this->dashCoolCounter = 0;
}

void Players::EquipWeapon(int weaponIndex, float itemDamage, float itemAtkSpeed)
{
	this->itemIndex = weaponIndex;
	this->damage = itemDamage;
	this->atkSpeed = itemAtkSpeed;
	this->equippedWeaponReloadSpeed = this->pistolReloadSpe;
}

void Players::stopEquipState() {
	this->pressedEquipKey = false;
}

void Players::setPowerUp(int value) {
	this->powerUp = value;
}

///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////// GETTER FUNCTIONS ////////////////////////////////

float Players::getX() {
	return this->x+w;
}

float Players::getY() {
	return this->y+h;
}

float Players::getYOffset() {
	return this->yOffset;
}

float Players::getW() {
	return this->w;
}

float Players::getH() {
	return this->h;
}

float Players::getPixelW() {
	return this->realw;
}

float Players::getPixelH() {
	return this->realh;
}

float Players::getLeftSide() {
	return this->x+this->w;
}

float Players::getRightSide() {
	return this->getX() + this->w;
}

float Players::getTopSide() {
	return this->y+this->h;
}

float Players::getBottomSide() {
	return this->getY() + this->h;
}

float Players::getDamage() {
	if (getAtkType() == 2) {
		return this->damage * this->damageMultipler;
	} else {
		return this->damage/4 * this->damageMultipler;
	}
}

float Players::getCastDamage() {
	return this->castDamage;
}


// Get x center of player
float Players::getCenterX() {
	return this->x + this->realw/2;
}

// Get y center of player
float Players::getCenterY() {
	return this->y + this->realh/2;
}

// Get value depending on direction facing
float Players::getXDir() {
	// Knockback player
	float xDir = 1;
	if (this->facing=="left") {
		xDir = -1;
	}
	if (this->facing=="right") {
		xDir = 1;
	}
	return xDir;
}

// Get knockback power
float Players::getKnockBackPower() {
	return this->knockBackPower;
}


float Players::getInvurnerableStatus() {
	return this->invurnerable;
}

float Players::getParryStatus() {
	return this->parry;
}

float Players::getDashStatus() {
	return this->dash;
}

float Players::getScore() {
	return this->score;
}

bool Players::getEquipState() {
	return this->pressedEquipKey;
}
int Players::getItemEqipped(int checkThisIndex) {
	if (checkThisIndex == itemIndex) {
		return true;
	} else{
		return false;
	}
}
int Players::getItemID() {
	return itemIndex;
}

int Players::getAtkType() {
	return this->attackType;
}

void Players::ResetHighScore(int LevelWeLoaded){

	// Set a file path for score.txt
	std::stringstream filePath;
	filePath << "data/maps/highscore";
	filePath << LevelWeLoaded;
	filePath << ".txt";

	// Overwrite "highscores.txt"
	std::ofstream fileSave;
	fileSave.open(filePath.str().c_str());
	fileSave << 0;
	fileSave.close();

	// Reset scores
	this->highscore = 0;
	this->score = 0;
}

void Players::ApplyHighScore(float previousHighScore) {
	this->highscore = previousHighScore;
}

void Players::SaveHighScore(int LevelWeLoaded) {

	// Open highscore first to check value
	unsigned int tempScore = -1;

	// Open file, and get previous High Score
	{
		std::stringstream filePath;
		filePath << "data/maps/highscore";
		filePath << LevelWeLoaded;
		filePath << ".txt";

		std::fstream fileOpen(filePath.str().c_str());

		// If a file exist
		if (fileOpen.is_open()) {
			// Store previous High score in this variable
			fileOpen >> tempScore;


			// Check if current Score is higher than previous High score
			if (this->score > tempScore) {

				// Save current Score as new High Score
				std::stringstream filePath;
				filePath << "data/maps/highscore";
				filePath << LevelWeLoaded;
				filePath << ".txt";

				std::ofstream fileSave;
				fileSave.open(filePath.str().c_str());
				fileSave << this->score;
				fileSave.close();
			}
		}

		// If no file exists, create new highscore
		else {

			std::cout<< "File does not exist on Saving Highscore, creating new file.\n";
			{
				std::stringstream filePath;
				filePath << "data/maps/highscore";
				filePath << LevelWeLoaded;
				filePath << ".txt";

				std::ofstream fileSave;
				fileSave.open(filePath.str().c_str());
				fileSave << this->score;
				fileSave.close();
			}
		}
		fileOpen.close();
	}
}

void Players::SaveData() {

	// Open file, and get previous High Score
	{
		std::stringstream filePath;
		filePath << "data/savefile.mp";
		std::fstream fileOpen(filePath.str().c_str());

		// If a file exist
		if (fileOpen.is_open()) {
			// Save current Score as new High Score
			std::stringstream filePath;
			filePath << "data/savefile.mp";

			std::ofstream fileSave;
			fileSave.open(filePath.str().c_str());

			fileSave << this->damage 			<< " "
					 << this->atkSpeed 			<< " "
					 << this->hearts 			<< " "
					 << this->healthMax 		<< " "
					 << this->castDamage 		<< " "
					 << this->atkSpeed 		<< " "
					 << this->maxMana  			<< " "
					 << this->manaRegenSpeed  	<< " "
					 << this->manaGainOnParry  	<< " "
					 << this->parryLength  		<< " "
					 << this->dashSpeed  		<< " "
					 << this->dashLength  		<< " "
			   	   	 << true 					<< "\n";

			fileSave.close();
		}

		// If no file exists, create new highscore
		else {

			//std::cout<< "File does not exist on Saving Highscore, creating new file.\n";
			{
				std::stringstream filePath;
				filePath << "data/savefile.mp";

				std::ofstream fileSave;
				fileSave.open(filePath.str().c_str());

				fileSave << 10 		<< " "
						 << 4.27 	<< " "
						 << 3 		<< " "
						 << 100 	<< " "
						 << 100 	<< " "
						 << 1 		<< " "
						 << 100  	<< " "
						 << 8.75  	<< " "
						 << 5.25  	<< " "
						 << 15  	<< " "
						 << 10  	<< " "
						 << 5  		<< " "
				   	   	 << true 	<< "\n";

				fileSave.close();
			}
		}
		fileOpen.close();
	}
}

void Players::LoadData() {

	std::stringstream filePath;
	filePath << "data/savefile.mp";

	std::fstream fileSave;
	fileSave.open(filePath.str().c_str());

	// File opened, load data
	if (fileSave.is_open()) {
		fileSave >>  	  this->damage 				>>
						  this->atkSpeed 			>>
						  this->hearts 				>>
						  this->healthMax 			>>
						  this->castDamage 			>>
						  this->atkSpeed 			>>
						  this->maxMana 			>>
						  this->manaRegenSpeed 		>>
						  this->manaGainOnParry 	>>
						  this->parryLength 		>>
						  this->dashSpeed 			>>
						  this->dashLength			>>
						  this->alive;
	}

	// File didn't open, create a new file, then set default parameters
	else {

		std::cout<< "File does not exist on Saving Highscore, creating new file.\n";
		{
			std::stringstream filePath;
			filePath << "data/savefile.mp";

			std::ofstream fileNewSave;
			fileNewSave.open(filePath.str().c_str());

			fileNewSave << 10 		<< " "
					 << 4.27 	<< " "
					 << 3 		<< " "
					 << 100 	<< " "
					 << 100 	<< " "
					 << 1 		<< " "
					 << 100  	<< " "
					 << 8.75  	<< " "
					 << 5.25  	<< " "
					 << 15  	<< " "
					 << 10  	<< " "
					 << 5  		<< " "
			   	   	 << true 	<< "\n";

			fileNewSave.close();
		}

		// Set default parameters after create a new file save
		this->damage 			= 10;
		this->atkSpeed 			= 4.27;
		this->hearts 			= 3;
		this->healthMax 		= 100;
		this->castDamage 		= 100;
		this->atkSpeed 		= 1;
		this->maxMana 			= 100;
		this->manaRegenSpeed 	= 8.75;
		this->manaGainOnParry 	= 5.25;
		this->parryLength 		= 15;
		this->dashSpeed 		= 10;
		this->dashLength		= 5;
		this->alive				= true;
	}
	fileSave.close();


	// Set default parameters
	//setStatsBasedOnType();
}












