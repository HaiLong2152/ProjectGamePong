#include "gamePong.h"

void PongGame::updatePaddle(Paddle &paddle)
{
    // Move paddle
    paddle.rect.y += paddle.speed;

    // Limit movement
    if (paddle.rect.y < 23)
    {
        paddle.rect.y = 23;
    }
    else if (paddle.rect.y > SCREEN_HEIGHT - 23 - paddle.rect.h)
    {
        paddle.rect.y = SCREEN_HEIGHT - 23 - paddle.rect.h;
    }
}

void PongGame::updateAI()
{
    int AI_DIFFICUTY = 30;
    int paddleCenter = rightPaddle.rect.y + rightPaddle.rect.h / 2;
    int ballCenter = ball.rect.y + ball.rect.h / 2;
    int moveSpeed = AI_SPEED - (rand() % 5);

    // Only move if the ball is moving towards and half screen
    if (ball.speedX > 0 && ball.rect.x > SCREEN_WIDTH / 2)
    {
        // Add some difficulty by limiting AI reaction
        if (paddleCenter < ballCenter - AI_DIFFICUTY)
        {
            rightPaddle.rect.y += moveSpeed;
        }
        else if (paddleCenter > ballCenter + AI_DIFFICUTY)
        {
            rightPaddle.rect.y -= moveSpeed;
        }

        // Limit movement
        if (rightPaddle.rect.y < 23)
        {
            rightPaddle.rect.y = 23;
        }
        else if (rightPaddle.rect.y > SCREEN_HEIGHT - 23 - rightPaddle.rect.h)
        {
            rightPaddle.rect.y = SCREEN_HEIGHT - 23 - rightPaddle.rect.h;
        }
    }
}

void PongGame::resetBall(int Direct)
{
    ball.rect.x = SCREEN_WIDTH / 2 - BALL_SIZE / 2;
    ball.rect.y = SCREEN_HEIGHT / 2 - BALL_SIZE / 2;
    ball.rect.w = BALL_SIZE;
    ball.rect.h = BALL_SIZE;

    ball.speedX =  Direct*BALL_MIN_SPEED;
    ball.speedY =  ((rand() % 2 == 0) ? 1 : -1) * BALL_MIN_SPEED;
}

void PongGame::updateBall()
{
    // Move ball
    ball.rect.x += ball.speedX;
    ball.rect.y += ball.speedY;

    //  impact with top
    if (ball.rect.y <= 23)
    {
        ball.rect.y = 23;
        ball.speedY = -ball.speedY + (rand() % 3 - 1);
        ball.speedX =  ball.speedX + (rand() % 3 - 1);

        if (abs(ball.speedY) < BALL_MIN_SPEED) ball.speedY = ((ball.speedY > 0) ? 1 : -1) * BALL_MIN_SPEED;
        if (abs(ball.speedX) < BALL_MIN_SPEED) ball.speedX = ((ball.speedX > 0) ? 1 : -1) * BALL_MIN_SPEED;

        if (abs(ball.speedY) > BALL_MAX_SPEED) ball.speedY = ((ball.speedY > 0) ? 1 : -1) * BALL_MAX_SPEED;
        if (abs(ball.speedX) > BALL_MAX_SPEED) ball.speedX = ((ball.speedX > 0) ? 1 : -1) * BALL_MAX_SPEED;
    }
    //  impact with bottom
    else if (ball.rect.y + ball.rect.h >= SCREEN_HEIGHT - 23)
    {
        ball.rect.y = SCREEN_HEIGHT - ball.rect.h - 23;
        ball.speedY = -ball.speedY + (rand() % 3 - 1);
        ball.speedX =  ball.speedX + (rand() % 3 - 1);

        if (abs(ball.speedY) < BALL_MIN_SPEED) ball.speedY = ((ball.speedY > 0) ? 1 : -1) * BALL_MIN_SPEED;
        if (abs(ball.speedX) < BALL_MIN_SPEED) ball.speedX = ((ball.speedX > 0) ? 1 : -1) * BALL_MIN_SPEED;

        if (abs(ball.speedY) > BALL_MAX_SPEED) ball.speedY = ((ball.speedY > 0) ? 1 : -1) * BALL_MAX_SPEED;
        if (abs(ball.speedX) > BALL_MAX_SPEED) ball.speedX = ((ball.speedX > 0) ? 1 : -1) * BALL_MAX_SPEED;
    }

    // impact with left paddle
    if (ball.speedX < 0 &&
            ball.rect.x <= leftPaddle.rect.x + leftPaddle.rect.w &&
            ball.rect.x + ball.rect.w >= leftPaddle.rect.x &&
            ball.rect.y <= leftPaddle.rect.y + leftPaddle.rect.h &&
            ball.rect.y + ball.rect.h >= leftPaddle.rect.y)
    {
        ball.speedX = -ball.speedX + (rand() % 3 - 1);
        ball.speedY =  ball.speedY + (rand() % 3 - 1);

        if (ball.speedX <= 0) ball.speedX = BALL_MIN_SPEED;
        if (abs(ball.speedY) < BALL_MIN_SPEED) ball.speedY = ((ball.speedY > 0) ? 1 : -1) * BALL_MIN_SPEED;
        if (abs(ball.speedY) > BALL_MAX_SPEED) ball.speedY = ((ball.speedY > 0) ? 1 : -1) * BALL_MAX_SPEED;

        // Play hit sound
        if (gHitSound != nullptr)
        {
            Mix_PlayChannel(-1, gHitSound, 0);
        }
    }

    // impact with right paddle
    if (ball.speedX > 0 &&
            ball.rect.x <= rightPaddle.rect.x + rightPaddle.rect.w &&
            ball.rect.x + ball.rect.w >= rightPaddle.rect.x &&
            ball.rect.y <= rightPaddle.rect.y + rightPaddle.rect.h &&
            ball.rect.y + ball.rect.h >= rightPaddle.rect.y)
    {
        ball.speedX = -ball.speedX + (rand() % 3 - 1);
        ball.speedY =  ball.speedY + (rand() % 3 - 1);

        if (ball.speedX <= 0) ball.speedX = -BALL_MIN_SPEED;
        if (abs(ball.speedY) < BALL_MIN_SPEED) ball.speedY = ((ball.speedY > 0) ? 1 : -1) * BALL_MIN_SPEED;
        if (abs(ball.speedY) > BALL_MAX_SPEED) ball.speedY = ((ball.speedY > 0) ? 1 : -1) * BALL_MAX_SPEED;

        // Play hit sound
        if (gHitSound != nullptr)
        {
            Mix_PlayChannel(-1, gHitSound, 0);
        }
    }

    // Ball out of bounds (scoring)
    if (ball.rect.x < 0)
    {
        // Right player scores
        rightPaddle.score++;

        SDL_Delay(100);
        if(rightPaddle.score > leftPaddle.score) resetBall(1);
        else if(rightPaddle.score < leftPaddle.score) resetBall(-1);
        else resetBall((rand() % 2 == 0) ? 1 : -1);
    }
    else if (ball.rect.x > SCREEN_WIDTH)
    {
        // Left player scores
        leftPaddle.score++;
        SDL_Delay(100);
        if(rightPaddle.score > leftPaddle.score) resetBall(1);
        else if(rightPaddle.score < leftPaddle.score) resetBall(-1);
        else resetBall((rand() % 2 == 0) ? 1 : -1);
    }
}


