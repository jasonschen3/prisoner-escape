#ifndef MAIN_H
#define MAIN_H

#include "gba.h"

// TODO: Create any necessary structs

/*
 * For example, for a Snake game, one could be:
 *
 * struct snake {
 *   int heading;
 *   int length;
 *   int row;
 *   int col;
 * };
 *
 * Example of a struct to hold state machine data:
 *
 * struct state {
 *   int currentState;
 *   int nextState;
 * };
 *
 */

struct sprite
{
    int row;
    int col;
    int width;
    int height;
};

struct movingSprite
{
    int row; // starting pos
    int col;
    int width; // dimensions
    int height;

    int vel;
};

struct block
{
    int row;
    int col;
    int width;
    int height;
    volatile u16 color;
};
#endif