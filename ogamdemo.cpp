// Filename    : OGAMDEMO.CPP
// Description : display the advertising page for the demo

#include <OSYS.H>
#include <OVGA.H>
#include <OMODEID.H>
#include <OVGALOCK.H>
#include <OMOUSE.H>
#include <OGAME.H>

//------ Begin of function Game::demo_disp_ad_page ------//

void Game::demo_disp_ad_page()
{
	//----- display the first page ------//

	vga.disp_image_file("DEMOPG1");
	mouse.wait_press(60);

	//------ display the 2nd page -----//

	vga.disp_image_file("DEMOPG2");
	mouse.wait_press(60);

	//------ display the 3rd page -----//

	vga.disp_image_file("DEMOPG3");
	mouse.wait_press(60);

	vga.finish_disp_image_file();
}
//------ End of function Game::demo_disp_ad_page ------//


//------ Begin of function Game::demo_disp_logo ------//

void Game::demo_disp_logo()
{
	//----- display the first page ------//

	vga.disp_image_file("O-IMAGIC", 180, 190);
	mouse.wait_press(10);

	//------ display the 2nd page -----//

	vga_back.bar( 0, 0, VGA_WIDTH-1, VGA_HEIGHT-1, V_BLACK );
	vga.disp_image_file("O-ENLIGH", 220, 104);
	mouse.wait_press(10);

	//------ display the 3rd page -----//

	vga_back.bar( 0, 0, VGA_WIDTH-1, VGA_HEIGHT-1, V_BLACK );
	vga.disp_image_file("O-GAMEBY", 248, 230);
	mouse.wait_press(10);

	//------ display the 4th page -----//

	vga_back.bar( 0, 0, VGA_WIDTH-1, VGA_HEIGHT-1, V_BLACK );
	vga.disp_image_file("DEMOQUO");
	mouse.wait_press(60);

	vga.finish_disp_image_file();
}
//------ End of function Game::demo_disp_logo ------//

