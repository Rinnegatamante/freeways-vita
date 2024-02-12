#include <vitasdk.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sndfile.h>
#include "soloud.h"
#include "soloud_wav.h"
#include "soloud_bus.h"

SoLoud::Soloud soloud;
SoLoud::Wav sounds[32];
int cur_snd_idx = 0;
extern "C" {

void init_soloud() {
	soloud.init(SoLoud::Soloud::CLIP_ROUNDOFF);
}

int load_sound(const char *fname) {
	printf("loading %s\n", fname);
	
	char full_fname[256];
	sprintf(full_fname, "ux0:data/freeways/assets/%s.wav", fname);

	sounds[cur_snd_idx].load(full_fname);
	sounds[cur_snd_idx].setSingleInstance(1);
	int res = cur_snd_idx;
	cur_snd_idx++;
	return res;
}

void play_sound(int id, float vol, int loop, float rate) {
	int h = soloud.play(sounds[id], vol / 100.0f, 0.0f, 1);
	soloud.setRelativePlaySpeed(h, rate);
	soloud.setLooping(h, loop);
	soloud.setPause(h, 0);
}

}