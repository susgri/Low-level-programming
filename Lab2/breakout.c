/***************************************************************************************************
 * DON'T REMOVE THE VARIABLES BELOW THIS COMMENT                                                   *
 **************************************************************************************************/
unsigned long long __attribute__((used)) VGAaddress = 0xc8000000; // Memory storing pixels
unsigned int __attribute__((used)) red = 0x0000F0F0;
unsigned int __attribute__((used)) green = 0x00000F0F;
unsigned int __attribute__((used)) blue = 0x000000FF;
unsigned int __attribute__((used)) white = 0xFFFF;
unsigned int __attribute__((used)) black = 0x0;

// Don't change the name of this variables
#define NCOLS 10 // <- Supported value range: [1,18]
#define NROWS 16 // <- This variable might change.
#define TILE_SIZE 15 // <- Tile size, might change.

char *won = "You Won";       // DON'T TOUCH THIS - keep the string as is
char *lost = "You Lost";     // DON'T TOUCH THIS - keep the string as is
unsigned short height = 240; // DON'T TOUCH THIS - keep the value as is
unsigned short width = 320;  // DON'T TOUCH THIS - keep the value as is
char font8x8[128][8];        // DON'T TOUCH THIS - this is a forward declaration
unsigned char tiles[NROWS][NCOLS] __attribute__((used)) = { 0 }; // DON'T TOUCH THIS - this is the tile map
/**************************************************************************************************/

/***
 * Variable definitions below this comment
 */
// {blue, pink, red, green, yellow , cyan}
unsigned int colors[6] = {0x641E, 0xF337, 0x87CC, 0xF20A, 0xF70C, 0x67DD};

int ballShape[7][7] = {
    {0, 0, 0, 1, 0, 0, 0},
    {0, 0, 1, 1, 1, 0, 0},
    {0, 1, 1, 1, 1, 1, 0},
    {1, 1, 1, 1, 1, 1, 1},
    {0, 1, 1, 1, 1, 1, 0},
    {0, 0, 1, 1, 1, 0, 0},
    {0, 0, 0, 1, 0, 0, 0}
};

/***
 * You might use and modify the struct/enum definitions below this comment
 */
typedef struct _ball 
{
    unsigned int pos_x;
    unsigned int pos_y;
    unsigned int old_pos_x;
    unsigned int old_pos_y;
    unsigned int diameter;
    unsigned int angle;
} Ball;
Ball gameBall = {20, 120, 21, 120, 7, 270};

typedef struct _bar
{
    unsigned int pos_x;
    unsigned int pos_y;
    unsigned int height;
    unsigned int width;
    unsigned int changePos;

} Bar;
Bar gameBar = {0, 120, 45, 7, 1};
 
typedef struct _block
{
    unsigned char destroyed;
    unsigned char deleted;
    unsigned int pos_x;
    unsigned int pos_y;
    unsigned int color;
} Block;

typedef enum _gameState
{
    Stopped = 0,
    Running = 1,
    Won = 2,
    Lost = 3,
    Exit = 4,
} GameState;
GameState currentState = Stopped;

/***
 * Here follow the C declarations for our assembly functions
 */
void ClearScreen(); 
// Don't modify the function headers
void SetPixel(unsigned int x_coord, unsigned int y_coord, unsigned int color);
int ReadUart();
void WriteUart(char c);

/***
 * Now follow the assembly implementations
 */

// It must only clear the VGA screen, and not clear any game state
asm("ClearScreen: \n\t"
    "    PUSH {LR} \n\t"
    "    PUSH {R4, R5} \n\t"
    "    LDR R2, =0xFFFF \n\t"
    "    LDR R3, =VGAaddress \n\t"
    "    MOV R4, #0 \n\t"
    "clearScreenLoop: CMP R4, #320 \n\t"
    "    BGE doneLoopX \n\t"
    "    MOV R5, #0 \n\t"
    "loopY: CMP R5, #240 \n\t"
    "    BGE doneLoopY \n\t"
    "    MOV R0, R4 \n\t"
    "    MOV R1, R5 \n\t"
    "    BL SetPixel \n\t"
    "    ADD R5, R5, #1 \n\t"
    "    B loopY \n\t"
    "doneLoopY: ADD R4, R4, #1 \n\t"
    "     B clearScreenLoop \n\t"
    "doneLoopX: \n\t"
    "    POP {R4,R5} \n\t"
    "    POP {LR} \n\t"
    "    BX LR");

// assumes R0 = x-coord, R1 = y-coord, R2 = colorvalue
asm("SetPixel: \n\t"
    "LDR R3, =VGAaddress \n\t"
    "LDR R3, [R3] \n\t"
    "LSL R1, R1, #10 \n\t"
    "LSL R0, R0, #1 \n\t"
    "ADD R1, R0 \n\t"
    "STRH R2, [R3,R1] \n\t"
    "BX LR");

asm("ReadUart:\n\t"
    "LDR R1, =0xFF201000 \n\t"
    "LDR R0, [R1]\n\t"
    "BX LR");

asm("WriteUart: \n\t"
    "LDR R1, =0xFF201000 \n\t"
    "LDR R2, [R1, #4] \n\t"
    "LDR R3, =0xffff0000 \n\t"
    "ANDS R2, R2, R3 \n\t"
    "BEQ WriteUart \n\t"
    "STR R0, [R1] \n\t"
    "BX LR");

// Don't modify any function header
void draw_block(unsigned int x, unsigned int y, unsigned int width, unsigned int height, unsigned int color)
{
    unsigned int x_coord = x;
    for(int i=0; i<width; i++){
        unsigned int y_coord = y;
        for(int j=0; j<height; j++){
            SetPixel(x_coord, y_coord, color);
            y_coord++;
        }
        x_coord++;
    }

}

void draw_bar(unsigned int y)
{
    unsigned int topOfBar = y - (gameBar.height/2);
    if(gameBar.changePos){
        draw_block(0, 0, gameBar.width, height, white);
        draw_block(gameBar.pos_x, topOfBar, gameBar.width, gameBar.height/3, 0xF337);
        draw_block(gameBar.pos_x, topOfBar+gameBar.height/3, gameBar.width, gameBar.height/3, 0x641E); 
        draw_block(gameBar.pos_x, topOfBar+2*gameBar.height/3, gameBar.width, gameBar.height/3, 0xF337);
        gameBar.changePos = 0;
    }
}

void draw_ball()
{   
    int x = gameBall.pos_x;
    int y = gameBall.pos_y;


    // remove old drawing of ball
    for(int i=0; i<gameBall.diameter; i++){
        for(int j=0; j<gameBall.diameter; j++){
            if(ballShape[i][j] == 1){
                SetPixel(gameBall.old_pos_x+i, gameBall.old_pos_y+j, white);
            }
        }
    }

    // draw ball in new position
    for(int i=0; i<gameBall.diameter; i++){
        for(int j=0; j<gameBall.diameter; j++){
            if(ballShape[i][j] == 1){
                SetPixel(x+i, y+j, black);
            }
        }
    }
}

void draw_playing_field()
{
    const int numColors = 6;
    int color_index = 0;

    int x_pos = 320-(15*NCOLS); // start of playing field

    for(int i=0; i<NCOLS; i++){
        int y_pos = 0;

        for(int j=0; j<NROWS; j++){
            if(!tiles[j][i]){
                draw_block(x_pos+1, y_pos+1, TILE_SIZE-2, TILE_SIZE-2, colors[color_index]);
            }
            else if (tiles[j][i] == 'h'){ // if block has been hit and not removed
                draw_block(x_pos+1, y_pos+1, TILE_SIZE-2, TILE_SIZE-2, white);
                tiles[j][i] = 'x'; // sets block as hit and removed
            }

            y_pos += TILE_SIZE;

            // color of next block
            if(color_index == (numColors-1)){
                color_index = 0;
            }
            else{
                color_index++;
            }
            
        }
        x_pos += TILE_SIZE;
    }

}

void update_game_state()
{
    if (currentState != Running)
    {
        return;
    }

    if(gameBall.pos_x < 7){
        currentState = Lost;
        return;
    }

    if(gameBall.pos_x == 320){
        currentState = Won;
        return;
    }
    
    switch (gameBall.angle)
    {
    case 0:
    case 360:
        gameBall.old_pos_x = gameBall.pos_x;
        gameBall.old_pos_y = gameBall.pos_y;
        gameBall.pos_y--;
        break;
    case 45:
        gameBall.old_pos_x = gameBall.pos_x;
        gameBall.old_pos_y = gameBall.pos_y;
        gameBall.pos_x++;
        gameBall.pos_y--;
        break;
    case 90:
        gameBall.old_pos_x = gameBall.pos_x;
        gameBall.old_pos_y = gameBall.pos_y;
        gameBall.pos_x++;
        break;
    case 135:
        gameBall.old_pos_x = gameBall.pos_x;
        gameBall.old_pos_y = gameBall.pos_y;
        gameBall.pos_x++;
        gameBall.pos_y++;
        break;
    case 180:
        gameBall.old_pos_x = gameBall.pos_x;
        gameBall.old_pos_y = gameBall.pos_y;
        gameBall.pos_y++;
        break;
    case 225:
        gameBall.old_pos_x = gameBall.pos_x;
        gameBall.old_pos_y = gameBall.pos_y;
        gameBall.pos_x--;
        gameBall.pos_y++;
        break;
    case 270:
        gameBall.old_pos_x = gameBall.pos_x;
        gameBall.old_pos_y = gameBall.pos_y;
        gameBall.pos_x--;
        break;
    case 315:
        gameBall.old_pos_x = gameBall.pos_x;
        gameBall.old_pos_y = gameBall.pos_y;
        gameBall.pos_x--;
        gameBall.pos_y--;
        break;
    default:
        break;
    }

    const unsigned int num_collision_pnts = 4;
    // array of all four collision points
    unsigned int collision_points[4][2] = {
        {gameBall.pos_x+gameBall.diameter-1, gameBall.pos_y + gameBall.diameter/2},
        {gameBall.pos_x+gameBall.diameter/2, gameBall.pos_y+gameBall.diameter-1},
        {gameBall.pos_x+gameBall.diameter/2, gameBall.pos_y},
        {gameBall.pos_x, gameBall.pos_y+gameBall.diameter/2}
    };

    // check if ball is hitting one or more blocks 
    if(gameBall.pos_x+gameBall.diameter-1 >= width - NCOLS*TILE_SIZE){
        for(int i = 0; i < num_collision_pnts; i++){
            unsigned int x_pnt = collision_points[i][0];
            unsigned int y_pnt = collision_points[i][1];

            // Finding out which block the ball can hit
            unsigned int x_indx = (x_pnt - (width - TILE_SIZE*NCOLS)) / TILE_SIZE;
            unsigned int y_indx = y_pnt / TILE_SIZE;

            if (x_indx < NCOLS && x_indx >= 0 && y_indx < NROWS && y_indx >= 0 && !tiles[y_indx][x_indx]){
                tiles[y_indx][x_indx] = 'h'; // mark block as hit
                switch (gameBall.angle){
                    case 360:
                    case 0: gameBall.angle = 180; break;
                    case 45: gameBall.angle = 315; break;
                    case 90: gameBall.angle = 270; break;
                    case 135: gameBall.angle = 225; break;
                    case 180: gameBall.angle = 0; break;
                    case 225: gameBall.angle = 135; break;
                    case 270: gameBall.angle = 90; break;
                    case 315: gameBall.angle = 45; break;
                    default: break;
                }


                // check if ball hit border with neighbour block
                unsigned int check_neighbour_y = y_pnt % TILE_SIZE;
                unsigned int check_neighbour_x = (x_pnt - (width - TILE_SIZE*NCOLS)) % TILE_SIZE;

                if(check_neighbour_y == 0 && y_indx < NROWS-1){
                    if(!tiles[y_indx-1][x_indx]) tiles[y_indx-1][x_indx] = 'h';
                }
                else if(check_neighbour_y == TILE_SIZE - 1 && y_indx > 0){
                    if (!tiles[y_indx+1][x_indx]) tiles[y_indx+1][x_indx] = 'h';
                }

                if(check_neighbour_x == 0 && x_indx > 0){
                    if (!tiles[y_indx][x_indx-1]) tiles[y_indx][x_indx+1] = 'h';
                }
                else if(check_neighbour_x == TILE_SIZE-1 && x_indx < NCOLS-1){

                    if (!tiles[y_indx][x_indx+1]) tiles[y_indx][x_indx-1] = 'h';
                }
                
            }            
        }
    }
    else{
        // check if ball is hitting the bar
        if(gameBall.pos_x == gameBar.pos_x+gameBar.width){
            if(gameBall.pos_y >= (gameBar.pos_y - gameBar.height/2) && gameBall.pos_y <= (gameBar.pos_y + gameBar.height/2)){
                if(gameBall.pos_y < ((gameBar.pos_y - gameBar.height/2) + gameBar.height/3)){
                    gameBall.angle = 45;
                }
                else if(gameBall.pos_y < ((gameBar.pos_y - gameBar.height/2) + 2*gameBar.height/3)){
                    gameBall.angle = 90;
                }
                else{
                    gameBall.angle = 135;
                }
            }
        }
    }
    // check if ball is hitting the walls
    if(gameBall.pos_y == 0 || gameBall.pos_y+gameBall.diameter == height){
        
            switch (gameBall.angle)
            {
            case 45:
            case 225:
                gameBall.angle += 90;
                break;
            case 135:
            case 315:
                gameBall.angle -= 90;
                break;
            default:
                gameBall.angle += 180;
                if (gameBall.angle > 360)
                {
                    gameBall.angle -= 360;
                }
                break;
            }
        }
}

void update_bar_state() {
    int readValue = ReadUart();
    int remaining = (readValue & 0xFF0000) >> 16;

    if(!(readValue & 0x8000)){ // if ready bit is not set -> return
        return;
    }   

    while (remaining >= 0){
        int button = readValue & 0x000000FF; 
        if (button == 's' && (gameBar.pos_y+gameBar.height/2) <= (height-15)){ // if 's' is pressed and the bar can move down
            gameBar.pos_y += 15;
            gameBar.changePos = 1; // makes sure the bar is drawed again
            break; // break to change position of bar
        }
        else if (button == 'w' && (gameBar.pos_y-gameBar.height/2) >= 15){ // if 'w' is pressed and the bar can move up
            gameBar.pos_y -= 15;
            gameBar.changePos = 1;
            break;
        }

        // check if there is more input 
        readValue = ReadUart();
        if(!(readValue & 0x8000)) break;
        remaining = (readValue & 0xFF0000) >> 16;
    }
}

void write(char *str)
{
    int index = 0;
    char character = str[index];

    while(character != 0){
        WriteUart(character);
        index++;
        character = str[index];
    }
}

void play()
{
    ClearScreen();
    // HINT: This is the main game loop
    while (1)
    {
        update_game_state();
        update_bar_state();
        if (currentState != Running)
        {
            break;
        }
        draw_playing_field();
        draw_ball();
        draw_bar(gameBar.pos_y);
    }
    if (currentState == Won)
    {
        write(won);
    }
    else if (currentState == Lost)
    {
        write(lost);
    }
    else if (currentState == Exit)
    {
        return;
    }
    currentState = Stopped;
}

// It must initialize the game
void reset()
{

    if (currentState == Exit) return; 

    write("\nResetting...");
    write("\n----------\n");
    // Hint: This is draining the UART buffer
    int remaining = 0;
    
    do
    {
        unsigned long long out = ReadUart();
        if (!(out & 0x8000))
        {
            // not valid - abort reading
            break;
        }
        remaining = (out & 0xFF0000) >> 16;
    } while (remaining > 0);


    // reset bar and ball position/angle
    gameBar.pos_y = 120;
    gameBar.changePos = 1;
    gameBall.angle = 45;
    gameBall.pos_x = 120;
    gameBall.pos_y = 40;

    // reset all tiles as non-destroyed 
    for(int i=0; i<NROWS; i++){
        for(int j=0; j<NCOLS; j++){
            tiles[i][j] = 0;
        }
    }
    
    ClearScreen();
    currentState = Stopped;
}

void wait_for_start()
{
    write("\nWrite 'w' or 's' to start the game.\nPress enter to exit the game.\n");
    write("------------------------------------\n");

    while (1) {
        int readValue = ReadUart();
        int readyBit = readValue & 0x8000;
        
        if(readyBit){ // if ready bit is set
            int remaining = (readValue & 0xFF0000) >> 16;
            
            while (remaining >= 0) {
                int button = readValue & 0x000000FF; 
                if (button == 'w' || button == 's'){
                    currentState = Running;
                    write("\nStarted the game!\n");
                    write("------------------\n");
                    return;
                }
                else if (button == '\n'){
                    currentState = Exit;
                    return;
                }
                
                // Read next value
                readValue = ReadUart();
                remaining = (readValue & 0xFF0000) >> 16;
            }
            
        }
    }
}

int main(int argc, char *argv[])
{
    // HINT: This loop allows the user to restart the game after loosing/winning the previous game
    while (1)
    {
        if(NCOLS > 18 || NCOLS < 1){
            write("\nYou do not have a playable configuration. Fix number of columns to be within range [1, 18] \n");
            break;
        }
        wait_for_start();
        play();
        reset();
        if (currentState == Exit)
        {
            write("\nExited the game :)\n");
            write("---------------------\n");
            break;
        }
    }
    return 0;
}

// THIS IS FOR THE OPTIONAL TASKS ONLY

// HINT: How to access the correct bitmask
// sample: to get character a's bitmask, use
// font8x8['a']
char font8x8[128][8] = {
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // U+0000 (nul)
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // U+0001
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // U+0002
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // U+0003
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // U+0004
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // U+0005
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // U+0006
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // U+0007
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // U+0008
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // U+0009
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // U+000A
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // U+000B
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // U+000C
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // U+000D
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // U+000E
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // U+000F
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // U+0010
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // U+0011
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // U+0012
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // U+0013
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // U+0014
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // U+0015
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // U+0016
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // U+0017
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // U+0018
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // U+0019
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // U+001A
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // U+001B
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // U+001C
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // U+001D
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // U+001E
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // U+001F
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // U+0020 (space)
    {0x18, 0x3C, 0x3C, 0x18, 0x18, 0x00, 0x18, 0x00}, // U+0021 (!)
    {0x36, 0x36, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // U+0022 (")
    {0x36, 0x36, 0x7F, 0x36, 0x7F, 0x36, 0x36, 0x00}, // U+0023 (#)
    {0x0C, 0x3E, 0x03, 0x1E, 0x30, 0x1F, 0x0C, 0x00}, // U+0024 ($)
    {0x00, 0x63, 0x33, 0x18, 0x0C, 0x66, 0x63, 0x00}, // U+0025 (%)
    {0x1C, 0x36, 0x1C, 0x6E, 0x3B, 0x33, 0x6E, 0x00}, // U+0026 (&)
    {0x06, 0x06, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00}, // U+0027 (')
    {0x18, 0x0C, 0x06, 0x06, 0x06, 0x0C, 0x18, 0x00}, // U+0028 (()
    {0x06, 0x0C, 0x18, 0x18, 0x18, 0x0C, 0x06, 0x00}, // U+0029 ())
    {0x00, 0x66, 0x3C, 0xFF, 0x3C, 0x66, 0x00, 0x00}, // U+002A (*)
    {0x00, 0x0C, 0x0C, 0x3F, 0x0C, 0x0C, 0x00, 0x00}, // U+002B (+)
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x0C, 0x06}, // U+002C (,)
    {0x00, 0x00, 0x00, 0x3F, 0x00, 0x00, 0x00, 0x00}, // U+002D (-)
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x0C, 0x00}, // U+002E (.)
    {0x60, 0x30, 0x18, 0x0C, 0x06, 0x03, 0x01, 0x00}, // U+002F (/)
    {0x3E, 0x63, 0x73, 0x7B, 0x6F, 0x67, 0x3E, 0x00}, // U+0030 (0)
    {0x0C, 0x0E, 0x0C, 0x0C, 0x0C, 0x0C, 0x3F, 0x00}, // U+0031 (1)
    {0x1E, 0x33, 0x30, 0x1C, 0x06, 0x33, 0x3F, 0x00}, // U+0032 (2)
    {0x1E, 0x33, 0x30, 0x1C, 0x30, 0x33, 0x1E, 0x00}, // U+0033 (3)
    {0x38, 0x3C, 0x36, 0x33, 0x7F, 0x30, 0x78, 0x00}, // U+0034 (4)
    {0x3F, 0x03, 0x1F, 0x30, 0x30, 0x33, 0x1E, 0x00}, // U+0035 (5)
    {0x1C, 0x06, 0x03, 0x1F, 0x33, 0x33, 0x1E, 0x00}, // U+0036 (6)
    {0x3F, 0x33, 0x30, 0x18, 0x0C, 0x0C, 0x0C, 0x00}, // U+0037 (7)
    {0x1E, 0x33, 0x33, 0x1E, 0x33, 0x33, 0x1E, 0x00}, // U+0038 (8)
    {0x1E, 0x33, 0x33, 0x3E, 0x30, 0x18, 0x0E, 0x00}, // U+0039 (9)
    {0x00, 0x0C, 0x0C, 0x00, 0x00, 0x0C, 0x0C, 0x00}, // U+003A (:)
    {0x00, 0x0C, 0x0C, 0x00, 0x00, 0x0C, 0x0C, 0x06}, // U+003B (;)
    {0x18, 0x0C, 0x06, 0x03, 0x06, 0x0C, 0x18, 0x00}, // U+003C (<)
    {0x00, 0x00, 0x3F, 0x00, 0x00, 0x3F, 0x00, 0x00}, // U+003D (=)
    {0x06, 0x0C, 0x18, 0x30, 0x18, 0x0C, 0x06, 0x00}, // U+003E (>)
    {0x1E, 0x33, 0x30, 0x18, 0x0C, 0x00, 0x0C, 0x00}, // U+003F (?)
    {0x3E, 0x63, 0x7B, 0x7B, 0x7B, 0x03, 0x1E, 0x00}, // U+0040 (@)
    {0x0C, 0x1E, 0x33, 0x33, 0x3F, 0x33, 0x33, 0x00}, // U+0041 (A)
    {0x3F, 0x66, 0x66, 0x3E, 0x66, 0x66, 0x3F, 0x00}, // U+0042 (B)
    {0x3C, 0x66, 0x03, 0x03, 0x03, 0x66, 0x3C, 0x00}, // U+0043 (C)
    {0x1F, 0x36, 0x66, 0x66, 0x66, 0x36, 0x1F, 0x00}, // U+0044 (D)
    {0x7F, 0x46, 0x16, 0x1E, 0x16, 0x46, 0x7F, 0x00}, // U+0045 (E)
    {0x7F, 0x46, 0x16, 0x1E, 0x16, 0x06, 0x0F, 0x00}, // U+0046 (F)
    {0x3C, 0x66, 0x03, 0x03, 0x73, 0x66, 0x7C, 0x00}, // U+0047 (G)
    {0x33, 0x33, 0x33, 0x3F, 0x33, 0x33, 0x33, 0x00}, // U+0048 (H)
    {0x1E, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x1E, 0x00}, // U+0049 (I)
    {0x78, 0x30, 0x30, 0x30, 0x33, 0x33, 0x1E, 0x00}, // U+004A (J)
    {0x67, 0x66, 0x36, 0x1E, 0x36, 0x66, 0x67, 0x00}, // U+004B (K)
    {0x0F, 0x06, 0x06, 0x06, 0x46, 0x66, 0x7F, 0x00}, // U+004C (L)
    {0x63, 0x77, 0x7F, 0x7F, 0x6B, 0x63, 0x63, 0x00}, // U+004D (M)
    {0x63, 0x67, 0x6F, 0x7B, 0x73, 0x63, 0x63, 0x00}, // U+004E (N)
    {0x1C, 0x36, 0x63, 0x63, 0x63, 0x36, 0x1C, 0x00}, // U+004F (O)
    {0x3F, 0x66, 0x66, 0x3E, 0x06, 0x06, 0x0F, 0x00}, // U+0050 (P)
    {0x1E, 0x33, 0x33, 0x33, 0x3B, 0x1E, 0x38, 0x00}, // U+0051 (Q)
    {0x3F, 0x66, 0x66, 0x3E, 0x36, 0x66, 0x67, 0x00}, // U+0052 (R)
    {0x1E, 0x33, 0x07, 0x0E, 0x38, 0x33, 0x1E, 0x00}, // U+0053 (S)
    {0x3F, 0x2D, 0x0C, 0x0C, 0x0C, 0x0C, 0x1E, 0x00}, // U+0054 (T)
    {0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x3F, 0x00}, // U+0055 (U)
    {0x33, 0x33, 0x33, 0x33, 0x33, 0x1E, 0x0C, 0x00}, // U+0056 (V)
    {0x63, 0x63, 0x63, 0x6B, 0x7F, 0x77, 0x63, 0x00}, // U+0057 (W)
    {0x63, 0x63, 0x36, 0x1C, 0x1C, 0x36, 0x63, 0x00}, // U+0058 (X)
    {0x33, 0x33, 0x33, 0x1E, 0x0C, 0x0C, 0x1E, 0x00}, // U+0059 (Y)
    {0x7F, 0x63, 0x31, 0x18, 0x4C, 0x66, 0x7F, 0x00}, // U+005A (Z)
    {0x1E, 0x06, 0x06, 0x06, 0x06, 0x06, 0x1E, 0x00}, // U+005B ([)
    {0x03, 0x06, 0x0C, 0x18, 0x30, 0x60, 0x40, 0x00}, // U+005C (\)
    {0x1E, 0x18, 0x18, 0x18, 0x18, 0x18, 0x1E, 0x00}, // U+005D (])
    {0x08, 0x1C, 0x36, 0x63, 0x00, 0x00, 0x00, 0x00}, // U+005E (^)
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF}, // U+005F (_)
    {0x0C, 0x0C, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00}, // U+0060 (`)
    {0x00, 0x00, 0x1E, 0x30, 0x3E, 0x33, 0x6E, 0x00}, // U+0061 (a)
    {0x07, 0x06, 0x06, 0x3E, 0x66, 0x66, 0x3B, 0x00}, // U+0062 (b)
    {0x00, 0x00, 0x1E, 0x33, 0x03, 0x33, 0x1E, 0x00}, // U+0063 (c)
    {0x38, 0x30, 0x30, 0x3e, 0x33, 0x33, 0x6E, 0x00}, // U+0064 (d)
    {0x00, 0x00, 0x1E, 0x33, 0x3f, 0x03, 0x1E, 0x00}, // U+0065 (e)
    {0x1C, 0x36, 0x06, 0x0f, 0x06, 0x06, 0x0F, 0x00}, // U+0066 (f)
    {0x00, 0x00, 0x6E, 0x33, 0x33, 0x3E, 0x30, 0x1F}, // U+0067 (g)
    {0x07, 0x06, 0x36, 0x6E, 0x66, 0x66, 0x67, 0x00}, // U+0068 (h)
    {0x0C, 0x00, 0x0E, 0x0C, 0x0C, 0x0C, 0x1E, 0x00}, // U+0069 (i)
    {0x30, 0x00, 0x30, 0x30, 0x30, 0x33, 0x33, 0x1E}, // U+006A (j)
    {0x07, 0x06, 0x66, 0x36, 0x1E, 0x36, 0x67, 0x00}, // U+006B (k)
    {0x0E, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x1E, 0x00}, // U+006C (l)
    {0x00, 0x00, 0x33, 0x7F, 0x7F, 0x6B, 0x63, 0x00}, // U+006D (m)
    {0x00, 0x00, 0x1F, 0x33, 0x33, 0x33, 0x33, 0x00}, // U+006E (n)
    {0x00, 0x00, 0x1E, 0x33, 0x33, 0x33, 0x1E, 0x00}, // U+006F (o)
    {0x00, 0x00, 0x3B, 0x66, 0x66, 0x3E, 0x06, 0x0F}, // U+0070 (p)
    {0x00, 0x00, 0x6E, 0x33, 0x33, 0x3E, 0x30, 0x78}, // U+0071 (q)
    {0x00, 0x00, 0x3B, 0x6E, 0x66, 0x06, 0x0F, 0x00}, // U+0072 (r)
    {0x00, 0x00, 0x3E, 0x03, 0x1E, 0x30, 0x1F, 0x00}, // U+0073 (s)
    {0x08, 0x0C, 0x3E, 0x0C, 0x0C, 0x2C, 0x18, 0x00}, // U+0074 (t)
    {0x00, 0x00, 0x33, 0x33, 0x33, 0x33, 0x6E, 0x00}, // U+0075 (u)
    {0x00, 0x00, 0x33, 0x33, 0x33, 0x1E, 0x0C, 0x00}, // U+0076 (v)
    {0x00, 0x00, 0x63, 0x6B, 0x7F, 0x7F, 0x36, 0x00}, // U+0077 (w)
    {0x00, 0x00, 0x63, 0x36, 0x1C, 0x36, 0x63, 0x00}, // U+0078 (x)
    {0x00, 0x00, 0x33, 0x33, 0x33, 0x3E, 0x30, 0x1F}, // U+0079 (y)
    {0x00, 0x00, 0x3F, 0x19, 0x0C, 0x26, 0x3F, 0x00}, // U+007A (z)
    {0x38, 0x0C, 0x0C, 0x07, 0x0C, 0x0C, 0x38, 0x00}, // U+007B ({)
    {0x18, 0x18, 0x18, 0x00, 0x18, 0x18, 0x18, 0x00}, // U+007C (|)
    {0x07, 0x0C, 0x0C, 0x38, 0x0C, 0x0C, 0x07, 0x00}, // U+007D (})
    {0x6E, 0x3B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // U+007E (~)
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}  // U+007F
};