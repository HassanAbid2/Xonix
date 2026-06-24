#pragma once

// Grid dimensions
const int M = 50;
const int N = 80;

// Grid declaration
extern int grid[M][N]; 

// Tile size
extern int ts;  


// Game states
const int STATE_MAIN_SCREEN = 0;
const int STATE_LOGIN = 1;
const int STATE_MAIN_MENU = 2;
const int STATE_PLAYING_SINGLE = 3;
const int STATE_PLAYING_MULTI = 4;  
const int STATE_PAUSED = 5;
const int STATE_START_MENU = 6;
const int STATE_SIGNUP = 7;
const int STATE_LEADERBOARD = 8;               
const int STATE_MATCH_SELECTION = 9;          
const int STATE_OFFLINE_MULTIPLAYER_SCREEN = 10; 
const int STATE_OFFLINE_MULTIPLAYER = 11;     
const int STATE_PLAYING_MATCH = 12;
const int STATE_INVENTORY = 13;               
const int STATE_GAMEOVER = 14;