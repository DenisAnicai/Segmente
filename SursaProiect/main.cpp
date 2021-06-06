#include <algorithm>

#include <iostream>

#include <cstring>

#include <winbgim.h>

#include <stdlib.h>

#include <graphics.h>

#include <time.h>

#include <math.h>

#include <conio.h>

#include <fstream>

using namespace std;

ifstream infile("memory_file.txt");

struct Point {
    int x, y;
    bool connected = false;
};

struct ConnectedPoints {
    Point a, b;
};

struct Game {
    Point points[200];
    ConnectedPoints pairs[100];
    bool done;
    bool needToRefresh;
    bool firstTime;
    int gameVariantChosen = 0;
    int pairsCounter;
    int x1, y1, x2, y2;
    int indexPoint1 = -1, indexPoint2 = -1;
    int numberOfPoints;
    char player1[60] = "", player2[60] = "";
    int turn = 1;
    int winner;
    bool playerChanged = true;
    bool returnToMenu = false;
    int computerDifficulty = 0;

    bool sound = true;
    int soundOption;
}
game;
//Resets all variables and prepares for a new game
void reset() { //2

    ofstream outfile("memory_file.txt");
    outfile.close();

    if (ismouseclick(WM_LBUTTONDOWN)) {
        clearmouseclick(WM_LBUTTONDOWN);
    }
    strcpy(game.player1, "");
    strcpy(game.player2, "");
    game.gameVariantChosen = 0;
    game.computerDifficulty = 0;
    game.done = false;
    game.needToRefresh = false;
    game.pairsCounter = 0;

    for (int i = 0; i < game.numberOfPoints; i++)
        game.points[i].connected = false;
    game.numberOfPoints = 0;
    game.playerChanged = true;
    game.turn = 1;

}
float dist(Point a, Point b) { //1
    return sqrt(1.0 * ((b.x - a.x) * (b.x - a.x) + (b.y - a.y) * (b.y - a.y)));
}
bool onSegment(Point p, Point q, Point r) { //1
    if (q.x <= max(p.x, r.x) + 20 && q.x >= min(p.x, r.x) - 20 &&
        q.y <= max(p.y, r.y) + 20 && q.y >= min(p.y, r.y) - 20)
        return true;

    return false;
}
int orientation(Point p, Point q, Point r) { //1

    int val = (q.y - p.y) * (r.x - q.x) -
        (q.x - p.x) * (r.y - q.y);

    if (val == 0) return 0; // Points are collinear

    return (val > 0) ? 1 : 2; // Points are positioned clockwise; Points are positioned counterclockwise
}
bool intersects(Point p1, Point q1, Point p2, Point q2) { //1
    //Checks if the imaginary line between p1 and q1 intersects the already drawn line between p2 and q2
    int o1 = orientation(p1, q1, p2);
    int o2 = orientation(p1, q1, q2);

    int o3 = orientation(p2, q2, p1);
    int o4 = orientation(p2, q2, q1);

    // General case
    if (o1 != o2 && o3 != o4)
        return true;

    return false;
}
bool checkIfTwoPointsAreConnectable(Point a, Point b) { //1

    //Checks if the line we want to draw intersects other already drawn lines
    for (int i = 0; i < game.pairsCounter; i++)
        if (intersects(a, b, game.pairs[i].a, game.pairs[i].b))
            return false;
    //Checks if the 2 points intersect with another one
    for (int z = 0; z < game.numberOfPoints; z++) {
        if ((game.points[z].x == a.x && game.points[z].y == a.y) || (game.points[z].x == b.x && game.points[z].y == b.y)) continue;

        int x3 = game.points[z].x;
        int y3 = game.points[z].y;

        double dreapta = 1.0 * (x3 - a.x) * (b.y - a.y) / (b.x - a.x);

        if (y3 - a.y >= dreapta - 15 && y3 - a.y <= dreapta + 15 && onSegment(a, game.points[z], b))
            return false;
    }
    return true;
}
int checkIfGameOver() { //1
    for (int i = 0; i < game.numberOfPoints - 1; i++) {
        if (!game.points[i].connected) {
            for (int j = i + 1; j < game.numberOfPoints; j++) {
                if (!game.points[j].connected) {
                    if (checkIfTwoPointsAreConnectable(game.points[i], game.points[j]))
                        return 0;
                }
            }
        }
    }
    return 1;
}
void refreshPoints() { //2
    for (int i = 0; i < game.numberOfPoints; i++) {
        setcolor(WHITE);
        circle(game.points[i].x, game.points[i].y, 7);

        if (game.firstTime) {

            setfillstyle(SOLID_FILL, BLUE);
            floodfill(game.points[i].x, game.points[i].y, WHITE);
        }

    }
    if (game.firstTime) {
        for (int i = 0; i < game.pairsCounter; i++) {
            if (i % 2) {

                setcolor(YELLOW);

                line(game.pairs[i].a.x, game.pairs[i].a.y, game.pairs[i].b.x, game.pairs[i].b.y);

                setcolor(WHITE);
                circle(game.pairs[i].a.x, game.pairs[i].a.y, 7);
                circle(game.pairs[i].b.x, game.pairs[i].b.y, 7);

                setfillstyle(SOLID_FILL, YELLOW);

                floodfill(game.pairs[i].a.x, game.pairs[i].a.y, WHITE);
                floodfill(game.pairs[i].b.x, game.pairs[i].b.y, WHITE);

            } else {
                setcolor(GREEN);

                line(game.pairs[i].a.x, game.pairs[i].a.y, game.pairs[i].b.x, game.pairs[i].b.y);

                setcolor(WHITE);

                circle(game.pairs[i].a.x, game.pairs[i].a.y, 7);
                circle(game.pairs[i].b.x, game.pairs[i].b.y, 7);

                setfillstyle(SOLID_FILL, GREEN);

                floodfill(game.pairs[i].a.x, game.pairs[i].a.y, WHITE);
                floodfill(game.pairs[i].b.x, game.pairs[i].b.y, WHITE);
            }
        }
        game.firstTime = false;
    }

    game.needToRefresh = false;

    if (checkIfGameOver()) {

        if (game.turn == 1)
            game.winner = 2;
        else
            game.winner = 1;

        game.done = true;

    }
}
void save() { //1
    ofstream outfile("memory_file.txt");

    outfile << game.gameVariantChosen << '\n';
    outfile << game.computerDifficulty << '\n';
    outfile << game.sound << '\n';
    outfile << game.player1 << '\n';
    outfile << game.player2 << '\n';
    outfile << game.numberOfPoints << '\n';

    for (int i = 0; i < game.numberOfPoints; i++) {
        outfile << game.points[i].x << ' ' << game.points[i].y << ' ' << game.points[i].connected << '\n';
    }

    outfile << game.pairsCounter << '\n';
    for (int i = 0; i < game.pairsCounter; i++) {
        outfile << game.pairs[i].a.x << ' ' << game.pairs[i].a.y << ' ' << game.pairs[i].b.x << ' ' << game.pairs[i].b.y << '\n';
    }
    outfile << game.turn;

    if (game.sound) Beep(100, 200);
}
void checkSave() { //1
    int x = mousex(), y = mousey();
    if (x >= 20 && x <= 160 && y >= 20 && y <= 70) {
        clearmouseclick(WM_LBUTTONDOWN);
        save();
    }
}
void checkBack() { //1
    int x = mousex(), y = mousey();
    if (x >= 1120 && x <= 1260 && y >= 20 && y <= 70) {
        clearmouseclick(WM_LBUTTONDOWN);
        reset();
        game.returnToMenu = true;
        game.done = true;
    }

}
void playGame() {

    if (game.playerChanged) {
        switch (game.turn) {
        case 1:
            settextstyle(SANS_SERIF_FONT, HORIZ_DIR, 20);
            setcolor(BLACK);
            outtextxy(640 - textwidth(game.player2) / 2, 5, game.player2);
            setcolor(GREEN);
            outtextxy(640 - textwidth(game.player1) / 2, 5, game.player1);
            game.playerChanged = false;
            break;
        case 2:
            settextstyle(SANS_SERIF_FONT, HORIZ_DIR, 20);
            setcolor(BLACK);
            outtextxy(640 - textwidth(game.player1) / 2, 5, game.player1);
            setcolor(YELLOW);
            outtextxy(640 - textwidth(game.player2) / 2, 5, game.player2);
            game.playerChanged = false;
            break;
        }
    }
    if ((game.computerDifficulty) && game.turn == 2) {

        setcolor(YELLOW);
        outtextxy(640 - textwidth("Computer.") / 2, 5, "Computer.");
        delay(500);
        setcolor(BLACK);
        outtextxy(640 - textwidth("Computer.") / 2, 5, "Computer.");
        setcolor(YELLOW);
        outtextxy(640 - textwidth("Computer..") / 2, 5, "Computer..");
        delay(500);
        setcolor(BLACK);
        outtextxy(640 - textwidth("Computer.") / 2, 5, "Computer..");
        setcolor(YELLOW);
        outtextxy(640 - textwidth("Computer...") / 2, 5, "Computer...");
        delay(500);
        setcolor(BLACK);
        outtextxy(640 - textwidth("Computer...") / 2, 5, "Computer...");
        setcolor(YELLOW);
        outtextxy(640 - textwidth("Computer") / 2, 5, "Computer");

        if (game.computerDifficulty == 1) {
            int closestDistance = 9999999;
            for (int i = 0; i < game.numberOfPoints - 1; i++) {
                if (game.points[i].connected) continue;
                for (int j = i + 1; j < game.numberOfPoints; j++) {
                    if (game.points[j].connected) continue;
                    if (dist(game.points[i], game.points[j]) < closestDistance && checkIfTwoPointsAreConnectable(game.points[i], game.points[j])) {
                        closestDistance = dist(game.points[i], game.points[j]);
                        game.indexPoint1 = i;
                        game.indexPoint2 = j;
                    }
                }
            }
        } else if (game.computerDifficulty == 2) {
            int i, j;
            do {

                do {
                    i = rand() % game.numberOfPoints;
                } while (game.points[i].connected);

                do {
                    j = rand() % game.numberOfPoints;
                } while (game.points[j].connected);

            } while (i == j || !checkIfTwoPointsAreConnectable(game.points[i], game.points[j]));

            game.indexPoint1 = i;
            game.indexPoint2 = j;

        } else if (game.computerDifficulty == 3) {
            int furthestDistance = 0;
            for (int i = 0; i < game.numberOfPoints - 1; i++) {
                if (game.points[i].connected) continue;
                for (int j = i + 1; j < game.numberOfPoints; j++) {
                    if (game.points[j].connected) continue;
                    if (dist(game.points[i], game.points[j]) > furthestDistance && checkIfTwoPointsAreConnectable(game.points[i], game.points[j])) {
                        furthestDistance = dist(game.points[i], game.points[j]);
                        game.indexPoint1 = i;
                        game.indexPoint2 = j;
                    }
                }
            }
        }
        game.x1 = game.points[game.indexPoint1].x;
        game.y1 = game.points[game.indexPoint1].y;

        game.x2 = game.points[game.indexPoint2].x;
        game.y2 = game.points[game.indexPoint2].y;

        if (game.sound) Beep(100, 300);
        game.points[game.indexPoint1].connected = game.points[game.indexPoint2].connected = true;

        game.pairs[game.pairsCounter].a = game.points[game.indexPoint1];
        game.pairs[game.pairsCounter].b = game.points[game.indexPoint2];

        game.pairsCounter++;

        if (game.turn == 1) setcolor(GREEN), game.turn = 2, game.playerChanged = true;
        else setcolor(YELLOW), game.turn = 1, game.playerChanged = true;

        line(game.x1, game.y1, game.x2, game.y2);

        setcolor(WHITE);
        circle(game.x1, game.y1, 7);
        circle(game.x2, game.y2, 7);

        if (game.turn == 1) setfillstyle(SOLID_FILL, YELLOW);
        else setfillstyle(SOLID_FILL, GREEN);

        floodfill(game.x1, game.y1, WHITE);
        floodfill(game.x2, game.y2, WHITE);

        game.indexPoint1 = game.indexPoint2 = -1;
        game.x1 = game.y1 = game.x2 = game.y2 = 0;

        game.needToRefresh = true;

    } else {

        int x, y;
        x = mousex();
        y = mousey();

        if (ismouseclick(WM_LBUTTONDOWN)) {
            clearmouseclick(WM_LBUTTONDOWN);
            bool found = false;

            for (int i = 0; i < game.numberOfPoints; i++) {
                if (game.points[i].connected == true) continue;

                if ((x >= game.points[i].x - 7 && x <= game.points[i].x + 7) && y >= game.points[i].y - 7 && y <= game.points[i].y + 7) {
                    found = true;
                    if (!game.x1 && !game.y1) {
                        setfillstyle(SOLID_FILL, RED);
                        floodfill(game.points[i].x, game.points[i].y, WHITE);

                        game.indexPoint1 = i;
                        game.x1 = game.points[i].x;
                        game.y1 = game.points[i].y;

                        if (game.sound) Beep(600, 100);
                    } else {
                        bool OK = true;
                        game.indexPoint2 = i;
                        if (game.indexPoint1 == game.indexPoint2) {

                            setfillstyle(SOLID_FILL, BLUE);
                            floodfill(game.x1, game.y1, WHITE);

                            game.indexPoint1 = game.indexPoint2 = -1;
                            game.x1 = game.y1 = game.x2 = game.y2 = 0;
                            OK = false;
                            if (game.sound) Beep(400, 100);
                        } else {
                            game.x2 = game.points[i].x;
                            game.y2 = game.points[i].y;
                            for (int i = 0; i < game.pairsCounter && OK; i++) {
                                if (intersects(game.points[game.indexPoint1], game.points[game.indexPoint2], game.pairs[i].a, game.pairs[i].b)) {
                                    OK = false;
                                    if (game.sound) Beep(200, 50);
                                    setfillstyle(SOLID_FILL, YELLOW);
                                    floodfill(game.x1, game.y1, WHITE);
                                    floodfill(game.x2, game.y2, WHITE);

                                    game.indexPoint1 = game.indexPoint2 = -1;
                                    if (game.sound) Beep(300, 50);
                                    delay(200);

                                    setfillstyle(SOLID_FILL, BLUE);
                                    floodfill(game.x1, game.y1, WHITE);
                                    floodfill(game.x2, game.y2, WHITE);
                                    if (game.sound) Beep(400, 50);
                                    delay(200);

                                    setfillstyle(SOLID_FILL, YELLOW);
                                    floodfill(game.x1, game.y1, WHITE);
                                    floodfill(game.x2, game.y2, WHITE);
                                    if (game.sound) Beep(500, 50);
                                    delay(200);

                                    setfillstyle(SOLID_FILL, BLUE);
                                    floodfill(game.x1, game.y1, WHITE);
                                    floodfill(game.x2, game.y2, WHITE);

                                    game.x1 = game.y1 = game.x2 = game.y2 = 0;
                                }
                            }
                            for (int j = 0; j < game.numberOfPoints && OK; j++) {
                                int x3 = game.points[j].x;
                                int y3 = game.points[j].y;
                                double dreapta = 1.0 * (x3 - game.x1) * (game.y2 - game.y1) / (game.x2 - game.x1);

                                if (j == game.indexPoint1 || j == game.indexPoint2) continue;

                                if (y3 - game.y1 >= dreapta - 20 && y3 - game.y1 <= dreapta + 20 && onSegment(game.points[game.indexPoint1], game.points[j], game.points[game.indexPoint2])) {
                                    OK = false;
                                    game.indexPoint1 = game.indexPoint2 = -1;

                                    setfillstyle(SOLID_FILL, YELLOW);
                                    floodfill(game.x1, game.y1, WHITE);
                                    floodfill(game.x2, game.y2, WHITE);
                                    if (!game.points[j].connected) floodfill(game.points[j].x, game.points[j].y, WHITE);
                                    if (game.sound) Beep(200, 50);
                                    delay(200);

                                    setfillstyle(SOLID_FILL, BLUE);
                                    floodfill(game.x1, game.y1, WHITE);
                                    floodfill(game.x2, game.y2, WHITE);
                                    if (!game.points[j].connected) floodfill(game.points[j].x, game.points[j].y, WHITE);
                                    if (game.sound) Beep(300, 50);
                                    delay(200);

                                    setfillstyle(SOLID_FILL, YELLOW);
                                    floodfill(game.x1, game.y1, WHITE);
                                    floodfill(game.x2, game.y2, WHITE);
                                    if (!game.points[j].connected) floodfill(game.points[j].x, game.points[j].y, WHITE);
                                    if (game.sound) Beep(400, 50);
                                    delay(200);

                                    setfillstyle(SOLID_FILL, BLUE);
                                    floodfill(game.x1, game.y1, WHITE);
                                    floodfill(game.x2, game.y2, WHITE);
                                    if (!game.points[j].connected) floodfill(game.points[j].x, game.points[j].y, WHITE);
                                    if (game.sound) Beep(500, 50);
                                    game.x1 = game.y1 = game.x2 = game.y2 = 0;

                                }
                            }
                        }

                        if (OK) {
                            if (game.sound) Beep(800, 300);
                            game.points[game.indexPoint1].connected = game.points[game.indexPoint2].connected = true;

                            game.pairs[game.pairsCounter].a = game.points[game.indexPoint1];
                            game.pairs[game.pairsCounter].b = game.points[game.indexPoint2];

                            game.pairsCounter++;

                            if (game.turn == 1) setcolor(GREEN), game.turn = 2, game.playerChanged = true;
                            else setcolor(YELLOW), game.turn = 1, game.playerChanged = true;

                            line(game.x1, game.y1, game.x2, game.y2);

                            setcolor(WHITE);
                            circle(game.x1, game.y1, 7);
                            circle(game.x2, game.y2, 7);

                            if (game.turn == 1) setfillstyle(SOLID_FILL, YELLOW);
                            else setfillstyle(SOLID_FILL, GREEN);

                            floodfill(game.x1, game.y1, WHITE);
                            floodfill(game.x2, game.y2, WHITE);

                            game.indexPoint1 = game.indexPoint2 = -1;
                            game.x1 = game.y1 = game.x2 = game.y2 = 0;

                            game.needToRefresh = true;
                        }

                    }

                    break;

                }
            }
            if (!found) {
                checkSave();
                checkBack();
            }

        }
    }
}
int checkPointCoordinates(int x, int y, int numberOfDrawnPoints) { //1
    for (int i = 0; i < numberOfDrawnPoints; i++) {
        if (x >= game.points[i].x - 30 && x <= game.points[i].x + 30 && y >= game.points[i].y - 30 && y <= game.points[i].y + 30)
            return 0;
    }
    return 1;
}
void randomPoints(int numberOfPoints) { //2
    srand(time(NULL));
    int numberOfDrawnPoints = 0;
    for (int i = 0; i < game.numberOfPoints; i++) {
        int x = 10 + rand() % 1259, y = 120 + rand() % 580;
        if (checkPointCoordinates(x, y, numberOfDrawnPoints)) {

            game.points[numberOfDrawnPoints].x = x;
            game.points[numberOfDrawnPoints].y = y;

            game.points[numberOfDrawnPoints].connected = false;
            numberOfDrawnPoints++;

            circle(x, y, 7);

            setfillstyle(SOLID_FILL, BLUE);
            floodfill(x, y, WHITE);
        } else i--;

    }
}
void menuCheck() { //2
    int x = mousex();
    int y = mousey();

    if (ismouseclick(WM_LBUTTONDOWN)) {

        clearmouseclick(WM_LBUTTONDOWN);
        if (x >= 400 && x <= 900 && y >= 100 && y <= 200) game.gameVariantChosen = 1;
        else if (x >= 400 && x <= 900 && y >= 250 && y <= 350) game.gameVariantChosen = 2;
        else if (x >= 400 && x <= 900 && y >= 400 && y <= 500) game.gameVariantChosen = 3;
        else if (x >= 400 && x <= 900 && y >= 550 && y <= 650) game.gameVariantChosen = 4;

    }

    settextstyle(SANS_SERIF_FONT, HORIZ_DIR, 4);
    setcolor(WHITE);
    if (x >= 400 && x <= 900 && y >= 100 && y <= 200) {
        setcolor(YELLOW);
        outtextxy(470, 130, "Player 1 vs Player 2");

    } else {
        setcolor(WHITE);
        outtextxy(470, 130, "Player 1 vs Player 2");
    }

    if (x >= 400 && x <= 900 && y >= 250 && y <= 350) {
        setcolor(YELLOW);
        outtextxy(470, 280, "Player vs Computer");
    } else {
        setcolor(WHITE);
        outtextxy(470, 280, "Player vs Computer");
    }
    if (x >= 400 && x <= 900 && y >= 400 && y <= 500) {
        setcolor(YELLOW);
        outtextxy(580, 430, "Settings");
    } else {
        setcolor(WHITE);
        outtextxy(580, 430, "Settings");
    }

    if (x >= 400 && x <= 900 && y >= 550 && y <= 650) {
        setcolor(YELLOW);
        outtextxy(615, 580, "Quit");
    } else {
        setcolor(WHITE);
        outtextxy(615, 580, "Quit");
    }
}
void chooseSound() { //2
    setcolor(LIGHTGREEN);

    rectangle(20, 40, 150, 100);

    setcolor(WHITE);
    settextstyle(SANS_SERIF_FONT, HORIZ_DIR, 4);
    outtextxy(38, 53, "Back");

    setcolor(LIGHTGREEN);
    rectangle(400, 200, 900, 300);

    setcolor(WHITE);
    settextstyle(SANS_SERIF_FONT, HORIZ_DIR, 17);
    outtextxy(535, 220, "Sound On");

    setcolor(LIGHTGREEN);
    rectangle(400, 400, 900, 500);

    setcolor(WHITE);
    outtextxy(540, 420, "Sound Off");

    setcolor(WHITE);
    settextstyle(SANS_SERIF_FONT, HORIZ_DIR, 4);
    outtextxy(0, 570, "The game is won by the player that connects the last possible segment. ");
    outtextxy(10, 610, "Try to win by isolating points and cutting off the other player. Good luck!");

}
void checkSound() { //2
    int x = mousex();
    int y = mousey();

    if (ismouseclick(WM_LBUTTONDOWN)) {
        clearmouseclick(WM_LBUTTONDOWN);

        if (x >= 400 && x <= 900 && y >= 200 && y <= 300) game.soundOption = 1;
        else if (x >= 400 && x <= 900 && y >= 400 && y <= 500) game.soundOption = 0;
        else if (x >= 20 && x <= 150 && y >= 40 && y <= 100) game.soundOption = 2;
    }

    if (x >= 400 && x <= 900 && y >= 200 && y <= 300) {
        setcolor(YELLOW);
        settextstyle(SANS_SERIF_FONT, HORIZ_DIR, 17);
        outtextxy(535, 220, "Sound On");
    } else {
        setcolor(WHITE);
        settextstyle(SANS_SERIF_FONT, HORIZ_DIR, 17);
        outtextxy(535, 220, "Sound On");
    }

    if (x >= 400 && x <= 900 && y >= 400 && y <= 500) {
        setcolor(YELLOW);
        settextstyle(SANS_SERIF_FONT, HORIZ_DIR, 17);
        outtextxy(540, 420, "Sound Off");
    } else {
        setcolor(WHITE);
        settextstyle(SANS_SERIF_FONT, HORIZ_DIR, 17);
        outtextxy(540, 420, "Sound Off");
    }

    if (x >= 20 && x <= 150 && y >= 40 && y <= 100) {
        setcolor(YELLOW);
        settextstyle(SANS_SERIF_FONT, HORIZ_DIR, 4);
        outtextxy(38, 53, "Back");
    } else {
        settextstyle(SANS_SERIF_FONT, HORIZ_DIR, 4);
        setcolor(WHITE);
        outtextxy(38, 53, "Back");
    }

}
void chooseComputerDifficulty() { //2
    if (ismouseclick(WM_LBUTTONDOWN)) {
        clearmouseclick(WM_LBUTTONDOWN);
    }
    cleardevice();

    setcolor(WHITE);

    settextstyle(SANS_SERIF_FONT, HORIZ_DIR, 17);

    outtextxy(640 - textwidth("Choose computer difficulty") / 2, 100, "Choose computer difficulty");

    setcolor(LIGHTGREEN);
    settextstyle(SANS_SERIF_FONT, HORIZ_DIR, 17);

    rectangle(20, 40, 150, 100);
    rectangle(400, 200, 900, 300);
    rectangle(400, 400, 900, 500);
    rectangle(400, 600, 900, 700);

    setcolor(WHITE);
    settextstyle(SANS_SERIF_FONT, HORIZ_DIR, 4);
    outtextxy(38, 53, "Back");

    settextstyle(SANS_SERIF_FONT, HORIZ_DIR, 17);

    outtextxy(580, 220, "Easy");
    outtextxy(550, 420, "Medium");
    outtextxy(580, 620, "Hard");

}
void checkDifficultyChoice() { //2
    int x = mousex();
    int y = mousey();

    if (ismouseclick(WM_LBUTTONDOWN)) {
        clearmouseclick(WM_LBUTTONDOWN);

        if (x >= 400 && x <= 900 && y >= 200 && y <= 300) game.computerDifficulty = 1;
        else if (x >= 400 && x <= 900 && y >= 400 && y <= 500) game.computerDifficulty = 2;
        else if (x >= 400 && x <= 900 && y >= 600 && y <= 700) game.computerDifficulty = 3;
        else if (x >= 20 && x <= 150 && y >= 40 && y <= 100) game.computerDifficulty = 4;
    }
    if (x >= 400 && x <= 900 && y >= 200 && y <= 300) {
        setcolor(YELLOW);
        settextstyle(SANS_SERIF_FONT, HORIZ_DIR, 17);
        outtextxy(580, 220, "Easy");

    } else {
        setcolor(WHITE);
        settextstyle(SANS_SERIF_FONT, HORIZ_DIR, 17);
        outtextxy(580, 220, "Easy");
    }

    if (x >= 400 && x <= 900 && y >= 400 && y <= 500) {
        setcolor(YELLOW);
        settextstyle(SANS_SERIF_FONT, HORIZ_DIR, 17);
        outtextxy(550, 420, "Medium");
    } else {
        setcolor(WHITE);
        settextstyle(SANS_SERIF_FONT, HORIZ_DIR, 17);
        outtextxy(550, 420, "Medium");
    }
    if (x >= 400 && x <= 900 && y >= 600 && y <= 700) {
        setcolor(YELLOW);
        outtextxy(580, 620, "Hard");
        settextstyle(SANS_SERIF_FONT, HORIZ_DIR, 17);
    } else {
        setcolor(WHITE);
        settextstyle(SANS_SERIF_FONT, HORIZ_DIR, 17);
        outtextxy(580, 620, "Hard");
    }
    if (x >= 20 && x <= 150 && y >= 40 && y <= 100) {
        setcolor(YELLOW);
        settextstyle(SANS_SERIF_FONT, HORIZ_DIR, 4);
        outtextxy(38, 53, "Back");
    } else {
        settextstyle(SANS_SERIF_FONT, HORIZ_DIR, 4);
        setcolor(WHITE);
        outtextxy(38, 53, "Back");
    }

}
void makeLineDisappear(int i) { //2
    setcolor(BLACK);

    line(game.pairs[i].a.x, game.pairs[i].a.y, game.pairs[i].b.x, game.pairs[i].b.y);

    setcolor(WHITE);

    circle(game.pairs[i].a.x, game.pairs[i].a.y, 7);
    circle(game.pairs[i].b.x, game.pairs[i].b.y, 7);

    setfillstyle(SOLID_FILL, BLUE);
    floodfill(game.pairs[i].a.x, game.pairs[i].a.y, WHITE);
    floodfill(game.pairs[i].b.x, game.pairs[i].b.y, WHITE);
}
void colorPointsForEndGame() { //2

    for (int i = 0; i < game.numberOfPoints; i++) {
        setcolor(WHITE);
        circle(game.points[i].x, game.points[i].y, 7);
    }

    for (int i = 0; i < game.numberOfPoints; i++) {
        setfillstyle(SOLID_FILL, BLUE);
        floodfill(game.points[i].x, game.points[i].y, WHITE);
    }
    if (game.sound) Beep(200, 200);
    else delay(200);
    for (int i = 0; i < game.numberOfPoints; i++) {
        setfillstyle(SOLID_FILL, YELLOW);
        floodfill(game.points[i].x, game.points[i].y, WHITE);
    }
    delay(200);
    for (int i = 0; i < game.numberOfPoints; i++) {
        setfillstyle(SOLID_FILL, BLUE);
        floodfill(game.points[i].x, game.points[i].y, WHITE);
    }
    if (game.sound) Beep(400, 200);
    else delay(200);
    for (int i = 0; i < game.numberOfPoints; i++) {
        setfillstyle(SOLID_FILL, YELLOW);
        floodfill(game.points[i].x, game.points[i].y, WHITE);
    }
    if (game.sound) Beep(200, 200);
    else delay(200);
    for (int i = 0; i < game.numberOfPoints; i++) {
        setfillstyle(SOLID_FILL, BLUE);
        floodfill(game.points[i].x, game.points[i].y, WHITE);
    }
    if (game.sound) Beep(400, 200);
    else delay(200);

    for (int i = 0; i < game.numberOfPoints; i++) {
        setfillstyle(SOLID_FILL, YELLOW);
        floodfill(game.points[i].x, game.points[i].y, WHITE);
    }
    if (game.sound) Beep(200, 200);
    else delay(200);
    for (int i = 0; i < game.numberOfPoints; i++) {
        setfillstyle(SOLID_FILL, BLUE);
        floodfill(game.points[i].x, game.points[i].y, WHITE);
    }
    if (game.sound) Beep(400, 200);
    else delay(200);

    for (int i = 0; i < game.numberOfPoints; i++) {
        setfillstyle(SOLID_FILL, YELLOW);
        floodfill(game.points[i].x, game.points[i].y, WHITE);
    }
    if (game.sound) Beep(600, 200);
    else delay(200);
    for (int i = 0; i < game.numberOfPoints; i++) {
        setfillstyle(SOLID_FILL, BLUE);
        floodfill(game.points[i].x, game.points[i].y, WHITE);
    }
    if (game.sound) Beep(800, 200);
    else delay(200);

    for (int i = 0; i < game.numberOfPoints; i++) {
        setfillstyle(SOLID_FILL, YELLOW);
        floodfill(game.points[i].x, game.points[i].y, WHITE);
    }
    delay(200);
}
void showWinnerName() { //2
    cleardevice();

    setcolor(GREEN);
    settextstyle(TRIPLEX_FONT, HORIZ_DIR, 28);
    outtextxy(640 - textwidth("WINNER") / 2, 250, "WINNER");

    settextstyle(TRIPLEX_FONT, HORIZ_DIR, 30);

    setcolor(GREEN);
    outtextxy(640 - textwidth((game.winner == 1) ? game.player1 : game.player2) / 2, 400, (game.winner == 1) ? game.player1 : game.player2);

    delay(600);

    setcolor(BLACK);
    outtextxy(640 - textwidth((game.winner == 1) ? game.player1 : game.player2) / 2, 400, (game.winner == 1) ? game.player1 : game.player2);

    delay(600);

    setcolor(GREEN);
    outtextxy(640 - textwidth((game.winner == 1) ? game.player1 : game.player2) / 2, 400, (game.winner == 1) ? game.player1 : game.player2);

    delay(600);

    setcolor(BLACK);
    outtextxy(640 - textwidth((game.winner == 1) ? game.player1 : game.player2) / 2, 400, (game.winner == 1) ? game.player1 : game.player2);

    delay(600);

    setcolor(GREEN);
    outtextxy(640 - textwidth((game.winner == 1) ? game.player1 : game.player2) / 2, 400, (game.winner == 1) ? game.player1 : game.player2);

    delay(600);

    setcolor(BLACK);
    outtextxy(640 - textwidth((game.winner == 1) ? game.player1 : game.player2) / 2, 400, (game.winner == 1) ? game.player1 : game.player2);

    delay(600);

}
void endGame() { //2
    ofstream outfile("memory_file.txt");
    outfile.close();

    for (int i = 0; i < game.numberOfPoints; i++) {
        setcolor(WHITE);
        circle(game.points[i].x, game.points[i].y, 7);
    }

    int beepCounter = 400;
    for (int i = 0; i < game.pairsCounter; i++) {
        if (game.sound) Beep(beepCounter, 50);
        beepCounter += 50;
        makeLineDisappear(i);

        if (!game.sound) delay(50);
        delay(50);

    }

    delay(400);
    colorPointsForEndGame();

    delay(400);

    showWinnerName();

    cleardevice();

}
void initMenu() { //2
    setcolor(LIGHTGREEN);
    rectangle(400, 100, 900, 200);
    rectangle(400, 250, 900, 350);
    rectangle(400, 400, 900, 500);
    rectangle(400, 550, 900, 650);

    setcolor(WHITE);
    settextstyle(SANS_SERIF_FONT, HORIZ_DIR, 4);
    outtextxy(470, 130, "Player 1 vs Player 2");
    outtextxy(470, 280, "Player vs Computer");
    outtextxy(580, 430, "Settings");
    outtextxy(615, 580, "Quit");
}
void saveButton() { //2
    setcolor(LIGHTGREEN);
    rectangle(20, 20, 160, 70);

    setcolor(WHITE);
    settextstyle(SANS_SERIF_FONT, HORIZ_DIR, 4);
    outtextxy(38, 30, "Save");

}
void backButton() { //2
    setcolor(LIGHTGREEN);
    rectangle(1120, 20, 1260, 70);

    setcolor(WHITE);
    settextstyle(SANS_SERIF_FONT, HORIZ_DIR, 4);
    outtextxy(1142, 30, "Back");

}
void initGame() { //2
    setcolor(WHITE);
    line(0, 99, 1280, 99);
    saveButton();
    backButton();

    game.done = false;
}

int main() {
    initwindow(1280, 720);

    infile >> game.gameVariantChosen;

    if (game.gameVariantChosen == 1 || game.gameVariantChosen == 2) {
        game.firstTime = true;
        infile >> game.computerDifficulty;
        infile >> game.sound;
        infile >> game.player1;
        infile >> game.player2;
        infile >> game.numberOfPoints;

        for (int i = 0; i < game.numberOfPoints; i++) {
            infile >> game.points[i].x >> game.points[i].y >> game.points[i].connected;
        }
        infile >> game.pairsCounter;
        for (int i = 0; i < game.pairsCounter; i++) {
            infile >> game.pairs[i].a.x >> game.pairs[i].a.y >> game.pairs[i].b.x >> game.pairs[i].b.y;
        }

        infile >> game.turn;
        infile.close();

        game.needToRefresh = true;

        cleardevice();

        initGame();
        refreshPoints();
        do {
            playGame();
            if (game.needToRefresh) refreshPoints();
        }
        while (!game.done);

        if (game.returnToMenu) {

            game.returnToMenu = false;
            goto loop;
        }
        delay(1000);
        endGame();

        reset();
        goto loop;

    }

    loop:
        cleardevice();
    initMenu();

    do {
        menuCheck();
    } while (!game.gameVariantChosen);

    switch (game.gameVariantChosen) {
    case 1: {

        cleardevice();
        settextstyle(SANS_SERIF_FONT, HORIZ_DIR, 5);
        outtextxy(450, 200, "Number of points: ");
        outtextxy(450, 250, "Player 1: ");
        outtextxy(450, 300, "Player 2: ");

        char numberOfPoints[20] = "";
        int whichField = 0;
        bool selection = true;
        do {

            setcolor(WHITE);
            outtextxy(850, 200, numberOfPoints);
            outtextxy(850, 250, game.player1);
            outtextxy(850, 300, game.player2);

            int len0 = strlen(numberOfPoints), len1 = strlen(game.player1), len2 = strlen(game.player2);
            char c = getch();

            switch (c) {
            case 8: { //Backspace
                setcolor(BLACK);
                outtextxy(850, 200, numberOfPoints);
                outtextxy(850, 250, game.player1);
                outtextxy(850, 300, game.player2);
                if (whichField == 0) numberOfPoints[len0 - 1] = '\0';
                else if (whichField == 1) game.player1[len1 - 1] = NULL;
                else if (whichField == 2) game.player2[len2 - 1] = NULL;

                break;
            }
            case 13: { //Enter
                if (whichField == 0) {
                    game.numberOfPoints = atoi(numberOfPoints);
                    whichField = 1;
                } else if (whichField == 1) {
                    whichField = 2;
                } else if (whichField == 2) {
                    selection = false;
                }

                break;
            }
            default: { // Adds the character to the current field
                setcolor(BLACK);
                outtextxy(850, 200, numberOfPoints);
                outtextxy(850, 250, game.player1);
                outtextxy(850, 300, game.player2);
                if (whichField == 0 && c >= '0' && c <= '9') numberOfPoints[len0] = c, numberOfPoints[len0 + 1] = '\0';
                else if (whichField == 1) game.player1[len1] = c, game.player1[len1 + 1] = NULL;
                else if (whichField == 2) game.player2[len2] = c, game.player2[len2 + 1] = NULL;

                break;
            }
            }

        } while (selection);

        cleardevice();
        initGame();
        randomPoints(game.numberOfPoints);
        do {
            playGame();
            if (game.needToRefresh) refreshPoints();

        } while (!game.done);

        if (game.returnToMenu) {
            reset();
            game.returnToMenu = false;
            goto loop;
        }

        delay(1000);
        endGame();

        reset();
        goto loop;
        break;
    }
    case 2: {
        strcpy(game.player2, "Computer");
        chooseComputerDifficulty();
        do {
            checkDifficultyChoice();
        } while (!game.computerDifficulty);

        if (game.computerDifficulty == 4) {

            reset();
            goto loop;
        }

        cleardevice();
        settextstyle(SANS_SERIF_FONT, HORIZ_DIR, 5);
        outtextxy(450, 200, "Number of points: ");
        outtextxy(450, 250, "Player 1: ");

        char numberOfPoints[20] = "";
        int whichField = 0;
        bool selection = true;
        do {

            setcolor(WHITE);
            outtextxy(850, 200, numberOfPoints);
            outtextxy(850, 250, game.player1);

            int len0 = strlen(numberOfPoints), len1 = strlen(game.player1);
            char c = getch();

            switch (c) {
            case 8: {
                setcolor(BLACK);
                outtextxy(850, 200, numberOfPoints);
                outtextxy(850, 250, game.player1);

                if (whichField == 0) numberOfPoints[len0 - 1] = '\0';
                else if (whichField == 1) game.player1[len1 - 1] = NULL;

                break;
            }
            case 13: {
                if (whichField == 0) {
                    game.numberOfPoints = atoi(numberOfPoints);
                    whichField = 1;
                } else if (whichField == 1) {
                    selection = false;
                }

                break;
            }
            default: {
                setcolor(BLACK);
                outtextxy(850, 200, numberOfPoints);
                outtextxy(850, 250, game.player1);

                if (whichField == 0 && c >= '0' && c <= '9') numberOfPoints[len0] = c, numberOfPoints[len0 + 1] = '\0';
                else if (whichField == 1) game.player1[len1] = c, game.player1[len1 + 1] = NULL;

                break;
            }
            }

        } while (selection);

        cleardevice();
        initGame();
        randomPoints(game.numberOfPoints);

        do {
            playGame();
            if (game.needToRefresh) refreshPoints();

        } while (!game.done);

        if (game.returnToMenu) {
            reset();
            game.returnToMenu = false;
            goto loop;
        }

        delay(1000);
        endGame();

        reset();
        goto loop;
        break;
    }
    case 3: {
        cleardevice();
        chooseSound();
        do {
            checkSound();
            if (game.soundOption == 0) {
                game.sound = false;
            } else if (game.soundOption == 1) {
                game.sound = true;
            } else if (game.soundOption == 2) {
                game.soundOption = 1;
                game.gameVariantChosen = 0;
                cleardevice();

                initMenu();

                goto loop;
            }

        } while (1);

    }

    case 4: {
        closegraph();
        return 0;
        break;
    }

    }
}
