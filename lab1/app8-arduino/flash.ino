void myDelay(unsigned int* delay_ms) {
  // software delays are more strictly optimized out by arduino's compiler
  // this uses a system call instead
  delay(*delay_ms);
}

void flash(unsigned int* delay_ms, char letter) {
  // Sets the draw cursor to the top-left corner
  tft.setCursor(0, 0);

	tft.print(letter);
	myDelay(delay_ms);

  // Resets the screen to all black
	tft.fillScreen(BLACK);

	myDelay(delay_ms);
}