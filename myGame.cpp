#include <SFML/Graphics.hpp>
#include <X11/Xlib.h>
#include <pthread.h>
#include <string>
#include <cstdlib>
#include <cmath>
#include <iostream>
using namespace sf;

int gameState = 0;
int gameOver = 0;
int initializeGame = 0;
int p1Score = 0;
int p2Score = 0;
bool update = false;
sf::Time gameTime;
float coll2;
float coll1;
int winPlayer;
bool pause;

class Menu
{
    public:
    Text menuTxt1;
    Text menuTxt2;
    Text overTxt1;
    Text overTxt2;
    sf::Font font;
    Menu()
    {
        if (!font.loadFromFile("Freedom-10eM.ttf"))
        {
            perror("No font found\n");
            exit(1);
        }     
    }
    void Initialize()
    {
        menuTxt1.setFont(font);
        menuTxt1.setString("PONG");

        menuTxt1.setCharacterSize(35);
        menuTxt1.setPosition(175, 50);
        menuTxt1.setFillColor(sf::Color::White);
        menuTxt1.setStyle(sf::Text::Bold);

        menuTxt2.setFont(font);
        menuTxt2.setString("START GAME");

        menuTxt2.setCharacterSize(43);
        menuTxt2.setPosition(70, 125);
        menuTxt2.setFillColor(sf::Color::White);
        menuTxt2.setStyle(sf::Text::Bold);  
    }
    void InitGameOver()
    {
        menuTxt1.setFont(font);
        menuTxt1.setString("Loss");

        overTxt1.setFont(font);
        overTxt1.setString("REMATCH");

        overTxt2.setFont(font);
        overTxt2.setString("MENU");

        menuTxt1.setCharacterSize(20);
        overTxt1.setCharacterSize(20);
        overTxt2.setCharacterSize(25);

        menuTxt1.setFillColor(Color::White);
        overTxt1.setFillColor(Color::White);
        overTxt2.setFillColor(Color::White);

        menuTxt1.setStyle(Text::Bold);
        overTxt1.setStyle(Text::Bold);
        overTxt2.setStyle(Text::Bold);

        if (winPlayer == 1)
        {
            menuTxt1.setPosition(350, 100);
            overTxt1.setPosition(325, 200);
            overTxt2.setPosition(340, 255);
        }
        else
        {
            menuTxt1.setPosition(100, 100);
            overTxt1.setPosition(65, 200);
            overTxt2.setPosition(80, 255);
        }
    }
    void DisplayGameOver(RenderWindow * win)
    {
        InitGameOver();

        sf::Vector2i mousePosition = Mouse::getPosition(*win);

        RectangleShape button1(Vector2f(121, 30));
        button1.setFillColor(Color::Black);
        button1.setOutlineColor(Color::White);
        button1.setOutlineThickness(7);
        button1.setPosition(Vector2f(overTxt1.getPosition().x - 3, overTxt1.getPosition().y));

        RectangleShape button2(Vector2f(90, 30));
        button2.setFillColor(Color::Black);
        button2.setOutlineColor(Color::White);
        button2.setOutlineThickness(7);
        button2.setPosition(Vector2f(overTxt2.getPosition().x, overTxt2.getPosition().y));

        IntRect button1Area(overTxt1.getPosition().x, overTxt1.getPosition().y, 130, 30);
        IntRect button2Area(overTxt2.getPosition().x, overTxt2.getPosition().y, 100, 30);

        if (button1Area.contains(mousePosition))
        {
            button1.setFillColor(Color::White);
            overTxt1.setFillColor(Color::Black);

            if (Mouse::isButtonPressed(Mouse::Left))
            {
                gameState = 1;
                initializeGame = 1;
                gameOver = 0;
                p1Score = 0;
                p2Score = 0;
                winPlayer = 0;
                update = false;
                pause = 0;
            }
        }

        if (button2Area.contains(mousePosition))
        {
            button2.setFillColor(Color::White);
            overTxt2.setFillColor(Color::Black);

            if (Mouse::isButtonPressed(Mouse::Left))
            {
                gameState = 0;
            }
        }
        win->draw(button1);
        win->draw(button2);
        win->draw(menuTxt1);
        win->draw(overTxt1);
        win->draw(overTxt2);
    }
    void Display(RenderWindow* win)
    {
        Initialize();

        sf::Vector2i mousePosition = Mouse::getPosition(*win);

        RectangleShape button(Vector2f(320, 70));
        button.setFillColor(Color::Black);
        button.setOutlineColor(Color::White);
        button.setOutlineThickness(10);
        button.setPosition(Vector2f(70, 125));

        sf::IntRect buttonArea(70, 125, 320, 70);
        
        if (buttonArea.contains(mousePosition))
        {
            button.setFillColor(Color::White);
            menuTxt2.setFillColor(Color::Black);

            if (Mouse::isButtonPressed(Mouse::Left))
            {
                gameState = 1;
                initializeGame = 1;
                gameOver = 0;
                p1Score = 0;
                p2Score = 0;
                winPlayer = 0;
                update = false;
                pause = 0;
            }
        }

        win->draw(button);
        win->draw(menuTxt1);
        win->draw(menuTxt2);
    }
};

struct ball
{
    int x;
    int y;
    float xStep;
    float yStep;
    float speed;
    float direction;
    RectangleShape body;
};

struct player
{
    int x;
    int y;
    float tx;
    float ty;
    RectangleShape paddle;
};

void* player1Update(void* play1)
{
    player* p1 = static_cast<player*>(play1);
    float speed = 5.0f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) 
        {
            if (p1->paddle.getPosition().y - speed > 25)
            {
                p1->paddle.setPosition(p1->paddle.getPosition().x, p1->paddle.getPosition().y - speed);
            }
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) 
        {
            if (p1->paddle.getPosition().y + speed + 70 < 375)
            {
                p1->paddle.setPosition(p1->paddle.getPosition().x, p1->paddle.getPosition().y + speed);
            }
        }
        coll1 = p1->paddle.getPosition().y;
        pthread_exit(0);
}

void* player2Update(void* play2)
{
    player* p2 = static_cast<player*>(play2);
    float speed = 5.0f;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) 
        {
            if (p2->paddle.getPosition().y + 70 + speed < 375)
            {
                p2->paddle.setPosition(p2->paddle.getPosition().x, p2->paddle.getPosition().y + speed);
            }
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) 
        {
            if (p2->paddle.getPosition().y  - speed > 25)
            {
                p2->paddle.setPosition(p2->paddle.getPosition().x, p2->paddle.getPosition().y - speed);
            }
        }
        coll2 = p2->paddle.getPosition().y;
        pthread_exit(0);
}

void AddPartition(RenderWindow* win)
{
    RectangleShape frill(Vector2f(17, 30));
    frill.setFillColor(Color::White);
    for (int i = 1, offset = 0; i <= 9; i++, offset += 15)
    {
        frill.setPosition(Vector2f(242, i*25 + offset));
        win->draw(frill);
    }
}

void Spawn(ball& gameBall)
{
    srand(time(nullptr));
    gameBall.body.setPosition(Vector2f(242, rand() % 301 + 50));

    int randomNum = rand();
    int randomNumber = rand() % 131 + 25;

    float angleRadians = randomNumber * M_PI / 180.0;
    
    if (rand() % 2)
    {
        gameBall.direction =  angleRadians;
    }
    else
    {
        gameBall.direction =  -angleRadians;
    }
}

void *Timer(void *param)
{
    sf::Clock clock;
    while (!gameOver)
    {
        gameTime = clock.getElapsedTime();
        if (gameTime.asMicroseconds() >= 100)
        {
            clock.restart();
            gameTime = gameTime.Zero;
            update = true;
        }
    }
    pthread_exit(0);
}

void *MoveBall(void *gameBall)
{
    ball* gBall = static_cast<ball*>(gameBall);
    if (update)
    {
        gBall->body.move(gBall->xStep * gBall->speed, gBall->yStep * gBall->speed);
        gBall->speed += 0.0005f;
        update = false;
    }
    pthread_exit(0);
}

struct CollisionArgs
{
    ball GameBall;
    player p1;
    player p2;
};

void *detectCollision(void *collArgs)
{
    CollisionArgs* args = static_cast<CollisionArgs*>(collArgs);
    
    if (args->GameBall.body.getPosition().x + 17 > 475 && args->GameBall.body.getPosition().y > coll2 && args->GameBall.body.getPosition().y < coll2 + 70)
        args->GameBall.xStep = -abs(args->GameBall.xStep);

    if (args->GameBall.body.getPosition().x < 25 && args->GameBall.body.getPosition().y > coll1 && args->GameBall.body.getPosition().y < coll1 + 70)
        args->GameBall.xStep = abs(args->GameBall.xStep);

    if (args->GameBall.body.getPosition().x > 483)
    {
        args->GameBall.xStep = -abs(args->GameBall.xStep);
        p1Score++;

        if (p1Score == 10)
        {
            gameOver = 1;
            winPlayer = 1;
        }
    }
    else if (args->GameBall.body.getPosition().x < 0)
    {
        args->GameBall.xStep = abs(args->GameBall.xStep);
        p2Score++;

        if (p2Score == 10)
        {
            gameOver = 1;
            winPlayer = 2;
        }
    }

    if (args->GameBall.body.getPosition().y > 355)
        args->GameBall.yStep = -abs(args->GameBall.yStep);
    else if (args->GameBall.body.getPosition().y < 25)
        args->GameBall.yStep = abs(args->GameBall.yStep);

    pthread_exit(0);
    
}

void StartGame(RenderWindow* win, player& p1, player& p2, ball& gameBall, sf::Font& font)
{
    if (initializeGame == 1)
    {
        p1.paddle.setSize(Vector2f(20, 70));
        p2.paddle.setSize(Vector2f(20, 70));

        p1.paddle.setPosition(Vector2f(5, 125));
        p2.paddle.setPosition(Vector2f(475, 125));

        gameBall.body.setSize(Vector2f(17, 20));
        gameBall.speed = 1.0f;
        Spawn(gameBall);
        gameBall.xStep = 1.0f;
        gameBall.yStep = 1.0f;
        pthread_t timeThread;
        pthread_create(&timeThread, NULL, Timer, NULL);
        initializeGame = 0;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
    {
        gameState = 0;
    }
    // else if (!gameOver && sf::Keyboard::isKeyPressed(sf::Keyboard::P))
    // {
    //     pause = !pause;
    // }

    if (!gameOver && !pause)
    {
        pthread_t t1, t2, t3, t4;
        pthread_create(&t1, NULL, player1Update, (void *)&p1);
        pthread_create(&t2, NULL, player2Update, (void *)&p2);
        pthread_create(&t3, NULL, MoveBall, (void *)&gameBall);

        CollisionArgs args;
        args.GameBall = gameBall;
        args.p1 = p1;
        args.p2 = p2;
        pthread_create(&t4, NULL, detectCollision, (void *)&gameBall);

        pthread_join(t1, NULL);
        pthread_join(t2, NULL);
        pthread_join(t3, NULL);
        pthread_join(t4, NULL);
    }

    RectangleShape upperBound(Vector2f(500, 25)), lowerBound(Vector2f(500, 25));
    upperBound.setPosition(Vector2f(0, 0));
    lowerBound.setPosition(Vector2f(0, 375));
    upperBound.setFillColor(Color::White);
    lowerBound.setFillColor(Color::White);

    Text score;
    score.setFont(font);
    score.setString(std::to_string(p1Score));
    score.setCharacterSize(30);
    score.setPosition(110, -8);
    score.setFillColor(sf::Color::Black);
    score.setStyle(sf::Text::Bold);

    win->draw(p1.paddle);
    win->draw(p2.paddle);
    win->draw(gameBall.body);

    AddPartition(win);

    win->draw(upperBound);

    win->draw(score);
    score.setString(std::to_string(p2Score));
    score.setPosition(375, -8);
    win->draw(score);

    win->draw(lowerBound);

    if (gameOver)
    {
        Menu gameOverScreen;
        gameOverScreen.DisplayGameOver(win);
    }
}

int main()
{
    XInitThreads();
    RenderWindow window(sf::VideoMode(500, 400), "Bing Pong");

    Menu* menuScreen = new Menu();
    gameState = 0;
    player Player1, Player2;
    ball gameBall;

    sf::Font font;
    if (!font.loadFromFile("Carre-JWja.ttf"))
    {
        perror("No font found\n");
        exit(1);
    } 

    while (window.isOpen())
    {
        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
                window.close();
        }

        window.clear();
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::P))
        {
            if (!gameOver)
            {
                pause = !pause;
            }
        }
        if (gameState == 0)
        {
            menuScreen->Display(&window);
        }
        else if (gameState == 1)
        {
            StartGame(&window, Player1, Player2, gameBall, font);
        }
        window.display();
    }

    delete menuScreen;
    return 0;
}