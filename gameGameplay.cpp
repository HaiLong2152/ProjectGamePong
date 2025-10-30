#include "gamePong.h"

void PongGame::updatePaddle(Paddle &paddle)
{
    // Move paddle
    paddle.rect.y += paddle.speed;

    // Limit movement
    if (paddle.rect.y < BORDER)
        paddle.rect.y = BORDER;
    else if (paddle.rect.y > SCREEN_HEIGHT - BORDER - paddle.rect.h)
        paddle.rect.y = SCREEN_HEIGHT - BORDER - paddle.rect.h;
}

void PongGame::updateAI()
{
    // Distance between ball and BOT
    int AI_DIFFICUTY = 7;
    int paddleCenter = rightPaddle.rect.y + rightPaddle.rect.h / 2;
    int ballCenter = ball.rect.y + ball.rect.h / 2;
    // Make movement less predictable
    int moveSpeed = AI_SPEED - (rand() % 4);

    // Only move if the ball is moving towards and half screen
    if (ball.speedX > 0 && ball.rect.x > SCREEN_WIDTH / 2)
    {
        // Add some difficulty by limiting AI reaction
        if (paddleCenter < ballCenter - AI_DIFFICUTY)
            rightPaddle.rect.y += moveSpeed;
        else if (paddleCenter > ballCenter + AI_DIFFICUTY)
            rightPaddle.rect.y -= moveSpeed;

        // Limit movement
        if (rightPaddle.rect.y < BORDER)
            rightPaddle.rect.y = BORDER;
        else if (rightPaddle.rect.y > SCREEN_HEIGHT - BORDER - rightPaddle.rect.h)
            rightPaddle.rect.y = SCREEN_HEIGHT - BORDER - rightPaddle.rect.h;
    }
}

void PongGame::resetBall(int Direct)
{
    ball.rect.x = SCREEN_WIDTH / 2 - BALL_SIZE / 2;
    ball.rect.y = SCREEN_HEIGHT / 2 - BALL_SIZE / 2;
    ball.rect.w = BALL_SIZE;
    ball.rect.h = BALL_SIZE;

    ballResetDirection = Direct;

    // Đặt thời gian reset và đánh dấu bóng đang chờ
    ballResetTime = SDL_GetTicks() + BALL_WAIT_TIME;
    isBallWaiting = true;

    ballTrail.clear();

    if (gCountdownSound !=nullptr)
    {
        Mix_Volume(-1, 18);
        Mix_PlayChannel(-1, gCountdownSound, 0); // Phát âm thanh 1 lần
    }
    // Ngay lập tức đặt tốc độ bóng về 0
    ball.speedX = 0;
    ball.speedY = 0;
}

void PongGame::updateBall()
{
    // Xử lý phím tạm dừng

    if (gameStartTime == 0)
        gameStartTime = SDL_GetTicks();

    static bool pauseKeyPressed = false;
    const Uint8* currentKeyStates = SDL_GetKeyboardState(NULL);
    if (currentKeyStates[SDL_SCANCODE_P])
    {
        if (!pauseKeyPressed)
        {
            isPaused = !isPaused; // Bật/tắt tạm dừng
            pauseKeyPressed = true;
            if (isPaused)
                Mix_Pause(-1); // Tạm dừng âm thanh
            else
                Mix_Resume(-1); // Tiếp tục âm thanh
        }
    }
    else
        pauseKeyPressed = false;

    if (isMatchPointPause)
    {
        if (SDL_GetTicks() - matchPointPauseTime >= MATCH_POINT_PAUSE)
        {
            isMatchPointPause = false;
            if(rightPaddle.score > leftPaddle.score) resetBall(1);
            else if(rightPaddle.score < leftPaddle.score) resetBall(-1);
            else resetBall((rand() % 2 == 0) ? 1 : -1);
        }
        return;  // KHÔNG UPDATE BÓNG/TRẠNG THÁI
    }
    // Chỉ cập nhật khi không tạm dừng
    if (!isPaused)
    {

        if (isBallWaiting)
        {
            if (SDL_GetTicks() >= ballResetTime)
            {
                // Hết thời gian chờ, cho bóng bắt đầu di chuyển
                isBallWaiting = false;

                // Sử dụng hướng đã lưu
                ball.speedX = ballResetDirection > 0 ? BALL_MIN_SPEED : -BALL_MIN_SPEED;
                ball.speedY = (rand() % (2*BALL_MIN_SPEED)) - BALL_MIN_SPEED;
            }
            else
            {
                // Trong thời gian chờ, giữ bóng ở vị trí giữa và không di chuyển
                ball.speedX = 0;
                ball.speedY = 0;
            }
        }



        // Move ball
        ball.rect.x += ball.speedX;
        ball.rect.y += ball.speedY;

        if (isPaused || isBallWaiting)
        {
            ballTrail.clear();  // Xóa sạch trail
            return;  // Không thêm trail mới
        }

        if (!isBallWaiting && !isPaused && (ball.speedX != 0 || ball.speedY != 0))
        {
            ballTrail.push_front(ball.rect);  // Thêm vị trí hiện tại vào đầu
            //cout << "Trail size: " << ballTrail.size() << endl;
            if (ballTrail.size() > TRAIL_LENGTH)
                ballTrail.pop_back();
        }

        //  impact with top
        if (ball.rect.y <= BORDER)
        {
            ball.rect.y = BORDER;
            ball.speedY = -ball.speedY + (rand() % 3 - 1);
            ball.speedX =  ball.speedX + (rand() % 3 - 1);

            if (abs(ball.speedY) < BALL_MIN_SPEED) ball.speedY = ((ball.speedY > 0) ? 1 : -1) * BALL_MIN_SPEED;
            if (abs(ball.speedX) < BALL_MIN_SPEED) ball.speedX = ((ball.speedX > 0) ? 1 : -1) * BALL_MIN_SPEED;

            if (abs(ball.speedY) > BALL_MAX_SPEED) ball.speedY = ((ball.speedY > 0) ? 1 : -1) * BALL_MAX_SPEED;
            if (abs(ball.speedX) > BALL_MAX_SPEED) ball.speedX = ((ball.speedX > 0) ? 1 : -1) * BALL_MAX_SPEED;
        }
        //  impact with bottom
        else if (ball.rect.y + ball.rect.h >= SCREEN_HEIGHT - BORDER)
        {
            ball.rect.y = SCREEN_HEIGHT - ball.rect.h - BORDER;
            ball.speedY = -ball.speedY + (rand() % 3 - 1);
            ball.speedX =  ball.speedX + (rand() % 3 - 1);

            if (abs(ball.speedY) < BALL_MIN_SPEED) ball.speedY = ((ball.speedY > 0) ? 1 : -1) * BALL_MIN_SPEED;
            if (abs(ball.speedX) < BALL_MIN_SPEED) ball.speedX = ((ball.speedX > 0) ? 1 : -1) * BALL_MIN_SPEED;

            if (abs(ball.speedY) > BALL_MAX_SPEED) ball.speedY = ((ball.speedY > 0) ? 1 : -1) * BALL_MAX_SPEED;
            if (abs(ball.speedX) > BALL_MAX_SPEED) ball.speedX = ((ball.speedX > 0) ? 1 : -1) * BALL_MAX_SPEED;
        }

        // impact with left paddle
        if (ball.speedX < 0 &&
                ball.rect.x < leftPaddle.rect.x + leftPaddle.rect.w &&
                ball.rect.x + ball.rect.w > leftPaddle.rect.x + leftPaddle.rect.w &&
                ball.rect.y + ball.rect.h > leftPaddle.rect.y &&
                ball.rect.y < leftPaddle.rect.y + leftPaddle.rect.h)
        {
            ball.speedX = -ball.speedX + (rand() % 3 - 1);
            ball.speedY =  ball.speedY + (rand() % 3 - 1);
            if (ball.speedX <= 0) ball.speedX = BALL_MIN_SPEED;
            if (abs(ball.speedY) < BALL_MIN_SPEED) ball.speedY = ((ball.speedY > 0) ? 1 : -1) * BALL_MIN_SPEED;
            if (abs(ball.speedY) > BALL_MAX_SPEED) ball.speedY = ((ball.speedY > 0) ? 1 : -1) * BALL_MAX_SPEED;

            leftHits++;
            currentRally++;
            if (currentRally > longestRally) longestRally = currentRally;

            // Play hit sound
            if (gHitSound != nullptr)
            {
                Mix_Volume(-1, 120);
                Mix_PlayChannel(-1, gHitSound, 0);
            }
        }

        // impact with right paddle
        if (ball.speedX > 0 &&
                ball.rect.x + ball.rect.w > rightPaddle.rect.x &&
                ball.rect.x < rightPaddle.rect.x &&
                ball.rect.y + ball.rect.h > rightPaddle.rect.y &&
                ball.rect.y < rightPaddle.rect.y + rightPaddle.rect.h)
        {
            ball.speedX = -ball.speedX + (rand() % 3 - 1);
            ball.speedY =  ball.speedY + (rand() % 3 - 1);
            if (ball.speedX <= 0) ball.speedX = -BALL_MIN_SPEED;
            if (abs(ball.speedY) < BALL_MIN_SPEED) ball.speedY = ((ball.speedY > 0) ? 1 : -1) * BALL_MIN_SPEED;
            if (abs(ball.speedY) > BALL_MAX_SPEED) ball.speedY = ((ball.speedY > 0) ? 1 : -1) * BALL_MAX_SPEED;

            rightHits++;
            currentRally++;
            if (currentRally > longestRally) longestRally = currentRally;

            // Play hit sound
            if (gHitSound != nullptr)
            {
                Mix_Volume(-1, 120);
                Mix_PlayChannel(-1, gHitSound, 0);
            }
        }

        // Ball out of bounds (scoring)
        if (ball.rect.x < 0)
        {
            // Right player scores
            rightPaddle.score++;
            currentRally=0;

            if ((rightPaddle.score == maxScore - 1 && leftPaddle.score < maxScore - 1) ||
                    (rightPaddle.score >= maxScore - 1 && rightPaddle.score - leftPaddle.score == 1))
            {
                // MATCH POINT PHẢI - DỪNG GAME 1.5s
                matchPointPlayer = 2;
                isMatchPointPause = true;
                matchPointPauseTime = SDL_GetTicks();
                isMatchPoint = true;
            }
            else
            {
                if(isMatchPoint && (matchPointPlayer == 2 || matchPointPlayer == 1) && leftPaddle.score == rightPaddle.score)
                {
                    isMatchPoint = false;
                    matchPointPlayer = 0;
                }
                if(rightPaddle.score > leftPaddle.score) resetBall(1);
                else if(rightPaddle.score < leftPaddle.score) resetBall(-1);
                else resetBall((rand() % 2 == 0) ? 1 : -1);
            }
        }
        else if (ball.rect.x > SCREEN_WIDTH)
        {
            // Left player scores
            leftPaddle.score++;
            currentRally=0;

            if ((leftPaddle.score == maxScore - 1 && rightPaddle.score < maxScore - 1) ||
                    (leftPaddle.score >= maxScore - 1 && leftPaddle.score - rightPaddle.score == 1))
            {
                // MATCH POINT PHẢI - DỪNG GAME 1.5s
                matchPointPlayer = 1;
                isMatchPointPause = true;
                matchPointPauseTime = SDL_GetTicks();
                isMatchPoint = true;
            }
            else
            {
                if(isMatchPoint && (matchPointPlayer == 1 || matchPointPlayer == 2) && leftPaddle.score == rightPaddle.score)
                {
                    isMatchPoint = false;
                    matchPointPlayer = 0;
                }
                if(rightPaddle.score > leftPaddle.score) resetBall(1);
                else if(rightPaddle.score < leftPaddle.score) resetBall(-1);
                else resetBall((rand() % 2 == 0) ? 1 : -1);
            }
        }


    }
}


