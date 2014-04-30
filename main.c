#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "util.h"

typedef struct room room_t;
typedef struct item item_t;

struct item {
	char * name;
	char * description;
	char ** actions;
};

struct room {
	char * description;
	room_t * north, * south, * east, * west;
	item_t * items;
};

typedef enum {
	NORTH,
	SOUTH,
	EAST,
	WEST
} compass;

/* global variable to hold room*/
room_t * room = NULL;

/* Loads data into structs */
void init(){
	/* This enum defines the sections of the datafile in order */
	enum {
		NONE,
		ROOM_DESC,
		ROOM_LINKS,
		OBJ_PROP,
		ROOM_OBJS
	} section = NONE;

	int x;
	int rv;
	int n, s, e, w;
	FILE * f;
	room_t * rooms = malloc(sizeof(room_t) * 4);
	char str[160]; /* This limits the length of room descriptions */
	str[159] = 0;

	/* probably should move this somewhere else */
	for (x = 0; x < 4; x++) {
		rooms[x].description = NULL;
		rooms[x].north = NULL;
		rooms[x].south = NULL;
		rooms[x].east  = NULL;
		rooms[x].west  = NULL;
		rooms[x].items = NULL;
	}

	f = fopen("data", "r");
	assert(f != NULL);

	/* KLUDGE - stupid assignment trick by Andrew */
	while ((rv = fscanf(f, "%d ", &x)) && rv != EOF) {
		if (-1 == x) {
			while ((rv = fgetc(f)) != '\n' && rv != EOF);
			section++;
		} else {
			switch (section) {
			case NONE:
				fprintf(stderr, "Error: Yous si fidodin somehtin srong.\n");
				break;

			case ROOM_DESC:
				assert(x < 4);
				rooms[x].description = getstring('\n', f);
				break;

			case ROOM_LINKS:
				assert(4 == fscanf(f, " %d %d %d %d\n", &n, &s, &e, &w));

				assert(x < 4);
				assert(n < 4);
				assert(s < 4);
				assert(e < 4);
				assert(w < 4);

				rooms[x].north = n != -1 ? rooms + n : NULL;
				rooms[x].south = s != -1 ? rooms + s : NULL;
				rooms[x].east  = e != -1 ? rooms + e : NULL;
				rooms[x].west  = w != -1 ? rooms + w : NULL;
				break;

			case OBJ_PROP:
				fgets(str, 159, f);
				break;

			case ROOM_OBJS:
				fgets(str, 159, f);
				break;

			default:
				fgets(str, 159, f);
				fprintf(stderr, "Error: default case reached?\n");
				break;
			}
		}
	}

	fclose(f);

	room = rooms;
	return;
}

/* Describes the situation */
int watsup(){
	/* This function should use a data file of rooms and a global room number to print the room description.
	   The room description should be able to change even within the same room.
	   Semi-random events should also be printed. (e.g. "Chris falls out of his chair")
	   Additionally, it should list objects in the room. */

	assert(room != NULL);

	puts(room->description);

	return 0;
}

void go(compass c)
{
	room_t * n;

	switch (c) {
	case NORTH: n = room->north; break;
	case SOUTH: n = room->south; break;
	case EAST:  n = room->east;  break;
	case WEST:  n = room->west;  break;
	}

	if (n == NULL) {
		puts("You cannot go that way.");
	} else {
		room = n;
		watsup();
	}
}

int main(){
	int dead=0,win=0,quit=0;
	char inp[80];
	inp[79] = 0;

	/* Load data, create world */
	init();
	watsup();

	/* loop runs watsup() and inner loop takes commands until one works */
	while(!dead && !win && !quit){
		printf("What do? ");
		scanf ("%79s",inp); /* Get commands */

		if (!strncmp(inp,"quit",4)) quit=1; /* Check if quitting */
		if (!strncmp(inp,"north",5)) go(NORTH);
		if (!strncmp(inp,"south",5)) go(SOUTH);
		if (!strncmp(inp,"east",4)) go(EAST);
		if (!strncmp(inp,"west",4)) go(WEST);

		/* This is where a parse function would be called, and it would call other functons accordingly.
		   To get started, let's implement "go <dir>, take <obj>, look, eat <inv item>. */
	}
	return 0;
}
