//Core Particle Based PRNG Class
//Author Sean Donachiue

#include "cryptorand.h"
#include "stdafx.h"


CryptoRand::CryptoRand() { m_particle = new Particle; }
CryptoRand::~CryptoRand() {
	
	for (int i = 0; i < num_particles; i++) {
		deleteParticle(&front, 0);
	}
	m_particle = nullptr;
}
//Initialize 16 particles to obtain 32 bytes
void CryptoRand::init(int num_bytes_to_gen, char* soundBuf) {
	Particle *currParticle = new Particle();
	Particle *nextParticle = nullptr;
	front = currParticle;
	auto t = Clock::now();
	float small_dt = 13;
	for (int i = 1; i < num_bytes_to_gen / 2 + 1; i++) {
		int a = (int)(small_dt) % 10;
		int b = (int)(small_dt / 10.f) % 10;
		small_dt = a + b * 10;
		std::cout << "time to init particle is: " << small_dt << "\n";
		currParticle->m_pos.x = (int)soundBuf[(int)(i * small_dt)] % 64;
		currParticle->m_pos.y = (int)soundBuf[(int)(i * 2 * small_dt)] % 64;
		currParticle->m_vel.x = (int)soundBuf[(int)(i * 3 * small_dt)];
		currParticle->m_vel.y = (int)soundBuf[(int)(i * 4 * small_dt)];
		//std::cout << (int)soundBuf[(int)(i * small_dt)] % 64;
		
		//prevent velocities of 0
		if (currParticle->m_vel.x == 0) {
			currParticle->m_vel.x += -small_dt * (int)soundBuf[currParticle->m_pos.y];
			currParticle->m_vel.y += small_dt * (int)soundBuf[currParticle->m_pos.x];
		}
		if (currParticle->m_vel.y == 0) {
			currParticle->m_vel.x += small_dt * (int)soundBuf[currParticle->m_pos.y];
			currParticle->m_vel.y += -small_dt * (int)soundBuf[currParticle->m_pos.x];
		}
		
		std::cout << "particle " << num_particles << " stats\n";
		std::cout << currParticle->m_pos.x << "\n";
		std::cout << currParticle->m_pos.y << "\n";
		std::cout << currParticle->m_vel.x << "\n";
		std::cout << currParticle->m_vel.y << "\n";
		//gets through all this fine! except that 2 and 3 have identical init values for some reason?
		auto now = Clock::now();
		small_dt = (float)(std::chrono::duration_cast<std::chrono::microseconds>(now - t)).count();
		t = now;
		//this breaks something
		num_particles++;
		update(small_dt);
		//or these break something
		//its the lines break somethin at the end boi.
		nextParticle = new Particle();
		currParticle->next = nextParticle;
		currParticle = nextParticle;
		std::cout << num_particles << "\n";
	}

	//currParticle is nothing right now, neither is next, should be able to delete both safely here.
	currParticle->next = nullptr;
	delete currParticle;		//deallocates the pointed memory and calls particle destructor
	nextParticle = nullptr;		//no dnglrs gnomesayin
	currParticle = nullptr;
	num_particles--;			//just cause i'm a citizen of derpistan.
}

void CryptoRand::initOne(float dt, char * soundBuf, vec2 v) {
	int aa = (int)dt % 10;
	int bb = (int)(dt / 10) % 10;
	int lowdigits = aa + bb * 10;
	Particle* newParticle = new Particle();
	newParticle->m_pos.x = (int)(soundBuf[v.x] + lowdigits) % 64;
	newParticle->m_pos.y = (int)(soundBuf[v.y] + lowdigits) % 64;
	newParticle->m_vel.x = (int)soundBuf[(int)(13 * dt)] - lowdigits;
	newParticle->m_vel.y = (int)soundBuf[(int)(17 * dt)] - lowdigits;
	(newParticle->next) = front;
	if (newParticle->m_pos.x == 0 && newParticle->m_pos.y == 0 || newParticle->m_vel.x == 0 && newParticle->m_vel.y == 0) {
		newParticle->m_pos.x = 19;
		newParticle->m_pos.y = 33;
		newParticle->m_vel.x = 43;
		newParticle->m_vel.y = -11;
	}
	front = newParticle;
	//copy into *front, front->next should be the old front variable?
	//will the old front variable be overwritten by this?
	update(dt);
	num_particles++;
	//used to delete currParticle here.
	//want to keep it, as well as the other particle, it will get deleted when it's pulled!
	//this sets front equal to itself.
}

void CryptoRand::update(float dt) {
	Particle* update_particle = front;
	int i = 0;
	while (update_particle->next != nullptr && i < num_particles) {
		update_particle->p_update(dt);
		update_particle = (update_particle->next);
		i++;
	}
	update_particle->p_update(dt);	//update last particle.
}

void CryptoRand::pullRandom32(float dt, char* soundBuf, char output[], int num_bytes) {
	auto t = Clock::now();
	auto t2 = Clock::now();
	float loop_dt = 16;
	for(int j = 0; j < num_bytes; j++) {
		bool picked = false;
		//check for permissible values in chosen particle
		//(int)soundBuf[(int)(143 * (loop_dt + 1)) % (69 + j)] % num_particles
		while (!picked) {
			int random_index = getLowOrderClockDigits(t2) % (num_particles + 1);
			t2 = Clock::now();
			std::cout << random_index << std::endl;			//always two... wacky.
			int i = 0;
			Particle* curr = front;
			while (curr->next != nullptr && i != random_index) {
				curr = curr->next;
				i++;
			}
			memcpy(m_particle, curr, sizeof(Particle));
			//check for particle coords in the value ranges for base64 charset
			//split cases for x and y by even and odd iterations and only take one at a time
			//but delete the entire particle just the same, in hopes of removing the high incidence of duplicate values.
			if (j % 2 == 0) {
				if (deleteParticle(&front, random_index)) {
					//Pull values from the particle just removed
					char x = base64_chars[m_particle->m_pos.x];
					output[j] = x;
					picked = true;
					auto now = Clock::now();
					loop_dt = (float)(std::chrono::duration_cast<std::chrono::microseconds>(now - t)).count() / 1000;
					initOne(loop_dt, soundBuf, m_particle->m_vel);
				}
				else {
					auto now = Clock::now();
					loop_dt = (float)(std::chrono::duration_cast<std::chrono::microseconds>(now - t)).count() / 1000;
					update(loop_dt);
				}
			} else if (deleteParticle(&front, random_index)) {
					char y = base64_chars[m_particle->m_pos.y];
					output[j] = y;
					picked = true;
					auto now = Clock::now();
					loop_dt = (float)(std::chrono::duration_cast<std::chrono::microseconds>(now - t)).count() / 1000;
					initOne(loop_dt, soundBuf, m_particle->m_vel);
			}
			else {
				auto now = Clock::now();
				loop_dt = (float)(std::chrono::duration_cast<std::chrono::microseconds>(now - t)).count() / 1000;
				update(loop_dt);
			}
				
		}
		t = Clock::now();
	}
	//std::cout << output << std::endl;
}

bool CryptoRand::deleteParticle(Particle ** head_ref, int position)
{
	// If linked list is empty
	if (*head_ref == nullptr)
		return false;

	// Store front particle
	Particle* temp = *head_ref;

	// If head needs to be removed
	if (position == 0)
	{
		*head_ref = temp->next;   // Change head
		memcpy(m_particle, temp, sizeof(Particle));
		delete temp; // delete old head
		num_particles--;
		return true;
	}

	// Find previous node of the node to be deleted
	for (int i = 0; temp != nullptr && i < position - 1; i++)
		temp = (temp->next);

	// If position is more than number of particles
	if (temp == nullptr || temp->next == nullptr)
		return false;

	// Node temp->next is the node to be deleted
	// Store pointer to the next of node to be deleted	//what if its at the very end!

	Particle* next = temp->next->next;

	// Unlink the node from linked list
	
	//m_particle = (temp->next);
	memcpy(m_particle, temp->next, sizeof(Particle));
	delete (temp->next);  // delete memory
	num_particles--;
	(temp->next) = next;  // Unlink the deleted node from list
	return true;
}
int CryptoRand::getLowOrderClockDigits(Clock::time_point oldT) {
	float time = (float)(std::chrono::duration_cast<std::chrono::microseconds>(Clock::now() - oldT)).count();
	if (time < 1) {
		time *= 100;
	}
	int a = (int)time % 10;
	int b = ((int)time / 10) % 10;
	return a + b * 10;
}

