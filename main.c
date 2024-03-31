#include "main.h"

#include <stdio.h>
#include <stdlib.h>

#include "gba.h"
#include "custom_imgs.h"
#define BACKGROUND_COLOR WHITE

// Needed for funcs defined below main
int isCollide(struct sprite player, struct movingSprite cop);
int isWin(struct sprite player, struct block winLoc);
int isCollideBlock(struct sprite player, struct block block);

// Add any additional states you need for your app. You are not required to use
// these specific provided states.
enum gba_state
{
  START,
  PLAY,
  WIN,
  LOSE,
};

void initialize(struct sprite *player, struct movingSprite *cop1, struct movingSprite *cop2, struct movingSprite *cop3, struct movingSprite *cop4, struct movingSprite *cop5, struct block *winLoc, struct movingSprite *startingSprite)
{
  player->row = 150;
  player->col = 10;
  player->width = 7;
  player->height = 8;

  // order goes up by height
  cop1->row = 110;
  cop1->col = 45;
  cop1->width = 10;
  cop1->height = 13;
  cop1->vel = 2;

  cop2->row = 85;
  cop2->col = 180;
  cop2->width = 10;
  cop2->height = 13;
  cop2->vel = 2;

  cop3->row = 60;
  cop3->col = 45;
  cop3->width = 10;
  cop3->height = 13;
  cop3->vel = 2;

  cop4->row = 50;
  cop4->col = 180;
  cop4->width = 10;
  cop4->height = 13;
  cop4->vel = 2;

  cop5->row = 40;
  cop5->col = 187;
  cop5->width = 10;
  cop5->height = 13;
  cop5->vel = 2;

  winLoc->row = 0;
  winLoc->col = 200;
  winLoc->width = 40;
  winLoc->height = 40;
  winLoc->color = GREEN;

  startingSprite->row = 80;
  startingSprite->col = 100;
  startingSprite->width = 70;
  startingSprite->height = 80;
  startingSprite->vel = 2;
  // drawImageWithTransparency(0, 0, 240, 160, start_screen);
  drawFullScreenImageDMA(start_screen);
  drawString(15, 40, "World's Hardest Prison Escape", BLACK);
  drawString(25, 80, "Jason Chen", BLACK);
  drawString(35, 60, "[Enter] to start", BLACK);

  // drawImageDMA(80, 85, 70, 80, prisoner_sprite);
  drawImageWithTransparency(startingSprite->row, startingSprite->col, startingSprite->width, startingSprite->height, prisoner_sprite);
}

// Global or static variables to keep track of time
static int frameCount = 0;
static int gameTimeSeconds = 0;
static int highScore = 100;

void updateTimer(void)
{
  // Increment the frame counter
  frameCount++;

  // If one second has passed (60 frames), increment the game time and reset the frame counter
  if (frameCount >= 60)
  {
    gameTimeSeconds++;
    frameCount = 0; // Reset the frame counter for the next second
  }
}

int main(void)
{
  REG_DISPCNT = MODE3 | BG2_ENABLE; // Display control, mode 3 for pixel manipulation

  // Save current and previous state of button input.
  u32 previousButtons = BUTTONS;
  u32 currentButtons = BUTTONS;

  // Load initial application state,
  struct sprite player;
  struct movingSprite startingSprite;
  struct movingSprite cop1;
  struct movingSprite cop2;
  struct movingSprite cop3;
  struct movingSprite cop4;
  struct movingSprite cop5;
  struct block winLoc;

  struct block b1;
  b1.color = BLACK;
  b1.col = 0;
  b1.row = 0;
  b1.height = 120;
  b1.width = 40;

  struct block b2;
  b2.color = BLACK;
  b2.col = 200;
  b2.row = 40;
  b2.height = 200;
  b2.width = 40;

  struct block b3;
  b3.color = BLACK;
  b3.row = 120;
  b3.col = 30;
  b3.width = 10;
  b3.height = 20;

  initialize(&player, &cop1, &cop2, &cop3, &cop4, &cop5, &winLoc, &startingSprite); // init values and starting screen

  enum gba_state state = START;

  while (1)
  {
    currentButtons = BUTTONS; // Load the current state of the buttons

    waitForVBlank();
    updateTimer();

    // global reset to title screen when click DEL
    if (KEY_JUST_PRESSED(BUTTON_SELECT, currentButtons, previousButtons))
    {
      state = START;
      initialize(&player, &cop1, &cop2, &cop3, &cop4, &cop5, &winLoc, &startingSprite);
    }
    switch (state)
    {
    case START:
      if (KEY_JUST_PRESSED(BUTTON_START, currentButtons, previousButtons))
      {
        // clear screen
        drawRectDMA(0, 0, 240, 160, WHITE);

        // draw the blocks
        drawRectDMA(b1.row, b1.col, b1.width, b1.height, b1.color);
        drawRectDMA(b2.row, b2.col, b2.width, b2.height, b2.color);
        drawRectDMA(b3.row, b3.col, b3.width, b3.height, b3.color);
        state = PLAY;

        // reset time
        gameTimeSeconds = 0;
        break;
      }

      // Undraw
      drawRectDMA(startingSprite.row, startingSprite.col, startingSprite.width, startingSprite.height, BACKGROUND_COLOR);

      // Logic
      if (startingSprite.col <= 85 || startingSprite.col >= 150)
      {
        startingSprite.vel *= -1;
      }
      startingSprite.col += startingSprite.vel;
      // Draw
      drawImageDMA(startingSprite.row, startingSprite.col, startingSprite.width, startingSprite.height, prisoner_sprite);

      break;

    case PLAY:
      //////////////////////// Undraw //////////////////////////////////
      drawRectDMA(player.row, player.col, player.width, player.height, BACKGROUND_COLOR);
      drawRectDMA(cop1.row, cop1.col, cop1.width, cop1.height, BACKGROUND_COLOR);
      drawRectDMA(cop2.row, cop2.col, cop2.width, cop2.height, BACKGROUND_COLOR);
      drawRectDMA(cop3.row, cop3.col, cop3.width, cop3.height, BACKGROUND_COLOR);
      drawRectDMA(cop4.row, cop4.col, cop4.width, cop4.height, BACKGROUND_COLOR);
      drawRectDMA(cop5.row, cop5.col, cop5.width, cop5.height, BACKGROUND_COLOR);
      // Cover up the old time
      drawRectDMA(b1.row, b1.col, b1.width, b1.height, b1.color);

      // Convert int to str
      char strTime[300];
      sprintf(strTime, "%d", gameTimeSeconds);
      drawString(10, 10, strTime, WHITE);

      //////////////////////// Game Logic //////////////////////////////////

      // GO UP
      if (KEY_DOWN(BUTTON_UP, currentButtons))
      {
        // Temporarily calculate the next position without actually moving
        int nextRow = player.row - 1;

        // Assume no collision initially
        int collisionWithBlock = 0;

        // Check for collision with each block
        if (isCollideBlock((struct sprite){nextRow, player.col, player.width, player.height}, b1) ||
            isCollideBlock((struct sprite){nextRow, player.col, player.width, player.height}, b2) ||
            isCollideBlock((struct sprite){nextRow, player.col, player.width, player.height}, b3))
        {
          collisionWithBlock = 1; // Found a collision
        }

        // Only move if there's no collision and within bounds
        if (!collisionWithBlock && nextRow >= 0)
        {
          player.row = nextRow;
        }
      }
      if (KEY_DOWN(BUTTON_DOWN, currentButtons))
      {
        int nextRow = player.row + 1;

        int collisionWithBlock = isCollideBlock((struct sprite){nextRow, player.col, player.width, player.height}, b1) ||
                                 isCollideBlock((struct sprite){nextRow, player.col, player.width, player.height}, b2) ||
                                 isCollideBlock((struct sprite){nextRow, player.col, player.width, player.height}, b3);

        // Check bounds and ensure no collision with blocks before moving
        if (!collisionWithBlock && nextRow + player.height <= HEIGHT)
        {
          player.row = nextRow;
        }
      }

      // GO LEFT
      if (KEY_DOWN(BUTTON_LEFT, currentButtons))
      {
        int nextCol = player.col - 1;

        int collisionWithBlock = isCollideBlock((struct sprite){player.row, nextCol, player.width, player.height}, b1) ||
                                 isCollideBlock((struct sprite){player.row, nextCol, player.width, player.height}, b2) ||
                                 isCollideBlock((struct sprite){player.row, nextCol, player.width, player.height}, b3);

        // Check bounds and ensure no collision with blocks before moving
        if (!collisionWithBlock && nextCol >= 0)
        {
          player.col = nextCol;
        }
      }
      if (KEY_DOWN(BUTTON_RIGHT, currentButtons))
      {
        int nextCol = player.col + 1;

        int collisionWithBlock = isCollideBlock((struct sprite){player.row, nextCol, player.width, player.height}, b1) ||
                                 isCollideBlock((struct sprite){player.row, nextCol, player.width, player.height}, b2) ||
                                 isCollideBlock((struct sprite){player.row, nextCol, player.width, player.height}, b3);

        // Check bounds and ensure no collision with blocks before moving
        if (!collisionWithBlock && nextCol + player.width <= WIDTH)
        {
          player.col = nextCol;
        }
      }

      // Lose condition if collides with cop
      if (isCollide(player, cop1) || isCollide(player, cop2) || isCollide(player, cop3) || isCollide(player, cop4) || isCollide(player, cop5))
      {
        state = LOSE;

        fillScreenDMA(RED);
        drawString(25, 40, "You lose :(", BLACK);
        break;
      }
      // Lose if take too long
      if (gameTimeSeconds >= 60)
      {
        state = LOSE;

        fillScreenDMA(RED);
        drawString(25, 40, "You lose :(", BLACK);
        break;
      }

      // Win condition if collides with green area
      if (isWin(player, winLoc))
      {
        state = WIN;

        drawFullScreenImageDMA(nature_sprite);
        drawString(25, 40, "You win!", BLACK);
        drawString(35, 40, "Time: ", BLACK);
        drawString(35, 75, strTime, BLACK);

        // Calc high score
        if (highScore > gameTimeSeconds)
        {
          highScore = gameTimeSeconds;
          drawString(55, 40, "NEW HIGH SCORE!", BLACK);
        }
        drawString(45, 40, "Highscore: ", BLACK);
        char strHS[300];
        sprintf(strHS, "%d", highScore);
        drawString(45, 105, strHS, BLACK);

        // drawImageWithTransparency(0, 0, 70, 80, prisoner_sprite);
        drawImageWithTransparency(125, 75, 7, 8, mini_prisoner);
        break;
      }

      if (cop1.col <= 42 || cop1.col >= 185)
      {
        cop1.vel *= -1;
      }
      if (cop2.col <= 42 || cop2.col >= 185)
      {
        cop2.vel *= -1;
      }
      if (cop3.col <= 42 || cop3.col >= 185)
      {
        cop3.vel *= -1;
      }
      if (cop4.col <= 42 || cop4.col >= 185)
      {
        cop4.vel *= -1;
      }
      if (cop5.row <= 0 || cop5.row >= 40)
      {
        cop5.vel *= -1;
      }
      cop1.col += cop1.vel;
      cop2.col += cop2.vel;
      cop3.col += cop3.vel;
      cop4.col += cop4.vel;
      cop5.row += cop5.vel;
      //////////////////////// DRAWING BELOW //////////////////////////////

      drawImageDMA(player.row, player.col, player.width, player.height, mini_prisoner);
      drawImageDMA(cop1.row, cop1.col, cop1.width, cop1.height, cop_sprite);
      drawImageDMA(cop2.row, cop2.col, cop2.width, cop2.height, cop_sprite);
      drawImageDMA(cop3.row, cop3.col, cop3.width, cop3.height, cop_sprite);
      drawImageDMA(cop4.row, cop4.col, cop4.width, cop4.height, cop_sprite);
      drawImageDMA(cop5.row, cop5.col, cop5.width, cop5.height, cop_sprite);
      drawRectDMA(winLoc.row, winLoc.col, winLoc.width, winLoc.height, winLoc.color);

      break;
    case WIN:
      break;
    case LOSE:
      break;
    }

    previousButtons = currentButtons; // Store the current state of the buttons
  }

  UNUSED(previousButtons); // You can remove this once previousButtons is used

  return 0;
}

// header is put above bc it's defined below
int isCollide(struct sprite player, struct movingSprite cop)
{
  return player.col < cop.col + cop.width &&
         player.col + player.width > cop.col &&
         player.row < cop.row + cop.height &&
         player.row + player.height > cop.row;
}
// define win condition if collide with winLoc
int isWin(struct sprite player, struct block winLoc)
{
  return player.col < winLoc.col + winLoc.width &&
         player.col + player.width > winLoc.col &&
         player.row < winLoc.row + winLoc.height &&
         player.row + player.height > winLoc.row;
}

int isCollideBlock(struct sprite player, struct block blk)
{
  return player.col < blk.col + blk.width &&
         player.col + player.width > blk.col &&
         player.row < blk.row + blk.height &&
         player.row + player.height > blk.row;
}