#include <vitasdk.h>
#include <vitaGL.h>
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <stdbool.h>
#include <so_util/so_util.h>
#include "player.h"
#include "utils.h"
#include "logger.h"

#define FB_ALIGNMENT 0x40000
#define ALIGN_MEM(x, align) (((x) + ((align) - 1)) & ~((align) - 1))

int audio_new;
int audio_p = -1;
int audio_len;
int audio_freq;
int audio_mode;

float * bg_attributes = NULL;

uint32_t current_timestamp = 0xFFFFFFFF;
uint32_t current_tick = 0;
#define AVPLAYER_BUG_TIMEFRAME 500000

SceAvPlayerHandle movie_player;
SceUID audio_thid;
int player_state = PLAYER_INACTIVE;

GLuint movie_frame[5] = {};
uint8_t movie_frame_idx = 0;
SceGxmTexture *movie_tex[5];
uint8_t first_frame = 1;

size_t video_fsize;

void video_audio_init(void) {
    // Check if we have an already available audio port
    audio_p = -1;
    for (int i = 0; i < 8; i++) {
        if (sceAudioOutGetConfig(i, SCE_AUDIO_OUT_CONFIG_TYPE_LEN) >= 0) {
            audio_p = i;
            break;
        }
    }

    // Configure the audio port (either new or old)
    if (audio_p == -1) {
        audio_p = sceAudioOutOpenPort(SCE_AUDIO_OUT_PORT_TYPE_MAIN, 1024, 48000, SCE_AUDIO_OUT_MODE_STEREO);
        video_set_volume(1.0f);
        audio_new = 1;
    } else {
        audio_len = sceAudioOutGetConfig(audio_p, SCE_AUDIO_OUT_CONFIG_TYPE_LEN);
        audio_freq = sceAudioOutGetConfig(audio_p, SCE_AUDIO_OUT_CONFIG_TYPE_FREQ);
        audio_mode = sceAudioOutGetConfig(audio_p, SCE_AUDIO_OUT_CONFIG_TYPE_MODE);
        audio_new = 0;
    }
}

int video_audio_thread(SceSize args, void *argp) {
    SceAvPlayerFrameInfo frame;
    sceClibMemset(&frame, 0, sizeof(SceAvPlayerFrameInfo));

    while (player_state != PLAYER_INACTIVE && sceAvPlayerIsActive(movie_player)) {
        if (sceAvPlayerGetAudioData(movie_player, &frame)) {
            sceAudioOutSetConfig(audio_p, 1024, frame.details.audio.sampleRate, frame.details.audio.channelCount == 1 ? SCE_AUDIO_OUT_MODE_MONO : SCE_AUDIO_OUT_MODE_STEREO);
            sceAudioOutOutput(audio_p, frame.pData);
        } else {
            sceKernelDelayThread(1000);
        }
    }

    return sceKernelExitDeleteThread(0);
}

uint8_t *vid_buf;
FILE *vid_handle;
int video_start(void *argp, const char *fname) {
    return 0;
}

int video_stop(void *argp) {
    free(vid_buf);
    return 0;
}

int video_read(void *argp, uint8_t *buffer, uint64_t pos, uint32_t len) {
    if (pos + len > video_fsize) {
        len = video_fsize - pos;
    }
    sceClibMemcpy(buffer, &vid_buf[pos], len);
    return len;
}

long long unsigned int video_size(void *argp) {
    return video_fsize;
}

void *mem_alloc(void *p, uint32_t align, uint32_t size) {
    return memalign(align, size);
}

void mem_free(void *p, void *ptr) {
    free(ptr);
}

void *gpu_alloc(void *p, uint32_t align, uint32_t size) {
    if (align < FB_ALIGNMENT) {
        align = FB_ALIGNMENT;
    }
    size = ALIGN_MEM(size, align);
    size = ALIGN_MEM(size, 1024 * 1024);
    SceUID memblock = sceKernelAllocMemBlock("Video Memblock", SCE_KERNEL_MEMBLOCK_TYPE_USER_MAIN_PHYCONT_NC_RW, size, NULL);

    void *res;
    sceKernelGetMemBlockBase(memblock, &res);
    sceGxmMapMemory(res, size, (SceGxmMemoryAttribFlags)(SCE_GXM_MEMORY_ATTRIB_READ | SCE_GXM_MEMORY_ATTRIB_WRITE));

    return res;
}

void gpu_free(void *p, void *ptr) {
    glFinish();
    SceUID memblock = sceKernelFindMemBlockByAddr(ptr, 0);
    sceGxmUnmapMemory(ptr);
    sceKernelFreeMemBlock(memblock);
}

extern so_module so_mod;
extern bool video_player_active;
void video_close() {
    if (player_state == PLAYER_ACTIVE) {
        sceAvPlayerStop(movie_player);
        sceAvPlayerClose(movie_player);
        player_state = PLAYER_INACTIVE;
        video_player_active = 0;
        void (* Java_video_MyVideoView_SetFinished)() = (void *)so_symbol(&so_mod, "Java_video_MyVideoView_SetFinished");
        Java_video_MyVideoView_SetFinished();
    }
}

void video_open(const char *path, const char * lang) {
    if (bg_attributes == NULL) {
        bg_attributes = malloc(44 * sizeof(float));
    }

    if (player_state == PLAYER_ACTIVE) {
        video_close();
    }

    current_timestamp = 0xFFFFFFFF;

    if (audio_p == -1)
        video_audio_init();

    first_frame = 1;
    if (movie_frame[0] == 0) {
        glGenTextures(5, movie_frame);
        for (int i = 0; i < 5; i++) {
            glBindTexture(GL_TEXTURE_2D, movie_frame[i]);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 8, 8, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
            movie_tex[i] = vglGetGxmTexture(GL_TEXTURE_2D);
            vglFree(vglGetTexDataPointer(GL_TEXTURE_2D));
        }
    }

    SceAvPlayerInitData playerInit;
    memset(&playerInit, 0, sizeof(SceAvPlayerInitData));

    playerInit.memoryReplacement.allocate = mem_alloc;
    playerInit.memoryReplacement.deallocate = mem_free;
    playerInit.memoryReplacement.allocateTexture = gpu_alloc;
    playerInit.memoryReplacement.deallocateTexture = gpu_free;

    playerInit.fileReplacement.objectPointer = NULL;
    playerInit.fileReplacement.open = video_start;
    playerInit.fileReplacement.close = video_stop;
    playerInit.fileReplacement.readOffset = video_read;
    playerInit.fileReplacement.size = video_size;

    playerInit.basePriority = 175;
    playerInit.numOutputVideoFrameBuffers = 5;
    playerInit.autoStart = 1;
#if DEBUG
    playerInit.debugLevel = 3;
#endif

    file_load(path, &vid_buf, &video_fsize);

    movie_player = sceAvPlayerInit(&playerInit);
    sceAvPlayerAddSource(movie_player, "dummy.mp4");

    audio_thid = sceKernelCreateThread("video_audio_thread", video_audio_thread, 0x10000100 - 10, 0x4000, 0, 0, NULL);
    sceKernelStartThread(audio_thid, 0, NULL);
    player_state = PLAYER_ACTIVE;
}

uint32_t video_get_current_time() {
    if (player_state == PLAYER_ACTIVE && sceAvPlayerIsActive(movie_player))
        return sceAvPlayerCurrentTime(movie_player);
    return 0;
}

GLuint video_get_frame(int *width, int *height) {
    if (player_state == PLAYER_ACTIVE) {
        if (sceAvPlayerIsActive(movie_player)) {
            uint32_t cur_time = sceAvPlayerCurrentTime(movie_player);
            if (cur_time != current_timestamp) {
                current_timestamp = cur_time;
            } else {
                uint32_t tick = sceKernelGetProcessTimeLow();
                if (tick - current_tick > AVPLAYER_BUG_TIMEFRAME) {
                    printf("ERROR: Lock Bug Detected on time %u\n", current_timestamp);
                }
                current_tick = tick;
            }
            SceAvPlayerFrameInfo frame;
            if (sceAvPlayerGetVideoData(movie_player, &frame)) {
                movie_frame_idx = (movie_frame_idx + 1) % 5;
                sceGxmTextureInitLinear(
                    movie_tex[movie_frame_idx],
                    frame.pData,
                    SCE_GXM_TEXTURE_FORMAT_YVU420P2_CSC1,
                    frame.details.video.width,
                    frame.details.video.height, 0);
                *width = frame.details.video.width;
                *height = frame.details.video.height;
                sceGxmTextureSetMinFilter(movie_tex[movie_frame_idx], SCE_GXM_TEXTURE_FILTER_LINEAR);
                sceGxmTextureSetMagFilter(movie_tex[movie_frame_idx], SCE_GXM_TEXTURE_FILTER_LINEAR);
                first_frame = 0;
            }
            return first_frame ? 0xDEADBEEF : movie_frame[movie_frame_idx];
        } else if (!first_frame)
            video_close();
    } else if (player_state == PLAYER_PAUSED) {
        return movie_frame[movie_frame_idx];
    }

    return 0xDEADBEEF;
}

void video_pause() {
    sceAvPlayerPause(movie_player);
    player_state = PLAYER_PAUSED;
}

void video_resume() {
    sceAvPlayerResume(movie_player);
    player_state = PLAYER_ACTIVE;
    current_tick = sceKernelGetProcessTimeLow();
}

void video_set_volume(float vol) {
    int vols[2] = {(int)(vol * 32767.0f), (int)(vol * 32767.0f)};
    sceAudioOutSetVolume(audio_p, SCE_AUDIO_VOLUME_FLAG_L_CH | SCE_AUDIO_VOLUME_FLAG_R_CH, vols);
}

void video_jump_to_time(uint64_t time) {
    if (player_state == PLAYER_ACTIVE) {
        uint64_t old_time = sceAvPlayerCurrentTime(movie_player);
        sceAvPlayerJumpToTime(movie_player, time);
        while (sceAvPlayerCurrentTime(movie_player) == old_time) { // Wait for sceAvPlayer FileStream thread response after jump finished
            sceKernelDelayThread(1000);
        }
    }
}

void draw_2d_frame() {
    int w, h;
    GLuint vid_tex = video_get_frame(&w, &h);
    if (vid_tex != 0xDEADBEEF) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, vid_tex);
        setup_2d_draw(0.0f, 0.0f, 960, 544);
        vglDrawObjects(GL_TRIANGLE_STRIP, 4, GL_TRUE);
    }
}

void setup_2d_draw(float x, float y, float x2, float y2) {
    glUseProgram(0);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glDisable(GL_ALPHA_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_TEXTURE_2D);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrthof(0, 960, 544, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    bg_attributes[0] = x;
    bg_attributes[1] = y;
    bg_attributes[2] = 0.0f;
    bg_attributes[3] = x2;
    bg_attributes[4] = y;
    bg_attributes[5] = 0.0f;
    bg_attributes[6] = x;
    bg_attributes[7] = y2;
    bg_attributes[8] = 0.0f;
    bg_attributes[9] = x2;
    bg_attributes[10] = y2;
    bg_attributes[11] = 0.0f;
    vglVertexPointerMapped(3, bg_attributes);

    bg_attributes[12] = 0.0f;
    bg_attributes[13] = 0.0f;
    bg_attributes[14] = 1.0f;
    bg_attributes[15] = 0.0f;
    bg_attributes[16] = 0.0f;
    bg_attributes[17] = 1.0f;
    bg_attributes[18] = 1.0f;
    bg_attributes[19] = 1.0f;

    vglTexCoordPointerMapped(&bg_attributes[12]);

    uint16_t *bg_indices = (uint16_t*)&bg_attributes[20];
    bg_indices[0] = 0;
    bg_indices[1] = 1;
    bg_indices[2] = 2;
    bg_indices[3] = 3;
    vglIndexPointerMapped(bg_indices);
}


