/*
 * Mob.cpp
 *
 *  Created on: Jul 21, 2017
 *      Author: Carl
 */

#include "Mob.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

void Mob::setSettings(Settings *settings) {
	this->settings = settings;
}

void Mob::Init(Mob mob[]) {
	count = 0;
	for (int i = 0; i < max; i++) {
		mob[i].x 					= 0;
		mob[i].y 					= 0;
		mob[i].w 					= 0;
		mob[i].h 					= 0;
		mob[i].vX 					= 0.0;
		mob[i].vY 					= 0.0;
		mob[i].speed 				= 0.0;
		mob[i].angle 				= 0.0;
		mob[i].angleFacingTarget   = 0.0;
		mob[i].health 				= 100;
		mob[i].healthDecay 			= 100;
		mob[i].maxHealth 			= 100;
		mob[i].distance 			= 1;
		mob[i].mouse 				= false;
		mob[i].flash 				= false;
		mob[i].alive 				= false;
		mob[i].collision 			= false;
		mob[i].onScreen 			= false;
		mob[i].alert 				= false;
		mob[i].renderInFront 		= false;
		mob[i].hasVision 			= false;
		mob[i].type 				= 0;
		mob[i].jumpstate			= "falling";
		mob[i].jumps				= 1;
		mob[i].jump 				= false;
		mob[i].animState			= -1;
		mob[i].randomAttack			= 0;
		mob[i].coolDownTimer		= this->coolDownTimeStart;
		mob[i].coolDownTimeStart	= this->coolDownTimeStart;

		mob[i].chargingAttack 	= false;
		mob[i].chargeTime 		= this->chargeTimeStart;
		mob[i].constantFiringTimer = 0;

		// Animations
		mob[i].animSpeed			= 15;
		mob[i].animTimer			= 0;
		mob[i].animFrame			= 0;

		// Idle animations
		mob[i].idleTimer			= 0;
		mob[i].idleSpe				= 15;

		// walk animations
		mob[i].walkTimer			= 0;
		mob[i].walkSpe				= 15;

		// Attack animations
		mob[i].atkAnimTimer			= 0;
		mob[i].atkAnimSpe			= 15;

		// Death animations
		mob[i].deathTimer			= 0;
		mob[i].deathSpe				= 15;

		// Sprite
		mob[i].sprite_index			= 0;
		mob[i].ClipIndex			= 0;

		// Hurt animation
		mob[i].startHurtAnim		= false;
	}

	// Other classes:
	LoadFonts();
}

void Mob::Load(SDL_Renderer *gRenderer) {

	// Skeleton
	gTexture[0][0].loadFromFile(gRenderer, "resource/gfx/Monsters_Creatures_Fantasy/Skeleton/Idle.png");
	gTexture[0][1].loadFromFile(gRenderer, "resource/gfx/Monsters_Creatures_Fantasy/Skeleton/Walk.png");
	gTexture[0][2].loadFromFile(gRenderer, "resource/gfx/Monsters_Creatures_Fantasy/Skeleton/Attack.png");
	gTexture[0][3].loadFromFile(gRenderer, "resource/gfx/Monsters_Creatures_Fantasy/Skeleton/Take Hit.png");
	gTexture[0][4].loadFromFile(gRenderer, "resource/gfx/Monsters_Creatures_Fantasy/Skeleton/Death.png");

	// Goblin
	gTexture[1][0].loadFromFile(gRenderer, "resource/gfx/Monsters_Creatures_Fantasy/Goblin/Idle.png");
	gTexture[1][1].loadFromFile(gRenderer, "resource/gfx/Monsters_Creatures_Fantasy/Goblin/Run.png");
	gTexture[1][2].loadFromFile(gRenderer, "resource/gfx/Monsters_Creatures_Fantasy/Goblin/Attack.png");
	gTexture[1][3].loadFromFile(gRenderer, "resource/gfx/Monsters_Creatures_Fantasy/Goblin/Take Hit.png");
	gTexture[1][4].loadFromFile(gRenderer, "resource/gfx/Monsters_Creatures_Fantasy/Goblin/Death.png");

	// Mushroom
	gTexture[2][0].loadFromFile(gRenderer, "resource/gfx/Monsters_Creatures_Fantasy/Mushroom/Idle.png");
	gTexture[2][1].loadFromFile(gRenderer, "resource/gfx/Monsters_Creatures_Fantasy/Mushroom/Run.png");
	gTexture[2][2].loadFromFile(gRenderer, "resource/gfx/Monsters_Creatures_Fantasy/Mushroom/Attack.png");
	gTexture[2][3].loadFromFile(gRenderer, "resource/gfx/Monsters_Creatures_Fantasy/Mushroom/Take Hit.png");
	gTexture[2][4].loadFromFile(gRenderer, "resource/gfx/Monsters_Creatures_Fantasy/Mushroom/Death.png");

	// Flying eye
	gTexture[3][0].loadFromFile(gRenderer, "resource/gfx/Monsters_Creatures_Fantasy/Eye/Flight.png");
	gTexture[3][1].loadFromFile(gRenderer, "resource/gfx/Monsters_Creatures_Fantasy/Eye/Flight.png");
	gTexture[3][2].loadFromFile(gRenderer, "resource/gfx/Monsters_Creatures_Fantasy/Eye/Attack.png");
	gTexture[3][3].loadFromFile(gRenderer, "resource/gfx/Monsters_Creatures_Fantasy/Eye/Take Hit.png");
	gTexture[3][4].loadFromFile(gRenderer, "resource/gfx/Monsters_Creatures_Fantasy/Eye/Death.png");

	gMobShadow.loadFromFile(gRenderer, "resource/gfx/shadow.png");

	// Skeleton Attack
	{
		rClips[0] = {150*0, 0, 150, 150};
		rClips[1] = {150*1, 0, 150, 150};
		rClips[2] = {150*2, 0, 150, 150};
		rClips[3] = {150*3, 0, 150, 150};
		rClips[4] = {150*4, 0, 150, 150};
		rClips[5] = {150*5, 0, 150, 150};
		rClips[6] = {150*6, 0, 150, 150};
		rClips[7] = {150*7, 0, 150, 150};
	}
}

void Mob::Free() {
	for (int i=0; i<4; i++) {
		for (int j=0; j<5; j++) {
			gTexture[i][j].free();
		}
	}
	gMobShadow.free();

	// Other classes:
	FreeFonts();
}

void Mob::RemoveAll(Mob mob[]) {
	count = 0;
	for (int i = 0; i < max; i++) {
		mob[i].alive = false;
	}
}


void Mob::Remove(Mob mob[]) {
	for (int i = 0; i < max; i++) {
		if (mob[i].alive){
			if (mob[i].mouse){
				mob[i].alive = false;
				count--;
			}
		}
	}
}

void Mob::Spawn(Mob mob[], float x, float y, float angle, float speed) {
	for (int i = 0; i < max; i++) {
		if (!mob[i].alive) {

			///////////////////////////////////////////////////////////////////////////////////////
			//----------- When mob is spawned, spawn going a random direction -----------------//
			int randDir = rand() % 2;
			int dir;
			if (randDir == 0)
				dir 				= 1;
			else
				dir 				= -1;
			//----------- When mob is spawned, spawn going a random direction -----------------//
			///////////////////////////////////////////////////////////////////////////////////////

			///////////////////////////////////////////////////////////////////////////////////////
			//------------------------------ Save/load this data --------------------------------//
			mob[i].x 					= x;
			mob[i].y 					= y;
			mob[i].speed 				= speed;
			mob[i].alive 				= true;
			mob[i].mouse 				= false;
			mob[i].flash 				= false;
			mob[i].hasVision 			= false;
			mob[i].vX 					= dir;
			mob[i].vY 					= sin( (3.14159265/180)*(angle) );
			mob[i].angle 				= 0.0;

			//------------------------------ Save/load this data --------------------------------//
			///////////////////////////////////////////////////////////////////////////////////////

			//------------------------------------------------------------------------//
			mob[i].angleFacingTarget   	= 0.0;
			/* initial centering of mob's circle collision shape */
			/*float radians 				= (3.1415926536/180)*(0.0);
			float Cos 					= floor(cos(radians)*10+0.5)/10;
			float Sin 					= floor(sin(radians)*10+0.5)/10;
			int newW 					= 11 * (-Cos);
			int newH 					= 11 * (-Sin);*/
			//------------------------------------------------------------------------//
			//mob[i].vX 				= cos( (3.14159265/180)*(angle) );
			mob[i].constantFiringTimer 	= 0;
			mob[i].animState			= -1;
			mob[i].coolDownTimer		= this->coolDownTimeStart;
			mob[i].coolDownTimeStart	= this->coolDownTimeStart;
			mob[i].chargingAttack 		= false;
			mob[i].chargingAttack 		= false;
			mob[i].chargeTime 			= this->chargeTimeStart;
			mob[i].distance 			= 1;
			mob[i].collision 			= false;
			mob[i].onScreen 			= false;

			// Spawning normal mob, set default parameters
			mob[i].type 				= type;
			setStatsBasedOnType(mob, i);

			count++;
			break;
		}
	}
}

void Mob::Update(Mob mob[], Object &obj, Object object[],
		Particle particle[], Particle &p_dummy,
		Map &map, int mex, int mey,
		int camx, int camy, bool playerAlive) {

	bool playCastSFX = false;

	for (int i = 0; i < this->max; i++) {
		if (mob[i].alive) {

			// mob screen check
			if (mob[i].xR + mob[i].wR >= camx && mob[i].xR <= camx + screenWidth &&
				mob[i].yR + mob[i].hR >= camy && mob[i].yR <= camy + screenHeight) {
				mob[i].onScreen = true;
			}else{
				mob[i].onScreen = false;
			}

			// Mob health decay
			if (mob[i].healthDecay > mob[i].health) {
				mob[i].healthDecay -= 0.25;
			}

			// Update circle collision w/ other Mobs
			//UpdateCollisionMobs(mob, i);

			// Animation
			{
				mob[i].animTimer += mob[i].animSpeed;

				if (mob[i].animTimer > 60) {
					mob[i].animTimer = 0;
					mob[i].animFrame++;
				}

				if (mob[i].animFrame > 4) {
					mob[i].animFrame = 0;
					mob[i].animTimer = 0;
				}
			}

			// Decelerate mob velocity every frame
			//mob[i].vX = mob[i].vX - mob[i].vX * 0.2;
			//mob[i].vY = mob[i].vY - mob[i].vY * 0.2;

			////////////////////////////////////////////////////////////////////////////
			/////////////////////////// Handle Mob Animations ////////////////////////

			// Cooldown animation State
			if (mob[i].animState == -2)
			{
				// Start idle animation
				this->DoIdleAnim(mob, i);

				// Start cooldown timer
				this->DoCDTimer(mob, i);

				// If hurt, override with Hurt animation
				this->DoHurtAnim(mob, i);
			}
			// Cooldown animation State
			else if (mob[i].animState == -1)
			{
				// Start idle animation
				this->DoIdleAnim(mob, i);

				// Start cooldown timer
				this->DoCDTimer(mob, i);

				// If hurt, override with Hurt animation
				this->DoHurtAnim(mob, i);
			}

			// Walking around State
			else if (mob[i].animState == 0)
			{
				// No vision of User
				if (!mob[i].hasVision)
				{
					// Not alert
					mob[i].alert = false;
				}

				// Has vision of User
				else {

					// If within sight range, set to alert
					if (mob[i].distance < mob[i].getSightRange())
					{
						// Alert
						mob[i].alert = true;
					} else {
						// Not Alert
						mob[i].alert = false;
					}
				}

				// Alert
				if (mob[i].alert)
				{
					// Walk towards player
					mob[i].vX += mob[i].moveSpe * (mob[i].bmx - mob[i].bmx2) / mob[i].distance;
					mob[i].vY += mob[i].moveSpe * (mob[i].bmy - mob[i].bmy2) / mob[i].distance;

					if (mob[i].vX > 1) {
						mob[i].vX = 1;
					}
					if (mob[i].vY > 1) {
						mob[i].vY = 1;
					}

					// Start Walk animation
					this->DoWalkAnim(mob, i);
				}

				// Not Alert
				else {

					// Start idle animation
					this->DoIdleAnim(mob, i);
				}

				// If distance less than attack range
				if (mob[i].hasVision)
				{
					if (mob[i].distance < mob[i].getAtkRange())
					{
						// If not attacking
						if (!mob[i].chargingAttack)
						{
							// Start charge-attack animation
							mob[i].chargingAttack = true;

							// Choose random attack for Mob before starting Shooting animations
							mob[i].randomAttack = rand() % 2;

							// Set direction of attack

							// Looking Right
			                if (mob[i].bmx > mob[i].getCenterX(mob, i)) {
								mob[i].atkDir		= "right";
								mob[i].atkXOffset	= 0;
			                }

			                // Looking Left
			                else {
								mob[i].atkDir		= "left";
								mob[i].atkXOffset	= -mob[i].wR*3;
			                }

							// Start attack animation
							mob[i].animState = 1;

							// Stop hurt animation
							mob[i].startHurtAnim = false;

							// Stop moving mob
							mob[i].vX = 0.0;
							mob[i].vY = 0.0;

							// Override function
							// Flying eye
							if (mob[i].type == 3) {

								// Start attack animation
								mob[i].animState = 3;
							}
						}
					}
				}

				// If hurt, override with Hurt animation
				this->DoHurtAnim(mob, i);
			}

			// Slash Attack
			else if (mob[i].animState == 1) {

				// If Charge attack animation
				if (mob[i].chargingAttack)
				{
					// Set Idle texture
					mob[i].sprite_index = 2;

					// Start atkAnim animation
					mob[i].atkAnimTimer += mob[i].atkAnimSpe;
					if (mob[i].atkAnimTimer > 60) {
						mob[i].atkAnimTimer = 0;
						mob[i].ClipIndex++;
					}

					// Spawn attack object on attack frame
					if (mob[i].ClipIndex == 6) {
						{
							float playerH = 32;
			                // Attack-object's width and height
			                int tempWidth = mob[i].wR*3;
			                int tempHeight = mob[i].hR + (playerH*2);

							// Play SFX
							if (!mob[i].playSlashSFX) {

								obj.spawn(object,
										mob[i].xR + mob[i].atkXOffset,
									    mob[i].yR - playerH,
										tempWidth, tempHeight,
										2, i, mob[i].damage);		// Save Bosss index for reference later (can be used to knockback this boss for example)

								mob[i].playSlashSFX = true;
								Mix_PlayChannel(-1, settings->sSlash, 0);

								// If mushroom do VFX
								if (mob[i].type == 2) {
									this->DoMushRoomVFX(mob, i, particle, p_dummy);
								}
							}
						}
					}

					// Stop attack
					if (mob[i].ClipIndex > 7) {
						mob[i].ClipIndex = 0;

						// Stop charge attack animation
						mob[i].chargingAttack = false;

						// Reset charge-attack count down
						mob[i].chargeTime = this->chargeTimeStart;

						// Set animation state to cooldown (-1)
						mob[i].animState = -1;

						// Reset playing slash SFX
						mob[i].playSlashSFX = false;

						// Not alert
						mob[i].alert = false;
					}
				}
			} else if (mob[i].animState == 3) {

				// If Charge attack animation
				if (mob[i].chargingAttack)
				{
					// Number of shots to fire
					const int totalProjectiles = 3;

					// Frames to spawn projectiles (up to 60)
					int frameToSpawn[totalProjectiles] = {8, 16, 24};

					// Projectile spawn variables
					int randsize = 10;
					float tempX = getCenterX(mob, i) - randsize/2;
					float tempY = getCenterY(mob, i) - randsize/2;

					// Loop through frames to check
					for (int h=0; h<totalProjectiles; h++)
					{
						// If at certain frame
						if (mob[i].chargeTime == frameToSpawn[h])
						{
							// Spawn from CENTER side
							p_dummy.spawnParticleAngle(particle, 1, 1,
												tempX,
												tempY,
												randsize, randsize,
												mob[i].angleFacingTarget, 2,
											   5, 0, 20,
											   {255, 255, 255, 255}, 1,
											   1, 1,
											   255, 0,
											   randDouble(5, 30), 0.2,
											   false, 0,
											   100, 10, 1);

							// Play SFX
							Mix_PlayChannel(-1, settings->sCast, 0);
						}
					}

					// If count down has not reached 0 seconds
					if (mob[i].chargeTime > 0) {

						// Start counting down charge-attack animation
						mob[i].chargeTime--;
					}

					// Countdown reached 0 seconds.
					else {

						// Stop charge attack animation
						mob[i].chargingAttack = false;

						// Reset charge-attack count down
						mob[i].chargeTime = this->chargeTimeStart;

						// Reset cool down timer
						mob[i].coolDownTimer = this->coolDownTimeStart;

						// Set animation state to cooldown (-1)
						mob[i].animState = -1;
					}
				}
			}

			// Cooldown animation State
			else if (mob[i].animState == 2)
			{
				// Start death animation
				this->DoDeathAnim(mob, i);
			}

			// Charging-attack animation
			/*else if (mob[i].animState == 2) {

				// If Charge attack animation
				if (mob[i].chargingAttack)
				{
					// Typical Mob Attack
					for (int j=0; j<3; j++)
					{
						// Mob shoots based on what our chargeTime is
						if (mob[i].chargeTime == j * 10)
						{
							// Spawn particle effect
							int rands = 24;
							int speed = randDouble(2, 2);
							float tempX = mob[i].x + mob[i].w/2 - rands/2;
							float tempY = mob[i].y + mob[i].h/2 - rands/2;
							for (double h=0.0; h< 360.0; h+=rand() % 10 + 90){

								// Spawn projectile
								p_dummy.spawn360RedAttack(particle, 1, tempX, tempY, rands, rands, h, mob[i].damage, speed, {255,255,255});
							}

							// Play SFX
							playCastSFX = true;
						}
					}

					// Do idle anyways, but this will get ovverridden by the code below
					{
						// Start idle timer
						mob[i].idleTimer += mob[i].idleSpe;

						// Timer is done, go to next frame
						if (mob[i].idleTimer > 60) {
							mob[i].idleTimer = 0;

							// Go to next frame for idle
							mob[i].sprite_index += 1;
						}

						// Loop around
						if (mob[i].sprite_index > 3) {
							mob[i].sprite_index = 0;
						}
					}

					// Ovveride above code: Set sprite index
					if (mob[i].chargeTime > 15) {
						mob[i].sprite_index = 5;
					}

					// If count down has not reached 0 seconds
					if (mob[i].chargeTime > 0) {

						// Start counting down charge-attack animation
						mob[i].chargeTime -= mob[i].chargeTimeSpe;
					}

					// Countdown reached 0 seconds.
					else {

						// Stop charge attack animation
						mob[i].chargingAttack = false;

						// Reset charge-attack count down
						mob[i].chargeTime = this->chargeTimeStart;

						// Set animation state to cooldown (-1)
						mob[i].animState = -1;

						//----------------------------- Spawn attack at the end of charge attack ----------------------------//
						///////////////////////////////////////////////////////////////////////////////////////////////////////
					}
				}
			}

			///////////////////////////////////////////////////////////////////////////////////
			//-------------------------------------------------------------------------------//
			//---------------------------- Barrage Attack Part 1 ----------------------------//
			else if (mob[i].animState == 3) {

				// If Charge attack animation
				if (mob[i].chargingAttack)
				{
					for (int j=0; j<5; j++) {
						if (mob[i].chargeTime == j * 3) {

							// Spawn particle effect
							int rands = 16;
							float speed = 4;

							p_dummy.spawnQuick360BlueAttackPart1(particle,
									1, mob[i].x, mob[i].y, mob[i].w, mob[i].h,
									rands, rands,
									h, mob[i].damage, speed, {244,144,40});

							// Play SFX
							playCastSFX = true;
						}
					}

					// If count down has not reached 0 seconds
					if (mob[i].chargeTime > 0) {

						// Start counting down charge-attack animation
						mob[i].chargeTime -= mob[i].chargeTimeSpe;
					}

					// Countdown reached 0 seconds.
					else {

						// Attack has 2 parts

						// Reset charge-attack count down
						mob[i].chargeTime = this->chargeTimeStart;

						// Set animation state
						mob[i].animState = 4;
					}
				}
			}
			//---------------------------- Barrage Attack Part 1 ----------------------------//
			//-------------------------------------------------------------------------------//
			///////////////////////////////////////////////////////////////////////////////////


			///////////////////////////////////////////////////////////////////////////////////
			//-------------------------------------------------------------------------------//
			//---------------------------- Barrage Attack Part 2 ----------------------------//
			else if (mob[i].animState == 4) {

				// If Charge attack animation
				if (mob[i].chargingAttack)
				{
					//for (int j=0; j<3; j++) {
						//if (mob[i].chargeTime == j * 1) {
						if (mob[i].chargeTime == mob[i].attackFrame) {

							// Spawn particle effect
							int rands = 16;
							float speed = 2;

							p_dummy.spawnQuick360BlueAttackPart2(particle,
									1, mob[i].x, mob[i].y+20, mob[i].w, mob[i].h,
									rands, rands,
									h, mob[i].damage, speed, {255,255,255});

							// Play SFX
							playCastSFX = true;
						}
					//}

					// Do idle anyways, but this will get ovverridden by the code below
					{
						// Start idle timer
						mob[i].idleTimer += mob[i].idleSpe;

						// Timer is done, go to next frame
						if (mob[i].idleTimer > 60) {
							mob[i].idleTimer = 0;

							// Go to next frame for idle
							mob[i].sprite_index += 1;
						}

						// Loop around
						if (mob[i].sprite_index > 3) {
							mob[i].sprite_index = 0;
						}
					}

					// Ovveride above code: Set sprite index
					if (mob[i].chargeTime > 15) {
						mob[i].sprite_index = 5;
					}

					// If count down has not reached 0 seconds
					if (mob[i].chargeTime > 0) {

						// Start counting down charge-attack animation
						mob[i].chargeTime -= mob[i].chargeTimeSpe;
					}

					// Countdown reached 0 seconds.
					else {

						// Stop charge attack animation
						mob[i].chargingAttack = false;

						// Reset charge-attack count down
						mob[i].chargeTime = this->chargeTimeStart;

						// Set animation state to cooldown (-1)
						mob[i].animState = -1;
					}
				}
			}
			*/
			//---------------------------- Barrage Attack Part 2 ----------------------------//
			//-------------------------------------------------------------------------------//
			///////////////////////////////////////////////////////////////////////////////////

			/////////////////////////// Handle Mob Animations ////////////////////////
			////////////////////////////////////////////////////////////////////////////

			// Camera level bounds
			if( mob[i].x < 0 ){
				mob[i].x = 0;
			}
			if( mob[i].y < 0 ){
				mob[i].y = 0;
			}
			if( mob[i].x+mob[i].w > map.w ){
				mob[i].x = map.w-mob[i].w;
			}
			if( mob[i].y+mob[i].h > map.h ){
				mob[i].y = map.h-mob[i].h ;
			}
			if (
				mob[i].flash) {

				mob[i].flash = false;
			}
		}
	}

	if (playCastSFX) {
		Mix_PlayChannel(-1, settings->sCast, 0);
	}

	// Other classes:
}

void UpdateCollisionMob(Mob mob[], int i) {

	// mob circle collision check with other mobs
	/*for (int j = 0; j < this->max; j++) {
		if (i != j) {
			if (mob[j].alive) {
				float bmx = mob[j].x+mob[j].w/2;
				float bmy = mob[j].y+mob[j].h/2;
				float bmx2 = mob[i].x+mob[i].w/2;
				float bmy2 = mob[i].y+mob[i].h/2;
				float angle = atan2(bmy - bmy2,bmx - bmx2);
				angle = angle * (180 / 3.1416);
				if (angle < 0) {
					angle = 360 - (-angle);
				}
				float radians = (3.1415926536/180)*(angle);
				float Cos = floor(cos(radians)*10+0.5)/10;
				float Sin = floor(sin(radians)*10+0.5)/10;
				float distance = sqrt((bmx - bmx2) * (bmx - bmx2)+
									  (bmy - bmy2) * (bmy - bmy2));
				if (distance <= 0.01) {
					distance = 0.01;
				}
				if (distance < mob[i].w/2 + mob[j].w/2) {
					//mob[i].x -= 1 * Cos;
					//mob[i].vX = mob[i].vX * -1;

					// This will move both mobs away from each other so they don't overlap
					mob[j].x += 1 * Cos;

					// This still only bounces the mob thats being launched
					//mob[j].vX = mob[j].vX * -1.2;

					//mob[i].y -= 1 * Sin;
				}
			}
		}
	}*/
}

void Mob::UpdateEditor(Mob mob[], int mex, int mey, int camx, int camy) {
	for (int i = 0; i < this->max; i++) {
		if (mob[i].alive) {

			//If the mouse is on the tile
			if (mex > mob[i].xR && mex < mob[i].xR + mob[i].wR &&
				mey > mob[i].yR && mey < mob[i].yR + mob[i].hR) {
				mob[i].mouse = true;
			} else {
				mob[i].mouse = false;
			}
		}
	}

	// Other classes:
}

void Mob::Move(Mob mob[], std::string direction){
	for (int i = 0; i < max; i++) {
		if (mob[i].alive){
			if (direction == "left") {
				mob[i].x -= 64;
			}
			else if (direction == "right") {
				mob[i].x += 64;
			}
			else if (direction == "up") {
				mob[i].y -= 64;
			}
			else if (direction == "down") {
				mob[i].y += 64;
			}

		}
	}
}

void Mob::RenderShadow(SDL_Renderer *gRenderer, Mob mob[], int camx, int camy) {
	for (int i = 0; i < max; i++) {
		if (mob[i].alive) {
			// Render mob shadow
			float shadowSize = mob[i].wR + 5;
			float shadowSizeH = mob[i].hR * 0.35;
			float yOffset = mob[i].hR/2;
			gMobShadow.setAlpha(110);
			gMobShadow.render(gRenderer, mob[i].xR+mob[i].wR/2-shadowSize/2 - camx,
										 mob[i].yR+mob[i].hR - shadowSizeH/2 - camy,
									     shadowSize, shadowSizeH);
		}
	}
}

void Mob::RenderBack(SDL_Renderer *gRenderer, Mob mob[], int camx, int camy) {
	for (int i = 0; i < max; i++) {
		if (mob[i].alive) {
			if (!mob[i].renderInFront) {

				if (mob[i].flash) {
					//gMobFlashed.setAlpha(255);
					//gMobFlashed.render(gRenderer, mob[i].x - camx, mob[i].y+mob[i].yOffset - camy,
					//							mob[i].w, mob[i].h, NULL,
					//							0.0, NULL, mob[i].flip);
				} else {

					// Render texture
					{
						int j = mob[i].sprite_index;

						gTexture[mob[i].type][j].setColor(255, 255, 255);
						gTexture[mob[i].type][j].setAlpha(255);
						gTexture[mob[i].type][j].render(gRenderer,
								mob[i].x - camx,
								mob[i].y + mob[i].yOffset - camy,
								mob[i].w,
								mob[i].h,
								&rClips[mob[i].ClipIndex],
								0.0, NULL,
								mob[i].flip);
					}
				}
			}
		}
	}
}

void Mob::RenderFront(SDL_Renderer *gRenderer, Mob mob[], int camx, int camy) {
	for (int i = 0; i < max; i++) {
		if (mob[i].alive) {
			if (mob[i].renderInFront) {

				if (mob[i].flash) {
					//gMobFlashed.setAlpha(255);
					//gMobFlashed.render(gRenderer, mob[i].x - camx, mob[i].y+mob[i].yOffset - camy,
					//							mob[i].w, mob[i].h, NULL,
					//							0.0, NULL, mob[i].flip);
				} else {

					// Render texture
					{
						int j = mob[i].sprite_index;

						gTexture[mob[i].type][j].setColor(255, 255, 255);
						gTexture[mob[i].type][j].setAlpha(255);
						gTexture[mob[i].type][j].render(gRenderer,
								mob[i].x - camx,
								mob[i].y + mob[i].yOffset - camy,
								mob[i].w,
								mob[i].h,
								&rClips[mob[i].ClipIndex],
								0.0, NULL,
								mob[i].flip);
					}
				}
			}
		}
	}
}

void Mob::RenderUI(SDL_Renderer *gRenderer, Mob mob[], int camx, int camy) {
	for (int i = 0; i < max; i++) {
		if (mob[i].alive) {
			// Mob UI
			// Text
			/*{
				// Render Text
				int uiX = screenWidth/2-gText.getWidth()/2;
				int uiY = 50;

				std::stringstream tempss;
				tempss << "THE FATTEST BOSS FIGHT EVER";
				gText.loadFromRenderedText(gRenderer, tempss.str().c_str(), {255, 255, 255}, gFont1236);
				gText.render(gRenderer, uiX, uiY, gText.getWidth(), gText.getHeight());
			}*/
			const float yOffsetBar = 0;
			const float barHeight = 6;
			const float barWidth = mob[i].wR*1.25;
			float uiX = mob[i].xR + mob[i].wR/2 - barWidth/2;
			float uiY = mob[i].yR+mob[i].yOffset - barHeight - yOffsetBar;

			// Health Decay bar on Mobes
			{
				// Health Decay bar, bg
				RenderFillRect(gRenderer, uiX-camx, uiY-camy, (barWidth*mob[i].maxHealth)/mob[i].maxHealth, barHeight, {0, 0, 0} );

				// Render Decay health
				RenderFillRect(gRenderer, uiX-camx, uiY-camy, (barWidth*mob[i].healthDecay)/mob[i].maxHealth, barHeight, {60, 30, 30} );
			}

			// Health bar on Mobes
			{
				// Render health
				RenderFillRect(gRenderer, uiX-camx, uiY-camy, (barWidth*mob[i].health)/mob[i].maxHealth, barHeight, {200, 30, 30} );
			}
		}
	}
}

void Mob::RenderDebug(SDL_Renderer *gRenderer, Mob mob[], int camx, int camy) {
	for (int i = 0; i < max; i++) {
		if (mob[i].alive) {

			// DEBUG SQAURES UNDER HERE

			// Render box around mob
			SDL_Rect tempRect = {mob[i].x-camx, mob[i].y-camy, mob[i].w, mob[i].h};
			SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);
			SDL_RenderDrawRect(gRenderer, &tempRect);

			tempRect = {mob[i].xR-camx, mob[i].yR-camy, mob[i].wR, mob[i].hR};
			SDL_SetRenderDrawColor(gRenderer, 0, 255, 0, 255);
			SDL_RenderDrawRect(gRenderer, &tempRect);

			// Mob imaginary line that checks if there is a block in the next step
			// if not, then the mob turns around
			// Where we check this is inside 'checkMobTilecCollision()' in PlayGame class
			//SDL_Rect rectMiddle = {mob[i].x+mob[i].w/2-1-camx, mob[i].y+mob[i].h-camy, 2, mob[i].h+2};
			//SDL_SetRenderDrawColor(gRenderer, 255, 0, 0, 255);
			//SDL_RenderDrawRect(gRenderer, &rectMiddle);

			// If mouse is hoveirng on Mob
			if (mob[i].mouse) {
				// Render Green box
				SDL_Rect tempr = {mob[i].xR+3 - camx, mob[i].yR+3 - camy, mob[i].wR-6, mob[i].hR-6};
				SDL_SetRenderDrawColor(gRenderer, 0, 255, 0, 255);
				SDL_RenderDrawRect(gRenderer, &tempr);
			} else {
				// Render Red box
				SDL_Rect tempr = {mob[i].xR+3 - camx, mob[i].yR+3 - camy, mob[i].wR-6, mob[i].hR-6};
				SDL_SetRenderDrawColor(gRenderer, 255, 0, 0, 255);
				SDL_RenderDrawRect(gRenderer, &tempr);
			}

			// If mob has vision of player
			/*if (mob[i].hasVision) {
				// Render Green box
				SDL_Rect tempr = {mob[i].x + mob[i].w/2-8/2-10 - camx, mob[i].y-55 - camy, 8, 30};
				SDL_SetRenderDrawColor(gRenderer, 110, 110, 110, 255);
				SDL_RenderFillRect(gRenderer, &tempr);

				// Render vision to player
				SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);
				SDL_RenderDrawLine(gRenderer, mob[i].bmx-camx, mob[i].bmy-camy, mob[i].bmx2-camx, mob[i].bmy2-camy);
			}*/

			// If mob is alert of player
			if (mob[i].alert) {
				// Render Red box
				SDL_Rect tempr = {mob[i].x + mob[i].w/2-8/2-10 - camx, mob[i].y-55 - camy, 8, 30};
				SDL_SetRenderDrawColor(gRenderer, 255, 0, 0, 255);
				SDL_RenderFillRect(gRenderer, &tempr);
			} else {
				// Render Grey box
				SDL_Rect tempr = {mob[i].x + mob[i].w/2-8/2-10 - camx, mob[i].y-55 - camy, 8, 30};
				SDL_SetRenderDrawColor(gRenderer, 110, 110, 110, 255);
				SDL_RenderFillRect(gRenderer, &tempr);
			}

			/// TEXT UNDER HERE

			// Render Text
			/*std::stringstream tempss;
			tempss << "ja: " << mob[i].jumpAttack << ", jcd: " << mob[i].jumpCD;
			gText.loadFromRenderedText(gRenderer, tempss.str().c_str(), {255, 255, 255}, gFont12);
			gText.render(gRenderer, mob[i].xR-gText.getWidth()-camx, mob[i].yR-gText.getHeight()-camy, gText.getWidth(), gText.getHeight());*/

		}
	}
}

void Mob::RenderHand(SDL_Renderer *gRenderer, Mob mob[], int newMx, int newMy, int mex, int mey, int camx, int camy){
	// Render mob in hand
	gTexture[type][0].setAlpha(110);
	gTexture[type][0].render(gRenderer, newMx, newMy, pixelSizeW, pixelSizeH, &rClips[0]);

	// Render mouse coordinates snapped to grid
	SDL_Rect tempr = {newMx, newMy, pixelSizeW, pixelSizeH};
	SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);
	SDL_RenderDrawRect(gRenderer, &tempr);
}

void Mob::Stun(Mob mob[], int i) {

	// Reset attack if attacking before getting stunned
	{
		mob[i].ClipIndex = 0;

		// Stop charge attack animation
		mob[i].chargingAttack = false;

		// Reset charge-attack count down
		mob[i].chargeTime = this->chargeTimeStart;

		// Set to stunned animation state
		mob[i].animState = -2;

		// Reset playing slash SFX
		mob[i].playSlashSFX = false;

		// Not alert
		mob[i].alert = false;
	}

	// Reset cool down timer
	mob[i].coolDownTimer = this->coolDownTimeStartOnParry;
}

void Mob::Hurt(Mob mob[], int i) {

	if (mob[i].animState != 1)
	{
		mob[i].startHurtAnim = true;
		mob[i].sprite_index = 3;
		mob[i].hurtTimer = 0;
		mob[i].ClipIndex = 0;
	}

	// Stop charge attack animation
	/*mob[i].chargingAttack = false;

	// Reset charge-attack count down
	mob[i].chargeTime = this->chargeTimeStart;

	// Set animation state to cooldown (-1)
	mob[i].animState = -2;*/

}

void Mob::DoCDTimer(Mob mob[], int i)
{
	// If cool down timer greater than 0
	if (mob[i].coolDownTimer > 0) {

		// Start countdown
		mob[i].coolDownTimer -= mob[i].atkSpe;
	}

	// Countdown finished
	else {
		// Reset cool down timer
		mob[i].coolDownTimer = this->coolDownTimeStart;

		// Reset animation state
		mob[i].animState = 0;
	}
}

void Mob::DoIdleAnim(Mob mob[], int i)
{
	if (!mob[i].startHurtAnim)
	{
		// Set Idle texture
		mob[i].sprite_index = 0;

		// Start idle animation
		mob[i].idleTimer += mob[i].idleSpe;
		if (mob[i].idleTimer > 60) {
			mob[i].idleTimer = 0;
			mob[i].ClipIndex++;
		}
		if (mob[i].ClipIndex > 3) {
			mob[i].ClipIndex = 0;
		}
	}
}

void Mob::DoWalkAnim(Mob mob[], int i)
{
	if (!mob[i].startHurtAnim)
	{
		// Set Walk texture
		mob[i].sprite_index = 1;

		// Start Walk animation
		mob[i].walkTimer += mob[i].walkSpe;
		if (mob[i].walkTimer > 60) {
			mob[i].walkTimer = 0;
			mob[i].ClipIndex++;
		}
		if (mob[i].ClipIndex > 3) {
			mob[i].ClipIndex = 0;
		}
	}
}

void Mob::DoHurtAnim(Mob mob[], int i)
{
	if (mob[i].startHurtAnim)
	{
		if (mob[i].animState != 1)
		{
			// Set hurt texture
			mob[i].sprite_index = 3;

			// Start hurt animation
			mob[i].hurtTimer += mob[i].hurtSpe;
			if (mob[i].hurtTimer > 60) {
				mob[i].hurtTimer = 0;
				mob[i].ClipIndex++;
			}
			if (mob[i].ClipIndex > 3) {
				mob[i].ClipIndex = 0;
				mob[i].startHurtAnim = false;
			}
		}
	}
}

void Mob::ResetDeathAnim(Mob mob[], int i)
{
	if (!mob[i].startDeathAnim) {
		mob[i].startDeathAnim 	= true;
		mob[i].animState 		= 2;
		mob[i].deathTimer 		= 0;
		mob[i].ClipIndex 		= 0;
	}
}

void Mob::DoDeathAnim(Mob mob[], int i)
{
	if (mob[i].startDeathAnim)
	{
		// Set Idle texture
		mob[i].sprite_index = 4;

		// Start idle animation
		mob[i].deathTimer += mob[i].deathSpe;
		if (mob[i].deathTimer > 60) {
			mob[i].deathTimer = 0;
			mob[i].ClipIndex++;
		}
		if (mob[i].ClipIndex > 3) {
			mob[i].ClipIndex = 3;

			// Remove mob delayed
			mob[i].delayRemoveMobTimer += 10;
			if (mob[i].delayRemoveMobTimer > 60 * 5) {
				mob[i].delayRemoveMobTimer = 0;
				mob[i].startDeathAnim = false;
				mob[i].alive = false;
				count--;
			}
		}
	}
}

void Mob::DoMushRoomVFX(Mob mob[], int i, Particle particle[], Particle &p_dummy) {

	float spawnXOffset;
	if (mob[i].facing == "right")
		spawnXOffset = 20;
	else
		spawnXOffset = -20;

	// Spawn blood VFX
	p_dummy.spawnBloodVFX(particle, mob[i].xR + spawnXOffset, mob[i].yR+20, mob[i].wR, mob[i].hR, {255,144,40});

	// play sound effect
	Mix_PlayChannel(-1, settings->sExplosion, 0);
}

// Functions that work with other classes
void Mob::GetDistanceOfPlayer(Mob mob[], float targetX, float targetY, float targetW, float targetH, float *xFollow, float *yFollow) {
	for (int i = 0; i < max; i++) {
		if (mob[i].alive) {

			////////////////////////////////////////////////////////////////////////////
			/////////////////////////// GET DISTANCE OF PLAYER /////////////////////////
			// Get target
			mob[i].xFollow = xFollow;
			mob[i].yFollow = yFollow;

			// Set real pixel position of Mob
			//if (mob[i].type == 0) {
			//}
			mob[i].xR = mob[i].x + 62*1;
			mob[i].yR = mob[i].y + 44*1;

			////////////////////////////////////////////////////////////////////////////
			/////////////////////////// GET DISTANCE OF PLAYER /////////////////////////
			// Get center of attack-object (spawned by the player attacking)
			mob[i].xFollow = xFollow;
			mob[i].yFollow = yFollow;

			////////////////////////////////////////////////////////////////////////////
			/////////////////////////// GET DISTANCE OF PLAYER /////////////////////////

			// Center of Mobs
			mob[i].x2 				= mob[i].x+mob[i].w/2;
			mob[i].y2 				= mob[i].y+mob[i].h/2;

			// Get center of attack-object (spawned by the player attacking)
			mob[i].bmx = targetX+targetW/2;
			mob[i].bmy = targetY+targetH/2;

			// Get center of mob
			mob[i].bmx2 = mob[i].xR+mob[i].wR/2;
			mob[i].bmy2 = mob[i].yR+mob[i].hR/2;

			// Get distance of target X and Y axis
			mob[i].targetDistanceX = sqrt((mob[i].bmx - mob[i].bmx2) * (mob[i].bmx - mob[i].bmx2));
			mob[i].targetDistanceY = sqrt((mob[i].bmy - mob[i].bmy2) * (mob[i].bmy - mob[i].bmy2));
			if (mob[i].targetDistanceX < 0.1) { mob[i].targetDistanceX = 0.1; }
			if (mob[i].targetDistanceY < 0.1) { mob[i].targetDistanceY = 0.1; }

			// Target is right of Mob
			if (mob[i].hasVision) {
				if (!mob[i].chargingAttack && !mob[i].startDeathAnim) {
					if (mob[i].bmx > mob[i].bmx2) {
						mob[i].flip = SDL_FLIP_NONE;
						mob[i].facing = "right";
					} else {
						mob[i].flip = SDL_FLIP_HORIZONTAL;
						mob[i].facing = "left";
					}
				}
			}

			// Get angle of mob relative to attack-object
			float angle = atan2(mob[i].bmy - mob[i].bmy2, mob[i].bmx - mob[i].bmx2);
			angle = angle * (180 / 3.1416);
			if (angle < 0) {
				angle = 360 - (-angle);
			}

			// Get angle of mob relative to attack-object
			mob[i].angleFacingTarget = atan2(mob[i].bmy - mob[i].bmy2, mob[i].bmx - mob[i].bmx2);
			mob[i].angleFacingTarget = mob[i].angleFacingTarget * (180 / 3.1416);
			if (mob[i].angleFacingTarget < 0) { mob[i].angleFacingTarget = 360 - (-mob[i].angleFacingTarget); }

			// Handle radians, cos, sin
			float radians = (3.1415926536/180)*(angle);
			float Cos = floor(cos(radians)*10+0.5)/10;
			float Sin = floor(sin(radians)*10+0.5)/10;

			// Distance of mob relative to attack-object
			mob[i].distance = sqrt((mob[i].bmx - mob[i].bmx2) * (mob[i].bmx - mob[i].bmx2)+
								  (mob[i].bmy - mob[i].bmy2) * (mob[i].bmy - mob[i].bmy2));

			// If it goes less than 0, the game crashes
			if (mob[i].distance <= 0.01) {
				mob[i].distance = 0.01;
			}

			float distanceW = sqrt((bmx - bmx2) * (bmx - bmx2));
			float distanceH = sqrt((bmy - bmy2) * (bmy - bmy2));
			float tempVX 	= 0.5 * (bmx - bmx2) / distanceW;
			float tempVY 	= 0.5 * (bmy - bmy2) / distanceH;

			/////////////////////////// GET DISTANCE OF PLAYER /////////////////////////
			////////////////////////////////////////////////////////////////////////////

			// Check if player is in front of mob or not
			if (mob[i].yR+mob[i].hR > targetY+targetH){
				mob[i].renderInFront = true;
			} else {
				mob[i].renderInFront = false;
			}
		}
	}
}

// Getter functions

float Mob::getX(Mob mob[], int i) {
	return mob[i].xR;
}

float Mob::getY(Mob mob[], int i) {
	return mob[i].yR;
}

float Mob::getLeftSide(Mob mob[], int i) {
	return mob[i].xR;
}

float Mob::getRightSide(Mob mob[], int i) {
	return mob[i].xR+mob[i].wR;
}

float Mob::getTopSide(Mob mob[], int i) {
	return mob[i].yR;
}

float Mob::getBottomSide(Mob mob[], int i) {
	return mob[i].yR+mob[i].hR;
}

// Get x center of player
float Mob::getCenterX(Mob mob[], int i) {
	return mob[i].xR + mob[i].wR/2;
}

// Get y center of player
float Mob::getCenterY(Mob mob[], int i) {
	return mob[i].yR + mob[i].hR/2;
}


// Setter functions


void Mob::setStatsBasedOnType(Mob mob[], int i) {
	// Skelton
	if (mob[i].type == 0) {
		mob[i].w 			= 150*1;
		mob[i].h 			= 150*1;
		mob[i].wR 			= 25*1;
		mob[i].hR 			= 23*1;
		mob[i].damage		= 18;
		mob[i].health 		= 980;
		mob[i].maxHealth 	= 980;
		mob[i].healthDecay 	= 980;
		mob[i].yOffset 		= -36;
		mob[i].moveSpe 		= 0.7;
		mob[i].idleSpe 		= 7;
		mob[i].walkSpe 		= 6;
		mob[i].hurtSpe 		= 9;
		mob[i].deathSpe		= 8;
		mob[i].atkAnimSpe = 10;
		mob[i].chargeTimeSpe = 2;
		mob[i].attackFrame = 16;
		mob[i].setSightRange(32*4);
		mob[i].setAtkRange(16*1);
		mob[i].atkSpe		= 0.25;
	}

	// Goblin
	else if (mob[i].type == 1) {
		mob[i].w 			= 150*1;
		mob[i].h 			= 150*1;
		mob[i].wR 			= 25*1;
		mob[i].hR 			= 23*1;
		mob[i].damage		= 9;
		mob[i].health 		= 980;
		mob[i].maxHealth 	= 980;
		mob[i].healthDecay 	= 980;
		mob[i].yOffset 		= -36;
		mob[i].moveSpe 		= 1.4;
		mob[i].idleSpe 		= 7;
		mob[i].walkSpe 		= 6;
		mob[i].hurtSpe 		= 9;
		mob[i].deathSpe		= 8;
		mob[i].atkAnimSpe = 15;
		mob[i].chargeTimeSpe = 2;
		mob[i].attackFrame = 16;
		mob[i].setSightRange(32*4);
		mob[i].setAtkRange(16*1);
		mob[i].atkSpe		= 0.5;
	}

	// Mushroom
	else if (mob[i].type == 2) {
		mob[i].w 			= 150*1;
		mob[i].h 			= 150*1;
		mob[i].wR 			= 25*1;
		mob[i].hR 			= 23*1;
		mob[i].damage		= 50;
		mob[i].health 		= 3890;
		mob[i].maxHealth 	= 3890;
		mob[i].healthDecay 	= 3890;
		mob[i].yOffset 		= -36;
		mob[i].moveSpe 		= 0.5;
		mob[i].idleSpe 		= 7;
		mob[i].walkSpe 		= 6;
		mob[i].hurtSpe 		= 9;
		mob[i].deathSpe		= 8;
		mob[i].atkAnimSpe = 9;
		mob[i].chargeTimeSpe = 2;
		mob[i].attackFrame = 16;
		mob[i].setSightRange(32*8);
		mob[i].setAtkRange(16*1);
		mob[i].atkSpe		= 0.18;
	}

	// Flying eye
	else if (mob[i].type == 3) {
		mob[i].w 			= 150*1;
		mob[i].h 			= 150*1;
		mob[i].wR 			= 25*1;
		mob[i].hR 			= 23*1;
		mob[i].damage		= 3;
		mob[i].health 		= 550;
		mob[i].maxHealth 	= 550;
		mob[i].healthDecay 	= 550;
		mob[i].yOffset 		= -36;
		mob[i].moveSpe 		= 2.1;
		mob[i].idleSpe 		= 7;
		mob[i].walkSpe 		= 6;
		mob[i].hurtSpe 		= 9;
		mob[i].deathSpe		= 8;
		mob[i].atkAnimSpe = 20;
		mob[i].chargeTimeSpe = 2;
		mob[i].attackFrame = 16;
		mob[i].setSightRange(32*8);
		mob[i].setAtkRange(32*3);
		mob[i].atkSpe		= 0.5;
	}
	else {
		mob[i].w 			= 150*1;
		mob[i].h 			= 150*1;
		mob[i].wR 			= 25*1;
		mob[i].hR 			= 23*1;
		mob[i].damage		= 18;
		mob[i].health 		= 200;
		mob[i].maxHealth 	= 200;
		mob[i].healthDecay 	= 200;
		mob[i].yOffset 		= -36;
		mob[i].moveSpe 		= 0.7;
		mob[i].idleSpe 		= 7;
		mob[i].walkSpe 		= 6;
		mob[i].hurtSpe 		= 9;
		mob[i].deathSpe		= 8;
		mob[i].atkAnimSpe = 10;
		mob[i].chargeTimeSpe = 2;
		mob[i].attackFrame = 16;
		mob[i].setSightRange(32*8);
		mob[i].setAtkRange(16*1);
	}

	// Other defaults on spawn
	mob[i].hasVision 			= false;
	mob[i].playSlashSFX 		= false;
	mob[i].startDeathAnim 		= false;
	mob[i].delayRemoveMobTimer = 0;
	mob[i].hurtTimer = 0;
	mob[i].idleTimer = 0;
	mob[i].walkTimer = 0;
	mob[i].atkAnimTimer = 0;
	mob[i].deathTimer = 0;
	mob[i].vX = 0.0;
	mob[i].vY = 0.0;
	mob[i].atkDir				= "right";
	mob[i].atkXOffset			= 100;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////// LOAD-SAVE ///////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////
//------------------------------------ Load Mob data --------------------------------------//
// Load Mob data
void Mob::LoadData(Mob mob[], std::fstream &fileTileDataL)
{
	// Reset all mobs
	Init(mob);

	//std::cout << "Attempting to load... Mob" << std::endl;
	// Load Mob count data
	fileTileDataL >> this->count;
	//std::cout << this->count << std::endl;

	// Load rest of Mob data
	for (int h = 0; h < this->count; h++) {
		if (!mob[h].alive) {

			fileTileDataL >>
				mob[h].x 		>>
				mob[h].y	 	>>
				mob[h].w 		>>
				mob[h].h 		>>
				mob[h].radius >>
				mob[h].speed 	>>
				mob[h].type 	>>
				mob[h].damage >>
				mob[h].health >>
				mob[h].maxHealth >>
				mob[h].vX 	>>
				mob[h].vY 	>>
				mob[h].angle 	>>
				mob[h].alive;

			//std::cout << "Mob h: " << h << ", x: " << mob[h].x << ", y: " << mob[h].y << std::endl;

			// When we load mobs, spawn them 1 pixel from the ground so that we dont have glitches or problems
			mob[h].y-=2;

			// Defaults
			{
				// Set defeault parameters depending on what mobs were spawning
				setStatsBasedOnType(mob, h);
			}
		}
		//break;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
//------------------------------------ Save Mob data --------------------------------------//
// Save Tile data
std::string Mob::SaveData(Mob mob[])
{
	/////////////////////////////////////////////////////////////////////////////////
	//-----------------------------------------------------------------------------//
	//----------------------------- Save Everything in One File -------------------//

	// Save all tile data in a string first
	std::stringstream tempsstb;
	for (int i = 0; i < max; i++) {
		if (mob[i].alive) {
			tempsstb
				<< mob[i].x 			<< " "
				<< mob[i].y 			<< " "
				<< mob[i].w  			<< " "
				<< mob[i].h  			<< " "
				<< mob[i].radius  		<< " "
				<< mob[i].speed  		<< " "
				<< mob[i].type  		<< " "
				<< mob[i].damage  		<< " "
				<< mob[i].health  		<< " "
				<< mob[i].maxHealth  	<< " "
				<< mob[i].vX  			<< " "
				<< mob[i].vY  			<< " "
				<< mob[i].angle  		<< " "
			   	<< mob[i].alive 		<< "\n";
		}
	}
	std::string temps;
	temps = tempsstb.str().c_str();
	//temps = temps.substr(0, temps.size()-1);

	// Save number of tiles in World
	std::stringstream aVeryLongString;
	aVeryLongString << count << "\n";

	// Save the data of all Tiles
	aVeryLongString << temps.c_str();
	//----------------------------- Save Everything in One File -------------------//
	//-----------------------------------------------------------------------------//
	/////////////////////////////////////////////////////////////////////////////////

	return aVeryLongString.str().c_str();
}
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////// LOAD-SAVE ///////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////











