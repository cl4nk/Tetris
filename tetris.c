/* pong.c - 24/02/2012 */

#include "info.h"
#include "gui.h"

int main (int argc, char *argv[])
{
    gtk_init (&argc, &argv);
	srand(time(0));
    Info info;
    Gui gui;
    BestScores bestScores;
    GameBonus bonus;
    SoundStack stack;
    soundStack_init (&stack, NBR_SOUNDS);

    gameBonus_init (&bonus);
    
    AnimatedText aText_down;
    animatedText_init(&aText_down);

    AnimatedText aText_line;
    animatedText_init(&aText_line);

    AnimatedDestroyedLines aLines;
    animatedDestroyedLines_init(&aLines);

    info.aText_down = &aText_down;
    info.aText_line = &aText_line;
    info.aLines = &aLines;
    info.bonus = &bonus;
    info.soundStack = &stack;
	
    bestScores_init (&bestScores);
    bestScores_fill_with_file(&bestScores, BS_FILENAME);
    info_init_default (&info, &bestScores);
    gui_init (&gui, &info, &bestScores);
    gui_config_window (&gui);

    gtk_main ();
    return 0;
}
