#include "draw_line.h"

#include <iostream>

int main()
{
    try
    {
	    auto render = new cg::LineDrawing(1920, 1080);

        render->Clear();
        render->DrawScene();
        render->Save("results/draw_line.png");
        system("start results/draw_line.png");

    	delete render;
    }
    catch (std::exception & e)
    {
        std::cerr << e.what() << std::endl;
    }
}