final color COLOR_BACKGROUND = #121212;
final color COLOR_WHITE = #FFFFFF;

final float TEXT_ALPHA_HIGH_EMPHASIS = 221.85;
final float TEXT_ALPHA_MEDIUM_EMPHASIS = 153;
final float TEXT_ALPHA_DISABLED = 96.9;

final float FONT_SIZE_BODY_1 = 16;
final float FONT_SIZE_BODY_2 = 14;
final float FONT_SIZE_CAPTION = 12;

PFont roboto;

void setup() {
  size(640, 480);
  surface.setTitle("Robot Remote");
  
  smooth();
  noStroke();
  
  roboto = createFont("Roboto", 16, false);
  textFont(roboto);
}

void draw() {
  background(COLOR_BACKGROUND);
  textSize(FONT_SIZE_BODY_1);
  fill(COLOR_WHITE, TEXT_ALPHA_HIGH_EMPHASIS);
  text("Hello, world!", 20, 20);
}
