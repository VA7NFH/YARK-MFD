#include "Widget.h"
#include "Util/Button.h"

#define BAR_HEIGHT 15
#define BORDER 4

#include <iostream>

GLuint window_x;

Widget::Widget(WidgetStuff ws) {
	if (!window_x) {
		window_x = loadTexture("Tex/x.png", false);
	}
	if (!IM::rad0) {
		IM::rad0 = loadTexture("Tex/radio-off.png", false);
	}
	if (!IM::rad1) {
		IM::rad1 = loadTexture("Tex/radio-on.png", false);
	}
	Resize(ws.pos, ws.size);
	this->f = ws.f;
	this->title = ws.title;
	this->win = ws.win;
	this->client = ws.client;
	this->TL = ws.TL;
	this->startUpName = ws.startUpName;
}
#include "../Reg.h"

void Widget::Resize(XY pos, XY size) {
	if (Registry["ENABLE_WINDOW_FRAMES"]) {
		border = BORDER;
		barHeight = BAR_HEIGHT;
	}
	else {
		border = 0;
		barHeight = 0;
	}
	this->pos = pos;
	this->size = size;
	this->pos.y += barHeight;
	this->size.y -= barHeight + border;
	this->pos.x += border;
	this->size.x -= border * 2;
}

void Widget::Tick(Draw* draw) {
	WindowUpdate(draw);
}

std::string Widget::getSaveParams() {
	return startUpName + " " + std::to_string(pos.x - border) + " " + std::to_string(pos.y - barHeight) + " " + std::to_string(size.x + border * 2) + " " + std::to_string(size.y + barHeight + border) + "\n";
}

extern std::vector<Widget*> widgets;

int Widget::Input() { //0-ignore //1-move to front //2-close
	if (close) return 2;
	mouseInWindow = (win->MouseX() > pos.x && win->MouseY() > pos.y && win->MouseX() < pos.x + size.x && win->MouseY() < pos.y + size.y);
	if (win->MouseDown(SDL_BUTTON_LEFT)) {
		if (drag || (win->MouseX() > pos.x - border && win->MouseY() > pos.y - barHeight && win->MouseX() < pos.x + size.x + border && win->MouseY() < pos.y)) {
			if (!drag) {
				win->MouseDXY();
				drag = true;
			}
			XY d = win->MouseDXY();
			pos += d;
		}
		else {
			if (dragBottom || (win->MouseX() > pos.x - border && win->MouseY() > pos.y + size.y && win->MouseX() < pos.x + size.x + border && win->MouseY() < pos.y + size.y + border)) {
				if (!dragBottom) {
					dragBottom = true;
					win->MouseDY();
				}
				size.y += win->MouseDY();
				if (size.y < 20) {
					size.y = 20;
				}
			}
			if (dragRight || (win->MouseX() > pos.x + size.x && win->MouseY() > pos.y && win->MouseX() < pos.x + size.x + border && win->MouseY() < pos.y + size.y)) {
				if (!dragRight) {
					dragRight = true;
					win->MouseDX();
				}
				size.x += win->MouseDX();
				if (size.x < 20) {
					size.x = 20;
				}
			}
			else if (dragLeft || (win->MouseX() > pos.x - border && win->MouseY() > pos.y && win->MouseX() < pos.x  && win->MouseY() < pos.y + size.y)) {
				if (!dragLeft) {
					dragLeft = true;
					win->MouseDX();
				}
				int dx = win->MouseDX();
				size.x -= dx;
				pos.x += dx;
				if (size.x < 20) {
					size.x = 20;
				}
			}
		}
	}
	else {
		drag = dragBottom = dragRight = dragLeft = false;
	}

	if (((win->MouseDown(SDL_BUTTON_LEFT) || win->MouseDown(SDL_BUTTON_MIDDLE) || win->MouseDown(SDL_BUTTON_RIGHT)) && mouseInWindow || drag || dragBottom || dragRight || dragLeft)) {
		return 1;
	}
	return 0;
}

void Widget::WindowUpdate(Draw* draw) {
	draw->BindDraw2DShader();
	draw->BindTex2D(0);
	draw->SetDrawColor2D(0, 0, 1);

	draw->DrawRect2D(pos.x - border, pos.y - barHeight, pos.x + size.x + border, pos.y + size.y + border);
	if (IM::Button(XY{ pos.x + size.x - 12,pos.y - barHeight + 2 }, XY{ 12, 12 }, win, draw, window_x)) {
		close = true;
	}

	draw->BindTextShader();
	draw->SetTextColor(1, 1, 1);
	draw->DrawString(f, title, pos.x, pos.y);

	glScissor(pos.x, win->size.y - pos.y - size.y, size.x, size.y);

	draw->BindDraw2DShader();
	draw->SetDrawColor2D(0, 0, 0);
	draw->DrawRect2D(pos.x, pos.y, pos.x + size.x, pos.y + size.y);
}