//Core Particle Based PRNG Class
//Author Sean Donachiue

#pragma once
#include <chrono>
#include <memory>
#include <string>
using Clock = std::chrono::high_resolution_clock;
static const char* base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
class CryptoRand
{
public:
	CryptoRand();
	~CryptoRand();

	void init(int num_bytes_to_gen, char* soundBuf);
	void update(float dt);
	void pullRandom32(float dt, char* soundBuf, char output[], int num_bytes);

private:
	struct vec2 { int x, y; };
	struct vec3 { int x, y, z; };
	struct mat3 { vec3 c0, c1, c2; };

	struct Particle {
		vec2 m_pos;
		vec2 m_vel;
		Particle* next = nullptr;
		void p_update(float dt) {
			m_pos.x += ((int)(m_vel.x * dt / 13.f) + 1);				//sometimes velocity values are very small
			m_pos.y += ((int)(m_vel.y * dt / 17.f) + 1);
			//Wrap coords in range of a signed char, as in char*
			if (m_pos.x > 63) m_pos.x %= 64;
			else if (m_pos.x < 0) m_pos.x %= 64;
			if (m_pos.y > 63) m_pos.y %= 64;
			else if (m_pos.y < 0) m_pos.y %= 64;
		}
	};
	Particle* front;
	Particle* m_particle;
	int num_particles;

	void initOne(float dt, char* soundBuf, vec2 v);
	bool deleteParticle(Particle **head_ref, int position);
	int getLowOrderClockDigits(Clock::time_point oldT);
	/*
	enum CHARSET {
		one = '1', two = '2', three = '3', four = '4', five = '5', six = '6', 
		seven = '7', eight = '8', nine = '9', zero = '0', dot = '.', slash = '/',
		a = 'a', b = 'b', c = 'c', d = 'd', e = 'e', f = 'f', g = 'g', h = 'h', 
		i = 'i', j = 'j', k = 'k', l = 'l', m = 'm', n = 'n', o = 'o', p = 'p',
		q = 'q', r = 'r', s = 's', t = 't', u = 'u', v = 'v', w = 'w', x = 'x',
		y = 'y', z = 'z',
		A = 'A', B = 'B', C = 'C', D = 'D', E = 'E', F = 'F', G = 'G', H = 'H',
		I = 'I', J = 'J', K = 'K', L = 'L', M = 'M', N = 'N', O = 'O', P = 'P',
		Q = 'Q', R = 'R', S = 'S', T = 'T', U = 'U', V = 'V', W = 'W', X = 'X',
		Y = 'Y', Z = 'Z'
	};
	*/
};