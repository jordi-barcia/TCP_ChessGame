#include "ServerGame.h"
#include "Pieces.h"
bool ServerGame::updateboard(int n, int j, sf::RectangleShape rectangle[64], sf::Sprite sprite[65])
{
    int cc;
    Vector2f secondpos;
    secondpos = rectangle[j].getPosition();
    int spritepos = spritepositions[n];
    bool game_finished;
    turn++;
    cc = spritepositions[j];
    if (j != n) {
        sprite[spritepos].setPosition(secondpos);
        sprite[cc].setPosition(100000000, 100000000);
        int suppos = spritepositions[j];
        spritepositions[j] = spritepositions[n];
        spritepositions[n] = 64;
        if (board[j] == -5 || board[j] == 5) {
            // Game finished
            return true;
        }
        if (j <= 63 & j >= 56 & board[n] == -6) {
            board[j] = -4;
        }
        else if (j >= 0 & j <= 7 & board[n] == 6) {
            board[j] = 4;
        }
        else {
            board[j] = board[n];
            board[n] = 0;
        }
        n = j;
    }
    return false; // Game not finished
}
void ServerGame::run()
{
    sf::RectangleShape rectangle[64];
    sf::Sprite sprite[65];
    Identity box;
    bool isMove, game_end = false;
    int n;
    int position;
    Vector2f firstpos, secondpos;
    int v; int q[64];
    static int cap = 0;
    for (int j = 0; j < 64; ++j)
        q[j] = 64;
    Vector2i pos;

    while (!game_end)
    {
        sf::Event event;
        while (true)
        {
            //if (event.type == sf::Event::Closed) {
            //    //window.close();
            //}
            // Pieces selection
            if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
                // Black pieces turn
                for (int j = 0; j < 64; ++j) {
                    if (turn % 2 == 0 & board[j] < 0) {
                        if (firstToPlay == 0) {
                            if (rectangle[j].getGlobalBounds().contains(pos.x, pos.y))
                            {
                                n = j;
                                firstpos = rectangle[j].getPosition();
                                v = spritepositions[j];
                                rectangle[n].setFillColor(sf::Color::Red);
                                if (spritepositions[n] != 64)
                                    cap++;
                            }
                        }
                    }
                }

                // White pieces turn  
                for (int j = 0; j < 64; ++j) {
                    if (turn % 2 != 0 & board[j] > 0) {
                        if (firstToPlay == 1) {
                            if (rectangle[j].getGlobalBounds().contains(pos.x, pos.y))
                            {
                                n = j;
                                firstpos = rectangle[j].getPosition();
                                v = spritepositions[j];
                                rectangle[n].setFillColor(sf::Color::Red);
                                if (spritepositions[n] != 64)
                                    cap++;
                            }
                        }
                    }
                }
            }
            if (cap != 0)
                // New position
                if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                    for (int j = 0; j < 64; ++j) {
                        if (rectangle[j].getGlobalBounds().contains(pos.x, pos.y)) {
                            isMove = box.identifier(n, j, board[n], board);
                            if (isMove) {
                                resetTimer = true;
                                game_end = updateboard(n, j, rectangle, sprite);
                                q[j] = spritepositions[j];
                                if (game_end) { /*window.close();*/ }
                            }

                            // Filling board colors
                           //int counter = 0;
                           //for (int i = 0; i < 8; ++i) {
                           //    for (int j = 0; j < 8; ++j) {
                           //        if ((i + j) % 2 == 0)
                           //            rectangle[counter].setFillColor(sf::Color::White);
                           //        else
                           //            rectangle[counter].setFillColor(sf::Color::Blue);
                           //        counter++;
                           //    }
                           //}
                        }
                    }
                    cap = 0;
                }
        }
        /*
        window.clear();
        for (int j = 0; j < 64; ++j)
            window.draw(rectangle[j]);
        for (int j = 0; j < 65; j++) {
            if (q[j] == 64)
                window.draw(sprite[j]);
        }
        window.display();
        */
    }
}