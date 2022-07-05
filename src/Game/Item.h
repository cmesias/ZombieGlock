/*
 * Item.h
 *
 *  Created on: Apr 18, 2017
 *      Author: Carl
 */

#ifndef ITEM_H_
#define ITEM_H_


#include "TextNFont.h"
#include "../LWindow.h"
#include "../LTexture.h"
#include "../Helper.h"

#include <fstream>
#include <sstream>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

class Item: public Helper, public TextNFont  {

public:	// Parent object Variables
	static const int ITEMS_UNIQUE = 16;
	int multiW = 1;
	int multiH = 1;
	const int max = 100;		// max number of instances
	int count;					// number of instances

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
	int id;			// id of instances

	// These get applied to the players stats
	float damage;
	float slashAtkSpe;

public:	// Object Variables
	float x, y;
	float w, h;
	float x2, y2;
	float vX, vY;
	bool collision;
	bool mouse;
	bool mouseBox;
	bool onScreen;
	bool alive;
	bool promptSelf;

public:	// Resources
	LTexture gSwords;

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

public:	// Variables
	float hoverAmount;
	float hoverDir;
	const float yOffset = -10;

public:	// Core functions

	// Initialize
	void Init(Item item[]);

	// Load
	void Load(SDL_Renderer *gRenderer);

	// Load
	void Free();

	// Copy
	void Copy(Item item[]);

	// Remove
	void Remove(Item item[], int click);

	// Remove all
	void RemoveAll(Item item[]);

	// Spawn
	void Spawn(Item item[], float x, float y, int id);

	// Spawn and throw in a direction
	void SpawnAndThrowItem(Item item[], float x, float y, int id, float vX, float vY);

	void SpawnAll(Item item[], int newMx, int newMy, int camx, int camy);

	// Update
	void Update(Item item[], int newMx, int newMy, int mex, int mey, int camx, int camy,
			  float targetX, float targetY, float targetW, float targetH);

	// Update
	void UpdateEditor(Item item[], int newMx, int newMy, int mex, int mey, int camx, int camy);

	bool PlayerBehindItem;

	void RenderBehindPlayer(SDL_Renderer *gRenderer, Item item[], int camx, int camy);

	void RenderOnTopOfPlayer(SDL_Renderer *gRenderer, Item item[], int camx, int camy);

	void RenderUI(SDL_Renderer *gRenderer, Item item[], int camx, int camy);

	// Render debug
	void RenderDebug(SDL_Renderer *gRenderer, Item item[], int camx, int camy);

	void RenderHand(SDL_Renderer *gRenderer, Item item[], int newMx, int newMy, int mex, int mey);

	void setStatsBasedOnType(Item tile[], int);

public:	// Getter functions for Editor

	// Get size of sword in Hand
	int getItemSizeW();
	int getItemSizeH();

	// Get size of item from item[i]
	int getW(Item item[], int i);
	int getH(Item item[], int i);

public: // Save functions

	void LoadData(Item item[], std::fstream &fileTileDataL);

	std::string SaveData(Item item[]);
};

#endif /* ITEM_H_ */
