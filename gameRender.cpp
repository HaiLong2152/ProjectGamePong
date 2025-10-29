#include "gamePong.h"

void PongGame::renderGame()
{
    // Clear screen
    SDL_SetRenderDrawColor(gRenderer, 0, 0, 40, 255);
    SDL_RenderClear(gRenderer);

    // Render background
    if (gBackgroundTexture != nullptr)
        SDL_RenderCopy(gRenderer, gBackgroundTexture, nullptr, nullptr);

    // Render paddles
    if (gLeftPaddleTexture != nullptr)
        SDL_RenderCopy(gRenderer, gLeftPaddleTexture, nullptr, &leftPaddle.rect);

    if (gRightPaddleTexture != nullptr)
        SDL_RenderCopy(gRenderer, gRightPaddleTexture, nullptr, &rightPaddle.rect);

    // Render ball
    for (size_t i = 0; i < ballTrail.size(); ++i)
    {
        // Độ trong suốt: vệt cũ càng mờ
        Uint8 alpha = 150 * (TRAIL_LENGTH - i) / (TRAIL_LENGTH + 2);
        SDL_SetTextureAlphaMod(gBallTexture, alpha);
        SDL_RenderCopy(gRenderer, gBallTexture, nullptr, &ballTrail[i]);
    }

    SDL_SetTextureAlphaMod(gBallTexture, 255);
    if (gBallTexture != nullptr)
        SDL_RenderCopy(gRenderer, gBallTexture, nullptr, &ball.rect);
    // Thong bao MatchPoint

if (isMatchPoint)
{
    // Nháy nhanh 0.15s – tăng kịch tính
    bool flashOn = (SDL_GetTicks() / 150) % 2 == 0;

    if (flashOn)
    {
        // MÀU MỚI: XANH BIỂN (P1) / XANH LÁ (P2) – SIÊU ĐẬM, SIÊU SÁNG
        SDL_Color flashColor = (matchPointPlayer == 1) ?
            SDL_Color{0, 191, 255, 255} :    // XANH BIỂN SÁNG (#00BFFF) – P1
            SDL_Color{50, 205, 50, 255};     // XANH LÁ SÁNG (#32CD32) – P2

        string MatchPointplayerName = (matchPointPlayer == 1) ? player1Name :
                           (gameMode == PLAYER_VS_AI ? "BOT" : player2Name);

        // TEXT TO, ĐẬM, GIỮA MÀN HÌNH
        renderText(to_string(abs(leftPaddle.score-rightPaddle.score)) + " MATCH POINT for player " + MatchPointplayerName,
                   SCREEN_WIDTH / 2 - 180,
                   SCREEN_HEIGHT / 2 - 80,
                   flashColor, gFont36);

        // === HIỆU ỨNG VIỀN SÁNG + RUNG NHẸ ===
        SDL_SetRenderDrawColor(gRenderer, flashColor.r, flashColor.g, flashColor.b, 200);
        int offset = (SDL_GetTicks() / 100) % 4 - 2;  // Rung nhẹ
        SDL_Rect glow = {
            SCREEN_WIDTH / 2 - 230 + offset,
            SCREEN_HEIGHT / 2 - 90 + offset,
            460, 100
        };
        SDL_RenderDrawRect(gRenderer, &glow);
        SDL_RenderDrawRect(gRenderer, &glow);  // Viền đậm
        SDL_RenderDrawRect(gRenderer, &glow);  // 3 lớp = siêu sáng
    }
}

    // Render scores
    SDL_Color textColorBlack = {0, 0, 0, 0};
    string leftScoreText = to_string(leftPaddle.score);
    renderText(leftScoreText, SCREEN_WIDTH / 4, 25, {0, 0, 255, 255}, gFont36);

    string rightScoreText = to_string(rightPaddle.score);
    renderText(rightScoreText, 3 * SCREEN_WIDTH / 4, 25, {0, 255, 0, 255}, gFont36);

    // Render game mode
    string modeText = (gameMode == PLAYER_VS_PLAYER) ? "Player vs Player" : "Player vs BOT";
    renderText(modeText, SCREEN_WIDTH / 2 - 80, SCREEN_HEIGHT - 20, textColorBlack, gFont24);

    // Controls tutorial
    if (gameMode == PLAYER_VS_PLAYER)
    {
        string leftControl  = player1Name + ": W/S";
        string rightControl = player2Name + ": UP/DOWN";

        renderText(leftControl,  70, SCREEN_HEIGHT - 20, textColorBlack, gFont24);
        renderText(rightControl, SCREEN_WIDTH - 250, SCREEN_HEIGHT - 20, textColorBlack, gFont24);
    }
    else
    {
        string leftControl = player1Name + ": W/S";
        renderText(leftControl, 70, SCREEN_HEIGHT - 20, textColorBlack, gFont24);
    }

    // Render countdown effect
    if (isBallWaiting)
    {
        Uint32 timeLeft = ballResetTime - SDL_GetTicks();
        int countdownNumber = timeLeft / 1000 + 1;  // Đếm từ 3, 2, 1
        if (countdownNumber > 0 && countdownNumber <= 3)
        {
            string countdownText = to_string(countdownNumber);
            // Tính toán scale dựa trên thời gian để tạo hiệu ứng phóng to
            float scale = 1.0f + 0.5f * (1.0f - (timeLeft % 1000) / 1000.0f);  // Phóng to từ 1x đến 1.5x

            int alpha = (timeLeft % 1000 < 500) ? 255 : 0;  // Bật 0.5s, tắt 0.5s mỗi giây

            // Render hình vuông nền bán trong suốt
            SDL_Surface* textSurface = TTF_RenderText_Solid(gFont36, countdownText.c_str(), {255, 255, 0, (Uint8)alpha});
            if (textSurface != nullptr)
            {
                int width = textSurface->w * scale;
                int height = textSurface->h * scale;
                SDL_Rect bgQuad = { SCREEN_WIDTH / 2 - width / 2 - 2, SCREEN_HEIGHT / 2 - height / 2 - 32, width + 4, height + 4 }; // Nền vừa phải, y dịch lên 30px
                SDL_SetRenderDrawBlendMode(gRenderer, SDL_BLENDMODE_BLEND);
                SDL_SetRenderDrawColor(gRenderer, 100, 100, 100, 100);
                SDL_RenderFillRect(gRenderer, &bgQuad);
                SDL_SetRenderDrawBlendMode(gRenderer, SDL_BLENDMODE_NONE);
                SDL_FreeSurface(textSurface);
            }

            // Render text chính với màu vàng sáng
            SDL_Color textColor = {255, 255, 0, (Uint8)alpha};  // Màu vàng sáng
            textSurface = TTF_RenderText_Solid(gFont36, countdownText.c_str(), textColor);
            if (textSurface != nullptr)
            {
                SDL_Texture* textTexture = SDL_CreateTextureFromSurface(gRenderer, textSurface);
                if (textTexture != nullptr)
                {
                    int width = textSurface->w * scale;
                    int height = textSurface->h * scale;
                    SDL_Rect renderQuad = { SCREEN_WIDTH / 2 - width / 2, SCREEN_HEIGHT / 2 - height / 2 - 30, width, height }; // Trung tâm sân, y dịch lên 30px
                    SDL_RenderCopy(gRenderer, textTexture, nullptr, &renderQuad);
                    SDL_DestroyTexture(textTexture);
                }
                SDL_FreeSurface(textSurface);
            }
        }
    }

    // Render pause text
    if (isPaused)
    {
        // Render nền bán trong suốt cho "Paused"
        SDL_Surface* pauseSurface = TTF_RenderText_Solid(gFont36, "Paused", {255, 255, 255, 255});
        if (pauseSurface != nullptr)
        {
            int width = pauseSurface->w;
            int height = pauseSurface->h;
            SDL_Rect bgQuad = { SCREEN_WIDTH / 2 - width / 2 - 2, SCREEN_HEIGHT / 2 - height / 2 - 2, width + 4, height + 4 }; // Nền vừa phải
            SDL_SetRenderDrawBlendMode(gRenderer, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(gRenderer, 100, 100, 100, 120);
            SDL_RenderFillRect(gRenderer, &bgQuad);
            SDL_SetRenderDrawBlendMode(gRenderer, SDL_BLENDMODE_NONE);
            SDL_FreeSurface(pauseSurface);
        }

        // Render text "Paused"
        SDL_Color pauseColor = {255, 255, 255, 255};
        renderText("Paused", SCREEN_WIDTH / 2 - 50, SCREEN_HEIGHT / 2 - 20, pauseColor, gFont36);
    }

    // Update screen
    SDL_RenderPresent(gRenderer);
}

// gameRender.cpp
bool PongGame::showNameEntryScreen()
{
    SDL_Event e;
    currentInput = "";  // Reset input

    while (isEnteringName && !quit)
    {
        while (SDL_PollEvent(&e) != 0)
        {
            if (e.type == SDL_QUIT)
            {
                quit = true;
                isEnteringName = false;
                return false;
            }

            else if (e.type == SDL_KEYDOWN)
            {
                if (e.key.keysym.sym == SDLK_RETURN)
                {
                    // Xác nhận tên
                    if (isPlayer1Turn)
                    {
                        player1Name = currentInput.empty() ? "Player 1" : currentInput;
                        isPlayer1Turn = false;
                        currentInput = "";
                    }
                    else
                    {
                        player2Name = currentInput.empty() ? "Player 2" : currentInput;
                        isEnteringName = false;  // Kết thúc nhập tên
                    }
                }
                else if (e.key.keysym.sym == SDLK_BACKSPACE && !currentInput.empty())
                {
                    currentInput.pop_back();
                }
                else if (e.key.keysym.sym == SDLK_ESCAPE)
                {
                    quit = true;
                }
            }
            else if (e.type == SDL_TEXTINPUT)
            {
                // Chỉ nhận ký tự in được (a-z, 0-9, space...)
                if (currentInput.length() < 12)  // Giới hạn 12 ký tự
                    currentInput += e.text.text;
            }
        }
        SDL_SetRenderDrawColor(gRenderer, 0, 0, 40, 255);
        SDL_RenderClear(gRenderer);

        if (gMenuBackTexture != nullptr)
            SDL_RenderCopy(gRenderer, gMenuBackTexture, nullptr, nullptr);

        SDL_Color white = {255, 255, 255, 255};
        SDL_Color yellow = {255, 255, 0, 255};

        renderText("NHAP TEN NGUOI CHOI", SCREEN_WIDTH / 2 - 180, 100, yellow, gFont36);

        string prompt = isPlayer1Turn ? "Nguoi choi 1:" : "Nguoi choi 2:";
        renderText(prompt, SCREEN_WIDTH / 2 - 150, 200, white, gFont36);

        // Hiển thị ô nhập + con trỏ nhấp nháy
        string displayText = currentInput + (SDL_GetTicks() % 1000 < 500 ? "_" : "");
        renderText(displayText, SCREEN_WIDTH / 2 - 150, 260, white, gFont36);

        renderText("Nhan ENTER de xac nhan", SCREEN_WIDTH / 2 - 160, 350, white, gFont24);
        renderText("Nhan ESC de thoat", SCREEN_WIDTH / 2 - 120, 400, white, gFont24);

        SDL_RenderPresent(gRenderer);
        SDL_Delay(16);
    }

    return !quit;
}

bool PongGame::showConfigScreen()
{
    std::string scoreInput = "";
    SDL_Event e;
    SDL_StartTextInput();

    bool configDone = false;
    while (!configDone && !quit)
    {
        while (SDL_PollEvent(&e) != 0)
        {
            if (e.type == SDL_QUIT)
                quit = true;

            else if (e.type == SDL_KEYDOWN)
            {
                if (e.key.keysym.sym == SDLK_RETURN && !scoreInput.empty())
                {
                    maxScore = std::stoi(scoreInput);
                    if (maxScore < 3) maxScore = 3;
                    if (maxScore > 50) maxScore = 50;
                    configDone = true;
                }
                else if (e.key.keysym.sym == SDLK_BACKSPACE && !scoreInput.empty())
                {
                    scoreInput.pop_back();
                }
                else if (e.key.keysym.sym == SDLK_ESCAPE)
                    quit = true;
            }
            else if (e.type == SDL_TEXTINPUT)
            {
                if (scoreInput.length() < 2 && isdigit(e.text.text[0]))
                    scoreInput += e.text.text;
            }
        }

        // === RENDER MÀN HÌNH CẤU HÌNH ===
        SDL_SetRenderDrawColor(gRenderer, 0, 0, 40, 255);
        SDL_RenderClear(gRenderer);
        if (gMenuBackTexture != nullptr)
            SDL_RenderCopy(gRenderer, gMenuBackTexture, nullptr, nullptr);

        SDL_Color white = {255, 255, 255, 255};
        SDL_Color yellow = {255, 255, 0, 255};

        renderText("CAI DAT TRAN DAU", SCREEN_WIDTH / 2 - 140, 80, {255, 100, 255, 255}, gFont36);

        renderText("NHAP SO DIEM THANG (5-20):", SCREEN_WIDTH / 2 - 180, 160, white, gFont24);
        string display = scoreInput + (SDL_GetTicks() % 1000 < 500 ? "_" : "");
        renderText(display, SCREEN_WIDTH / 2 - 40, 200, yellow, gFont36);

        renderText("QUY TAC:", SCREEN_WIDTH / 2 - 80, 260, white, gFont24);
        renderText("- Cach 2 diem moi thang", SCREEN_WIDTH / 2 - 140, 290, white, gFont24);

        renderText("HUONG DAN:", SCREEN_WIDTH / 2 - 90, 370, white, gFont24);
        string p1 = player1Name + ": W/S";
        string p2 = (gameMode == PLAYER_VS_AI) ? "BOT: Tu dong" : player2Name + ": UP/DOWN";
        renderText(p1, SCREEN_WIDTH / 2 - 100, 400, white, gFont24);
        renderText(p2, SCREEN_WIDTH / 2 - 100, 425, white, gFont24);
        renderText("P: Pause", SCREEN_WIDTH / 2 - 100, 450, white, gFont24);

        renderText("Nhan ENTER de bat dau", SCREEN_WIDTH / 2 - 130, 500, yellow, gFont24);

        SDL_RenderPresent(gRenderer);
    }

    SDL_StopTextInput();
    return !quit && configDone;
}

bool PongGame::showMainMenu()
{
    bool modeSelected = false; // Chosen a game mode
    SDL_Event e;

    while (!modeSelected && !quit)
    {
        while (SDL_PollEvent(&e) != 0) // Window close button
        {
            if (e.type == SDL_QUIT)
                quit = true;
            else if (e.type == SDL_KEYDOWN) // Keyboard input
            {
                switch (e.key.keysym.sym)
                {
                case SDLK_1:
                    gameMode = PLAYER_VS_PLAYER;
                    modeSelected = true;
                    break;
                case SDLK_2:
                    gameMode = PLAYER_VS_AI;
                    modeSelected = true;
                    break;
                case SDLK_ESCAPE:
                    quit = true;
                    break;
                }
            }
        }

        // Clear screen
        SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 255);
        SDL_RenderClear(gRenderer);

        if (gMenuBackTexture != nullptr)
            SDL_RenderCopy(gRenderer, gMenuBackTexture, nullptr, nullptr);

        // Render menu text
        SDL_Color textColor = {255, 255, 255, 255};
        renderText("PONG GAME", SCREEN_WIDTH / 2 - 100, 150, {255, 100, 255, 255}, gFont36);
        renderText("Press 1. Player vs Player", SCREEN_WIDTH / 2 - 120, 250, textColor, gFont36);
        renderText("Press 2. Player vs BOT", SCREEN_WIDTH / 2 - 120, 300, textColor, gFont36);
        renderText("Press ESC to quit", SCREEN_WIDTH / 2 - 120, 400, textColor, gFont36);
        renderText("Press P to pause", SCREEN_WIDTH / 2 - 120, 500, textColor, gFont36);

        SDL_RenderPresent(gRenderer);
    }
    return modeSelected;
}

bool PongGame::showGameOverScreen()
{
    bool restartGame = false;
    bool backToMenu = false;
    SDL_Event e;

    Mix_HaltChannel(-1); // Dừng tất cả âm thanh

    while (!restartGame && !backToMenu && !quit)
    {
        while (SDL_PollEvent(&e) != 0)
        {
            if (e.type == SDL_QUIT)
                quit = true;
            else if (e.type == SDL_KEYDOWN)
            {
                switch (e.key.keysym.sym)
                {
                case SDLK_r:
                    restartGame = true;
                    break;
                case SDLK_m:
                    backToMenu = true;
                    break;
                case SDLK_ESCAPE:
                    quit = true;
                    break;
                }
            }
        }

        // Clear screen
        SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 255);
        SDL_RenderClear(gRenderer);

        if (gMenuBackTexture != nullptr)
            SDL_RenderCopy(gRenderer, gMenuBackTexture, nullptr, nullptr);

        // Render game over text
        SDL_Color textColor = {255, 255, 255, 255};

        if(gameMode == PLAYER_VS_PLAYER)
        {
            if(leftPaddle.score >= maxScore)
                renderText(player1Name + " Wins!", SCREEN_WIDTH / 2 - 40, 150, {0, 0, 255, 255}, gFont36);
            else
                renderText(player2Name + " Wins!", SCREEN_WIDTH / 2 - 40, 150, {0, 255, 0, 255}, gFont36);
        }
        else
        {
            if(leftPaddle.score >= maxScore)
                renderText("VICTORY!", SCREEN_WIDTH / 2 - 40, 150, {0, 0, 255, 255}, gFont36);
            else
                renderText("DEFEAT!", SCREEN_WIDTH / 2 - 40, 150, {0, 255, 0, 255}, gFont36);
        }

        string scoreText = to_string(leftPaddle.score) + " - " + to_string(rightPaddle.score);
        renderText(scoreText.c_str(), SCREEN_WIDTH / 2 - 30, 200, textColor, gFont36);

        renderText("Press R - Restart Game", SCREEN_WIDTH / 2 - 120, 250, textColor, gFont36);
        renderText("Press M - Back to Menu", SCREEN_WIDTH / 2 - 120, 300, textColor, gFont36);
        renderText("Press ESC - Quit", SCREEN_WIDTH / 2 - 120, 350, textColor, gFont36);

        SDL_RenderPresent(gRenderer);
    }

    if (restartGame)
    {
        // Reset scores
        leftPaddle.score = 0;
        rightPaddle.score = 0;

        resetBall((rand() % 2 == 0) ? 1 : -1);
        return true;
    }

    if (backToMenu)
    {
        // Reset scores
        leftPaddle.score = 0;
        rightPaddle.score = 0;
        return showMainMenu();
    }
    //if player chose to quit
    return false;
}
