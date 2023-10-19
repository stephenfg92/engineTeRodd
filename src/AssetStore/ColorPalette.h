#pragma once

struct Color {
	int r;
	int g;
	int b;
};

struct Green : public Color {
	int r = 124;
	int g = 252;
	int b = 0;
};

struct Yellow : public Color {
	int r = 255;
	int g = 255;
	int b = 0;
};

struct Red : public Color {
	int r = 255;
	int g = 0;
	int b = 0;
};