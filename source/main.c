#include "utils/init.h"
#include "utils/glutil.h"
#include "utils/logger.h"
#include "utils/player.h"

#include <psp2/kernel/threadmgr.h>

#include <falso_jni/FalsoJNI.h>
#include <so_util/so_util.h>
#include <sys/types.h>
#include <malloc.h>
#include <pthread.h>
#include <psp2/touch.h>
#include <math.h>
#include <stdbool.h>
#include <psp2/ctrl.h>
#include <string.h>
#include <pthread-svelte/include/pthread_svelte.h>
#include <psp2/kernel/processmgr.h>
#include <psp2/kernel/clib.h>
#include <psp2/sysmodule.h>

bool video_player_active = false;
bool video_player_wantexit = false;

int _newlib_heap_size_user = 280 * 1024 * 1024;

#ifdef USE_SCELIBC_IO
int sceLibcHeapSize = 4 * 1024 * 1024;
#endif

so_module so_mod;
void* game_thread();
void * controls_thread();

int main() {
    soloader_init_all();

    sceSysmoduleLoadModule(SCE_SYSMODULE_AVPLAYER);

    int (* JNI_OnLoad)(void *jvm) = (void *)so_symbol(&so_mod, "JNI_OnLoad");
    JNI_OnLoad(&jvm);

    // DRM
    int **lockPointer1 = (uintptr_t)so_mod.text_base + 0x0098a278; // DAT_0098a278
    int **lockPointer2 = (uintptr_t)so_mod.text_base + 0x0098a26c; // DAT_0098a26c
    int **lockPointer3 = (uintptr_t)so_mod.text_base + 0x0098a27c; // DAT_0098a27c

    *lockPointer1 = malloc(4);
    *lockPointer2 = malloc(4);
    *lockPointer3 = malloc(4);

    **lockPointer1 = 1;
    **lockPointer2 = 1;
    **lockPointer3 = 1;

//sceRazorGpuCaptureEnableSalvage("ux0:data/salvage.sgx");

    // Running the .so in a thread with enlarged stack size.
    pthread_t t;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setstacksize(&attr, 9 * 1024 * 1024);
    pthread_create(&t, &attr, game_thread, NULL);
    pthread_join(t, NULL);
}

int lastX[SCE_TOUCH_MAX_REPORT] = {-1, -1, -1, -1, -1, -1, -1, -1};
int lastY[SCE_TOUCH_MAX_REPORT] = {-1, -1, -1, -1, -1, -1, -1, -1};

float lerp(float x1, float y1, float x3, float y3, float x2) {
    return ((x2-x1)*(y3-y1) / (x3-x1)) + y1;
}

void coord_normalize(float * x, float * y, float deadzone) {
    float magnitude = sqrtf((*x * *x) + (*y * *y));
    if (magnitude < deadzone) {
        *x = 0;
        *y = 0;
        return;
    }

    // normalize
    *x = *x / magnitude;
    *y = *y / magnitude;

    float multiplier = ((magnitude - deadzone) / (1 - deadzone));
    *x = *x * multiplier;
    *y = *y * multiplier;
}

void (* GL2JNILib_touchEvent)(JNIEnv *env, jclass clazz, jint p1, jint p2, jint p3, jint p4);
void (* GL2JNILib_keyboardEvent)(JNIEnv *env, jclass clazz, jint key, jint isDown);
void (* GL2JNILib_setTouchPadDTLeft)(JNIEnv *env, jclass clazz, jfloat x, jfloat y, jint pointer_id);
void (* GL2JNILib_setTouchPadDT)(JNIEnv *env, jclass clazz, jfloat x, jfloat y, jint pointer_id);
int (* GL2JNILib_isGamePlay)();



void controls_init() {
    // Enable analog sticks and touchscreen
    sceCtrlSetSamplingModeExt(SCE_CTRL_MODE_ANALOG_WIDE);
    sceTouchSetSamplingState(SCE_TOUCH_PORT_FRONT, 1);

    GL2JNILib_touchEvent = (void*)so_symbol(&so_mod,"Java_com_gameloft_glf_GL2JNILib_touchEvent");
    GL2JNILib_keyboardEvent = (void*)so_symbol(&so_mod,"Java_com_gameloft_glf_GL2JNILib_keyboardEvent");
    GL2JNILib_setTouchPadDTLeft = (void*)so_symbol(&so_mod,"Java_com_gameloft_glf_GL2JNILib_nativeSetTouchPadDTLeft");
    GL2JNILib_setTouchPadDT = (void*)so_symbol(&so_mod,"Java_com_gameloft_glf_GL2JNILib_nativeSetTouchPadDT");
    GL2JNILib_isGamePlay = (void*)so_symbol(&so_mod,"Java_com_gameloft_android_ANMP_GloftM3HM_GloftM3HM_isGamePlay");
}

/*
 *
 */
SceTouchData touch_old;
void pollTouch() {
    SceTouchData touch;
    sceTouchPeek(SCE_TOUCH_PORT_FRONT, &touch, 1);
    for (int i = 0; i < touch.reportNum; i++) {
        int x = (int) ((float) touch.report[i].x * 960.f / 1920.0f);
        int y = (int) ((float) touch.report[i].y * 544.f / 1088.0f);

        int finger_down = 0;

        if (touch_old.reportNum > 0) {
            for (int j = 0; j < touch_old.reportNum; j++) {
                if (touch.report[i].id == touch_old.report[j].id) {
                    finger_down = 1;
                }
            }
        }

        if (!finger_down) {
            GL2JNILib_touchEvent(&jni, (void *) 0x42424242, 1, x, y, touch.report[i].id);
        } else {
            GL2JNILib_touchEvent(&jni, (void *) 0x42424242, 2, x, y, touch.report[i].id);
        }
    }

    for (int i = 0; i < touch_old.reportNum; i++) {
        int finger_up = 1;
        if (touch.reportNum > 0) {
            for (int j = 0; j < touch.reportNum; j++) {
                if (touch.report[j].id == touch_old.report[i].id ) {
                    finger_up = 0;
                }
            }
        }
        if (finger_up == 1) {
            int x = (int) ((float) touch_old.report[i].x * 960.f / 1920.0f);
            int y = (int) ((float) touch_old.report[i].y * 544.f / 1088.0f);

            GL2JNILib_touchEvent(&jni, (void*)0x42424242, 0, x, y, touch_old.report[i].id);
        }
    }

    sceClibMemcpy(&touch_old, &touch, sizeof(touch));
}


enum {
    AKEYCODE_BACK = 4,
    AKEYCODE_DPAD_UP = 19,
    AKEYCODE_DPAD_DOWN = 20,
    AKEYCODE_DPAD_LEFT = 21,
    AKEYCODE_DPAD_RIGHT = 22,
    AKEYCODE_DPAD_CENTER = 23,
    AKEYCODE_A = 29,
    AKEYCODE_B = 30,
    AKEYCODE_BUTTON_X = 99,
    AKEYCODE_BUTTON_Y = 100,
    AKEYCODE_BUTTON_L1 = 102,
    AKEYCODE_BUTTON_R1 = 103,
    AKEYCODE_BUTTON_START = 108,
    AKEYCODE_BUTTON_SELECT = 109,
    AKEYCODE_MOVE_END = 123
};

typedef struct {
    uint32_t sce_button;
    uint32_t android_button;
} ButtonMapping;

#define L_OUTER_DEADZONE 0.992f
#define R_OUTER_DEADZONE 1.0f
#define L_INNER_DEADZONE 0.2f
#define R_INNER_DEADZONE 0.2f

static ButtonMapping mapping[] = {
        { SCE_CTRL_UP,        AKEYCODE_DPAD_UP },
        { SCE_CTRL_DOWN,      AKEYCODE_DPAD_DOWN },
        { SCE_CTRL_LEFT,      AKEYCODE_DPAD_LEFT },
        { SCE_CTRL_RIGHT,     AKEYCODE_DPAD_RIGHT },
        { SCE_CTRL_CROSS,     AKEYCODE_DPAD_CENTER },
        { SCE_CTRL_CIRCLE,    AKEYCODE_MOVE_END },
        { SCE_CTRL_SQUARE,    AKEYCODE_BUTTON_X },
        { SCE_CTRL_TRIANGLE,  AKEYCODE_BUTTON_Y },
        { SCE_CTRL_L1,        AKEYCODE_BUTTON_L1 },
        { SCE_CTRL_R1,        AKEYCODE_BUTTON_R1 },
        { SCE_CTRL_START,     AKEYCODE_BUTTON_START },
        { SCE_CTRL_SELECT,    AKEYCODE_BUTTON_SELECT },
};
uint32_t old_buttons = 0, current_buttons = 0, pressed_buttons = 0, released_buttons = 0;
float lastLx = 0.0f, lastLy = 0.0f, lastRx = 0.0f, lastRy = 0.0f;
float lastLastLx = 0.0f, lastLastLy = 0.0f, lastLastRx = 0.0f, lastLastRy = 0.0f;
float lx = 0.0f, ly = 0.0f, rx = 0.0f, ry = 0.0f;

void pollPad() {

    SceCtrlData pad;
    sceCtrlPeekBufferPositiveExt2(0, &pad, 1);

    { // Gamepad buttons
        old_buttons = current_buttons;
        current_buttons = pad.buttons;
        pressed_buttons = current_buttons & ~old_buttons;
        released_buttons = ~current_buttons & old_buttons;

        if (video_player_active) {
            if (pressed_buttons & SCE_CTRL_CROSS) {
                video_player_wantexit = true;
            }
        }

        for (int i = 0; i < sizeof(mapping) / sizeof(ButtonMapping); i++) {
            if (GL2JNILib_isGamePlay() && mapping[i].sce_button == SCE_CTRL_CROSS) {
                // Disable cross in gameplay to avoid a nasty bug
                continue;
            }

            if (pressed_buttons & mapping[i].sce_button) {
                GL2JNILib_keyboardEvent(&jni, (void *) 0x42424242, mapping[i].android_button, 1);
            }
            if (released_buttons & mapping[i].sce_button) {
                //debugPrintf("NativeOnKeyUp %i\n", mapping[i].android_button);
                GL2JNILib_keyboardEvent(&jni, (void *) 0x42424242, mapping[i].android_button, 0);
            }
        }
    }

    // Analog sticks

    lx = ((float)pad.lx - 128.0f) / 128.0f;
    ly = ((float)pad.ly - 128.0f) / 128.0f;
    rx = ((float)pad.rx - 128.0f) / 128.0f;
    ry = ((float)pad.ry - 128.0f) / 128.0f;
    coord_normalize(&lx, &ly, L_INNER_DEADZONE);
    coord_normalize(&rx, &ry, R_INNER_DEADZONE);

    if ((lx == 0.f && ly == 0.f) && (lastLx == 0.f && lastLy == 0.f) && (lastLastLx != 0.f || lastLastLy != 0.f)) {
        // lstick stop
        GL2JNILib_setTouchPadDTLeft(&jni, (void *) 0x42424242, lx, ly, 1);
    }
    if ((rx == 0.f && ry == 0.f) && (lastRx == 0.f && lastRy == 0.f) && (lastLastRx != 0.f || lastLastRy != 0.f)) {
        // rstick stop
        GL2JNILib_setTouchPadDT(&jni, (void *) 0x42424242, rx, ry, 2);
    }
    if ((lx != 0.f || ly != 0.f) || ((lx == 0.f && ly == 0.f) && (lastLx != 0.f || lastLy != 0.f))) {
        // lstick move
        GL2JNILib_setTouchPadDTLeft(&jni, (void *) 0x42424242, lx * 1.10f, ly * 1.10f * -1, 1);
    }
    if ((rx != 0.f || ry != 0.f) || ((rx == 0.f && ry == 0.f) && (lastRx != 0.f || lastRy != 0.f))) {
        // rstick move
        GL2JNILib_setTouchPadDT(&jni, (void *) 0x42424242, rx, ry, 2);
    }

    lastLastLx = lastLx;
    lastLastLy = lastLy;
    lastLastRx = lastRx;
    lastLastRy = lastRy;

    lastLx = lx;
    lastLy = ly;
    lastRx = rx;
    lastRy = ry;
}

//SceGxmErrorCode sceGxmPadHeartbeat(const SceGxmColorSurface *displaySurface, SceGxmSyncObject *displaySyncObject);

void * controls_thread() {
    // Move to 4th core if available
    sceKernelChangeThreadCpuAffinityMask(sceKernelGetThreadId(), 0x80000);

    while (1) {
        pollTouch();
        pollPad();
        sceKernelDelayThread(8335); // half a frame assuming 60fps
    }
}

bool is_gameplay = false;

void* game_thread() {
    controls_init();
    void (* Java_com_gameloft_glf_GL2JNILib_init)(JNIEnv * env, jclass clazz) = (void *)so_symbol(&so_mod, "Java_com_gameloft_glf_GL2JNILib_init");
    Java_com_gameloft_glf_GL2JNILib_init(&jni, (jclass)0x42424242);
    l_info("Java_com_gameloft_glf_GL2JNILib_init passed");

#if 0 // 1.1.2
    void (* InAppBilling_nativeInit)(JNIEnv * env, jclass clazz, jobject obj) = (void *)so_symbol(&so_mod, "Java_com_gameloft_android_GAND_GloftM3HP_iab_InAppBilling_nativeInit");
#else // 1.1.7g
    void (* InAppBilling_nativeInit)(JNIEnv * env, jclass clazz, jobject obj) = (void *)so_symbol(&so_mod, "Java_com_gameloft_android_ANMP_GloftM3HM_iab_InAppBilling_nativeInit");
#endif
    InAppBilling_nativeInit(&jni, (jclass)0x42424242, (jobject)0x24242424);
    l_info("InAppBilling_nativeInit passed");

#if 0 // 1.1.2
    void (* C2DMAndroidUtils_nativeInit)(JNIEnv * env, jclass clazz) = (void *)so_symbol(&so_mod, "Java_com_gameloft_android_GAND_GloftM3HP_PushNotification_C2DMAndroidUtils_nativeInit");
    C2DMAndroidUtils_nativeInit(&jni, (jclass)0x42424242);
    log_info("C2DMAndroidUtils_nativeInit passed");
#endif

    void (* Java_video_MyVideoView_Init)(JNIEnv * env) = (void *)so_symbol(&so_mod, "Java_video_MyVideoView_Init");
    Java_video_MyVideoView_Init(&jni);
    l_info("Java_video_MyVideoView_Init passed");

#if 0 // 1.1.2
    void (* Game_nativeInit)(JNIEnv * env, jclass clazz) = (void *)so_symbol(&so_mod, "Java_com_gameloft_android_GAND_GloftM3HP_GloftM3HP_Init");
#else // 1.1.7g
    void (* Game_nativeInit)(JNIEnv * env, jclass clazz) = (void *)so_symbol(&so_mod, "Java_com_gameloft_android_ANMP_GloftM3HM_GloftM3HM_Init");
#endif
    Game_nativeInit(&jni, (jclass)0x42424242);
    l_info("Game_nativeInit passed");

#if 0 // 1.1.2
    void (* Game_nativeMC3Init)(JNIEnv * env, jclass clazz) = (void *)so_symbol(&so_mod, "Java_com_gameloft_android_GAND_GloftM3HP_GloftM3HP_nativeMC3Init");
#else // 1.1.7g
    void (* Game_nativeMC3Init)(JNIEnv * env, jclass clazz) = (void *)so_symbol(&so_mod, "Java_com_gameloft_android_ANMP_GloftM3HM_GloftM3HM_nativeMC3Init");
#endif
    Game_nativeMC3Init(&jni, (jclass)0x42424242);
    l_info("Game_nativeMC3Init passed");

    int (* GL2JNILib_getViewSettings)() = (void *)so_symbol(&so_mod, "Java_com_gameloft_glf_GL2JNILib_getViewSettings");
    GL2JNILib_getViewSettings();
    l_info("GL2JNILib_getViewSettings passed");

    int (* GL2JNILib_resize)(JNIEnv * env, jclass clazz, int w, int h) = (void *)so_symbol(&so_mod, "Java_com_gameloft_glf_GL2JNILib_resize");
    GL2JNILib_resize(&jni, (jclass)0x42424242, 960, 544);
    l_info("GL2JNILib_resize passed");

    pthread_t t;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setstacksize(&attr, 256 * 1024);
    pthread_create(&t, &attr, controls_thread, NULL);
    pthread_detach(t);

    sceKernelDelayThread(200000);
    l_info("Controls thread initialized");

    int (* GL2JNILib_step)() = (void *)so_symbol(&so_mod, "Java_com_gameloft_glf_GL2JNILib_step");

    const uint32_t delta = (1000000 / (24+1));
    uint32_t last_render_time = sceKernelGetProcessTimeLow();

    while (1) {
        if (video_player_active) {
            l_info("about to draw 2d frame");
            glViewport(0, 0, 960, 544);
            glScissor(0, 0, 960, 544);
            glClear(GL_COLOR_BUFFER_BIT);
            draw_2d_frame();
            gl_swap();

            if (video_player_wantexit == true) {
                video_close();
                video_player_wantexit = false;
            }
        } else {
            GL2JNILib_step();
            gl_swap();

            while (sceKernelGetProcessTimeLow() - last_render_time < delta) {
                sched_yield();
            }

            last_render_time = sceKernelGetProcessTimeLow();
            is_gameplay = (GL2JNILib_isGamePlay() > 0);
        }
    }

    sceKernelExitDeleteThread(0);
}
