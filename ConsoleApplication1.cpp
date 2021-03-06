// ConsoleApplication1.cpp : Defines the entry point for the console application.
// Author Sean Donachiue

#include "stdafx.h"
#include "VoiceRecording.h"
#include "cryptorand.h"
#include "base64.h"
using Clock = std::chrono::high_resolution_clock;
int main()
{
	CVoiceRecording m_Record;
	//m_Record.DevCaps();
	m_Record.SetFormat(48000, 24, 2);	//48000 hz sample rate, 24 bit, 2 channel recording format
	m_Record.PrepareBuffer(1);    //prepare buffer for recording 1 second (48000 samples)
	m_Record.Open();
	if (m_Record.IsOpen())
	{
		if (!m_Record.Record()) {
			std::cout << "recording failed\n";
			return 1;
		}
	}
	else {
		std::cout << "open failed\n";
		return 1;
	}
	//std::cout << m_Record.buffer[4000];
	//might need to make sure there's stuff in the buffer before continuing.
	char* sound_buf = m_Record.buffer;

	m_Record.Close();


	std::ofstream myfile;

	myfile.open("randoms.txt");

	CryptoRand cryptorand;
	int num_bytes = 32;
	cryptorand.init(num_bytes, sound_buf);

	Clock::time_point t;
	float t_total = 0;
	float elapsed_ms;
	t = Clock::now();
	/*
	while (t_total < 700) {

		auto now = Clock::now();
		elapsed_ms = (float)(std::chrono::duration_cast<std::chrono::microseconds>(now - t)).count() / 1000;
		t_total += elapsed_ms;
		t = now;
		cryptorand.update(elapsed_ms);
	}
	*/
	for (int j = 0; j < 1; j++) {
		for (int i = 500; i > 0; i--) {
			char output[33] = { '\0' };
			auto now = Clock::now();
			elapsed_ms = (float)(std::chrono::duration_cast<std::chrono::microseconds>(now - t)).count() / 1000;
			t = now;
			cryptorand.pullRandom32(elapsed_ms, sound_buf, output, num_bytes);
			//std::string o = base64_encode(output, 32);
			//std::cout << o << ;
			myfile << output;
		}
	}
	myfile.close();
	return 0;
}

