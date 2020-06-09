#pragma once
// currently unused

class RGB {
public:
	unsigned char r;
	unsigned char g;
	unsigned char b;
public:
	RGB() {}
	RGB(unsigned char m_r, unsigned char m_g, unsigned char m_b) {
		r = m_r;
		g = m_g;
		b = m_b;
	}
};

class RGBA {
public:
	unsigned char r;
	unsigned char g;
	unsigned char b;
	unsigned char a;
public:
	RGBA() {}
	RGBA(unsigned char m_r, unsigned char m_g, unsigned char m_b, unsigned char m_a) {
		r = m_r;
		g = m_g;
		b = m_b;
		a = m_a;
	}
};