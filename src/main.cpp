#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>
#include <algorithm>

class Player {
public:
    Player (sf::Vector2f size, sf::Vector2f pos);
    Player () = default;
    void render(sf::RenderWindow &win) {
        win.draw(rect, sf::BlendMode(sf::BlendMode::OneMinusDstColor, sf::BlendMode::Zero, sf::BlendMode::Add));
        
    }
    void move(sf::Vector2f dir = sf::Vector2f(0, 0)) {
        acc = dir;
        acc *= speed;
    
        vel += acc;
        pos += vel;

        vel *= disp;
        rect.setPosition(pos);
    }
    sf::Vector2f outbounds(sf::Vector2f boundsTL, sf::Vector2f boundsBR) {
        float bX, bY;
        if (pos.y > boundsTL.y) bY=1;
        if (pos.y < boundsBR.y) bY=-1;
        if (pos.x < boundsTL.x) bX=-1;
        if (pos.x > boundsBR.x) bX=1;

        return sf::Vector2f(bX, bY);
    }

private:
    sf::RectangleShape rect;
    sf::Vector2f size;
    sf::Vector2f pos;

    sf::Vector2f acc;
    sf::Vector2f vel;
    float disp, speed;
};

Player::Player (sf::Vector2f size, sf::Vector2f pos){
    rect.setSize(size);
    rect.setPosition(pos);
    rect.setFillColor(sf::Color::White);

    vel = sf::Vector2f(0, 0);
    acc = sf::Vector2f(0, 0);
    disp = 0.999;
    speed = 0.05;
}

class CameraController {
public:
    CameraController (sf::Vector2f size, sf::Vector2f centre);
    CameraController () = default;

    void updateView (Player plr, sf::RenderWindow& win) {
        transitionX(plr.outbounds(bound1, bound2).x);
        win.setView(cam);
        sf::RectangleShape newScreen;
        newScreen.setFillColor(sf::Color::White);
        newScreen.setSize(sf::Vector2f(800,600));
        newScreen.setPosition(bound1);
        if (drawScreen == -1) win.draw(newScreen);
        else {
            newScreen.setPosition(bound1p);
            win.draw(newScreen);
        }
    }
private:
    sf::View cam;
    sf::Vector2f size;
    sf::Vector2f bound1, bound2, bound1p, bound2p;
    float time, duration;
    float prevDis, currDis;
    int movementDir, movementDirY;
    int drawScreen = 1;

    void transitionX (int dir) {
        // starting a new one
        if (dir != 0 && time == -1 ) {
            time = 0;
            prevDis = 0;
            movementDir = dir;
            bound1p = bound1;
            bound2p = bound2p;
            bound1.x += size.x*movementDir;
            bound2.x += size.x*movementDir;
            drawScreen*=-1;
        } else if (time >= 0 && time <= duration) {
            time+=0.1;
            float timeR = (time*3.14)/180;
            currDis = (size.x * pow(sin(timeR),6))-prevDis;
            cam.move(sf::Vector2f(movementDir*currDis, 0));
            prevDis = size.x * pow(sin(timeR),6);

        } if (time > duration) {
            time = -1;
            movementDir=0;
        }
    }
};

CameraController::CameraController(sf::Vector2f size_, sf::Vector2f centre_) {
    cam.setSize(size_);
    size = size_;
    cam.setCenter(centre_);
    duration = 90;
    prevDis = 0;
    currDis=0;
    time=-1;
    bound1 = sf::Vector2f(0,0);
    bound2 = sf::Vector2f(size);
}

class Game {
public:
    Game ();

    void play(sf::RenderWindow& win) {
        events(win);
        update();
        render(win);
    }
private: 
    CameraController gCam;
    Player gPlr;
    int state;

    sf::RectangleShape startBtn;
    sf::RectangleShape quitBtn;
    sf::Font font;
    sf::Text title, startText, quitText;

    void update() {
        gPlr.move();
    }

    void titlescreen(sf::RenderWindow& win) {
        win.draw(title);
        win.draw(startBtn);
        win.draw(quitBtn);
        win.draw(startText);
        win.draw(quitText);

        sf::Vector2i mosPos = sf::Mouse::getPosition(win);
        if (startBtn.getGlobalBounds().contains(mosPos.x, mosPos.y) && sf::Mouse::isButtonPressed(sf::Mouse::Left)) state=1;
        else if (quitBtn.getGlobalBounds().contains(mosPos.x, mosPos.y) && sf::Mouse::isButtonPressed(sf::Mouse::Left)) win.close();
    }

    void events (sf::RenderWindow& win) {
        if (win.isOpen())
        {
            sf::Event event;
            if (win.pollEvent(event))
            {
                if (event.type == sf::Event::Closed)
                    win.close();
                
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
                    gPlr.move(sf::Vector2f(-1, 0));
                }
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
                    gPlr.move(sf::Vector2f(1, 0));
                }
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
                    gPlr.move(sf::Vector2f(0, -1));
                } 
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
                    gPlr.move(sf::Vector2f(0, 1));
                }
            }
        }

    }

    void render (sf::RenderWindow& win) {

        if (state==1) {
            win.clear();
            gCam.updateView(gPlr, win);         
            gPlr.render(win);
        } else {
            win.clear(sf::Color::White);
            titlescreen(win);
        }
        win.display();
    }
};

Game::Game() {

    gPlr = Player(sf::Vector2f(100, 100), sf::Vector2f(50, 50));
    gCam = CameraController(sf::Vector2f(800, 600), sf::Vector2f(800/2, 600/2));

    startBtn.setSize(sf::Vector2f(200, 100));
    startBtn.setPosition(sf::Vector2f(800/2 - startBtn.getGlobalBounds().width/2, 600/2 -startBtn.getGlobalBounds().height/2 ));
    startBtn.setFillColor(sf::Color::Black);
    sf::FloatRect startBounds = startBtn.getGlobalBounds();

    quitBtn.setSize(sf::Vector2f(200, 100));
    quitBtn.setPosition(sf::Vector2f(800/2 - quitBtn.getGlobalBounds().width/2, 600/2-quitBtn.getGlobalBounds().height/2 +105));
    quitBtn.setFillColor(sf::Color::Black);
    sf::FloatRect quitBounds = quitBtn.getGlobalBounds();

    font.loadFromFile("../src/Maldini.ttf");
    title.setFont(font);
    title.setCharacterSize(50);
    title.setString("TRANSISTIONS");
    title.setPosition(sf::Vector2f(5,5));
    title.setFillColor(sf::Color::Black);

    startText.setFont(font);
    startText.setCharacterSize(40);
    startText.setString("START.");
    startText.setPosition(sf::Vector2f(startBounds.left+startBounds.width/2 - startText.getGlobalBounds().width/2, 
                                       startBounds.top+startBounds.height/2 - startText.getGlobalBounds().height/2));
    startText.setFillColor(sf::Color::White);

    quitText.setFont(font);
    quitText.setCharacterSize(40);
    quitText.setString("QUIT.");
    quitText.setPosition(sf::Vector2f(quitBounds.left+quitBounds.width/2 - quitText.getGlobalBounds().width/2, 
                                       quitBounds.top+quitBounds.height/2 - quitText.getGlobalBounds().height/2));
    quitText.setFillColor(sf::Color::White);

}

int main()
{
    sf::RenderWindow window(sf::VideoMode(800, 600), "Transitions");
    Game game;
    while (true) game.play(window);
    return 0;
}