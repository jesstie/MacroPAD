// Function to display the game mode menu
void showMenu() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setTextWrap(false);
  display.setCursor(23, 13);
  display.print("PRESS TO PLAY!");
  display.display();

  // Switch game modes based on button presses
  if (digitalRead(pinB) == LOW) {
    delay(300);
    gameMode = GAME;
  }
}

// Function to play the game
void playGame() {
  // Player 1 Paddle movement
  if (gameMode == GAME && digitalRead(pinB) == LOW && p1Y > 0) p1Y -= paddleSpeed;
  if (gameMode == GAME && digitalRead(pinE) == LOW && p1Y + paddleHeight < SCREEN_HEIGHT) p1Y += paddleSpeed;

  // Player 2 (CPU or Player 2) Paddle movement
  moveCPU(); // Move the CPU paddle

  // Ball movement
  ballX += vx;
  ballY += vy;

  // Ball bounce off top and bottom walls
  if (ballY <= 0 || ballY + ballSize >= SCREEN_HEIGHT) vy *= -1;

  // Ball bounce off paddles
  if (ballX <= paddleWidth && ballY + ballSize >= p1Y && ballY <= p1Y + paddleHeight) {
    vx *= -1; // Reverse ball direction
    ballX = paddleWidth + 1; // Avoid ball getting stuck in the paddle
    //playPaddleHitTone(); // Play sound on paddle hit
  }
  if (ballX + ballSize >= SCREEN_WIDTH - paddleWidth && ballY + ballSize >= p2Y && ballY <= p2Y + paddleHeight) {
    vx *= -1; // Reverse ball direction
    ballX = SCREEN_WIDTH - paddleWidth - ballSize - 1;
    //playPaddleHitTone(); // Play sound on paddle hit
  }

  // Reset game if ball goes out of bounds
  if (ballX < 0 || ballX > SCREEN_WIDTH) {
    gameOver(); // Show game over screen
    return;
  }

  // Draw the game (paddles, ball)
  display.clearDisplay();
  display.fillRect(0, p1Y, paddleWidth, paddleHeight, SSD1306_WHITE); // Player 1 paddle
  display.fillRect(SCREEN_WIDTH - paddleWidth, p2Y, paddleWidth, paddleHeight, SSD1306_WHITE); // Player 2 paddle
  display.fillRect((int)ballX, (int)ballY, ballSize, ballSize, SSD1306_WHITE); // Ball
  display.display(); // Update the display

  delay(10); // Small delay for smoother gameplay
}

// Function to reset the game
void resetGame() {
  p1Y = SCREEN_HEIGHT / 2 - paddleHeight / 2; // Reset Player 1 paddle position
  p2Y = SCREEN_HEIGHT / 2 - paddleHeight / 2; // Reset Player 2 paddle position
  ballX = SCREEN_WIDTH / 2; // Reset ball position
  ballY = SCREEN_HEIGHT / 2;
  delay(1000); // Wait before starting a new game
}


// Function to control CPU paddle movement
void moveCPU() {
  if (ballY < p2Y + paddleHeight / 2 && p2Y > 0) {
    p2Y--; // Move CPU paddle up
  } else if (ballY > p2Y + paddleHeight / 2 && p2Y + paddleHeight < SCREEN_HEIGHT) {
    p2Y++; // Move CPU paddle down
  }

  // Hardcore mode: CPU moves faster
  // if (gameMode == HARDCORE) {
  //   if (random(2) == 0) {
  //     p2Y += (ballY > p2Y + paddleHeight / 2) ? 1 : -1; // Move CPU paddle more aggressively
  //   }
  // }
}

void gameOver() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(35, 13);
  display.print("GAME OVER!"); // Display "GAME OVER!"
  display.display();  // Update the display

  delay(1500);  
  gameMode = MENU; 
  resetGame();
}