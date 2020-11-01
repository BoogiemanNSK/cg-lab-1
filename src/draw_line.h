#pragma once


#include "clear_rendertarget.h"

namespace cg
{

	class LineDrawing : public ClearRenderTarget
	{
	public:
		LineDrawing(unsigned short width, unsigned short height);
		virtual ~LineDrawing();

		void DrawLine(unsigned x_begin, unsigned y_begin, unsigned x_end, unsigned y_end, color color);

		void DrawScene();

	};

}
