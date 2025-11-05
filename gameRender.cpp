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

    if (isMatchPointPause)
    {
        bool flashOn = (SDL_GetTicks() / 150) % 2 == 0;
        if (flashOn)
        {
            SDL_Color flashColor = (matchPointPlayer == 1) ?
                                   SDL_Color{0, 191, 255, 255} :
                                   SDL_Color{50, 205, 50, 255};
            string MatchPointplayerName = (matchPointPlayer == 1) ? player1Name :
                                          (gameMode == PLAYER_VS_AI ? "BOT" : player2Name);
            // TEXT TO, ĐẬM, GIỮA MÀN HÌNH
            renderText(" MATCH POINT for " + MatchPointplayerName,
                       SCREEN_WIDTH / 2 - 180,
                       SCREEN_HEIGHT / 2 - 80,
                       flashColor, gFont36);
            SDL_SetRenderDrawColor(gRenderer, flashColor.r, flashColor.g, flashColor.b, 200);
            int offset = (SDL_GetTicks() / 100) % 4 - 2;
            SDL_Rect glow =
            {
                SCREEN_WIDTH / 2 - 230 + offset,
                SCREEN_HEIGHT / 2 - 90 + offset,
                460, 100
            };
            SDL_RenderDrawRect(gRenderer, &glow);
            SDL_RenderDrawRect(gRenderer, &glow);
            SDL_RenderDrawRect(gRenderer, &glow);
        }
    }
    if (isMatchPoint){
        // HIỆU ỨNG Ở ĐIỂM SỐ CỦA PLAYER CÓ MATCH POINT
        SDL_Color flashColor = (matchPointPlayer == 1) ?
                           SDL_Color{0, 191, 255, 255} :
                           SDL_Color{50, 205, 50, 255};
        if (matchPointPlayer == 1)  // Player trái
        {
            // Vòng tròn phát sáng quanh điểm số trái
            int pulseSize = 40 + (SDL_GetTicks() / 50) % 20;  // Nhấp nháy từ 40-60
            SDL_SetRenderDrawColor(gRenderer, flashColor.r, flashColor.g, flashColor.b, 150);
            for (int i = 0; i < 3; i++)
            {
                SDL_Rect scoreGlow =
                {
                    SCREEN_WIDTH / 4 - pulseSize / 2 + i * 2 + 10,
                    50 - pulseSize / 2 + i * 2 - 10,
                    pulseSize - i * 4,
                    pulseSize - i * 4
                };
                SDL_RenderDrawRect(gRenderer, &scoreGlow);
            }
        }
        else if (matchPointPlayer == 2)  // Player phải
        {
            // Vòng tròn phát sáng quanh điểm số phải
            int pulseSize = 40 + (SDL_GetTicks() / 50) % 20;  // Nhấp nháy từ 40-60
            SDL_SetRenderDrawColor(gRenderer, flashColor.r, flashColor.g, flashColor.b, 150);
            for (int i = 0; i < 3; i++)
            {
                SDL_Rect scoreGlow =
                {
                    3 * SCREEN_WIDTH / 4 - pulseSize / 2 + i * 2 + 10,
                    50 - pulseSize / 2 + i * 2 - 10,
                    pulseSize - i * 4,
                    pulseSize - i * 4
                };
                SDL_RenderDrawRect(gRenderer, &scoreGlow);
            }
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

bool PongGame::showNameEntryScreen()
{
    SDL_Event e;
    string input = "";
    int currentPlayer = 1;  // 1 = P1, 2 = P2
    bool done = false;
    string errorMsg = "";

    SDL_StartTextInput();

    while (!done && !quit)
    {
        while (SDL_PollEvent(&e) != 0)
        {
            if (e.type == SDL_QUIT)
            {
                quit = true;
                break;
            }
            if (e.type == SDL_KEYDOWN)
            {
                if (e.key.keysym.sym == SDLK_RETURN && !input.empty())
                {
                    if (currentPlayer == 1)
                    {
                        player1Name = input;
                        if (gameMode == PLAYER_VS_AI)
                        {
                            player2Name = "BOT";
                            done = true;
                        }
                        else
                        {
                            currentPlayer = 2;
                            input = "";
                        }
                    }
                    else
                    {
                        if (input == player1Name)
                        {
                            errorMsg = "Ten khong duoc trung voi P1\n Vui long nhap ten khac.";
                            input = "";  // Xóa để nhập lại
                        }
                        else
                        {
                            player2Name = input;
                            done = true;
                        }
                    }
                }
                else if (e.key.keysym.sym == SDLK_BACKSPACE && !input.empty())
                    input.pop_back();
            }
            else if (e.type == SDL_TEXTINPUT && input.length() < 12)
                input += e.text.text;
        }

        SDL_SetRenderDrawColor(gRenderer, 0, 0, 40, 255);
        SDL_RenderClear(gRenderer);
        if (gMenuBackTexture) SDL_RenderCopy(gRenderer, gMenuBackTexture, nullptr, nullptr);

        renderText("NHAP TEN", SCREEN_WIDTH / 2 - 100, 100, {255, 100, 255, 255}, gFont36);

        string p1 = "P1: " + (currentPlayer == 1 ? input + "_" : player1Name);
        string p2 = (gameMode == PLAYER_VS_AI) ? "P2: BOT" :
                    "P2: " + (currentPlayer == 2 ? input + "_" : player2Name);

        renderText(p1, SCREEN_WIDTH / 2 - 120, 200, {255, 255, 255, 255}, gFont24);
        renderText(p2, SCREEN_WIDTH / 2 - 120, 250, {255, 255, 255, 255}, gFont24);
        if (!errorMsg.empty())
        {
            renderText(errorMsg, SCREEN_WIDTH / 2 - 180, 320, {255, 100, 100, 255}, gFont24);
            // Tự động xóa sau 2 giây
            static Uint32 errorTime = 0;
            if (errorTime == 0) errorTime = SDL_GetTicks();
            if (SDL_GetTicks() - errorTime > 2000)
            {
                errorMsg = "";
                errorTime = 0;
            }
        }
        else
        {
            renderText("ENTER de xac nhan", SCREEN_WIDTH / 2 - 130, 320, {255, 255, 0, 255}, gFont24);
        }

        SDL_RenderPresent(gRenderer);
    }

    SDL_StopTextInput();
    return !quit && done;
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

        SDL_SetRenderDrawColor(gRenderer, 0, 0, 40, 255);
        SDL_RenderClear(gRenderer);
        if (gMenuBackTexture != nullptr)
            SDL_RenderCopy(gRenderer, gMenuBackTexture, nullptr, nullptr);

        SDL_Color white = {255, 255, 255, 255};
        SDL_Color yellow = {255, 255, 0, 255};

        renderText("CAI DAT TRAN DAU", SCREEN_WIDTH / 2 - 140, 80, {255, 100, 255, 255}, gFont36);

        renderText("NHAP SO DIEM THANG (3-20):", SCREEN_WIDTH / 2 - 180, 160, white, gFont24);
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
    bool modeSelected = false;
    SDL_Event e;

    while (!modeSelected && !quit)
    {
        while (SDL_PollEvent(&e) != 0)
        {
            if (e.type == SDL_QUIT)
                quit = true;
            else if (e.type == SDL_KEYDOWN)
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

    // Dừng tất cả âm thanh
    Mix_HaltChannel(-1);

    // Tính thời gian chơi
    Uint32 gameTime = (SDL_GetTicks() - gameStartTime) / 1000;
    int minutes = gameTime / 60;
    int seconds = gameTime % 60;

    // Xác định người thắng
    bool isP1Win = (leftPaddle.score > rightPaddle.score);
    string p2Name = (gameMode == PLAYER_VS_AI ? "BOT" : player2Name);

    string winner;
    SDL_Color winColor;

    if (gameMode == PLAYER_VS_AI)
    {
        winner = (isP1Win) ? "VICTORY!" : "DEFEAT!";
        winColor = (isP1Win) ? SDL_Color{0, 255, 0, 255} :
                   SDL_Color{255, 0, 0, 255};
    }
    else
    {
        winner = (isP1Win) ? player1Name + " WINS!" : player2Name + " WINS!";
        winColor = (isP1Win) ? SDL_Color{0, 191, 255, 255} :
                   SDL_Color{50, 205, 50, 255};
    }

    SDL_Event e;

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

        //  RENDER MÀN HÌNH KẾT QUẢ
        SDL_SetRenderDrawColor(gRenderer, 0, 0, 40, 255);
        SDL_RenderClear(gRenderer);
        if (gMenuBackTexture != nullptr)
            SDL_RenderCopy(gRenderer, gMenuBackTexture, nullptr, nullptr);
            // Bảng thống kê
        int y = 60;
        renderText("=== MATCH SUMMARY ===", SCREEN_WIDTH / 2 - 150, y, {255, 215, 0, 255}, gFont36);
        y += 95;

        renderText(winner, SCREEN_WIDTH / 2 - 60, y, winColor, gFont36);

        y += 60;
        // Tỉ số
        string scoreText = to_string(leftPaddle.score) + " - " + to_string(rightPaddle.score);
        renderText(scoreText, SCREEN_WIDTH / 2 - 50, y, {255, 215, 0, 255}, gFont36);


        y += 60;
        renderText("Time: " + to_string(minutes) + "m" + to_string(seconds) + "s",
                   SCREEN_WIDTH / 2 - 80, y, {255, 215, 0, 255}, gFont24);
        y += 35;

        renderText(player1Name + ": " + to_string(leftHits) + " hits",
                   SCREEN_WIDTH / 2 - 120, y, {0, 191, 255, 255}, gFont24);

        renderText(p2Name + ": " + to_string(rightHits) + " hits",
                   SCREEN_WIDTH / 2 + 30, y, {50, 205, 50, 255}, gFont24);
        y += 35;

        renderText("Longest Rally: " + to_string(longestRally) + " hits",
                   SCREEN_WIDTH / 2 - 120, y, {255, 215, 0, 255}, gFont24);
        y += 50;

        // Nút điều khiển
        renderText("R - Restart Game", SCREEN_WIDTH / 2 - 110, y, {255, 215, 0, 255}, gFont24);
        y += 35;
        renderText("M - Back to Menu", SCREEN_WIDTH / 2 - 110, y, {255, 215, 0, 255}, gFont24);
        y += 35;
        renderText("ESC - Quit", SCREEN_WIDTH / 2 - 80, y, {255, 215, 0, 255}, gFont24);

        SDL_RenderPresent(gRenderer);
    }

    //  XỬ LÝ SAU KHI CHỌN
    if (restartGame)
    {
        leftPaddle.score = 0;
        rightPaddle.score = 0;
        leftHits = rightHits = 0;
        currentRally = longestRally = 0;
        gameStartTime = SDL_GetTicks();  // Reset thời gian
        resetBall((rand() % 2 == 0) ? 1 : -1);
        return true;
    }

    if (backToMenu)
    {
        leftPaddle.score = 0;
        rightPaddle.score = 0;
        leftHits = rightHits = 0;
        currentRally = longestRally = 0;
        return showMainMenu();
    }

    return false;  // Thoát game
}
