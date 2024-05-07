
// code for game FEH Survivors

// authored by Ryan Jung, Meenakshi Varadarajan, Agi Jobe


#include "FEHImages.h"

#include "FEHLCD.h"

#include "FEHUtility.h"

#include "FEHRandom.h"


#include <cmath>


const int WINDOW_WIDTH = 320;

const int WINDOW_HEIGHT = 240;


// define the structs for the weapons, including all the values that will change as they level up

struct Marble {

    int level = -1;

    int damage[3] = {1, 2, 3};

    int cooldown[3] = {3000, 2000, 1000};

};

struct Airfoil {

    int level = -1;

    int damage[3] = {1, 3, 5};

    int cooldown[3] = {2000, 1750, 1500};

    float moveBonus[3] = {0.2, 0.4, 0.6};

};

struct Beam {

    int level = -1;

    int damage[3] = {1, 2, 3};

    int length[3] = {40, 50, 70};

    int color[3] = {SILVER, BROWN, GRAY};

};

struct Circuit {

    int level = -1;

    int damage[3] = {1, 3, 5};

    int cooldown[3] = {10000, 9000, 8000};

    int hits[3] = {2, 4, 6};

};

struct Report {

    int level = -1;

    int healing[3] = {20, 45, 80};

};


// define the Items struct that will be used to hold all of the player's items

struct Items {

    Marble marble;

    Airfoil airfoil;

    Beam beam;

    Circuit circuit;

    Report report;

};


/*

*   general base class for a moving entity with health

*   enemies, attacks, and player all inherit from this class

*  

*   @author Ryan

*/

class Entity {

    protected:

        int health;

        float x, y, speed;

        float angleFacing;

        int width, height;

        char spriteName[30];

        FEHImage sprite;

    public:

        int getHealth();

        void setHealth(int set);

        float getX();

        float getY();

        int getWidth();

        int getHeight();

        float getAngle();

        void setSpeed(float set);

        void moveToPoint(float xTo, float yTo);

        bool isColliding(Entity other);

        bool isCollidingWithPoint(float x, float y);

        void drawSelf();

        void updateSprite(char[]);

        void hurt(int dmg);

};


// self explanatory gets and sets

int Entity::getHealth() {

    return health;

}

void Entity::setHealth(int set) {

    health = set;

}

float Entity::getX() {

    return x;

}

float Entity::getY() {

    return y;

}

int Entity::getWidth() {

    return width;

}

int Entity::getHeight() {

    return height;

}

float Entity::getAngle() {

    return angleFacing;

}

void Entity::setSpeed(float set) {

    speed = set;

}


/*

*   moves the entity to a point at their speed variable

*

*   @author: Ryan

*/

void Entity::moveToPoint(float xTo, float yTo) {


    float xMove, yMove;


    // calculate the x and y component sizes from the current position to the desired position

    float xDiff = xTo - x;

    float yDiff = yTo - y;


    // update anglefacing

    angleFacing = atan2(yDiff, xDiff);


    // uses the distance formula to find the length of the line from the current to desired position

    float length = sqrt(xDiff*xDiff + yDiff*yDiff);


    // don't move if close enough to the desired point (prevents position flickering)

    if (length <= 4) {

        xMove = 0;

        yMove = 0;

    } else {

        // normalize the vector

        xDiff /= length;

        yDiff /= length;

        // make vector length equal to speed

        xMove = xDiff * speed;

        yMove = yDiff * speed;

    }


    // move the entity

    x += xMove;

    y += yMove;


    // if the entity would be going over the border of the window, lock them to the edge so they don't go offscreen

    if (x > WINDOW_WIDTH - width) x = WINDOW_WIDTH - width;

    if (x < 0) x = 0;

    if (y > WINDOW_HEIGHT - height) y = WINDOW_HEIGHT - height;

    if (y < 0) y = 0;

}


/*

*   returns true if the entity is colliding with another entity

*

*   @author Ryan

*/

bool Entity::isColliding(Entity other) {


    // calculation to check if within bounds of other entity

    if (x < other.getX() + other.getWidth() &&

        x + width > other.getX() &&

        y < other.getY() + other.getHeight() &&

        y + height > other.getY()) {

       

        return true;

    }


    return false;

}


/*

*   returns true if the entity is colliding with a point in space

*

*   @author Ryan

*/

bool Entity::isCollidingWithPoint(float pointX, float pointY) {


    if (x < pointX && x + width > pointX && y < pointY && y + height > pointY) {

        return true;

    }


    return false;

}


// main function that entities call to draw themselves to the screen

void Entity::drawSelf() {

    sprite.Open(spriteName);

    sprite.Draw((int)x, (int)y);

    sprite.Close();

}


// used to change the used sprite of the entity (walking animation, for example)

void Entity::updateSprite(char spr[]) {

    strcpy(spriteName, spr);

}


void Entity::hurt(int dmg) {

    health -= dmg;

}


/*

*   Player class for the player, most logic is handled in game method

*

*   @author Ryan, Meenakshi

*/

class Player : public Entity {

    private:

        int xp = 0, level = 0;

    public:

        Player(float _x, float y, int width, int height, int _health, float speed, char spriteName[]);

        int getXp();

        int getLevel();

        void setXp(int set);

        void setLevel(int set);

};


Player::Player(float _x, float _y, int _width, int _height, int _health, float _speed, char _spriteName[]) {

    x = _x;

    y = _y;

    width = _width;

    height = _height;

    health = _health;

    speed = _speed;

    strcpy(spriteName, _spriteName);

}


// self explanatory gets and sets

int Player::getXp() {

    return xp;

}

int Player::getLevel() {

    return level;

}

void Player::setXp(int set) {

    xp = set;

}

void Player::setLevel(int set) {

    level = set;

}


/*

*   Basic enemy class, most logic handled in game method

*

*   @author Ryan, Meenakshi

*/

class Enemy : public Entity {

    private:

        int damage;

        int damageCooldown = 30;

        bool onCooldown = false;

    public:

        Enemy();

        Enemy(float x, float y, int width, int height, int health, float speed, int damage, char spriteName[]);

        int getDamage();

        void lowerDamageCooldown();

        int getDamageCooldown();

        void setDamageCooldown(int set);

        bool isOnCooldown();

        void setIsOnCooldown(bool set);

};


// default constructor for when ititializing enemy array

Enemy::Enemy() {}


Enemy::Enemy(float _x, float _y, int _width, int _height, int _health, float _speed, int _damage, char _spriteName[]) {

    x = _x;

    y = _y;

    width = _width;

    height = _height;

    health = _health;

    speed = _speed;

    damage = _damage;

    strcpy(spriteName, _spriteName);

}


int Enemy::getDamage() {

    return damage;

}


/*

*   damagecooldown methods are used so enemies get invincibility frames for a moment after getting hit

*

*   @author Ryan

*/

void Enemy::lowerDamageCooldown() {

    damageCooldown--;

}

int Enemy::getDamageCooldown() {

    return damageCooldown;

}

void Enemy::setDamageCooldown(int set) {

    damageCooldown = set;

}

bool Enemy::isOnCooldown() {

    return onCooldown;

}

void Enemy::setIsOnCooldown(bool set) {

    onCooldown = set;


    // draw a box so theres a hit 'animation'

    if (set) {

        LCD.SetFontColor(WHITE);

        LCD.FillRectangle(x, y, width, height);

    }

}


/*

*   Basic attack class

*

*   @author Ryan

*/

class Attack : public Entity {

    private:

        float angle;

        int damage;

    public:

        Attack();

        Attack(float x, float y, int width, int height, int health, float angle, float speed, int damage, char spriteName[]);

        void move();

        int getDamage();

};


// default constructor for attack array

Attack::Attack() {}


Attack::Attack(float _x, float _y, int _width, int _height, int _health, float _angle, float _speed, int _damage, char _spriteName[]) {

    x = _x;

    y = _y;

    width = _width;

    height = _height;

    health = _health;

    angle = _angle;

    speed = _speed;

    damage = _damage;

    strcpy(spriteName, _spriteName);

}


// moves the attack in a direction based on its angle and speed

void Attack::move() {

    float xMove, yMove;


    xMove = cos(angle) * speed;

    yMove = sin(angle) * speed;


    x += xMove;

    y += yMove;

}


int Attack::getDamage() {

    return damage;

}


// template type T used so the removeFromArray function can be used for both attacks and enemies

template <class T>

void removeFromArray(T[], int, int);

void promptItemMenu(Items& items);

void menu();

int game();

void stats(int, int);

void info();

void credits();

bool promptDifficulty();

Enemy createEnemy(int lvl, bool hard, bool boss);


/*

*   main game function. returns the score achieved during the session

*

*   @author Ryan, Meenakshi, Agi

*/

int game() {


    // open the background to be drawn on each frame

    FEHImage background;

    background.Open("BGFEH.pic");


    // initialize score to 0 and starting xp to next level

    int score = 0;

    int xpToNextLevel = 5;


    // prompt the difficulty menu

    LCD.Clear();

    bool hardMode = promptDifficulty();


    // create the main player object

    Player player(150, 110, 16, 32, 20000, 1, "playerFEH.pic");

    // lower player health if in hard mode

    if (hardMode) {

        player.setHealth(15000);

    }

   

    // create the items struct and timers for each cooldown based item

    Items items;

    unsigned long marbleTimer = 0;

    unsigned long airfoilTimer = 0;

    unsigned long circuitTimer = 0;


    // create the array of enemies and a counter variable

    Enemy enemy[50]; // 50 is max

    int enemyCounter = 0;


    // initialize the spawn cooldown timers for each enemy spawning pattern

    int enemySpawnCooldown = 4000;

    unsigned long enemySpawnTimer = 0;


    int enemyBurstSpawnCooldown = 26000;

    unsigned long enemyBurstSpawnTimer = TimeNowMSec();


    int enemyBossSpawnCooldown = 63000;

    unsigned long enemyBossSpawnTimer = TimeNowMSec();


    // create the array of attacks and a counter variable

    Attack attack[50]; // 50 is max

    int attackCounter = 0;


    // keeps track of touch locations

    float xTouch, yTouch;


    // prompt the user for first item

    LCD.Clear();

    promptItemMenu(items);


    // used for drawing player sprite flipped if facing left

    bool playerFacingRight = true;


    bool endGame = false;

    while (!endGame) {


        // if player is touching the screen

        if (LCD.Touch(&xTouch, &yTouch)) {


            // move the player to the touched point

            player.moveToPoint((xTouch - player.getWidth()/2), (yTouch - player.getHeight()/2));


            // update the direction the player is facing

            playerFacingRight = (player.getAngle() > -M_PI_2 && player.getAngle() < M_PI_2);


            if (playerFacingRight) {

                // based on current time, alternate between walk and default sprite

                if (TimeNowMSec() % 500 < 250) {

                    player.updateSprite("PlayerWalkFEH.pic");

                } else {

                    player.updateSprite("PlayerFEH.pic");

                }

            } else { // player is facing left

                // based on current time alternate between walk and default sprite, both flipped because facing left

                if (TimeNowMSec() % 500 < 250) {

                    player.updateSprite("PlayerWalkFlippedFEH.pic");

                } else {

                    player.updateSprite("PlayerFlippedFEH.pic");

                }

            }

        } else { // force update sprite to the not walking one if not walking

            if (playerFacingRight) {

                player.updateSprite("PlayerFEH.pic");

            } else {

                player.updateSprite("PlayerFlippedFEH.pic");

            }

        }


        // if time to spawn enemy

        if (TimeNowMSec() - enemySpawnTimer > enemySpawnCooldown) {

            // reset spawn timer

            enemySpawnTimer = TimeNowMSec();


            // create enemy and add to the array

            Enemy e = createEnemy(player.getLevel(), hardMode, false);

            enemy[enemyCounter] = e;

            if (enemyCounter < 50) enemyCounter++;

        }


        // if time to spawn burst of enemies (3 at once, weaker) (can't happen while level 0)

        if (TimeNowMSec() - enemyBurstSpawnTimer > enemyBurstSpawnCooldown && player.getLevel() > 0) {

            // reset spawn timer

            enemyBurstSpawnTimer = TimeNowMSec();


            // create three enemies of lower level and add to the array

            for (int i = 0; i < 3; i++) {

                Enemy e = createEnemy(player.getLevel() / 2, hardMode, false);

                enemy[enemyCounter] = e;

                if (enemyCounter < 50) enemyCounter++;

            }

        }


        // if time to spawn BOSS(-like) enemy

        if (TimeNowMSec() - enemyBossSpawnTimer > enemyBossSpawnCooldown) {

            // reset spawn timer

            enemyBossSpawnTimer = TimeNowMSec();


            // create boss enemy and add to the array

            Enemy e = createEnemy(player.getLevel(), hardMode, true);

            enemy[enemyCounter] = e;

            if (enemyCounter < 50) enemyCounter++;

        }



        // handle attacks when player has the marble weapon

        if (items.marble.level > -1) {


            // if time to spawn marble attack

            if (TimeNowMSec() - marbleTimer > items.marble.cooldown[items.marble.level]) {

                // resets marble cooldown

                marbleTimer = TimeNowMSec();


                // makes 8 of them in 45 degree increments and adds them to the attack array

                for (int i = 1; i < 9; i++) {

                    Attack a(player.getX() + player.getWidth()/2, player.getY() + player.getHeight()/2, 4, 4, 1, i*(M_PI_4), 2, items.marble.damage[items.marble.level], "MarbleFEH.pic");

                    attack[attackCounter] = a;

                    if (attackCounter < 50) attackCounter++;

                }

            }

        }


        // handle attacks when player has the airfoil weapon

        if (items.airfoil.level > -1) {


            // updates player movespeed bonus from the item

            player.setSpeed(1 + items.airfoil.moveBonus[items.airfoil.level]);


            // if time to spawn airfoil attack

            if (TimeNowMSec() - airfoilTimer > items.airfoil.cooldown[items.airfoil.level]) {

                // resets airfoil cooldown

                airfoilTimer = TimeNowMSec();


                // creates attack with player angle + pi so it shoots backwards and adds it to the attack array

                Attack a(player.getX() + player.getWidth()/2, player.getY() + player.getHeight()/2, 6, 6, 8, player.getAngle() + M_PI, 2.5, items.airfoil.damage[items.airfoil.level], "AirfoilFEH.pic");

                attack[attackCounter] = a;

                if (attackCounter < 50) attackCounter++;

            }

        }


        // handle attacks when player has the circuit weapon

        if (items.circuit.level > -1) {


            // if time to spawn the circuit attack

            if (TimeNowMSec() - circuitTimer > items.circuit.cooldown[items.circuit.level]) {

                // resets circuit cooldown

                circuitTimer = TimeNowMSec();


                // creates circuit attack with 0 speed and adds it to the attack array

                Attack a(player.getX() + player.getWidth()/2, player.getY() + player.getHeight()/2, 6, 4, items.circuit.hits[items.circuit.level], 0, 0, items.circuit.damage[items.circuit.level], "CircuitFEH.pic");

                attack[attackCounter] = a;

                if (attackCounter < 50) attackCounter++;

            }

        }


        // clears the LCD *after* most a good amount of computation work to prevent flickering

        LCD.Clear();


        // draw background

        background.Draw(0, 0);


        // handle attacks when player has the beam weapon (because drawing it isnt handled from the attack array, it goes after the lcd is cleared)

        if (items.beam.level > -1) {

            // temp variables for the center of the player

            float centerX = player.getX() + player.getWidth()/2;

            float centerY = player.getY() + player.getHeight()/2;


            // sets color of the beam based on item level

            LCD.SetFontColor(items.beam.color[items.beam.level]);


            // temp variables for the end of the beam, calculated based on level

            float beamX = items.beam.length[items.beam.level]*cos(player.getAngle());

            float beamY = items.beam.length[items.beam.level]*sin(player.getAngle());


            // draw the beam

            // conditional rendering if the angle is near vertical to prevent weird artifacting with vertical lines

            if (sin(player.getAngle()) > 0.99) {

                LCD.DrawLine(centerX, centerY, centerX, centerY + items.beam.length[items.beam.level]);

            } else if (sin(player.getAngle()) < -0.99) {

                LCD.DrawLine(centerX, centerY, centerX, centerY - items.beam.length[items.beam.level]);

            } else {

                LCD.DrawLine(centerX, centerY, centerX + beamX, centerY + beamY);

            }

        }


        // handle logic for each enemy on screen

        for (int i = 0; i < enemyCounter; i++) {

           

            // move enemy to player and draw self

            enemy[i].moveToPoint(player.getX(), player.getY());

            enemy[i].drawSelf();


            // if colliding with player

            if (enemy[i].isColliding(player)) {

                player.hurt(enemy[i].getDamage());


                // if player dies

                if (player.getHealth() < 1) {

                    endGame = true;

                }

            }


            // if the enemy has been hit recently, give it i-frames until its damage cooldown is over

            if (enemy[i].isOnCooldown()) {

                enemy[i].lowerDamageCooldown();


                if (enemy[i].getDamageCooldown() < 0) {

                    enemy[i].setDamageCooldown(30);

                    enemy[i].setIsOnCooldown(false);

                }

            }


            // special collision detection for beam weapon

            if (items.beam.level > -1) {

                // temp variables for center of player

                float centerX = player.getX() + player.getWidth()/2;

                float centerY = player.getY() + player.getHeight()/2;


                // checks if enemy is colliding with the end of the beam, and halfway down the beam (ugly but works)

                if (enemy[i].isCollidingWithPoint(centerX + items.beam.length[items.beam.level]*cos(player.getAngle()),

                centerY + items.beam.length[items.beam.level]*sin(player.getAngle())) ||

                enemy[i].isCollidingWithPoint(centerX + 0.5*items.beam.length[items.beam.level]*cos(player.getAngle()),

                centerY + 0.5*items.beam.length[items.beam.level]*sin(player.getAngle()))) {

                    // should not affect the enemy if on damage cooldown

                    if (!enemy[i].isOnCooldown()) {

                        enemy[i].setIsOnCooldown(true);

                        enemy[i].hurt(items.beam.damage[items.beam.level]);

                    }

                }

            }


            // for each attack on screen

            for (int j = 0; j < attackCounter; j++) {

                // checks if enemy is colliding with the attack

                // should not affect the enemy if on damage cooldown

                if (enemy[i].isColliding(attack[j]) && !enemy[i].isOnCooldown()) {

                    enemy[i].setIsOnCooldown(true);

                    enemy[i].hurt(attack[j].getDamage());

                    attack[j].hurt(1);

                }

            }


            // if enemy health below 0

            if (enemy[i].getHealth() < 1) {


                // removes the enemy from the array and updates counters accordingly

                removeFromArray(enemy, 50, i);

                enemyCounter--;

                i--;


                // grant player XP

                player.setXp(player.getXp() + 1);


                // if player has Lab Report item, heal them on enemy death

                if (items.report.level > -1) {

                    player.setHealth(player.getHealth() + items.report.healing[items.report.level]);

                }


                // update score

                score += 10;

            }

        }


        // handle logic for each attack on screen

        for (int i = 0; i < attackCounter; i++) {

           

            // move the attacks and draw each one

            attack[i].move();

            attack[i].drawSelf();


            int attackX = attack[i].getX();

            int attackY = attack[i].getY();


            // if attack goes off screen or if health below 0, delete the attack

            if (attack[i].getHealth() < 1 || attackX > WINDOW_WIDTH - attack[i].getWidth() || attackX < 0 || attackY > WINDOW_HEIGHT - attack[i].getHeight() || attackY < 0) {

                // remove from attack array and update counters accordingly

                removeFromArray(attack, 50, i);

                attackCounter--;

                i--;

            }

        }


        // if the player has enough xp to level up

        if (xpToNextLevel - player.getXp() <= 0) {


            // reset the player xp and increments level

            player.setXp(0);

            player.setLevel(player.getLevel() + 1);


            // only prompt new items if there are new items to give

            if (player.getLevel() < 15) promptItemMenu(items);


            // update xp requirement and make enemies spawn more frequently

            xpToNextLevel = 5 + 5*player.getLevel();

            enemySpawnCooldown = 4000 - 200*player.getLevel();

            if (enemySpawnCooldown < 1000) enemySpawnCooldown = 1000;


            // update score

            score += 100;

        }


        player.drawSelf();


        // display health and xp to level up

        LCD.SetFontColor(WHITE);

        LCD.Write("Health: ");

        LCD.WriteLine(player.getHealth());

        // if max level, show "Max!" for xp

        LCD.Write("XP To Lvl Up: ");

        if (player.getLevel() < 15) {

            LCD.WriteLine(xpToNextLevel - player.getXp());

        } else {

            LCD.WriteLine("Max!");

        }

        LCD.Write("Score: ");

        LCD.WriteLine(score);

    }


    // close background image

    background.Close();


    // display session score

    LCD.Clear();

    LCD.WriteAt("Your Score: ", 50, 60);

    LCD.WriteAt(score, 100, 80);


    // wait for player to let go, then touch screen, then let go again

    while (LCD.Touch(&xTouch, &yTouch));

    while (!LCD.Touch(&xTouch, &yTouch));

    while (LCD.Touch(&xTouch, &yTouch));


    return score;


}


/*

*   creates an enemy with scaling parameters based on player level.

*   also increases health if on hardMode, and increases stats if its a boss enemy

*

*   @author Ryan

*/

Enemy createEnemy(int level, bool hardMode, bool boss) {

    int enemyWidth = 14;

    int enemyHeight = 32;


    // if hardmode, make the health modifier higher

    float healthModifier = 1;

    if (hardMode) {

        healthModifier = 1.3;

    }


    // initialize default values, scaling with player level

    int enemyHealth = (level + 1) * healthModifier;

    float enemySpeed = 0.4 + 0.04*level;

    int enemyDamage = 5 + level;


    if (boss) { // if boss enemy then overwrite values

        enemyHealth = (2*level + 4) * healthModifier;

        enemySpeed = 0.2 + 0.02*level;

        enemyDamage = 10 + level*2;

    };


    float spawnX, spawnY;


    // randomly pick if enemy should be spawned on n/e/s/w wall

    // RandInt / 32767.0 is random float between 0 and 1

    if (Random.RandInt() % 2 == 0) {

        if (Random.RandInt() % 2 == 0) {

            spawnX = 0;

            spawnY = (WINDOW_HEIGHT - enemyHeight) * (Random.RandInt() / 32767.0);

        } else {

            spawnX = WINDOW_WIDTH - enemyWidth;

            spawnY = (WINDOW_HEIGHT - enemyHeight) * (Random.RandInt() / 32767.0);

        }

    } else {

        if (Random.RandInt() % 2 == 0) {

            spawnX = (WINDOW_WIDTH - enemyWidth) * (Random.RandInt() / 32767.0);

            spawnY = 0;

        } else {

            spawnX = (WINDOW_WIDTH - enemyWidth) * (Random.RandInt() / 32767.0);

            spawnY = WINDOW_HEIGHT - enemyHeight;

        }

    }


    // randomly pick enemy sprite, rand is random value between 0 and 4

    int rand = Random.RandInt() / 32768.0 * 5;

    char spriteName[30];

    switch (rand) {

        case 0:

            strcpy(spriteName, "Enemy1FEH.pic");

            break;

        case 1:

            strcpy(spriteName, "Enemy2FEH.pic");

            break;

        case 2:

            // enemy 3 sprite is thinner, so update enemyWidth

            strcpy(spriteName, "Enemy3FEH.pic");

            enemyWidth = 11;

            break;

        case 3:

            strcpy(spriteName, "Enemy4FEH.pic");

            break;

        default:

            strcpy(spriteName, "Enemy5FEH.pic");

            break;

    }


    // create and return the enemy

    Enemy e(spawnX, spawnY, enemyWidth, enemyHeight, enemyHealth, enemySpeed, enemyDamage, spriteName);

    return e;

}



/*

*   prompt menu for difficulty setting

*

*   @author Ryan

*/

bool promptDifficulty() {

    // Set up and draw menu

    FEHIcon::Icon menu[2];

    char menuLabels[2][20] = {"NORMAL","HARD"};

    FEHIcon::DrawIconArray(menu, 1, 2, 10, 10, 5, 5, menuLabels, RED, GOLD);


    // keep track of touch locations

    float x, y;


    // makes it so if you were holding the screen before it waits for you to let go first

    while (LCD.Touch(&x, &y)) {}


    // returns true if pressed hard, and false if pressed normal

    while (true) {

        if (LCD.Touch(&x, &y)) {

            if (menu[0].Pressed(x, y, 0)) {

                return false;

            }

            if (menu[1].Pressed(x, y, 0)) {

                return true;

            }

        }

    }

}


/*

*   prompt menu for picking an item and updates items struct accordingly

*

*   @author Ryan, Meenakshi

*/

void promptItemMenu(Items &items) {


    // declare item choice pointers and item sprite displays

    int* itemChoices[3];

    char itemImageNames[3][20];


    // for each of the three options

    for (int i = 0; i < 3; i++) {


        // will keep looping until it randomly picks a valid option

        bool selected = false;

        while (!selected) {

            // random int between 0 and 4

            int rand = Random.RandInt()/32768.0 * 5;

            switch (rand) {

                case 0:

                    // based on the current level, displays the proper sprite for upgrade text

                    switch (items.airfoil.level) {

                        case -1:

                            // sets the item image to the proper sprite

                            strcpy(itemImageNames[i], "airfoilItem1FEH.pic");

                            // set itemChoices to the proper item so we know which item level to increment if chosen

                            itemChoices[i] = &items.airfoil.level;

                            // break out of the loop, something valid was chosen

                            selected = true;

                            break;

                        case 0:

                            strcpy(itemImageNames[i], "airfoilItem2FEH.pic");

                            itemChoices[i] = &items.airfoil.level;

                            selected = true;

                            break;

                        case 1:

                            strcpy(itemImageNames[i], "airfoilItem3FEH.pic");

                            itemChoices[i] = &items.airfoil.level;

                            selected = true;

                            break;

                        default:

                            break;

                    }

                    break;

                case 1:

                    // based on the current level, displays the proper sprite for upgrade text

                    switch (items.marble.level) {

                        case -1:

                            strcpy(itemImageNames[i], "marbleItem1FEH.pic");

                            itemChoices[i] = &items.marble.level;

                            selected = true;

                            break;

                        case 0:

                            strcpy(itemImageNames[i], "marbleItem2FEH.pic");

                            itemChoices[i] = &items.marble.level;

                            selected = true;

                            break;

                        case 1:

                            strcpy(itemImageNames[i], "marbleItem3FEH.pic");

                            itemChoices[i] = &items.marble.level;

                            selected = true;

                            break;

                        default:

                            break;

                    }

                    break;

                case 2:

                    // based on the current level, displays the proper sprite for upgrade text

                    switch (items.beam.level) {

                        case -1:

                            strcpy(itemImageNames[i], "beamItem1FEH.pic");

                            itemChoices[i] = &items.beam.level;

                            selected = true;

                            break;

                        case 0:

                            strcpy(itemImageNames[i], "beamItem2FEH.pic");

                            itemChoices[i] = &items.beam.level;

                            selected = true;

                            break;

                        case 1:

                            strcpy(itemImageNames[i], "beamItem3FEH.pic");

                            itemChoices[i] = &items.beam.level;

                            selected = true;

                            break;

                        default:

                            break;

                    }

                    break;

                case 3:

                    // based on the current level, displays the proper sprite for upgrade text

                    switch (items.circuit.level) {

                        case -1:

                            strcpy(itemImageNames[i], "circuitItem1FEH.pic");

                            itemChoices[i] = &items.circuit.level;

                            selected = true;

                            break;

                        case 0:

                            strcpy(itemImageNames[i], "circuitItem2FEH.pic");

                            itemChoices[i] = &items.circuit.level;

                            selected = true;

                            break;

                        case 1:

                            strcpy(itemImageNames[i], "circuitItem3FEH.pic");

                            itemChoices[i] = &items.circuit.level;

                            selected = true;

                            break;

                        default:

                            break;

                    }

                    break;

                case 4:

                    // ensure you cant get it first item because it does no damage

                    if (items.airfoil.level == -1 && items.beam.level == -1 && items.circuit.level == -1 && items.marble.level == -1) {

                        break;

                    }


                    // based on the current level, displays the proper sprite for upgrade text

                    switch (items.report.level) {

                        case -1:

                            strcpy(itemImageNames[i], "reportItem1FEH.pic");

                            itemChoices[i] = &items.report.level;

                            selected = true;

                            break;

                        case 0:

                            strcpy(itemImageNames[i], "reportItem2FEH.pic");

                            itemChoices[i] = &items.report.level;

                            selected = true;

                            break;

                        case 1:

                            strcpy(itemImageNames[i], "reportItem3FEH.pic");

                            itemChoices[i] = &items.report.level;

                            selected = true;

                            break;

                        default:

                            break;

                    }

                    break;

                default:

                    break;

            }

        }

    }


    // draw the menu

    LCD.SetFontColor(DARKGRAY);

    LCD.FillRectangle(14, 14, WINDOW_WIDTH - 28, WINDOW_HEIGHT - 28);

    LCD.SetFontColor(BLACK);

    LCD.WriteAt("Level Up!", 105, 20);


    LCD.DrawRectangle(20, 40, 90, 180);

    LCD.DrawRectangle(114, 40, 90, 180);

    LCD.DrawRectangle(208, 40, 90, 180);


    // open the proper sprites as selected in the loop above and draw them

    FEHImage itemImages[3];

    itemImages[0].Open(itemImageNames[0]);

    itemImages[1].Open(itemImageNames[1]);

    itemImages[2].Open(itemImageNames[2]);


    itemImages[0].Draw(20, 40);

    itemImages[1].Draw(114, 40);

    itemImages[2].Draw(208, 40);


    LCD.Update();


    float xTouch, yTouch;


    // makes it so if you were holding the screen before it waits for you to let go first

    while (LCD.Touch(&xTouch, &yTouch)) {}

   

    // waits until the user picks an option and updates its level accordingly

    bool madeSelection = false;

    while (!madeSelection) {

        if (LCD.Touch(&xTouch, &yTouch)) {

            if (yTouch > 40 && yTouch < 220) {

                if (xTouch > 20 && xTouch < 110) {

                    (*itemChoices[0])++;

                    madeSelection = true;

                }

                if (xTouch > 114 && xTouch < 214) {

                    (*itemChoices[1])++;

                    madeSelection = true;

                }

                if (xTouch > 208 && xTouch < 298) {

                    (*itemChoices[2])++;

                    madeSelection = true;

                }

            }

        }

    }

}


/*

*   removes an object from an array by moving everything above it down 1.

*   uses template type T so the method can be used for any object type

*

*   @author Ryan

*/

template <class T>

void removeFromArray(T array[], int arraySize, int indexToRemove) {

    // moves everything above the thing to remove the the array down by 1

    for (int i = indexToRemove; i < arraySize - 1; i++) {

        array[i] = array[i + 1];

    }

}



int main() {

    // Clear background

    LCD.SetBackgroundColor(BLACK);


    menu();


    return 0;

}


/*

*   displays the main menu

*

*   @author Ryan, Meenakshi

*/

void menu() {

    LCD.Clear();


    // Set up and draw menu

    FEHIcon::Icon menu[4];

    char menuLabels[4][20] = {"PLAY","STATS","INFO","CREDITS"};

    FEHIcon::DrawIconArray(menu, 2, 2, 10, 10, 5, 5, menuLabels, RED, GOLD);


    // draw the logo

    FEHImage logo;

    logo.Open("logoFEH.pic");

    logo.Draw(100,98);


    // keeps track of where touch location is

    float x, y;


    // makes it so if you were holding the screen before it waits for you to let go first

    while (LCD.Touch(&x, &y)) {}


    // initialize statistics

    int gamesPlayed = 0;

    int highscore = 0;


    // this loop runs forever until the game is closed

    while (true) {

        if (LCD.Touch(&x, &y)) {

            if (menu[0].Pressed(x, y, 0)) {

                // increment games played, run the game and store the score

                gamesPlayed++;

                int score = game();


                // if score is higher than highscore, update value

                if (highscore < score) highscore = score;

            }

            if (menu[1].Pressed(x, y, 0)) {

                stats(gamesPlayed, highscore);

            }

            if (menu[2].Pressed(x, y, 0)) {

                info();

            }

            if (menu[3].Pressed(x, y, 0)) {

                credits();

            }


            // after coming back from a menu option, redraw the main menu

            LCD.Clear();

            FEHIcon::DrawIconArray(menu, 2, 2, 10, 10, 5, 5, menuLabels, RED, GOLD);

            logo.Draw(100,98);


        }

    }


    logo.Close();

}


/*

*   displays the stats menu

*

*   @author Ryan, Meenakshi

*/

void stats(int gamesPlayed, int highscore) {

    LCD.Clear();


    // display the statistics

    LCD.SetFontColor(WHITE);

    LCD.WriteAt("Games Played: ", 50, 50);

    LCD.WriteAt(gamesPlayed, 100, 70);

    LCD.WriteAt("High Score: ", 50, 120);

    LCD.WriteAt(highscore, 100, 140);


    // wait for user to let go and tap the screen again

    float x, y;

    while (LCD.Touch(&x, &y));

    while (!LCD.Touch(&x, &y));

    while (LCD.Touch(&x, &y));

}


/*

*   displays the info menu

*

*   @author Ryan, Meenakshi

*/

void info() {

    LCD.Clear();


    // display basic gamplay info

    LCD.SetFontColor(WHITE);

    LCD.WriteAt("Welcome to FEH Survivors!", 10, 15);

    LCD.WriteAt("Touch the screen to move.", 10, 50);

    LCD.WriteAt("You automatically attack.", 10, 80);

    LCD.WriteAt("Level up & get new items!", 10, 110);


    // wait for user to let go and tap the screen again

    float x, y;

    while (LCD.Touch(&x, &y));

    while (!LCD.Touch(&x, &y));

    while (LCD.Touch(&x, &y));

}


/*

*   displays the credits menu

*

*   @author Ryan, Meenakshi

*/

void credits() {

    LCD.Clear();


    // display credits

    LCD.SetFontColor(WHITE);

    LCD.WriteAt("Created by:", 10, 20);

    LCD.WriteAt("Ryan Jung", 20, 50);

    LCD.WriteAt("Meenakshi Varadarajan", 20, 70);

    LCD.WriteAt("Agi Jobe", 20, 90);


    // wait for user to let go and tap the screen again

    float x, y;

    while (LCD.Touch(&x, &y));

    while (!LCD.Touch(&x, &y));

    while (LCD.Touch(&x, &y));

}
