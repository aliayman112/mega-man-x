#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <vector>

using namespace std;
using namespace sf;

RenderWindow window(sf::VideoMode(1680, 1050), "SFML works!");
Sprite player;
Texture jump;
Texture shootidleAnimation;
Texture shootwalkAnimation;
Texture idle;
Texture walk;
Texture bulletTexture;
Texture shootJump;
Texture Bg;
Texture block;
Texture spawn;
Texture levelBg;
Sprite level;
Sprite block1;
Sprite background;
bool ismoving = false;
bool isShooting = false;
bool isjumping = false;
int animationIndex = 0;
float idleAnimationTimer = 0.0f;
float walkAnimationTimer = 0.0f;
float jumpAnimationTimer = 0.0f;
float shootAnimationTimer = 0.0f;
const float IDLE_FRAME_TIME = 0.2f;
const float WALK_FRAME_TIME = 0.1f;
const float JUMP_FRAME_TIME = 0.15f;
bool isSpawning = true;
float spawnTimer = 0.0f;
const float SPAWN_DURATION = 1.5f; 
const float SPAWN_FRAME_TIME = 0.03f;

int playersize = 3;
View camera1(Vector2f(0, 0), Vector2f(window.getSize().x, window.getSize().y));

struct Bullet {
    Sprite shape;
    Vector2f velocity;
};

enum Direction { Left, Right };
Direction playerDirection = Right;

vector<Bullet> bullets;

void Update();
void Start();
void Draw();
void playerMouvement();
void PlayerAnimation();
void shooting();
void shoot(Vector2f playerPosition);
void UpdateBullets();
void SpawnAnimation();

float deltaTime;
bool canShoot = true;
float shootingTimer = 0.0f;
const float shootingDuration = 0.3f;
float speedY;

Music music;

int main() {
    Start();
    Clock clock;
    while (window.isOpen()) {
        deltaTime = clock.restart().asSeconds();
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == Event::KeyReleased && event.key.code == Keyboard::C)
                canShoot = true;
        }
        Update();
        Draw();
    }
    return 0;
}

void Start() {
    window.setFramerateLimit(60);
    player.setOrigin(51 / 2, 51 / 2);
    idle.loadFromFile("playerAnimations/mega_idle.png");
    walk.loadFromFile("playerAnimations/mega_walk.png");
    bulletTexture.loadFromFile("playerAnimations/blast.png");
    shootwalkAnimation.loadFromFile("playerAnimations/mega_walk_shot.png");
    shootidleAnimation.loadFromFile("playerAnimations/mega_shot.png");
    jump.loadFromFile("playerAnimations/mega_jump.png");
    shootJump.loadFromFile("playerAnimations/mega_jump_shot.png");
    levelBg.loadFromFile("playerAnimations/foreground1.png");
    Bg.loadFromFile("playerAnimations/background1.png");
    block.loadFromFile("playerAnimations/block2.png");
    spawn.loadFromFile("playerAnimations/mega_spawn.png");
    music.openFromFile("playerAnimations/bg music.mp3");
    /*block1.setTexture(block);
    block1.setPosition(0, 677);*/
    player.setScale(playersize, playersize);
    player.setPosition({ 0,(-51/2)});
    background.setScale(5, 5);
    background.setPosition(0, 0);
    background.setTexture(Bg);
    level.setScale(3, 3);
    level.setPosition(0, 0);
    level.setTexture(levelBg);
    player.setTexture(spawn);
    player.setTextureRect(IntRect(0, 0, 51, 51));
    music.setLoop(true);
    music.setVolume(3);
    music.play();
}

void Update() {
    window.setView(camera1);
    camera1.setCenter({ player.getPosition().x,(float)window.getSize().y/2});
    if (isSpawning) {
        SpawnAnimation();
    }
    else {
        playerMouvement();
        shooting();
        PlayerAnimation();
        UpdateBullets();
    }
}

void playerMouvement() {
    ismoving = false;
    if (Keyboard::isKeyPressed(Keyboard::Right)) {
        player.move(5, 0);
        ismoving = true;
        player.setScale(playersize, playersize);
        playerDirection = Right;
    }
    if (Keyboard::isKeyPressed(Keyboard::Left)) {
        player.move(-5, 0);
        ismoving = true;
        player.setScale(-playersize, playersize);
        playerDirection = Left;
    }
    
    player.move(0, speedY);
    
    if (player.getPosition().y >= 600) {
        player.setPosition({ player.getPosition().x,600 });
        speedY = 0;
        isjumping = false;
        if (Keyboard::isKeyPressed(Keyboard::X)) {
            speedY = -20;
            isjumping = true;
        }
    }
    else {
        speedY += 1;
    }
}

void PlayerAnimation() {
    static bool wasMoving = false;
    static bool wasJumping = false;

    if (ismoving != wasMoving || isjumping != wasJumping) {
        animationIndex = 0;
        wasMoving = ismoving;
        wasJumping = isjumping;
    }
    if (isShooting) {
        if (isjumping) {
            if (speedY < 0) {
                if (animationIndex > 2) {
                    animationIndex = 2;
                }
            }
            else {
                if (animationIndex != 3) {
                    animationIndex = 3;
                }

            }

            player.setTexture(shootJump);
            player.setTextureRect(IntRect(animationIndex * 51, 0, 51, 51));
            animationIndex = ((animationIndex + 1) / 3) % 7;
        }
        else if (ismoving) {
            walkAnimationTimer += deltaTime;
            if (walkAnimationTimer >= WALK_FRAME_TIME) {
                walkAnimationTimer = 0.0f;
                player.setTexture(shootwalkAnimation);
                player.setTextureRect(IntRect(animationIndex * 51, 0, 51, 51));
                animationIndex = (animationIndex + 1) % 10;
            }
            
        }
        else  {
            player.setTexture(shootidleAnimation);
            player.setTextureRect(IntRect(animationIndex * 51, 0, 51, 51));
            animationIndex = (animationIndex + 1) % 2;
            
        }
        shootingTimer += deltaTime;
        if (shootingTimer >= shootingDuration) {
            isShooting = false;
            shootingTimer = 0.0f;
        }
    }
    else {
        if (isjumping) {
            jumpAnimationTimer += deltaTime;
            if (jumpAnimationTimer >= JUMP_FRAME_TIME) {
                jumpAnimationTimer = 0.0f;
            }
            if (speedY < 0) {
                if (animationIndex > 2) {
                    animationIndex = 2;
                }
            }
            else {
                if (animationIndex != 3) {
                    animationIndex = 3;
                }

            }

            player.setTexture(jump);
            player.setTextureRect(IntRect(animationIndex * 51, 0, 51, 51));
            animationIndex = ((animationIndex + 1) / 3) % 7;
        }
        else if (ismoving) {
            walkAnimationTimer += deltaTime;
            if (walkAnimationTimer >= WALK_FRAME_TIME) {
                walkAnimationTimer = 0.0f;
                player.setTexture(walk);
                player.setTextureRect(IntRect(animationIndex * 51, 0, 51, 51));
                animationIndex = (animationIndex + 1) % 11;
            }
        }


        else {
            idleAnimationTimer += deltaTime;
            if (idleAnimationTimer >= IDLE_FRAME_TIME) {
                idleAnimationTimer = 0.0f;
                player.setTexture(idle);
                player.setTextureRect(IntRect(animationIndex * 51, 0, 51, 51));
                animationIndex = (animationIndex + 1) % 4;
            }
        }
    }
    // Reset timers when animation state changes
    if (!ismoving && !isjumping) {
        walkAnimationTimer = 0.0f;
        jumpAnimationTimer = 0.0f;
    }
    if (!isjumping) {
        jumpAnimationTimer = 0.0f;
    }
   
    
}

void shooting() {
    if (Keyboard::isKeyPressed(Keyboard::C) && canShoot) {
        shoot(player.getPosition());
        canShoot = false;
        isShooting = true;
        shootingTimer = 0.0f;
    }
}

void shoot(Vector2f playerPosition) {
    Bullet bullet;
    bullet.shape.setTexture(bulletTexture);
    bullet.shape.setPosition(player.getPosition());
    float bulletspeed = 1800.0f;
    bullet.velocity = (playerDirection == Right) ? Vector2f(bulletspeed, 0) : Vector2f(-bulletspeed, 0);
    bullets.push_back(bullet);
}

void UpdateBullets() {
    for (size_t i = 0; i < bullets.size(); i++) {
        bullets[i].shape.move(bullets[i].velocity * deltaTime);
        if (bullets[i].shape.getPosition().x+bullets[i].shape.getGlobalBounds().getSize().x < camera1.getCenter().x - camera1.getSize().x/2 || bullets[i].shape.getPosition().x > camera1.getCenter().x + camera1.getSize().x / 2) {
            bullets.erase(bullets.begin() + i);
            i--;
        }
    }
}
void SpawnAnimation() {
    player.move(0, 50);
    if (player.getPosition().y >= 600) {
        player.setPosition(0, 600);
        spawnTimer += deltaTime;
        if (spawnTimer >= SPAWN_FRAME_TIME) {
            spawnTimer = 0.0f;
            if (animationIndex >= 6) {
                idleAnimationTimer = IDLE_FRAME_TIME;
                animationIndex = 0;
                isSpawning = false;
                player.setTexture(idle);
                return;
            }
            player.setTextureRect(IntRect(animationIndex * 51, 0, 51, 51));
            animationIndex = (animationIndex + 1) % 7;
        }
    }
    
}


void Draw() {
    window.clear();
    window.draw(background);
    window.draw(level);
    window.draw(player);
    for (const auto& bullet : bullets) {
        window.draw(bullet.shape);
    }
    window.display();
}


