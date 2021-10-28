#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <array>
#include <random>
#include <iostream>
#include <algorithm>
#include <iterator>
#include <chrono>

using namespace sf;

int main()
{
    /* create window */
    RenderWindow window(VideoMode(1920, 1080), "Buzzy's Revenge");

    /* load background image */
    Texture backgroundTexture;
    backgroundTexture.loadFromFile("graphics/background.png");

    /* create background sprite */
    Sprite backgroundSprite(backgroundTexture);
    backgroundSprite.setPosition(0, 0);

    /* load font */
    Font font;
    font.loadFromFile("fonts/KOMIKAP_.ttf");

    /* add title text */
    Text titleText("Buzzy's Revenge", font, 75);
    titleText.setPosition(600, 100);
    titleText.setFillColor(Color::Red);

    /* add instruction text below title */
    Text instructionLine1("Press Enter to Restart Game", font, 50);
    instructionLine1.setPosition(535, 300);
    instructionLine1.setFillColor(Color::White);

    Text instructionLine2("Press Esc to Exit", font, 50);
    instructionLine2.setPosition(700, 375);
    instructionLine2.setFillColor(Color::White);

    Text instructionLine3("Press Space to Powerup", font, 50);
    instructionLine3.setPosition(605, 450);
    instructionLine3.setFillColor(Color::White);

    Text instructionLine4("Press Up/Down Arrow to Aim", font, 50);
    instructionLine4.setPosition(560, 525);
    instructionLine4.setFillColor(Color::White);

    /* add 'created by' lines */
    Text authorLine1("Created by", font, 20);
    authorLine1.setPosition(875, 725);
    authorLine1.setFillColor(Color::White);

    Text authorLine2("Gracen Wallace", font, 20);
    authorLine2.setPosition(850, 750);
    authorLine2.setFillColor(Color::White);

    /* load insect tecture */
    Texture beeTexture;
    beeTexture.loadFromFile("graphics/insect.png");

    /* create insect sprite */
    Sprite beeSprite;
    beeSprite.setTexture(beeTexture);
    beeSprite.setScale(-1.75, 1.75);
    beeSprite.setPosition(1700, 150);

    /* add launch power bar */
    RectangleShape powerBar;
    powerBar.setSize(Vector2f(250, 50));
    powerBar.setFillColor(Color::Red);
    powerBar.setPosition(200, 975);

    RectangleShape powerBarOutline;
    powerBarOutline.setOutlineColor(Color::Black);
    powerBarOutline.setOutlineThickness(5);
    powerBarOutline.setSize(Vector2f(500, 50));
    powerBarOutline.setFillColor(Color::Transparent);
    powerBarOutline.setPosition(200, 975);

    /* add text label for power bar */
    Text powerText("Power", font, 30);
    powerText.setPosition(50, 985);
    powerText.setFillColor(Color::White);

    /* add lives label */
    Text livesText("Lives", font, 30);
    livesText.setPosition(50, 30);
    livesText.setFillColor(Color::White);

    /* load texture for buzzy's life indicators */
    Texture lifeTexture;
    lifeTexture.loadFromFile("graphics/buzzy_life.png");

    /* create buzzy's life sprites */
    Sprite firstLife, secondLife, thirdLife, fourthLife, fifthLife;

    firstLife.setTexture(lifeTexture);
    secondLife.setTexture(lifeTexture);
    thirdLife.setTexture(lifeTexture);
    fourthLife.setTexture(lifeTexture);
    fifthLife.setTexture(lifeTexture);

    firstLife.setPosition(160, 15);
    secondLife.setPosition(220, 15);
    thirdLife.setPosition(280, 15);
    fourthLife.setPosition(340, 15);
    fifthLife.setPosition(400, 15);

    Sprite buzzyLives[] = {firstLife, secondLife, thirdLife, fourthLife, fifthLife};

    for (int i = 0; i < 5; i++)
    {
        buzzyLives[i].setScale(0.5, 0.5);
    }

    /* add score label */
    Text scoreText("Score: 0", font, 30);
    scoreText.setPosition(1700, 30);
    scoreText.setFillColor(Color::White);

    /* load unicorn texture */
    Texture unicornTexture;
    unicornTexture.loadFromFile("graphics/angry_unicorn.png");

    /* load bulldog texture */
    Texture bulldogTexture;
    bulldogTexture.loadFromFile("graphics/bulldog.png");

    /* load bunny texture */
    Texture bunnyTexture;
    bunnyTexture.loadFromFile("graphics/bunny.png");

    /* load chicken texture */
    Texture chickenTexture;
    chickenTexture.loadFromFile("graphics/chicken.png");

    /* load dog texture */
    Texture dogTexture;
    dogTexture.loadFromFile("graphics/dog.png");

    /* load frog texture */
    Texture frogTexture;
    frogTexture.loadFromFile("graphics/frog.png");

    /* load mouse texture */
    Texture mouseTexture;
    mouseTexture.loadFromFile("graphics/mouse.png");

    /* load pig texture */
    Texture pigTexture;
    pigTexture.loadFromFile("graphics/pig.png");

    /* load sheep texture */
    Texture sheepTexture;
    sheepTexture.loadFromFile("graphics/sheep.png");

    /* load tiger texture */
    Texture tigerTexture;
    tigerTexture.loadFromFile("graphics/tiger.png");

    /* create unicorn sprite */
    Sprite unicornSprite;
    unicornSprite.setTexture(unicornTexture);
    unicornSprite.setScale(1.25, 1.25);

    /* create bulldog sprite */
    Sprite bulldogSprite;
    bulldogSprite.setTexture(bulldogTexture);
    bulldogSprite.setScale(1.4, 1.4);

    /* create bunny sprite */
    Sprite bunnySprite;
    bunnySprite.setTexture(bunnyTexture);
    bunnySprite.setScale(1.5, 1.5);

    /* create chicken sprite */
    Sprite chickenSprite;
    chickenSprite.setTexture(chickenTexture);
    chickenSprite.setScale(1.5, 1.5);

    /* create dog sprite */
    Sprite dogSprite;
    dogSprite.setTexture(dogTexture);
    dogSprite.setScale(1.25, 1.25);

    /* create frog sprite */
    Sprite frogSprite;
    frogSprite.setTexture(frogTexture);
    frogSprite.setScale(1.25, 1.25);

    /* create mouse sprite */
    Sprite mouseSprite;
    mouseSprite.setTexture(mouseTexture);
    mouseSprite.setScale(1.25, 1.25);

    /* create pig sprite */
    Sprite pigSprite;
    pigSprite.setTexture(pigTexture);
    pigSprite.setScale(1.25, 1.25);

    /* create sheep sprite */
    Sprite sheepSprite;
    sheepSprite.setTexture(sheepTexture);
    sheepSprite.setScale(1.25, 1.25);

    /* create tiger sprite */
    Sprite tigerSprite;
    tigerSprite.setTexture(tigerTexture);
    tigerSprite.setScale(0.15, 0.15);

    /* set randomized positions of creatures */
    Sprite sprites[] = {unicornSprite, bulldogSprite, bunnySprite, chickenSprite, dogSprite, frogSprite, mouseSprite, pigSprite, sheepSprite, tigerSprite};
    bool livingSprites[] = {true, true, true, true, true, true, true, true, true, true};
    Vector2f positions[] = {Vector2f(1550, 300), Vector2f(1750, 300), Vector2f(1550, 455), Vector2f(1750, 455), Vector2f(1550, 605),
                            Vector2f(1750, 605), Vector2f(1550, 755), Vector2f(1750, 755), Vector2f(1550, 905), Vector2f(1750, 905)};

    std::array<int, 10> randomizedOrder = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

    bool cont = true;

    int unicornIndex = 0;
    int bulldogIndex = 1;
    int tigerIndex = 9;

    /* ensure mascots are not in same column */
    while (cont)
    {
        shuffle(randomizedOrder.begin(), randomizedOrder.end(), std::default_random_engine(std::chrono::system_clock::now().time_since_epoch().count()));
        if ((randomizedOrder[bulldogIndex] % 2) != (randomizedOrder[tigerIndex] % 2))
        {
            cont = false;
        }
    }

    for (int i = 0; i < 10; i++)
    {
        sprites[i].setPosition(positions[randomizedOrder[i]]);
    }

    beeSprite.setPosition(1700, 150);

    /* load buzzy texture */
    Texture buzzyTexture;
    buzzyTexture.loadFromFile("graphics/small_buzzy.png");

    /* create buzzy sprite */
    Sprite buzzySprite;
    buzzySprite.setTexture(buzzyTexture);
    buzzySprite.setScale(1.25, 1.25);
    buzzySprite.setRotation(-30);
    buzzySprite.setPosition(5, 550);

    bool started = false;
    int numLives = 5;
    int score = 0;

    float velocityMin = 200;
    float velocityMax = 300;
    float velocityStep = 5;
    float powerSize = 250;
    float velocity = ((powerSize * (velocityMax - velocityMin)) / 500) + velocityMin;

    bool buzzyActive = false;
    float initialX = buzzySprite.getPosition().x;
    float initialY = buzzySprite.getPosition().y;
    float angle = 360 - buzzySprite.getRotation();
    float velocityX = velocity * std::cos(angle);
    float velocityY = velocity * std::sin(angle);
    bool loseLife;
    float dY;
    Clock clock;

    std::default_random_engine generator;
    generator.seed(std::chrono::system_clock::now().time_since_epoch().count());
    std::uniform_int_distribution<int> vertical(-10, 10);

    bool beeActive = false;

    while (window.isOpen())
    {
        Event event;

        /* check for death of mascots or zero lives */
        if (numLives == 0 || (started && !livingSprites[bulldogIndex] && !livingSprites[tigerIndex]))
        {
            started = false;
            numLives = 5;
            score = 0;

            for (int i = 0; i < 10; i++)
            {
                livingSprites[i] = true;
            }

            /* ensure mascots are not in same column */
            while (true)
            {
                shuffle(randomizedOrder.begin(), randomizedOrder.end(), std::default_random_engine(std::chrono::system_clock::now().time_since_epoch().count()));
                if ((randomizedOrder[bulldogIndex] % 2) != (randomizedOrder[tigerIndex] % 2))
                {
                    break;
                }
            }

            for (int i = 0; i < 10; i++)
            {
                sprites[i].setPosition(positions[randomizedOrder[i]]);
            }

            beeSprite.setScale(-1.75, 1.75);
            beeSprite.setPosition(1700, 150);
            buzzySprite.setPosition(5, 550);
            buzzySprite.setRotation(-30);
        }

        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
            {
                window.close();
            }

            if (event.type == Event::KeyPressed && !buzzyActive)
            {
                switch (event.key.code)
                {
                case Keyboard::Enter:
                    if (!started)
                    {
                        started = true;
                        beeActive = true;
                    }
                    else
                    {
                        started = false;
                        numLives = 5;
                        score = 0;

                        for (int i = 0; i < 10; i++)
                        {
                            livingSprites[i] = true;
                        }

                        /* ensure mascots are not in same column */
                        while (true)
                        {
                            shuffle(randomizedOrder.begin(), randomizedOrder.end(), std::default_random_engine(std::chrono::system_clock::now().time_since_epoch().count()));
                            if ((randomizedOrder[bulldogIndex] % 2) != (randomizedOrder[tigerIndex] % 2))
                            {
                                break;
                            }
                        }

                        for (int i = 0; i < 10; i++)
                        {
                            sprites[i].setPosition(positions[randomizedOrder[i]]);
                        }

                        beeSprite.setScale(-1.75, 1.75);
                        beeSprite.setPosition(1700, 150);
                        buzzySprite.setPosition(5, 550);
                        buzzySprite.setRotation(-30);
                    }
                    break;
                case Keyboard::Space:
                    if (started && powerSize <= 500)
                    {
                        if (powerSize == 500)
                        {
                            velocity = velocityMin;
                        }
                        velocity += velocityStep;
                        powerSize = (((velocity - velocityMin) * 500) / (velocityMax - velocityMin));
                        powerBar.setSize(Vector2f(powerSize, 50));
                    }
                    break;
                case Keyboard::Escape:
                    window.close();
                    break;
                case Keyboard::Up:
                    if (started && buzzySprite.getRotation() > 270)
                    {
                        buzzySprite.setRotation(buzzySprite.getRotation() - 5);
                    }
                    break;
                case Keyboard::Down:
                    if (started && buzzySprite.getRotation() < 355)
                    {
                        buzzySprite.setRotation(buzzySprite.getRotation() + 5);
                    }
                    break;
                default:
                    break;
                }
            }

            if (event.type == Event::KeyReleased && !buzzyActive)
            {
                if (started && event.key.code == Keyboard::Space)
                {
                    buzzyActive = true;
                    angle = 360 - buzzySprite.getRotation();
                    velocityX = velocity * std::cos(angle * M_PI / 180);
                    velocityY = velocity * std::sin(angle * M_PI / 180);
                    clock.restart();
                    loseLife = true;
                }
            }
        }

        if (beeActive)
        {
            Vector2f currentPos = beeSprite.getPosition();
            Vector2f currentScl = beeSprite.getScale();

            int numVerticalMoves;
            do
            {
                numVerticalMoves = vertical(generator);
            } while ((currentPos.y + numVerticalMoves) < 20 && (currentPos.y + numVerticalMoves) > 1060);

            if (beeSprite.getScale().x < 0)
            {
                if (currentPos.x > 150)
                {
                    beeSprite.setPosition(currentPos.x - 1, currentPos.y + numVerticalMoves);
                }
                else
                {
                    beeSprite.setScale(currentScl.x * -1, currentScl.y);
                }
            }
            else
            {
                if (currentPos.x < 1770)
                {
                    beeSprite.setPosition(currentPos.x + 1, currentPos.y + numVerticalMoves);
                }
                else
                {
                    beeSprite.setScale(currentScl.x * -1, currentScl.y);
                }
            }
        }

        //move buzzy
        if (started && buzzyActive)
        {
            float t = clock.getElapsedTime().asSeconds();
            float dX = velocityX * t;
            float prevDY = dY;
            dY = velocityY * t - 4 * 9.8 * t * t;
            buzzySprite.setPosition(initialX + 2 * dX, initialY - 2 * dY);
            buzzySprite.setRotation(buzzySprite.getRotation() + 0.15);
        }

        //check for intersections w/ animals
        if (started && buzzyActive)
        {
            for (int i = 0; i < 10; i++)
            {
                if (buzzySprite.getGlobalBounds().intersects(sprites[i].getGlobalBounds()) && livingSprites[i])
                {
                    livingSprites[i] = false;
                    if (i == unicornIndex)
                    {
                        loseLife = false;
                        if (numLives < 5)
                        {
                            numLives++;
                        }

                        for (int i = 0; i < 10; i++)
                        {
                            if (sprites[i].getPosition().x == positions[randomizedOrder[unicornIndex]].x && sprites[i].getPosition().y < positions[randomizedOrder[unicornIndex]].y && livingSprites[i])
                            {
                                sprites[i].setPosition(sprites[i].getPosition().x, sprites[i].getPosition().y + 155);
                            }
                        }
                    }
                    else if (i == bulldogIndex)
                    {
                        loseLife = false;
                        score += 25;
                        for (int i = 0; i < 10; i++)
                        {
                            if (sprites[i].getPosition().x == positions[randomizedOrder[bulldogIndex]].x)
                            {
                                livingSprites[i] = false;
                            }
                        }
                    }
                    else if (i == tigerIndex)
                    {
                        loseLife = false;
                        score += 25;
                        for (int i = 0; i < 10; i++)
                        {
                            if (sprites[i].getPosition().x == positions[randomizedOrder[tigerIndex]].x)
                            {
                                livingSprites[i] = false;
                            }
                        }
                    }

                    buzzySprite.setPosition(initialX, initialY);
                    buzzySprite.setRotation(-30);
                    buzzyActive = false;
                    if (loseLife)
                    {
                        numLives--;
                    }
                }
            }

            if (beeActive && buzzySprite.getGlobalBounds().intersects(beeSprite.getGlobalBounds()))
            {
                loseLife = false;
                beeActive = false;
                score += 75;
            }
        }

        // check for leaving screen
        if (started && buzzyActive && (buzzySprite.getPosition().x > 1920 || buzzySprite.getPosition().y < 0 || buzzySprite.getPosition().y > 1080))
        {
            buzzySprite.setPosition(initialX, initialY);
            buzzySprite.setRotation(-30);
            buzzyActive = false;
            if (loseLife)
            {
                numLives--;
            }
        }

        window.clear();

        /* draw background */
        window.draw(backgroundSprite);

        /* draw game status */
        window.draw(powerBar);
        window.draw(powerBarOutline);
        window.draw(powerText);
        window.draw(livesText);
        String scoreString = "Score: " + std::to_string(score);
        scoreText.setString(scoreString);
        window.draw(scoreText);

        /* draw insect and buzzy */
        if (beeActive)
        {
            window.draw(beeSprite);
        }
        window.draw(buzzySprite);

        /* draw current lives */
        for (int i = 0; i < numLives; i++)
        {
            window.draw(buzzyLives[i]);
        }

        /* draw living creatures */
        for (int i = 0; i < 10; i++)
        {
            if (livingSprites[i])
            {
                window.draw(sprites[i]);
            }
        }

        /* draw title screen info if game not started yet */
        if (!started)
        {
            window.draw(titleText);
            window.draw(instructionLine1);
            window.draw(instructionLine2);
            window.draw(instructionLine3);
            window.draw(instructionLine4);
            window.draw(authorLine1);
            window.draw(authorLine2);
        }

        window.display();
    }

    return EXIT_SUCCESS;
}