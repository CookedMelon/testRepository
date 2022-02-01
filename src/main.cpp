#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include<SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <math.h>
#define WINDOWWIDTH 1200
#define WINDOWHEIGHT 700
#define VRMaxSpeed 60
#define UPMaxSpeed 100
#define BulletSpeed 10
#define EnemyMaxNum 50
#define EnemyNumAtFirst 10
#define BulletMaxNum 100
#define LASERHEIGHT 20
#define UPCD 6
#define VRCD 15
#define CHANGECD 20
#define EnemyGrowCD 20
#define laserDelay 3
#define LIFE 3
#define TURNSNEED 1000
int turns = 0;
int VRWidth,VRHeight,UPWidth,UPHeight,EnemyWidth,EnemyHeight,bulletWidth,bulletHeight,laserWidth,laserHeight,highTextWidth,highTextHeight,changeTextWidth,changeTextHeight;
int mouseX = 100,mouseY = 100,mode=0;
int nowEnemyNum=EnemyNumAtFirst,nowBulletNum=0;
int life=LIFE;
SDL_Rect VR,VRPos,UP,UPPos,Enemy,bullet,explode,laserRec,high,highPos,highText,change,changePos,changeText,laserPos;
SDL_Texture *VRTexture[2],*UPTexture[2],*bulletTexture,*explodeTexture,*laserTexture,*EnemyTexture[3],*Enemy_downTexture,*highTexture,*highTextTexture,*changeTexture,*changeTextTexture;
Mix_Chunk *highChunk,*changeChunk,*BGMChunk,*boomChunk,*bulletChunk,*hitChunk,*laserChunk,*loseChunk,*lose2Chunk,*vectoryChunk;

SDL_Window *window = SDL_CreateWindow("Toilet Wars!", 100, 100, WINDOWWIDTH, WINDOWHEIGHT, SDL_WINDOW_SHOWN);
SDL_Renderer *rend = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
typedef struct bullet
{
    int ifexist;
    int notnull;
    SDL_Rect BullPos;
} BULLET;
typedef struct las{
    int x;
    int y;
    int type;
}LASER;
typedef struct enemy
{
    int type;
    int stage;
    int speed;
    int life;
    int notnull;
    SDL_Rect EnePos;
} ENEMY;
BULLET bulletList[BulletMaxNum];
ENEMY enemyList[EnemyMaxNum];
LASER laser;
void growNewEnemy(int i){
    enemyList[i].life=3;
    enemyList[i].stage=0;
    enemyList[i].notnull=1;
    enemyList[i].speed=rand()%5+2;
    enemyList[i].EnePos.x=rand()%40+WINDOWWIDTH;
    enemyList[i].EnePos.w=EnemyWidth/2;
    enemyList[i].EnePos.h=EnemyHeight/2;
    enemyList[i].EnePos.y=rand()%(WINDOWHEIGHT-enemyList[i].EnePos.h);
    if(i>=nowEnemyNum)nowEnemyNum=i+1;
}
void CopyVRByHeart()
{
    int nowHeartX;
    int nowHeartY;
    int pastHeartX = VRPos.x + VRPos.w / 2;
    int pastHeartY = VRPos.y + VRPos.h / 2;
    int gapX = mouseX - pastHeartX;
    int gapY = mouseY - pastHeartY;
    double gapDistance = sqrt(gapX * gapX + gapY * gapY);
    if (gapDistance <= VRMaxSpeed)
    {
        nowHeartX = mouseX;
        nowHeartY = mouseY;
    }
    else
    {
        double times = gapDistance / VRMaxSpeed;
        nowHeartX = pastHeartX + (int)gapX / times;
        nowHeartY = pastHeartY + (int)gapY / times;
    }
    SDL_Rect nowPos;
    nowPos.x = nowHeartX - VRPos.w / 2;
    nowPos.y = nowHeartY - VRPos.h / 2;
    nowPos.w = VRPos.w;
    nowPos.h = VRPos.h;
    VRPos.x=nowPos.x;
    VRPos.y=nowPos.y;
    SDL_RenderCopy(rend, VRTexture[turns % 2], &VR, &nowPos);
}
void CopyUPByHeart()
{
    int nowHeartX;
    int nowHeartY;
    int pastHeartX = UPPos.x + UPPos.w / 2;
    int pastHeartY = UPPos.y + UPPos.h / 2;
    int gapX = mouseX - pastHeartX;
    int gapY = mouseY - pastHeartY;
    double gapDistance = sqrt(gapX * gapX + gapY * gapY);

    if (gapDistance <= UPMaxSpeed)
    {
        nowHeartX = mouseX;
        nowHeartY = mouseY;
    }
    else
    {
        double times = gapDistance / UPMaxSpeed;
        nowHeartX = pastHeartX + (int)gapX / times;
        nowHeartY = pastHeartY + (int)gapY / times;
    }
    SDL_Rect nowPos;
    nowPos.x = nowHeartX - UPPos.w / 2;
    nowPos.y = nowHeartY - UPPos.h / 2;
    nowPos.w = UPPos.w;
    nowPos.h = UPPos.h;
    UPPos.x=nowPos.x;
    UPPos.y=nowPos.y;
    SDL_RenderCopy(rend, UPTexture[turns % 2], &UP, &nowPos);
}
void CopyEnemies()
{
    for (int i = 0; i < EnemyMaxNum; i++)
    {
        if (enemyList[i].notnull)
        {
            if(enemyList[i].life>0){
                if(enemyList[i].stage==0){
                    SDL_RenderCopy(rend, EnemyTexture[enemyList[i].type], &Enemy, &enemyList[i].EnePos);
                }
                else{
                    SDL_RenderCopy(rend, EnemyTexture[2], &Enemy, &enemyList[i].EnePos);
                    enemyList[i].stage=0;
                }
            }

            else{
                SDL_RenderCopy(rend, Enemy_downTexture, &Enemy, &enemyList[i].EnePos);
            }

        }
    }
}
void CopyBullets()
{
    for (int i = 0; i < BulletMaxNum; i++)
    {
        if (bulletList[i].notnull&&bulletList[i].ifexist==1)
        {
            SDL_Rect newPos=bulletList[i].BullPos;
            newPos.x+=rand()%5-2;
            newPos.y+=rand()%3-1;
            SDL_RenderCopy(rend, bulletTexture, &bullet, &newPos);
        }
    }
}
void CopyLaser(){
    if(laser.type<=laserDelay&&laser.type>0){
        laserPos.x=laser.x;
        laserPos.y=laser.y;
        SDL_RenderCopy(rend, laserTexture, &laserRec, &laserPos);
        laser.type++;
    }else if(laser.type==laserDelay+1){
        laserPos.y=laser.y;
        for(int i=0;i<50;i++){
            laserPos.x=laser.x+20*i;
            laserPos.y=laser.y+rand()%5;
            SDL_RenderCopy(rend, laserTexture, &laserRec, &laserPos);
        }
        laser.type=0;
    }
}
void MoveMnemies(){
    for (int i = 0; i < EnemyMaxNum; i++)
    {
        if (enemyList[i].notnull)
        {
            if(enemyList[i].life>0){
                enemyList[i].EnePos.x=enemyList[i].EnePos.x-enemyList[i].speed;
                int tempy=enemyList[i].EnePos.y+=rand()%13-6;
                if(tempy>=0&&tempy<=WINDOWHEIGHT-enemyList[i].EnePos.h)
                    enemyList[i].EnePos.y=tempy;
                enemyList[i].type=1-enemyList[i].type;
                int yourHeartX;
                int yourHeartY;
                if(mode==0){
                    yourHeartX = VRPos.x + VRPos.w / 2;
                    yourHeartY = VRPos.y + VRPos.h / 2;
                }else{
                    yourHeartX = UPPos.x + UPPos.w / 2;
                    yourHeartY = UPPos.y + UPPos.h / 2;
                }

                int enemyHeartX=enemyList[i].EnePos.x+enemyList[i].EnePos.w/2;
                int enemyHeartY=enemyList[i].EnePos.y+enemyList[i].EnePos.h/2;
                if(enemyList[i].EnePos.x<=200||sqrt((yourHeartX-enemyHeartX)*(yourHeartX-enemyHeartX)+(yourHeartY-enemyHeartY)*(yourHeartY-enemyHeartY))<=enemyList[i].EnePos.w){
                    enemyList[i].life=-5;
                    life--;
                    SDL_RenderCopy(rend, explodeTexture, &explode, &enemyList[i].EnePos);
                    Mix_PlayChannel( -1, boomChunk, 0 );
                }

            }else if(enemyList[i].life<=0&&enemyList[i].life>-5){
                enemyList[i].life--;
            }else{
                growNewEnemy(i);
            }
        }
    }
}
void VRShoot(){
    Mix_PlayChannel( -1, laserChunk, 0 );
    laser.x=VRPos.x+VRPos.w;
    laser.y=VRPos.y+VRPos.h/2;
    laser.type=1;
}
void UPShoot(){
    Mix_PlayChannel( -1, bulletChunk, 0 );
    for(int i=0;i<BulletMaxNum;i++){
        if(bulletList[i].notnull==0||bulletList[i].ifexist==0){
            bulletList[i].ifexist=1;
            bulletList[i].notnull=1;
            bulletList[i].BullPos.x=UPPos.x+UPPos.w;
            bulletList[i].BullPos.y=UPPos.y+UPPos.h/2-6;
            bulletList[i].BullPos.w=bulletWidth/2;
            bulletList[i].BullPos.h=bulletHeight/2;

            for(int j=i+1;j<BulletMaxNum;j++){
                if(bulletList[j].notnull==0||bulletList[j].ifexist==0){
                bulletList[j].ifexist=1;
                bulletList[j].notnull=1;
                bulletList[j].BullPos.x=UPPos.x+UPPos.w;
                bulletList[j].BullPos.y=UPPos.y+UPPos.h/2+6;
                bulletList[j].BullPos.w=bulletWidth/2;
                bulletList[j].BullPos.h=bulletHeight/2;
                if(j+1>nowBulletNum)nowBulletNum=j+1;
                break;
                }
            }

            break;
        }
    }
}
void moveBullets(){
    for(int i=0;i<BulletMaxNum;i++){
        if(bulletList[i].ifexist==1){
            bulletList[i].BullPos.x+=BulletSpeed;
            if(bulletList[i].BullPos.x>WINDOWWIDTH+20)bulletList[i].ifexist=0;
        }
    }
}
void bulletHitEnemy(){
    int k=0;
    for(int i=0;i<nowBulletNum;i++){
        if(bulletList[i].notnull&&bulletList[i].ifexist==1){
            int bulletFrontX=bulletList[i].BullPos.x+bulletList[i].BullPos.w;
            int bulletFrontY=bulletList[i].BullPos.y+bulletList[i].BullPos.h/2;
            for(int j=0;j<nowEnemyNum;j++){
                if(enemyList[j].notnull&&enemyList[j].life>0){
                    if(bulletFrontX>=enemyList[j].EnePos.x+enemyList[j].EnePos.w/2&&bulletFrontX<=enemyList[j].EnePos.x+enemyList[j].EnePos.w&&bulletFrontY>enemyList[j].EnePos.y&&bulletFrontY<enemyList[j].EnePos.y+enemyList[j].EnePos.h) {
                        enemyList[j].stage=1;
                        enemyList[j].life--;
                        bulletList[i].ifexist=0;
                        k=1;
                        break;
                    }
                }
            }
        }
    }
    if(k==1){
        Mix_PlayChannel( -1, hitChunk, 0 );
    }
}
void laserHitEnemy(){
    int k=0;
    if(laser.type==laserDelay+1){
        for(int j=0;j<nowEnemyNum;j++){
            if(enemyList[j].notnull&&enemyList[j].life>0){
                if(laser.x<=enemyList[j].EnePos.x&&laser.y<enemyList[j].EnePos.y+enemyList[j].EnePos.h&&laser.y+laserPos.h>enemyList[j].EnePos.y){
                    enemyList[j].life--;
                    enemyList[j].stage=1;
                    k=1;
                }
            }
        }
    }
    if(k==1){
        Mix_PlayChannel( -1, hitChunk, 0 );
    }
}
void playhigh(){
    SDL_Rect TextPos1;
    TextPos1.y=WINDOWHEIGHT/2-highTextHeight/2;
    TextPos1.w=highTextWidth/2;
    TextPos1.h=highTextHeight/2;
    TextPos1.x=WINDOWWIDTH/4-TextPos1.w/2;

    SDL_Rect TextPos2;
    TextPos2.y=WINDOWHEIGHT/2-highTextHeight/2;
    TextPos2.w=highTextWidth/2;
    TextPos2.h=highTextHeight/2;
    TextPos2.x=(WINDOWWIDTH*3)/4-TextPos2.w/2;
    SDL_Texture *tempTextTexture=highTextTexture;
    SDL_SetTextureBlendMode(tempTextTexture,SDL_BLENDMODE_BLEND);
    for(int i=0;i<=200;i++){

        SDL_Texture *tempTexture=highTexture;
        SDL_SetTextureBlendMode(tempTexture,SDL_BLENDMODE_BLEND);
        SDL_SetTextureAlphaMod(tempTexture,i);
        SDL_RenderCopy(rend, tempTexture, &high, &highPos);

        if(i%20==0&&i<=90){
            Mix_PlayChannel( -1, highChunk, 0 );
        }
        SDL_SetTextureAlphaMod(tempTextTexture,i);
        SDL_RenderCopy(rend, tempTextTexture, &highText, &TextPos1);
        SDL_RenderCopy(rend, tempTextTexture, &highText, &TextPos2);

        SDL_RenderPresent(rend);
        SDL_Delay(20);
    }
    SDL_Rect TextPos11=TextPos1;
    SDL_Rect TextPos12=TextPos1;
    SDL_Rect TextPos13=TextPos1;
    SDL_Rect TextPos14=TextPos1;
    SDL_Rect TextPos21=TextPos2;
    SDL_Rect TextPos22=TextPos2;
    SDL_Rect TextPos23=TextPos2;
    SDL_Rect TextPos24=TextPos2;

    for(int i=0;i<10;i++){
        SDL_SetTextureAlphaMod(tempTextTexture,200-20*i);
        TextPos11.x+=TextPos1.h;
        TextPos11.y+=TextPos1.h;
        TextPos12.x+=TextPos1.h;
        TextPos12.y-=TextPos1.h;
        TextPos13.x-=TextPos1.h;
        TextPos13.y+=TextPos1.h;
        TextPos14.x-=TextPos1.h;
        TextPos14.y-=TextPos1.h;

        TextPos21.x-=TextPos1.h;
        TextPos21.y+=TextPos1.h;
        TextPos22.x-=TextPos1.h;
        TextPos22.y-=TextPos1.h;
        TextPos23.x+=TextPos1.h;
        TextPos23.y+=TextPos1.h;
        TextPos24.x+=TextPos1.h;
        TextPos24.y-=TextPos1.h;

        SDL_RenderCopy(rend, tempTextTexture, &highText, &TextPos11);
        SDL_RenderCopy(rend, tempTextTexture, &highText, &TextPos12);
        SDL_RenderCopy(rend, tempTextTexture, &highText, &TextPos13);
        SDL_RenderCopy(rend, tempTextTexture, &highText, &TextPos14);
        SDL_RenderCopy(rend, tempTextTexture, &highText, &TextPos21);
        SDL_RenderCopy(rend, tempTextTexture, &highText, &TextPos22);
        SDL_RenderCopy(rend, tempTextTexture, &highText, &TextPos23);
        SDL_RenderCopy(rend, tempTextTexture, &highText, &TextPos24);

        SDL_RenderPresent(rend);
        SDL_Delay(50);
    }
}
void playchangeUP(){
    SDL_Rect TextPos1;
    TextPos1.y=WINDOWHEIGHT/2-changeTextHeight/2;
    TextPos1.w=changeTextWidth/2;
    TextPos1.h=changeTextHeight/2;
    TextPos1.x=WINDOWWIDTH/4-TextPos1.w/2;

    SDL_Rect TextPos2;
    TextPos2.y=WINDOWHEIGHT/2-changeTextHeight/2;
    TextPos2.w=changeTextWidth/2;
    TextPos2.h=changeTextHeight/2;
    TextPos2.x=(WINDOWWIDTH*3)/4-TextPos2.w/2;
    SDL_Texture *tempTextTexture=changeTextTexture;
    SDL_SetTextureBlendMode(tempTextTexture,SDL_BLENDMODE_BLEND);
    for(int i=0;i<=200;i++){

        SDL_Texture *tempTexture=changeTexture;
        SDL_SetTextureBlendMode(tempTexture,SDL_BLENDMODE_BLEND);
        SDL_SetTextureAlphaMod(tempTexture,i);
        SDL_RenderCopy(rend, tempTexture, &change, &changePos);

        if(i%40==0&&i<=40){
            Mix_PlayChannel( -1, changeChunk, 0 );
        }
        SDL_SetTextureAlphaMod(tempTextTexture,i);
        SDL_RenderCopy(rend, tempTextTexture, &changeText, &TextPos1);
        SDL_RenderCopy(rend, tempTextTexture, &changeText, &TextPos2);

        SDL_RenderPresent(rend);
        SDL_Delay(10);
    }
    SDL_Rect TextPos11=TextPos1;
    SDL_Rect TextPos12=TextPos1;
    SDL_Rect TextPos13=TextPos1;
    SDL_Rect TextPos14=TextPos1;
    SDL_Rect TextPos21=TextPos2;
    SDL_Rect TextPos22=TextPos2;
    SDL_Rect TextPos23=TextPos2;
    SDL_Rect TextPos24=TextPos2;

    for(int i=0;i<10;i++){
        SDL_SetTextureAlphaMod(tempTextTexture,200-20*i);
        TextPos11.x+=TextPos1.h;
        TextPos11.y+=TextPos1.h;
        TextPos12.x+=TextPos1.h;
        TextPos12.y-=TextPos1.h;
        TextPos13.x-=TextPos1.h;
        TextPos13.y+=TextPos1.h;
        TextPos14.x-=TextPos1.h;
        TextPos14.y-=TextPos1.h;

        TextPos21.x-=TextPos1.h;
        TextPos21.y+=TextPos1.h;
        TextPos22.x-=TextPos1.h;
        TextPos22.y-=TextPos1.h;
        TextPos23.x+=TextPos1.h;
        TextPos23.y+=TextPos1.h;
        TextPos24.x+=TextPos1.h;
        TextPos24.y-=TextPos1.h;

        SDL_RenderCopy(rend, tempTextTexture, &changeText, &TextPos11);
        SDL_RenderCopy(rend, tempTextTexture, &changeText, &TextPos12);
        SDL_RenderCopy(rend, tempTextTexture, &changeText, &TextPos13);
        SDL_RenderCopy(rend, tempTextTexture, &changeText, &TextPos14);
        SDL_RenderCopy(rend, tempTextTexture, &changeText, &TextPos21);
        SDL_RenderCopy(rend, tempTextTexture, &changeText, &TextPos22);
        SDL_RenderCopy(rend, tempTextTexture, &changeText, &TextPos23);
        SDL_RenderCopy(rend, tempTextTexture, &changeText, &TextPos24);

        SDL_RenderPresent(rend);
        SDL_Delay(50);
    }
}

int main(int argc, char *argv[])
{
    SDL_Init(SDL_INIT_VIDEO);
    if(TTF_Init()==-1) {
        printf("TTF_Init: %s\n", TTF_GetError()); return 1;
    }

    TTF_Font *font=TTF_OpenFont("ALGER.TTF", 64);

    if(!font) {
        printf("TTF_OpenFont: %s\n", TTF_GetError());
    }
    SDL_Color blue;
    blue.a=255;
    blue.b=255;
    blue.g=0;
    blue.r=0;
    SDL_Color red;
    red.a=255;
    red.b=0;
    red.g=0;
    red.r=255;
    SDL_Surface* timeTxt=TTF_RenderUTF8_Blended(font,"Time Left:",blue);
    SDL_Texture* timeTxtTexture=SDL_CreateTextureFromSurface(rend,timeTxt);
    SDL_Rect timeTxtRect;
    timeTxtRect.x=20;
    timeTxtRect.y=100;
    timeTxtRect.w=timeTxt->w/2;
    timeTxtRect.h=timeTxt->h/2;
    SDL_Surface* lifeTxt=TTF_RenderUTF8_Blended(font,"Life Left:",blue);
    SDL_Texture* lifeTxtTexture=SDL_CreateTextureFromSurface(rend,lifeTxt);
    SDL_Rect lifeTxtRect;
    lifeTxtRect.x=20;
    lifeTxtRect.y=100+timeTxtRect.h;
    lifeTxtRect.w=lifeTxt->w/2;
    lifeTxtRect.h=lifeTxt->h/2;


    Mix_OpenAudio(44100,MIX_DEFAULT_FORMAT,2,2048);

    highChunk = Mix_LoadWAV("MP3/high.mp3");
    changeChunk = Mix_LoadWAV("MP3/change.mp3");
    BGMChunk = Mix_LoadWAV("MP3/BGM.mp3");
    boomChunk = Mix_LoadWAV("MP3/boom.MP3");
    bulletChunk = Mix_LoadWAV("MP3/bullet.MP3");
    hitChunk = Mix_LoadWAV("MP3/hit.MP3");
    laserChunk = Mix_LoadWAV("MP3/laser.MP3");
    loseChunk = Mix_LoadWAV("MP3/lose.MP3");
    lose2Chunk = Mix_LoadWAV("MP3/lose2.MP3");
    vectoryChunk = Mix_LoadWAV("MP3/vectory.MP3");
    Mix_Chunk* loseChunk = Mix_LoadWAV("MP3/lose.mp3");

    if(!BGMChunk||!highChunk||!changeChunk||!loseChunk||!boomChunk||!bulletChunk||!hitChunk||!laserChunk||!loseChunk||!lose2Chunk||!vectoryChunk) {
        printf(Mix_GetError());
    }

    int bgmInt=Mix_PlayChannel( -1, BGMChunk, -1 );


    SDL_Surface *surface = SDL_GetWindowSurface(window);

    SDL_Surface *VRImg[2] = {SDL_LoadBMP("BMP/VR.bmp"), SDL_LoadBMP("BMP/VR2.bmp")};
    SDL_SetColorKey(VRImg[0], SDL_TRUE, SDL_MapRGB(VRImg[0]->format, 255, 255, 255));
    SDL_SetColorKey(VRImg[1], SDL_TRUE, SDL_MapRGB(VRImg[1]->format, 255, 255, 255));
    VRTexture[0] = SDL_CreateTextureFromSurface(rend, VRImg[0]);
    VRTexture[1] = SDL_CreateTextureFromSurface(rend, VRImg[1]);
    VRWidth = VRImg[0]->w;
    VRHeight = VRImg[0]->h;

    VR.x = 0;
    VR.y = 0;
    VR.w = VRWidth;
    VR.h = VRHeight;

    VRPos.x = 0;
    VRPos.y = 0;
    VRPos.w = VRWidth/2;
    VRPos.h = VRHeight/2;

    SDL_Surface *UPImg[2] = {SDL_LoadBMP("BMP/UP.bmp"),SDL_LoadBMP("BMP/UP2.bmp")};
    SDL_SetColorKey(UPImg[0], SDL_TRUE, SDL_MapRGB(UPImg[0]->format, 255, 255, 255));
    SDL_SetColorKey(UPImg[1], SDL_TRUE, SDL_MapRGB(UPImg[1]->format, 255, 255, 255));
    UPTexture[0] = SDL_CreateTextureFromSurface(rend, UPImg[0]);
    UPTexture[1] = SDL_CreateTextureFromSurface(rend, UPImg[1]);
    UPWidth = UPImg[0]->w;
    UPHeight = UPImg[1]->h;

    UP.x = 0;
    UP.y = 0;
    UP.w = UPWidth;
    UP.h = UPHeight;

    UPPos.x = 0;
    UPPos.y = 0;
    UPPos.w = UPWidth/2;
    UPPos.h = UPHeight/2;

    SDL_Surface *EnemyImg[3]={SDL_LoadBMP("BMP/enemy.bmp"),SDL_LoadBMP("BMP/enemy2.bmp"),SDL_LoadBMP("BMP/enemy_hit.bmp")};
    SDL_SetColorKey(EnemyImg[0], SDL_TRUE, SDL_MapRGB(EnemyImg[0]->format, 255, 255, 255));
    SDL_SetColorKey(EnemyImg[1], SDL_TRUE, SDL_MapRGB(EnemyImg[1]->format, 255, 255, 255));
    SDL_SetColorKey(EnemyImg[2], SDL_TRUE, SDL_MapRGB(EnemyImg[2]->format, 255, 255, 255));
    EnemyTexture[0] = SDL_CreateTextureFromSurface(rend, EnemyImg[0]);
    EnemyTexture[1] = SDL_CreateTextureFromSurface(rend, EnemyImg[1]);
    EnemyTexture[2] = SDL_CreateTextureFromSurface(rend, EnemyImg[2]);
    SDL_Surface *EnemyImg_down=SDL_LoadBMP("BMP/enemy_down.bmp");
    SDL_SetColorKey(EnemyImg_down, SDL_TRUE, SDL_MapRGB(EnemyImg_down->format, 255, 255, 255));
    Enemy_downTexture = SDL_CreateTextureFromSurface(rend, EnemyImg_down);
    EnemyWidth=EnemyImg[0]->w;
    EnemyHeight=EnemyImg[0]->h;

    Enemy.x = 0;
    Enemy.y = 0;
    Enemy.w = EnemyWidth;
    Enemy.h = EnemyHeight;

    SDL_Surface *bulletImg = SDL_LoadBMP("BMP/bullet1.bmp");
    SDL_SetColorKey(bulletImg, SDL_TRUE, SDL_MapRGB(bulletImg->format, 255, 255, 255));
    bulletTexture = SDL_CreateTextureFromSurface(rend, bulletImg);
    bulletWidth = bulletImg->w;
    bulletHeight = bulletImg->h;

    bullet.x = 0;
    bullet.y = 0;
    bullet.w = bulletWidth;
    bullet.h = bulletHeight;

    SDL_Surface *laserImg = SDL_LoadBMP("BMP/laser1.bmp");
    SDL_SetColorKey(laserImg, SDL_TRUE, SDL_MapRGB(laserImg->format, 255, 255, 255));
    laserTexture = SDL_CreateTextureFromSurface(rend, laserImg);
    laserWidth = laserImg->w;
    laserHeight = laserImg->h;

    laserRec.x = 0;
    laserRec.y = 0;
    laserRec.w = laserWidth;
    laserRec.h = laserHeight;

    laserPos.h=laserHeight/2;
    laserPos.w=laserWidth/2;

    SDL_Surface *explodeImg = SDL_LoadBMP("BMP/explode.bmp");
    SDL_SetColorKey(explodeImg, SDL_TRUE, SDL_MapRGB(explodeImg->format, 255, 255, 255));
    explodeTexture = SDL_CreateTextureFromSurface(rend, explodeImg);
    int explodeWidth = explodeImg->w;
    int explodeHeight = explodeImg->h;

    explode.x = 0;
    explode.y = 0;
    explode.w = explodeWidth;
    explode.h = explodeHeight;


    SDL_Surface *toiletImg = SDL_LoadBMP("BMP/toilet.bmp");
    SDL_SetColorKey(toiletImg, SDL_TRUE, SDL_MapRGB(toiletImg->format, 255, 255, 255));
    SDL_Texture *toiletTexture = SDL_CreateTextureFromSurface(rend, toiletImg);
    int toiletWidth = toiletImg->w;
    int toiletHeight = toiletImg->h;
    SDL_Rect toilet;
    toilet.x = 0;
    toilet.y = 0;
    toilet.w = toiletWidth;
    toilet.h = toiletHeight;
    SDL_Rect toiletPos;
    toiletPos.x = 0;
    toiletPos.y = 0;
    toiletPos.w = 300;
    toiletPos.h = WINDOWHEIGHT;

    SDL_Surface *BGImg = SDL_LoadBMP("BMP/background.bmp");
    SDL_SetColorKey(BGImg, SDL_TRUE, SDL_MapRGB(BGImg->format, 255, 255, 255));
    SDL_Texture *BGTexture = SDL_CreateTextureFromSurface(rend, BGImg);
    int BGWidth = BGImg->w;
    int BGHeight = BGImg->h;
    SDL_Rect BG;
    BG.x = 0;
    BG.y = 0;
    BG.w = BGWidth;
    BG.h = BGHeight;
    SDL_Rect BGPos;
    BGPos.x = 0;
    BGPos.y = 0;
    BGPos.w = WINDOWWIDTH;
    BGPos.h = WINDOWHEIGHT;

    SDL_Surface *DEImg = SDL_LoadBMP("BMP/describe.bmp");
    SDL_SetColorKey(DEImg, SDL_TRUE, SDL_MapRGB(DEImg->format, 255, 255, 255));
    SDL_Texture *DETexture = SDL_CreateTextureFromSurface(rend, DEImg);
    int DEWidth = DEImg->w;
    int DEHeight = DEImg->h;
    SDL_Rect DE;
    DE.x = 0;
    DE.y = 0;
    DE.w = DEWidth;
    DE.h = DEHeight;
    SDL_Rect DEPos;
    DEPos.w = DEWidth;
    DEPos.h = DEHeight/1.5;
    DEPos.x = WINDOWWIDTH/2-DEPos.w/2;
    DEPos.y = WINDOWHEIGHT/2-DEPos.h/2;

    SDL_Surface *highImg = SDL_LoadBMP("BMP/high.bmp");
    SDL_SetColorKey(highImg, SDL_TRUE, SDL_MapRGB(highImg->format, 255, 255, 255));
    highTexture = SDL_CreateTextureFromSurface(rend, highImg);
    int highWidth = highImg->w;
    int highHeight = highImg->h;
    high.x = 0;
    high.y = 0;
    high.w = highWidth;
    high.h = highHeight;
    highPos.x = WINDOWWIDTH/2-highWidth/6;
    highPos.y = WINDOWHEIGHT/2-highHeight/6;
    highPos.w = highWidth/3;
    highPos.h = highHeight/3;

    SDL_Surface *highTextImg = SDL_LoadBMP("BMP/highText.bmp");
    SDL_SetColorKey(highTextImg, SDL_TRUE, SDL_MapRGB(highTextImg->format, 255, 255, 255));
    highTextTexture = SDL_CreateTextureFromSurface(rend, highTextImg);
    highTextWidth = highTextImg->w;
    highTextHeight = highTextImg->h;
    highText.x = 0;
    highText.y = 0;
    highText.w = highTextWidth;
    highText.h = highTextHeight;

    SDL_Surface *changeImg = SDL_LoadBMP("BMP/change.bmp");
    SDL_SetColorKey(changeImg, SDL_TRUE, SDL_MapRGB(changeImg->format, 255, 255, 255));
    changeTexture = SDL_CreateTextureFromSurface(rend, changeImg);
    int changeWidth = changeImg->w;
    int changeHeight = changeImg->h;
    change.x = 0;
    change.y = 0;
    change.w = changeWidth;
    change.h = changeHeight;
    changePos.x = WINDOWWIDTH/2-changeWidth/6;
    changePos.y = WINDOWHEIGHT/2-changeHeight/6;
    changePos.w = changeWidth/3;
    changePos.h = changeHeight/3;

    SDL_Surface *changeTextImg = SDL_LoadBMP("BMP/changeText.bmp");
    SDL_SetColorKey(changeTextImg, SDL_TRUE, SDL_MapRGB(changeTextImg->format, 255, 255, 255));
    changeTextTexture = SDL_CreateTextureFromSurface(rend, changeTextImg);
    changeTextWidth = changeTextImg->w;
    changeTextHeight = changeTextImg->h;
    changeText.x = 0;
    changeText.y = 0;
    changeText.w = changeTextWidth;
    changeText.h = changeTextHeight;

    for(int i=0;i<EnemyNumAtFirst;i++){
        growNewEnemy(i);
    }

    bool quit = false;
    SDL_Event event;

    int upcd=0;
    int vrcd=0;
    int changecd=CHANGECD;
    int prepare=0;
    while (quit == false)
    {
        if(turns==1){
            SDL_SetTextureBlendMode(DETexture,SDL_BLENDMODE_BLEND);
            for(int i=0;i<50;i++){
                SDL_SetTextureAlphaMod(DETexture,i*4);
                SDL_RenderCopy(rend, DETexture, &DE, &DEPos);
                SDL_RenderPresent(rend);
                SDL_Delay(100);
            }
        };
        turns++;
        upcd--;
        vrcd--;
        changecd--;
        if(life<=0){
            Mix_Pause(bgmInt);
            SDL_Surface* Txt=TTF_RenderUTF8_Blended(font,"YOU LOSE！",red);
            SDL_Texture* TxtTexture=SDL_CreateTextureFromSurface(rend,Txt);
            SDL_Rect TxtRect;
            TxtRect.w=Txt->w*2;
            TxtRect.h=Txt->h*2;
            TxtRect.x=WINDOWWIDTH/2-TxtRect.w/2;
            TxtRect.y=WINDOWHEIGHT/2-TxtRect.h/2;
            SDL_RenderCopy(rend, TxtTexture, nullptr, &TxtRect);
            SDL_RenderPresent(rend);
            printf("你输了\n");
            Mix_PlayChannel( -1, lose2Chunk, 0 );
            SDL_Delay(8000);
            Mix_PlayChannel( -1, loseChunk, 0 );
            SDL_Delay(5000);
            break;
        }
        if(turns==TURNSNEED){
            Mix_Pause(bgmInt);
            SDL_Surface* Txt=TTF_RenderUTF8_Blended(font,"YOU WIN！",red);
            SDL_Texture* TxtTexture=SDL_CreateTextureFromSurface(rend,Txt);
            SDL_Rect TxtRect;
            TxtRect.w=Txt->w*2;
            TxtRect.h=Txt->h*2;
            TxtRect.x=WINDOWWIDTH/2-TxtRect.w/2;
            TxtRect.y=WINDOWHEIGHT/2-TxtRect.h/2;
            SDL_RenderCopy(rend, TxtTexture, nullptr, &TxtRect);
            SDL_RenderPresent(rend);
            printf("你赢了\n");
            Mix_PlayChannel(-1, vectoryChunk, 0);
            SDL_Delay(32000);
            break;
        }

        if(turns%EnemyGrowCD==0&&nowEnemyNum<EnemyMaxNum){
            growNewEnemy(nowEnemyNum);
        }
        if(prepare==1){
            if(mode==0&&vrcd<=0){
                VRShoot();
                vrcd=VRCD+1;
                prepare=0;
            }else if(mode==1&&upcd<=0){
                UPShoot();
                upcd=UPCD+1;
                prepare=0;
            }
        }
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                quit = true;
                break;
            }else if (event.type == SDL_MOUSEMOTION)
            {
                mouseX = event.motion.x;
                mouseY = event.motion.y;
            }else if(event.type==SDL_MOUSEBUTTONDOWN){
                if(event.button.button==3){
                    if(changecd<=0){
                        mode=1-mode;
                        if(mode==0){
                            VRPos.x=UPPos.x+UPPos.w/2-VRPos.w/2;
                            VRPos.y=UPPos.y+UPPos.h/2-VRPos.h/2;
                            playhigh();
                        }else{
                            UPPos.x=VRPos.x+VRPos.w/2-UPPos.w/2;
                            UPPos.y=VRPos.y+VRPos.h/2-UPPos.h/2;
                            playchangeUP();
                        }
                        prepare=0;
                        changecd=CHANGECD;
                    }
                }
            }else if(event.type==SDL_KEYDOWN){
                if(event.key.keysym.sym==122){
                    if(mode==0&&vrcd<=0){
                        VRShoot();
                        vrcd=VRCD+1;
                    }else if(mode==0&&vrcd>0&&vrcd<=VRCD/2){
                        prepare=1;
                    }
                    else if(mode==1&&upcd<=0){
                        UPShoot();
                        upcd=UPCD+1;
                    }else if(mode==1&&upcd>0&&upcd<=UPCD/2){
                        prepare=1;
                    }

                }
            }
        }
        char TimeStr[7];
        sprintf(TimeStr,"%d",TURNSNEED-turns);
        SDL_Surface* timeTxt2=TTF_RenderUTF8_Blended(font,TimeStr,blue);
        SDL_Texture* timeTxtTexture2=SDL_CreateTextureFromSurface(rend,timeTxt2);
        SDL_Rect timeTxtRect2;
        timeTxtRect2.x=20+timeTxtRect.w;
        timeTxtRect2.y=100;
        timeTxtRect2.w=timeTxt2->w/2;
        timeTxtRect2.h=timeTxt2->h/2;
        char lifeStr[3];
        sprintf(lifeStr,"%d",life);
        SDL_Surface* lifeTxt2=TTF_RenderUTF8_Blended(font,lifeStr,blue);
        SDL_Texture* lifeTxtTexture2=SDL_CreateTextureFromSurface(rend,lifeTxt2);
        SDL_Rect lifeTxtRect2;
        lifeTxtRect2.x=20+lifeTxtRect.w;
        lifeTxtRect2.y=100+timeTxtRect.h;
        lifeTxtRect2.w=lifeTxt2->w/2;
        lifeTxtRect2.h=lifeTxt2->h/2;

        SDL_RenderClear(rend);
        SDL_RenderCopy(rend, BGTexture, &BG, &BGPos);
        SDL_RenderCopy(rend, toiletTexture, &toilet, &toiletPos);
        SDL_RenderCopy(rend, timeTxtTexture, nullptr, &timeTxtRect);
        SDL_RenderCopy(rend, timeTxtTexture2, nullptr, &timeTxtRect2);
        SDL_RenderCopy(rend, lifeTxtTexture, nullptr, &lifeTxtRect);
        SDL_RenderCopy(rend, lifeTxtTexture2, nullptr, &lifeTxtRect2);
        moveBullets();
        CopyEnemies();
        SDL_RenderPresent(rend);
        CopyLaser();
        CopyBullets();
        laserHitEnemy();
        bulletHitEnemy();
        if(mode==0)
            CopyVRByHeart();
        else
            CopyUPByHeart();
        MoveMnemies();


        SDL_RenderPresent(rend);
        SDL_Delay(100);
    }


    SDL_UpdateWindowSurface(window);
    SDL_FreeSurface(surface);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
