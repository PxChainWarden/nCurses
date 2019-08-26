#include <iostream>
#include <ncurses.h>
#include <unistd.h>
#include <thread>
#include <vector>

using namespace std;

wstring TetrisBlocks[7];
int TetrisWidth = 12;
int TetrisHeight = 18;
unsigned char  *pScreen = nullptr;

int kbhit(void)
{
    int ch = getch();

    if (ch != ERR) 
    {
        ungetch(ch);
        return 1;
    } 
    else 
    {
        return 0;
    }
}

int RotateBlocks(int px, int py, int r)
{
    switch(r%4)
    {                           //4 e 4 luk bir cisimin dondurunce hangi parcasi nerede onu bulmak icin bir algoritma
        case 0: return py * 4 + px;        //  0 derece
        case 1: return 12 + py - (px * 4); // 90 derece
        case 2: return 15 - (py * 4) - px; //180 derece
        case 3: return 3 - py + (px * 4);  //270 derece
    }
    return 0;
}

bool DoesPieceFit(int nTetris, int nRotation, int PosX, int PosY)
{
    for(int px=0; px < 4; px++)
        for(int py=0; py < 4; py++)
        {
            //Cisim dondurulunce hangi parcasi nereye geliyo diye bakiyoruz boylece sigip sigmiycagini anlicaz
            int pi = RotateBlocks(px, py, nRotation);
            //Bu sefer de oyun alanindaki hangi parca denk geliyo diye
            int fi = (PosY+py)*TetrisWidth+(PosX+px);

            if(PosX+px >= 0 && PosX+px < TetrisWidth)
            {
                if(PosY+py >= 0 && PosY+py < TetrisHeight)
                {
                    if(TetrisBlocks[nTetris][pi] == L'x' && pScreen[fi] != 0) //Mevcut tetris seklinin rotasyon oldugu yerde baska bir sey varsa false donuyor
                        return false;
                }
            }
        }

    return true;
}

int main()
{
    
    // Sekilleri tanimliyoruz
    TetrisBlocks[0].append(L"..x.");
    TetrisBlocks[0].append(L"..x.");
    TetrisBlocks[0].append(L"..x.");
    TetrisBlocks[0].append(L"..x.");

    TetrisBlocks[1].append(L"..x.");
    TetrisBlocks[1].append(L".xx.");
    TetrisBlocks[1].append(L".x..");
    TetrisBlocks[1].append(L"....");

    TetrisBlocks[2].append(L".x..");
    TetrisBlocks[2].append(L".xx.");
    TetrisBlocks[2].append(L"..x.");
    TetrisBlocks[2].append(L"....");

    TetrisBlocks[3].append(L"....");
    TetrisBlocks[3].append(L".xx.");
    TetrisBlocks[3].append(L".xx.");
    TetrisBlocks[3].append(L"....");
    
    TetrisBlocks[4].append(L"..x.");
    TetrisBlocks[4].append(L".xx.");
    TetrisBlocks[4].append(L"..x.");
    TetrisBlocks[4].append(L"....");

    TetrisBlocks[5].append(L"....");
    TetrisBlocks[5].append(L".xx.");
    TetrisBlocks[5].append(L"..x.");
    TetrisBlocks[5].append(L"..x.");

    TetrisBlocks[6].append(L"....");
    TetrisBlocks[6].append(L".xx.");
    TetrisBlocks[6].append(L".x..");
    TetrisBlocks[6].append(L".x..");

    pScreen = new unsigned char[TetrisWidth * TetrisHeight]; // Cizmek istedigim ekran icin buffer sayilabilir burdaki degerlere gore gercek buffera olmasi gereken karakterleri basiyorum

    for(int x=0; x < TetrisWidth; x++)
        for(int y=0; y < TetrisHeight; y++)
            pScreen[y * TetrisWidth + x] = (x == 0 || (x == TetrisWidth - 1) || y == (TetrisHeight - 1)) ? 9 : 0; // Oyun alaninin kenarlarini 9 ile dolduruyorum 9 u ileride kullanicam

    initscr();
    noecho();
    curs_set(FALSE);
    nodelay(stdscr, TRUE);

    // Game logic variables
    
    bool gameOver = false;
    bool bKey[4];
    bool RotateHold=false;
    bool ForceDown=false;

    int nCurrentPiece    = 0;
    int nCurrentRotation = 0;
    int nCurrentX        = TetrisWidth / 2;
    int nCurrentY        = 0;
    int nSpeed           = 20;
    int nSpeedCounter    = 0;
    int nPieceCount      = 0;
    int nScore           = 0;

    char key;

    vector<int> vLines;

    while (!gameOver)
    {
        bKey[0]=false;
        bKey[1]=false;
        bKey[2]=false;
        bKey[3]=false;
        key=' ';
        //GAME TIMING / Zaman ===================================================================
        refresh();                                                      // Ekrani yenileyelim ki cizdiklerimiz gelsin
        this_thread::sleep_for(50ms);                                   // 64 Tick'e laf eden ben 20 tick oyun yapiyom nice :D
        nSpeedCounter++;
        ForceDown = (nSpeedCounter == nSpeed);


        //INPUT / Girdi =========================================================================
        if(kbhit())
        {
            key=getch();
            if(key==106) bKey[0]=true;
            if(key==107) bKey[1]=true;
            if(key==108) bKey[2]=true;
            if(key==122) bKey[3]=true;
        }
        // 106=j 107=k 108=l z=122

        //GAME LOGIC / Mantik en sevdigim =======================================================
        nCurrentX        -= ((bKey[0])&&(DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX - 1, nCurrentY))) ? 1 : 0;
        nCurrentY        += ((bKey[1])&&(DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY + 1))) ? 1 : 0;
        nCurrentX        += ((bKey[2])&&(DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX + 1, nCurrentY))) ? 1 : 0;
        
        if(bKey[3])
        {
            nCurrentRotation += (!(RotateHold)&&(DoesPieceFit(nCurrentPiece, nCurrentRotation + 1, nCurrentX, nCurrentY))) ? 1 : 0; 
            RotateHold=true;
        }
        else 
            RotateHold=false;
        
        if(ForceDown)
        {
            if(DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY + 1))
                nCurrentY++;
            else
            {
                //Eski parcayi sabitle
                for(int x=0; x < 4; x++)
                    for(int y=0; y < 4; y++)
                        if(TetrisBlocks[nCurrentPiece][RotateBlocks(x, y, nCurrentRotation)] == L'x')
                            pScreen[(nCurrentY + y) * TetrisWidth + (nCurrentX + x)] = nCurrentPiece + 1;
                
                nPieceCount++;
                nSpeed -= (nPieceCount % 10 == 0 && nSpeed >= 10) ? 1 : 0;

                //PATLAAAAAAT
                for(int py=0; py < 4; py++)
                    if(nCurrentY + py < TetrisHeight - 1)
                    {
                        bool bLine = true;
                        for(int px = 1; px < TetrisWidth - 1; px++)
                        {
                            bLine &= (pScreen[(nCurrentY + py) * TetrisWidth + px]) != 0;
                        }
                        if(bLine)
                        {
                            //Blocklari yok etmeden once = e ceviriyom ki havali olsun
                            vLines.push_back(nCurrentY + py);
                            for(int px=1; px < TetrisWidth - 1; px++)
                            {
                                pScreen[(nCurrentY + py) * TetrisWidth + px] = 8;
                            }
                        }
                    }                
                
                nScore += 25;
                if(!vLines.empty()) nScore += (1 << vLines.size()) * 100;

                //Yeni Parca
                nCurrentX        = TetrisWidth/2;
                nCurrentY        = 0;
                nCurrentRotation = 0;
                nCurrentPiece    = rand()%7;
                
                //Oyunn bitti mi?
                gameOver = !DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY);
            }
            nSpeedCounter = 0;
        }

        //RENDERING / Cizim alani ===============================================================
             
        //Oyun alanini ciz

        for(int x=0; x < TetrisWidth; x++)
            for(int y=0; y < TetrisHeight; y++)
            {
                char character;
                character = L" ABCDEFG=#"[pScreen[y*TetrisWidth + x]]; // Sahte bufferdan asil buffera basilacak karakterleri aktariyoruz
                 mvprintw(y+2,x+2, &character);                        // Karakterleri asil buffera yaziyoruz
            }

        //Tetris parcasini ciz

        for(int x=0; x < 4; x++)
            for(int y=0; y < 4; y++)
            {
                if(TetrisBlocks[nCurrentPiece][RotateBlocks(x, y, nCurrentRotation)] == L'x')
                {      
                    char character = nCurrentPiece + 65;                                                     // her tetris seklinin bi ascii karakteri almasi icin 65 ekliyorum
                    mvprintw((nCurrentY + y + 2), (nCurrentX + x + 2), &character);                          // Buffer'a yazdir
                }
            }
        
        if(!vLines.empty())
        {
            refresh();
            this_thread::sleep_for(400ms);

            for(auto &v : vLines)
                for(int px = 1; px < TetrisWidth - 1; px++)
                {
                    for(int py = v; py > 0; py--)
                        pScreen[py * TetrisWidth + px] = pScreen[(py - 1) * TetrisWidth + px];
                    pScreen[px] = 0;
                }
            vLines.clear();
        }

        mvprintw(0,20,"Score: %8d",nScore);
        mvprintw(3,20,"Use J key to go Left.");
        mvprintw(4,20,"Use K key to go Down.");
        mvprintw(5,20,"Use L key to go Right.");
        mvprintw(6,20,"Use Z key to Rotate.");

        refresh();        
    }
    //Game Over!!!!
    endwin();
    cout << "Game Over!!! Score: " << nScore << endl << "Press enter to quit...";
    getchar();
    
    return 0;
}
