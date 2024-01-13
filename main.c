#include <GL/glut.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
 
#define HUMANSPEED 0.06
#define MELEEDELAY 400
#define MAXBULLETS 50
#define BULLETSPEED 0.3
 
int waveIncrement = 0;
int fireDelay = 0;
int guntype = 0;
 
int keyState[256];
 
int wave = 0;
 
 
struct vector
{
    float x, y;
};
typedef struct vector vector;
 
vector mouse;
 
vector shotFired;
 
int gamemode=0;
 
struct zombie
{
    float size;
    float speed;
    int hearingRadius;
    int health;
    int class;
    int active;
    vector position;
    vector direction;
};
typedef struct zombie zombie;
zombie *walker;    
int zombiesActive;
int zombiesKilled;
int totalKilled=0;
 
void killZombie(int);
void damageZombie(int);
 
struct bullet
{
    int active;
    float x, y;
    float xDirection, yDirection;
    float unitx;
    float unity;
    float speed;
    float type;
    float originx;
    float originy;
};
typedef struct bullet bullet;
bullet* pbullet = NULL;
int clip = 999999;
 
    int meleeRequest;
    int fireRequest;
struct human
{
    int active;
    vector position;
    vector direction;
    clock_t fireTime;
    clock_t meleeTime;
    int meleeActive;
};
typedef struct human human;
human *abdo;
 
void changeSize(int w, int h) 
{
    if (h == 0)
        h = 1;
 
    float ratio =  w * 1.0 / h;
 
    glMatrixMode(GL_PROJECTION);
 
    glLoadIdentity();
 
    glViewport(0, 0, w, h);
 
    glOrtho(-9.6f, 9.6f, -5.4f, 5.4f, 0.0f, 10.0f);
    
    
    glMatrixMode(GL_MODELVIEW);
}
 
vector unitVector(vector input)
{
    
    float mag;
    mag = sqrt(pow(input.x, 2) + pow(input.y, 2));
    input.x = (input.x/mag);
    input.y = (input.y/mag);
    return input;
}
 
vector scaleVector(int scalar, vector input)
{
    input.x*=scalar;
    input.y*=scalar;
    return input;
}
 
void processKeys()
{  
    if(keyState[119]) abdo->position.y+=HUMANSPEED;
    if(abdo->position.y>5.3) abdo->position.y=5.3;
    if(keyState[97]) abdo->position.x-=HUMANSPEED;
    if(abdo->position.x<-9.6) abdo->position.x=-9.6;
    if(keyState[115]) abdo->position.y-=HUMANSPEED;
    if(abdo->position.y<-5.3) abdo->position.y=-5.3;
    if(keyState[100]) abdo->position.x+=HUMANSPEED;
    if(abdo->position.x>9.6) abdo->position.x=9.6;
}
 
void createWave()
{  
    wave++; //increment to the next wave
    walker = (zombie*)malloc(wave*waveIncrement*sizeof(zombie));
    
    zombiesActive=wave*waveIncrement;
    
    int i;
    for(i=0;i<(wave*waveIncrement);i++)
    {
        (walker+i)->active=1;
        (walker+i)->position.x=(((float)(rand()%1900)/100)-9.5);
        (walker+i)->position.y=((float)((rand()%200)+300)/100);
        (walker+i)->direction.x=0;
        (walker+i)->direction.y=0;
        int class = rand()%100;
        if(class<50) 
        {
            (walker+i)->class=1;
            (walker+i)->size=0.2;
            (walker+i)->speed=1;
            (walker+i)->hearingRadius=7;
            (walker+i)->health=1;
        }
        else if(class>=50&&class<75)
        {
            (walker+i)->class=2;
            (walker+i)->size=0.3;
            (walker+i)->speed=0.75;
            (walker+i)->hearingRadius=6;
            (walker+i)->health=5;
        }
        else
        {
            (walker+i)->class=3;
            (walker+i)->size=0.15;
            (walker+i)->speed=1.5;
            (walker+i)->hearingRadius=10;
            (walker+i)->health=1;
        }
    }
    shotFired.x=0;
    shotFired.y=0;
    zombiesKilled=0;
}
 
void createBullet()
{   
    pbullet = (bullet*)malloc(MAXBULLETS*sizeof(bullet));
    
    
    int i;
    for(i=0;i<(MAXBULLETS);i++)
    {
        (pbullet+i)->active=0;
        (pbullet+i)->x=0;
        (pbullet+i)->y=0;
        (pbullet+i)->xDirection=0;
        (pbullet+i)->yDirection=0;
        (pbullet+i)->type=0;
        (pbullet+i)->originx = 0;
        (pbullet+i)->originy = 0;
    }
}
 
 
void drawShooting()
{
    //printf("Bullets drawn");
    float speed = BULLETSPEED;
    int i;
    float length = 0.1f;
    for(i = 0; i<MAXBULLETS; i++)
    {
        if((pbullet + i)->active)
        {
            (pbullet + i)->x += speed*((pbullet+i)->unitx);
            (pbullet + i)->y += speed*((pbullet+i)->unity);
            glPushMatrix();
            //glTranslatef((pbullet + i)->x, (pbullet + i)->y, 2.00);
            glColor3f(1.0f, 1.0f, 0.0f);
            //glRectf(-0.025f, -0.1f, 0.0f, 0.0f);
            glLineWidth(3.0);
            glBegin(GL_LINES);
                glVertex3f((pbullet + i)->x, (pbullet + i)->y, 2.0f);
                glVertex3f((pbullet + i)->x+length*((pbullet+i)->unitx), (pbullet + i)->y+length*((pbullet+i)->unity), 2.0f);
            glEnd();
            glPopMatrix();
        }
        if (guntype ==0)
        {
            if(((pbullet + i)->x > 10.0f) || ((pbullet + i)->x < -10.0f) ||((pbullet + i)->y > 6.0f) || ((pbullet + i)->y < -6.0f))
            {
                (pbullet + i)->active = 0;
            }
        }
        
        if((pbullet+i)->active==0)
        {
            (pbullet+i)->x=0;
            (pbullet+i)->y=0;
            (pbullet+i)->xDirection=0;
            (pbullet+i)->yDirection=0;
            (pbullet+i)->type=0;
            (pbullet+i)->originx = 0;
            (pbullet+i)->originx = 0;
        }
        
    }   
    
    
}
 
void collision()
{
    int i;
    for(i=0; i<MAXBULLETS; i++)
    {
        if((pbullet+i)->active == 1)
        {
            int a;
            for(a=0; a<wave*waveIncrement; a++)
            {
                if((walker+a)->active == 1)
                {
                    if (sqrt(pow(((walker+a)->position.x -(pbullet + i)->x),2)+pow(((walker+a)->position.y -(pbullet + i)->y),2))<(walker+a)->size+0.05) 
                    {
                        (walker+a)->health--;
                        damageZombie(a);
                        if(!(walker+a)->health) killZombie(a);
                        (pbullet+i)->active = 0;
                    }
                }
            }
        }
    }
}
 
void drawFilledCircle(float x, float y, float radius, int quality)
{   
    int i;
    int triangleAmount = quality;
    
    float twicePi = 2.0f * 3.14159265358979323846;
    
    glBegin(GL_TRIANGLE_FAN);
        glVertex2f(x, y);
        for(i = 0; i <= triangleAmount;i++) 
        { 
            glVertex2f(
                x + (radius * cos(i *  twicePi / triangleAmount)), 
                y + (radius * sin(i * twicePi / triangleAmount))
            );
        }
    glEnd();
}
 
void EnemySpawning(float x, float y, int i)
{
    glPushMatrix();
    glTranslatef(x, y, 2.0f);
 
    glColor3f(0.16f, 0.53f, 0.23f);
    drawFilledCircle(0.0f, 0.0f, (walker+i)->size, 15);
    glPopMatrix();
}
 
void damageZombie(int i)
{
    glPushMatrix();
    glTranslatef((walker+i)->position.x, (walker+i)->position.y, 2.1f);
    glColor3f(0.75f, 0.23f, 0.23f);
    drawFilledCircle(0.0f, 0.0f, (walker+i)->size+.05, 12);
    glPopMatrix();
}
void killZombie(int i)
{
    (walker+i)->active=0;
    zombiesActive--;
    zombiesKilled++;
    totalKilled++;
}
 
void drawZombies()
{
    int i;
    for(i=0;i<(wave*waveIncrement);i++)
    {
        if((walker+i)->active) EnemySpawning((walker+i)->position.x,(walker+i)->position.y, i);
    }
}
 
void calculateZombieMovement()
{
    int i;
    for(i=0;i<(wave*waveIncrement);i++)
    {
        
        if((walker+i)->active)
        {
            vector randomVector;
            randomVector.x=(((rand()%1000)-500));
            randomVector.y=(((rand()%1000)-500));
            randomVector=unitVector(randomVector);
            
            vector distance;
            distance.x=abdo->position.x-(walker+i)->position.x;
            distance.y=abdo->position.y-(walker+i)->position.y;
            if(sqrt(pow(distance.x,2)+pow(distance.y,2))<4)
            {
                if(sqrt(pow(distance.x,2)+pow(distance.y,2))<(0.5+(walker+i)->size)&&abdo->meleeActive)
                {
                    (walker+i)->health--;
                    damageZombie(i);
                    if(!(walker+i)->health) killZombie(i);
                    abdo->meleeActive=0;
                }
                else if(sqrt(pow(distance.x,2)+pow(distance.y,2)<(walker+i)->size)) abdo->active = 0;
                distance=unitVector(distance);
                distance=scaleVector(5, distance);
                (walker+i)->direction.x+=distance.x;
                (walker+i)->direction.y+=distance.y;
            }
            if((walker+i)->class==2&&(walker+i)->health<5)
            {
                distance=unitVector(distance);
                distance=scaleVector(5, distance);
                (walker+i)->direction.x+=distance.x;
                (walker+i)->direction.y+=distance.y;
            }
            else if(shotFired.x&&shotFired.y)
            {
                distance.x=shotFired.x-(walker+i)->position.x;
                distance.y=shotFired.y-(walker+i)->position.y;
                if(sqrt(pow(distance.x,2)+pow(distance.y,2))<(walker+i)->hearingRadius)
                {
                    distance=unitVector(distance);
                    distance=scaleVector(5, distance);
                    (walker+i)->direction.x+=distance.x;
                    (walker+i)->direction.y+=distance.y;
                }
            }
            int j;
            int blocked=0;
            for(j=0;j<(wave*waveIncrement);j++)
            {
                if(j!=i)
                {
                    distance.x=(walker+j)->position.x-(walker+i)->position.x;
                    distance.y=(walker+j)->position.y-(walker+i)->position.y;
                    if(sqrt(pow(distance.x,2)+pow(distance.y,2))<0.3)
                    {
                        blocked = 1;
                        distance=unitVector(distance);
                        distance=scaleVector(-1, distance);
                        (walker+i)->direction.x+=distance.x;
                        (walker+i)->direction.y+=distance.y;
                    }
                }
            }
            if(!blocked)
            {
                (walker+i)->direction.x+=randomVector.x;
                (walker+i)->direction.y+=randomVector.y;
            }
            (walker+i)->direction=unitVector((walker+i)->direction);
            
            
        }
    }
}
 
void moveZombies()
{
    int i;
    for(i=0;i<(wave*waveIncrement);i++)
    {
        if((walker+i)->active)
        {
            (walker+i)->position.x+=(((float)(walker+i)->speed*(walker+i)->direction.x)/17);
            (walker+i)->position.y+=(((float)(walker+i)->speed*(walker+i)->direction.y)/17);
            if((walker+i)->position.y>5.3) (walker+i)->position.y=5.3;
            else if((walker+i)->position.y<-5.3) (walker+i)->position.y=-5.3;
            if((walker+i)->position.x>9.5) (walker+i)->position.x=9.5;
            else if((walker+i)->position.x<-9.5) (walker+i)->position.x=-9.5;
            
        }
    }
}
 
void createHuman()
{
    abdo=(human*)malloc(sizeof(human));
    abdo->position.x=0;
    abdo->position.y=-3;
    abdo->direction.x=0;
    abdo->direction.y=1;
    abdo->active=1;
    abdo->meleeTime=abdo->fireTime=0;
    abdo->meleeActive=0;
}
 
void drawHuman() {
    glPushMatrix();
    glColor3f(0.0f, 0.0f, 1.0f);
    glTranslatef(abdo->position.x, abdo->position.y, 1.0f);
    drawFilledCircle(0.0f, 0.0f, 0.2f, 12);
    glColor3f(1.0f, 0.0f, 0.0f);
        glBegin(GL_LINES);
            glVertex3f(0.0f, 0.0f, 0.1f);
            glVertex3f(0.2*mouse.x, 0.2*mouse.y, 0.1f);
        glEnd(); 
    glPopMatrix();
}
 
void humanAttack()
{
    if(meleeRequest&&(clock()-abdo->meleeTime)>(MELEEDELAY*(CLOCKS_PER_SEC/1000)))
    {
        abdo->meleeActive=1;
        abdo->meleeTime=clock();
    }
    
    if(((clock()-abdo->meleeTime)<(.2*CLOCKS_PER_SEC))&&(abdo->meleeActive))
    {
        glPushMatrix();
        glColor3f(1.0f,0.0f,0.0f);
        glTranslatef(abdo->position.x, abdo->position.y, 0.5f);
        drawFilledCircle(0.0f, 0.0f, 0.5f, 24);
        glPopMatrix();
    }
    else abdo->meleeActive=0;
    
    if(clip>0&&fireRequest&&(clock()-abdo->fireTime)>(fireDelay*(CLOCKS_PER_SEC/1000)))
    {
        
        abdo->fireTime=clock();
        
        
        if (guntype==0)
        {
            int i;
            for(i=0;i<MAXBULLETS;i++)
            {
                if(!(pbullet+i)->active)
                {
                    (pbullet + i)->active = 1;
                    (pbullet + i)->unitx = mouse.x;
                    (pbullet + i)->unity = mouse.y;
                    (pbullet + i)->x = abdo->position.x;
                    (pbullet + i)->y = abdo->position.y;
                    shotFired.x=abdo->position.x;
                    shotFired.y=abdo->position.y;
                    (pbullet + i)->type = 0;
                    break;
                }
            }
        }
    }
    if((clock()-abdo->fireTime)>5*CLOCKS_PER_SEC)
    {
        shotFired.x=0;
        shotFired.y=0;
    }
}
 
char* fitoa(int i, char b[33])
{
    char const digit[] = "0123456789";
    char* p = b;
    int shifter = i;
    if(i<0)
    {
        *p++ = '-';
        i *= -1;
    }
    do
    {
        ++p;
        shifter = shifter/10;
    } while(shifter);
    *p = '\0';
    do
    { 
        *--p = digit[i%10];
        i = i/10;
    } while(i);
    return b;
}
 
void drawStats(void *font, char *string, float red, float green, float blue, float xstart, float ystart, float zstart)
{
    char *charPtr;
    glColor3f(red,green,blue);
    glRasterPos3f(xstart,ystart,zstart);
    for (charPtr=string; *charPtr; charPtr++) 
    {
        glutBitmapCharacter(font, *charPtr);
    }
}
 
void drawTotalKilled(void *font, float red, float green, float blue, float xstart, float ystart, float zstart)
{
    char *cTotalKilled;
    char b[4];
    char *charPtr;
    cTotalKilled = fitoa(totalKilled, b);
    glColor3f(red,green,blue);
    glRasterPos3f(xstart,ystart,zstart); 
    for (charPtr=cTotalKilled; *charPtr; charPtr++) 
    {
        glutBitmapCharacter(font, *charPtr);
    }
}
 
void drawGameOver(void *font, char *string, float red, float green, float blue, float xstart, float ystart, float zstart)
{
    char *charPtr;
    glColor3f(red,green,blue);
    glRasterPos3f(xstart,ystart,zstart);  
    for (charPtr=string; *charPtr; charPtr++) 
    {
        glutBitmapCharacter(font, *charPtr);
    }
}
 
void drawLevel(void *font, float red, float green, float blue, float xstart, float ystart, float zstart)
{
    char *cLevel;
    char b[4];
    char *charPtr;
    cLevel = fitoa(wave, b);
    glColor3f(red,green,blue);
    glRasterPos3f(xstart,ystart,zstart);  
    for (charPtr=cLevel; *charPtr; charPtr++) 
    {
        glutBitmapCharacter(font, *charPtr);
    }
}
 
void renderScene(void) 
{
    float framerate = 33*(CLOCKS_PER_SEC/1000);
    clock_t now, then;
    now=then=clock();
    if(keyState[102]) goto PAUSE;
    glClearColor(0, 0, 0, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    gluLookAt(0.0f, 0.0f, 10.0f, 
            0.0f, 0.0f, 0.0f,
            0.0f, 1.0f,  0.0f);
    
    if(gamemode==0)
    {
        drawStats(GLUT_BITMAP_TIMES_ROMAN_24, "Press A to play!!", 1, 0, 0, -1.0, 0, 5.0);
        drawStats(GLUT_BITMAP_TIMES_ROMAN_24, "", 1, 0, 0, -1.0, -0.4, 5.0);
        drawStats(GLUT_BITMAP_TIMES_ROMAN_24, "How to Play", 1, 0, 0, -9.5, 5.0, 5.0);
        drawStats(GLUT_BITMAP_TIMES_ROMAN_24, "Welcome survivor. If we are going to last the night we need to make quick work of these undead.", 1, 0, 0, -9.5, 4.6, 5.0);
        drawStats(GLUT_BITMAP_TIMES_ROMAN_24, "Here are the controls you need to know", 1, 0, 0, -9.5, 4.2, 5.0);
        drawStats(GLUT_BITMAP_TIMES_ROMAN_24, "To Move: WASD", 1, 0, 0, -9.5, 3.8, 5.0);
        drawStats(GLUT_BITMAP_TIMES_ROMAN_24, "To shoot: Left Mouse Button", 1, 0, 0, -9.5, 3.4, 5.0);
        drawStats(GLUT_BITMAP_TIMES_ROMAN_24, "To aim: Mouse", 1, 0, 0, -9.5, 3.0, 5.0);
        drawStats(GLUT_BITMAP_TIMES_ROMAN_24, "To use melee: Right Mouse Button", 1, 0, 0, -9.5, 2.6, 5.0);
            
        if(keyState[97])
        {
            gamemode=1;
            fireDelay = 360;
            waveIncrement = 5;
        }
    }
    else if(gamemode==1)
    {
        if(abdo==NULL) createHuman();
        processKeys(); //Find human position. Right now would implicitly indicate human speed is dependent upon frame rate.
        humanAttack();
        
        if(walker==NULL) createWave();
        calculateZombieMovement();
        moveZombies();
        drawZombies();
        
        
        
        if(pbullet==NULL) createBullet();
        drawShooting();
        
        collision();
        
        drawHuman();
        
        if(!zombiesActive) 
        {
            free(walker);
            walker=NULL;
        }
 
        drawStats(GLUT_BITMAP_TIMES_ROMAN_24, "Total Zombies Killed:", 1, 0, 0, 5.1, 4.2, 5.0);
        drawTotalKilled(GLUT_BITMAP_TIMES_ROMAN_24, 1, 0, 0, 8.2, 4.2, 5.0);
        
        if(abdo->active==0)
        {
            gamemode=2;
        }
    }
    else if (gamemode==2)
    {
        drawGameOver(GLUT_BITMAP_TIMES_ROMAN_24, "GAME OVER", 1, 0, 0, -1.0, 0, 5.0);
        drawGameOver(GLUT_BITMAP_TIMES_ROMAN_24, "Level: ", 1, 0, 0, -0.7, -0.4, 5.0);
        drawGameOver(GLUT_BITMAP_TIMES_ROMAN_24, "Total Zombies Killed:", 1, 0, 0, -1.6, -0.8, 5.0);
        drawLevel(GLUT_BITMAP_TIMES_ROMAN_24, 1, 0, 0, 0.2, -0.4, 5.0);
        drawTotalKilled(GLUT_BITMAP_TIMES_ROMAN_24, 1, 0, 0, 1.5, -0.8, 5.0);
                
        calculateZombieMovement();
        moveZombies();
        drawZombies();
        drawHuman();
        
    }
    
    glutSwapBuffers();
    PAUSE:do{
        now=clock();
    }while( (now-then) < framerate);
    
} 
 
void processNormalKeys(unsigned char key, int xx, int yy) 
{
    if(key == 27)
    {
        free(walker);
        exit(0);
    }
    keyState[key] = 1;
} 
 
void clearNormalKeys(unsigned char key, int xx, int yy)
{   
    keyState[key] = 0;
}
 
void mouseUpdate(int x, int y)
{
    mouse.x = (float)(x-960)/100.0;
    mouse.y = -(float)(y-540)/100.0;
    mouse=unitVector(mouse);
}
 
void mousePressedUpdate(int button, int state, int x, int y)
{
    mouse.x = (float)(x-960)/100.0;
    mouse.y = -(float)(y-540)/100.0;
    mouse=unitVector(mouse);
    if(gamemode)
    {
        switch(button)
        {
            case GLUT_LEFT_BUTTON:
                if(state==GLUT_DOWN)
                {
                    fireRequest=1;
                }
                else fireRequest=0;
                break;
            case GLUT_RIGHT_BUTTON:
                if(state==GLUT_DOWN)
                {
                    meleeRequest=1;
                }   
                else meleeRequest=0;
                break;
        }
    }
}
 
int main(int argc, char **argv) {
    srand(time(NULL));
    glutInit(&argc, argv);
    glutInitWindowPosition(0,0);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(300, 300);
    glutCreateWindow("Humans Vs. Zombies");
    glutSetCursor(GLUT_CURSOR_NONE);
    glutFullScreen();
 
    glutDisplayFunc(renderScene);
    glutReshapeFunc(changeSize);
    glutIdleFunc(renderScene);
 
    glutKeyboardFunc(processNormalKeys);
    glutKeyboardUpFunc(clearNormalKeys);
 
    glEnable(GL_DEPTH_TEST);
    glutWarpPointer(1920/2, 1080/2);
    glutPassiveMotionFunc(mouseUpdate);
    glutMotionFunc(mouseUpdate);
    glutMouseFunc(mousePressedUpdate);
    glutMainLoop();
    
    return 1;
}
