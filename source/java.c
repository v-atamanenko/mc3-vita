#include <falso_jni/FalsoJNI_Impl.h>
#include <falso_jni/FalsoJNI_Logger.h>
#include <string.h>
#include <psp2/audioout.h>
#include <so_util/so_util.h>
#include <psp2/kernel/threadmgr/thread.h>
#include <pthread.h>
#include "utils/logger.h"
#include "utils/trophies.h"
#include "utils/player.h"

/*
 * JNI Methods
*/


jobject getPreferenceString(jmethodID id, va_list args) {
    jstring arg1 = va_arg(args, jstring);
    jstring arg2 = va_arg(args, jstring);
    fjni_logv_info("getPreferenceString(\"%s\", \"%s\")", arg1, arg2);
    return (jobject)strdup("dummy");
}

jobject getPackage(jmethodID id, va_list args) {
    fjni_log_info("getPackage(void)");
    return (jobject)strdup("dummy");
}

jobject getSaveFolder(jmethodID id, va_list args) {
    fjni_log_info("getSaveFolder(void)");
    return (jobject)strdup("dummy");
}

jobject getContext(jmethodID id, va_list args) {
    fjni_log_info("getContext(void)");
    return (jobject)0x42424242;
}

jobject getUserAgent(jmethodID id, va_list args) {
    fjni_log_info("getUserAgent(void)");
    return (jobject)strdup("dummy");
}

jbyteArray getResource(jmethodID id, va_list args) {
    jstring arg1 = va_arg(args, jstring);
    fjni_logv_info("getResource(\"%s\")", arg1);
    return (jbyteArray)0;
}

void setupPaths(jmethodID id, va_list args) {
    fjni_log_info("setupPaths(void)");
}

void createView(jmethodID id, va_list args) {
    fjni_log_info("createView(void)");
}

void setViewSettings(jmethodID id, va_list args) {
    jint arg1 = va_arg(args, jint);
    jint arg2 = va_arg(args, jint);
    jint arg3 = va_arg(args, jint);
    jint arg4 = va_arg(args, jint);
    jint arg5 = va_arg(args, jint);
    fjni_logv_info("setViewSettings(%d, %d, %d, %d, %d)", arg1, arg2, arg3, arg4, arg5);
}

jboolean setCurrentContext(jmethodID id, va_list args) {
    jint arg1 = va_arg(args, jint);
    fjni_logv_info("setCurrentContext(%d)", arg1);
    return JNI_FALSE;
}

void enableAccelerometer(jmethodID id, va_list args) {
    //jboolean arg1 = JNI_TRUE;
    fjni_log_info("enableAccelerometer()");
}

void enableGyroscope(jmethodID id, va_list args) {
    //jboolean arg1 = va_arg(args, int);
    //jfloat arg2 = va_arg(args, jfloat);
    fjni_log_info("enableGyroscope()");
}

void openBrowser(jmethodID id, va_list args) {
    jstring arg1 = va_arg(args, jstring);
    fjni_logv_info("openBrowser(\"%s\")", arg1);
}

void openGLLive(jmethodID id, va_list args) {
    jint arg1 = va_arg(args, jint);
    fjni_logv_info("openGLLive(%d)", arg1);
}

void notifyTrophy(jmethodID id, va_list args) {
    jint arg1 = va_arg(args, jint);
    fjni_logv_info("notifyTrophy(%d)", arg1);
    trophies_unlock(arg1 + 1);
}

void openIGP(jmethodID id, va_list args) {
    jint arg1 = va_arg(args, jint);
    fjni_logv_info("openIGP(%d)", arg1);
}

jobject getDeviceLanguage(jmethodID id, va_list args) {
    fjni_log_info("getDeviceLanguage(void)");
    return (jobject)strdup("dummy");
}

void setCalibratingGyro(jmethodID id, va_list args) {
    //jboolean arg1 = va_arg(args, int);
    fjni_log_info("setCalibratingGyro()");//, (arg1 == JNI_TRUE) ? "JNI_TRUE" : "JNI_FALSE");
}

void resetGyroCalibration(jmethodID id, va_list args) {
    fjni_log_info("resetGyroCalibration(void)");
}

void setVisibility(jmethodID id, va_list args) {
    jint arg1 = va_arg(args, jint);
    fjni_logv_info("setVisibility(%d)", arg1);
}

jobject getText(jmethodID id, va_list args) {
    fjni_log_info("getText(void)");
    return (jobject)0x42424242;
}

void setText(jmethodID id, va_list args) {
    jobject arg1 = va_arg(args, jobject);
    fjni_logv_info("setText(%p)", arg1);
}

jobject getSystemService(jmethodID id, va_list args) {
    jstring arg1 = va_arg(args, jstring);
    fjni_logv_info("getSystemService(\"%s\")", arg1);
    return (jobject)0x42424242;
}

jboolean hasGyroscope(jmethodID id, va_list args) {
    fjni_log_info("hasGyroscope(void)");
    return JNI_FALSE;
}

jboolean isXperiaPlay(jmethodID id, va_list args) {
    fjni_log_info("isXperiaPlay(void)");
    return JNI_TRUE;
}

jboolean isDeviceOpened(jmethodID id, va_list args) {
    //fjni_log_info("isDeviceOpened(void)");
    return JNI_TRUE;
}

jboolean mustSwapButtons(jmethodID id, va_list args) {
    fjni_log_info("mustSwapButtons(void)");
    return JNI_FALSE;
}

jboolean isHTCDevice(jmethodID id, va_list args) {
    fjni_log_info("isHTCDevice(void)");
    return JNI_FALSE;
}

jboolean isGyroSensibilityHigh(jmethodID id, va_list args) {
    fjni_log_info("isGyroSensibilityHigh(void)");
    return JNI_FALSE;
}

jboolean isGyroSensibilityLow(jmethodID id, va_list args) {
    fjni_log_info("isGyroSensibilityLow(void)");
    return JNI_FALSE;
}

jboolean isGyroPendingToFix(jmethodID id, va_list args) {
    fjni_log_info("isGyroPendingToFix(void)");
    return JNI_FALSE;
}

jboolean ignoreGyroLowInc(jmethodID id, va_list args) {
    fjni_log_info("ignoreGyroLowInc(void)");
    return JNI_FALSE;
}

jboolean isGyroInvertedX(jmethodID id, va_list args) {
    fjni_log_info("isGyroInvertedX(void)");
    return JNI_FALSE;
}

jboolean isInvertedPitchYaw(jmethodID id, va_list args) {
    fjni_log_info("isInvertedPitchYaw(void)");
    return JNI_FALSE;
}

jboolean isSwitchRollToYaw(jmethodID id, va_list args) {
    fjni_log_info("isSwitchRollToYaw(void)");
    return JNI_FALSE;
}

jboolean useGeneration0(jmethodID id, va_list args) {
    fjni_log_info("useGeneration0(void)");
    return JNI_FALSE;
}

jboolean useGeneration1(jmethodID id, va_list args) {
    fjni_log_info("useGeneration1(void)");
    return JNI_FALSE;
}

jboolean useGeneration2(jmethodID id, va_list args) {
    fjni_log_info("useGeneration2(void)");
    return JNI_TRUE;
}

jint getProcessorNum(jmethodID id, va_list args) {
    fjni_log_info("getProcessorNum(void)");
    return (jint)3;
}

jboolean isFirstRunOfTheDay(jmethodID id, va_list args) {
    fjni_log_info("isFirstRunOfTheDay(void)");
    return JNI_FALSE;
}

jboolean isHoneycombDevice(jmethodID id, va_list args) {
    fjni_log_info("isHoneycombDevice(void)");
    return JNI_FALSE;
}

jboolean isIceCreamSandwich(jmethodID id, va_list args) {
    fjni_log_info("isIceCreamSandwich(void)");
    return JNI_FALSE;
}

jboolean isSamsung_I9250(jmethodID id, va_list args) {
    fjni_log_info("isSamsung_I9250(void)");
    return JNI_FALSE;
}

jboolean isSamsung_I510(jmethodID id, va_list args) {
    fjni_log_info("isSamsung_I510(void)");
    return JNI_FALSE;
}

jboolean isSamsung_P7100(jmethodID id, va_list args) {
    fjni_log_info("isSamsung_P7100(void)");
    return JNI_FALSE;
}

jboolean isAsus_TF201(jmethodID id, va_list args) {
    fjni_log_info("isAsus_TF201(void)");
    return JNI_FALSE;
}

jboolean isHTC_Nexus9(jmethodID id, va_list args) {
    fjni_log_info("isHTC_Nexus9(void)");
    return JNI_FALSE;
}

jobject getZipName(jmethodID id, va_list args) {
    fjni_log_info("getZipName(void)");
    return (jobject)strdup(DATA_PATH"main.1120.com.gameloft.android.ANMP.GloftM3HM.obb");
}

jobject getPatchFileLocation(jmethodID id, va_list args) {
    fjni_log_info("getPatchFileLocation(void)");
    return (jobject)strdup(DATA_PATH"patch.11428.com.gameloft.android.ANMP.GloftM3HM.obb");
}

jboolean deleteOldData(jmethodID id, va_list args) {
    fjni_log_info("deleteOldData(void)");
    return JNI_FALSE;
}

void sendURLRequest(jmethodID id, va_list args) {
    jstring arg1 = va_arg(args, jstring);
    fjni_logv_info("sendURLRequest(\"%s\")", arg1);
}

jobject getServerResponse(jmethodID id, va_list args) {
    fjni_log_info("getServerResponse(void)");
    return (jobject)strdup("dummy");
}

jboolean hasServerResponse(jmethodID id, va_list args) {
    fjni_log_info("hasServerResponse(void)");
    return JNI_FALSE;
}

void showDisclaimer(jmethodID id, va_list args) {
    jint arg1 = va_arg(args, jint);
    fjni_logv_info("showDisclaimer(%d)", arg1);
}

void showCantGoBackPopup(jmethodID id, va_list args) {
    jint arg1 = va_arg(args, jint);
    fjni_logv_info("showCantGoBackPopup(%d)", arg1);
}

void showParentButton(jmethodID id, va_list args) {
    jint arg1 = va_arg(args, jint);
    fjni_logv_info("showParentButton(%d)", arg1);
}

void hideParentButton(jmethodID id, va_list args) {
    fjni_log_info("hideParentButton(void)");
}

jint getTotalItems(jmethodID id, va_list args) {
    fjni_log_info("getTotalItems(void)");
    return (jint)0;
}

jobject getData(jmethodID id, va_list args) {
    jobject arg1 = va_arg(args, jobject);
    fjni_logv_info("getData(%p)", arg1);
    return (jobject)0x42424242;
}

jint GetState(jmethodID id, va_list args) {
    fjni_log_info("GetState(void)");
    return (jint)0;
}

jobject androidContentIntentInit(jmethodID id, va_list args) {
    fjni_log_info("android/content/Intent/<init>(void)");
    return (jobject)0x42424242;
}

jobject setClassName(jmethodID id, va_list args) {
    jstring arg1 = va_arg(args, jstring);
    jstring arg2 = va_arg(args, jstring);
    fjni_logv_info("setClassName(\"%s\", \"%s\")", arg1, arg2);
    return (jobject)0x42424242;
}

char video_name[256];
char video_lang[256];

jobject putExtra(jmethodID id, va_list args) {
    jstring arg1 = va_arg(args, jstring);
    jstring arg2 = va_arg(args, jstring);
    if (strcmp(arg1, "video_name") == 0) {
        snprintf(video_name, sizeof(video_name), "%s", arg2);
    } else if (strcmp(arg2, "language") == 0) {
        snprintf(video_lang, sizeof(video_lang), "%s", arg2);
    }

    fjni_logv_info("putExtra(\"%s\", \"%s\")", arg1, arg2);
    return (jobject)0x42424242;
}

jobject getSDFolder(jmethodID id, va_list args) {
    fjni_log_info("getSDFolder()");
    return (jstring)strdup(DATA_PATH);
}

jobject androidOsBundleInit(jmethodID id, va_list args) {
    fjni_log_info("android/os/Bundle/<init>(void)");
    return (jobject)0x42424242;
}

void putString(jmethodID id, va_list args) {
    jstring arg1 = va_arg(args, jstring);
    jstring arg2 = va_arg(args, jstring);
    fjni_logv_info("putString(\"%s\", \"%s\")", arg1, arg2);
}

jbyteArray getByteArray(jmethodID id, va_list args) {
    jstring arg1 = va_arg(args, jstring);
    fjni_logv_info("getByteArray(\"%s\")", arg1);
    return (jbyteArray)0;
}

void putByteArray(jmethodID id, va_list args) {
    jstring arg1 = va_arg(args, jstring);
    jbyteArray arg2 = va_arg(args, jbyteArray);
    fjni_logv_info("putByteArray(\"%s\", %p)", arg1, arg2);
}

jint getInt(jmethodID id, va_list args) {
    jstring arg1 = va_arg(args, jstring);
    fjni_logv_info("getInt(\"%s\")", arg1);
    return (jint)0;
}

void putInt(jmethodID id, va_list args) {
    jstring arg1 = va_arg(args, jstring);
    jint arg2 = va_arg(args, jint);
    fjni_logv_info("putInt(\"%s\", %d)", arg1, arg2);
}

jlong getLong(jmethodID id, va_list args) {
    jstring arg1 = va_arg(args, jstring);
    fjni_logv_info("getLong(\"%s\")", arg1);
    return (jlong)0;
}

void putLong(jmethodID id, va_list args) {
    jstring arg1 = va_arg(args, jstring);
    jlong arg2 = va_arg(args, jlong);
    fjni_logv_info("putLong(\"%s\", %d)", arg1, arg2);
}

void showloading(jmethodID id, va_list args) {
    jint arg1 = va_arg(args, jint);
    fjni_logv_info("showloading(%d)", arg1);
}

void hideloading(jmethodID id, va_list args) {
    fjni_log_info("hideloading()");
}

void hidelogoscreen(jmethodID id, va_list args) {
    fjni_log_info("hidelogoscreen()");
}

jboolean containsKey(jmethodID id, va_list args) {
    jstring arg1 = va_arg(args, jstring);
    fjni_logv_info("containsKey(\"%s\")", arg1);
    return JNI_FALSE;
}

void clear(jmethodID id, va_list args) {
    fjni_log_info("clear(void)");
}

void setThreadPriority(jmethodID id, va_list args) {
    jint arg1 = va_arg(args, jint);
    fjni_logv_info("setThreadPriority(%d)", arg1);
}

jobject AudioTrack_init(jmethodID id, va_list args) {
    jint arg1 = va_arg(args, jint);
    jint arg2 = va_arg(args, jint);
    jint arg3 = va_arg(args, jint);
    jint arg4 = va_arg(args, jint);
    jint arg5 = va_arg(args, jint);
    jint arg6 = va_arg(args, jint);
    fjni_logv_info("android/media/AudioTrack/<init>(%d, %d, %d, %d, %d, %d)", arg1, arg2, arg3, arg4, arg5, arg6);
    return (jobject)strdup("dummy");
}

const char * _channelConfigToStr(jint channelConfig) {
    static char ret[64];
    switch (channelConfig) {
    case 12:
        return "CHANNEL_OUT_STEREO (12)";
    case 4:
        return "CHANNEL_OUT_MONO (4)";
    default:
        snprintf(ret, sizeof(ret), "UNKNOWN (%i)", channelConfig);
        return ret;
    }
}

const char * _audioFormatToStr(jint audioFormat) {
    static char ret[64];
    switch (audioFormat) {
    case 2:
        return "ENCODING_PCM_16BIT (2)";
    case 3:
        return "ENCODING_PCM_8BIT (3)";
    case 4:
        return "ENCODING_PCM_FLOAT (4)";
    default:
        snprintf(ret, sizeof(ret), "UNKNOWN (%i)", audioFormat);
        return ret;
    }
}

jint getMinBufferSize(jmethodID id, va_list args) {
    jint sampleRateInHz = va_arg(args, jint);
    jint channelConfig = va_arg(args, jint);
    jint audioFormat = va_arg(args, jint);
    fjni_logv_info("getMinBufferSize(%d, %s, %s)", sampleRateInHz, _channelConfigToStr(channelConfig), _audioFormatToStr(audioFormat));
    return (jint)4096;
}

void play(jmethodID id, va_list args) {
    fjni_log_info("play(void)");
}

void pause(jmethodID id, va_list args) {
    fjni_log_info("pause(void)");
}

void stop(jmethodID id, va_list args) {
    fjni_log_info("stop(void)");
}

void release(jmethodID id, va_list args) {
    fjni_log_info("release(void)");
}

int audio_port = -1;
extern bool video_player_active;

jint audio_write(jmethodID id, va_list args) {
    void* _buf = va_arg(args, jbyteArray);
    int32_t offs = va_arg(args, jint);
    int32_t len = va_arg(args, jint);

    JavaDynArray * jda = jda_find(_buf);
    if (!jda) {
        fjni_log_err("Provided buffer is not a valid JDA.");
        return 0;
    }

    jshort * buf = jda->array;

    if (audio_port == -1) {
        audio_port = sceAudioOutOpenPort(SCE_AUDIO_OUT_PORT_TYPE_BGM, 1024, 44100, SCE_AUDIO_OUT_MODE_STEREO);
        fjni_logv_info("got audio_port %i (calling thread %p)\n", audio_port, sceKernelGetThreadId());
    }

    if (!video_player_active) {
        sceAudioOutOutput(audio_port, buf);
    }

    return len;
}

jint getPlayState(jmethodID id, va_list args) {
    fjni_log_info("getPlayState(void)");
    return (jint)0;
}

jint getNativeOutputSampleRate(jmethodID id, va_list args) {
    jint arg1 = va_arg(args, jint);
    fjni_logv_info("getNativeOutputSampleRate(%d)", arg1);
    return (jint)44100;
}

jobject setFlags(jmethodID id, va_list args) {
    jint arg1 = va_arg(args, jint);
    fjni_logv_info("setFlags(%d)", arg1);
    return (jobject)0x42424242;
}

jint getIntExtra(jmethodID id, va_list args) {
    jstring arg1 = va_arg(args, jstring);
    jint arg2 = va_arg(args, jint);
    fjni_logv_info("getIntExtra(\"%s\", %d)", arg1, arg2);
    return (jint)0;
}

void removeExtra(jmethodID id, va_list args) {
    jstring arg1 = va_arg(args, jstring);
    fjni_logv_info("removeExtra(\"%s\")", arg1);
}

jobject addCategory(jmethodID id, va_list args) {
    jstring arg1 = va_arg(args, jstring);
    fjni_logv_info("addCategory(\"%s\")", arg1);
    return (jobject)0x42424242;
}

extern bool video_player_active;

void startActivity(jmethodID id, va_list args) {
    jobject arg1 = va_arg(args, jobject);
    fjni_logv_info("startActivity(%p)", arg1);

    video_open(video_name, video_lang);
    video_player_active = true;

    l_info("video_open called with %s, %s", video_name, video_lang);
}

void startActivityForResult(jmethodID id, va_list args) {
    jobject arg1 = va_arg(args, jobject);
    jint arg2 = va_arg(args, jint);
    fjni_logv_info("startActivityForResult(%p, %d)", arg1, arg2);
}

jboolean hasWindowFocus(jmethodID id, va_list args) {
    fjni_log_info("hasWindowFocus(void)");
    return JNI_FALSE;
}

jobject getWindow(jmethodID id, va_list args) {
    fjni_log_info("getWindow(void)");
    return (jobject)0x42424242;
}

void setVolumeControlStream(jmethodID id, va_list args) {
    jint arg1 = va_arg(args, jint);
    fjni_logv_info("setVolumeControlStream(%d)", arg1);
}

jboolean requestWindowFeature(jmethodID id, va_list args) {
    jint arg1 = va_arg(args, jint);
    fjni_logv_info("requestWindowFeature(%d)", arg1);
    return JNI_FALSE;
}

void setContentView(jmethodID id, va_list args) {
    jobject arg1 = va_arg(args, jobject);
    fjni_logv_info("setContentView(%p)", arg1);
}

jobject getIntent(jmethodID id, va_list args) {
    fjni_log_info("getIntent(void)");
    return (jobject)0x42424242;
}

void finish(jmethodID id, va_list args) {
    fjni_log_info("finish(void)");
}

jobject getWindowManager(jmethodID id, va_list args) {
    fjni_log_info("getWindowManager(void)");
    return (jobject)0x42424242;
}

jint HasPushNotification(jmethodID id, va_list args) {
    //fjni_log_info("HasPushNotification(void)");
    return (jint)0;
}

jint LaunchGamebyNotification(jmethodID id, va_list args) {
    //fjni_log_info("LaunchGamebyNotification(void)");
    return (jint)0;
}

void ResetNotificationStatus(jmethodID id, va_list args) {
    fjni_log_info("ResetNotificationStatus(void)");
}

void ResetLaunchStatus(jmethodID id, va_list args) {
    fjni_log_info("ResetLaunchStatus(void)");
}

void EnableDisablePushNotifications(jmethodID id, va_list args) {
    jint arg1 = va_arg(args, jint);
    fjni_logv_info("EnableDisablePushNotifications(%d)", arg1);
}

void ShowAlert(jmethodID id, va_list args) {
    fjni_log_info("ShowAlert(void)");
}

jobject GetTokenID(jmethodID id, va_list args) {
    fjni_log_info("GetTokenID(void)");
    return (jobject)strdup("dummy");
}

jobject GetUserID(jmethodID id, va_list args) {
    fjni_log_info("GetUserID(void)");
    return (jobject)strdup("dummy");
}

jobject SendPush(jmethodID id, va_list args) {
    jobject arg1 = va_arg(args, jobject);
    jstring arg2 = va_arg(args, jstring);
    jstring arg3 = va_arg(args, jstring);
    fjni_logv_info("SendPush(%p, \"%s\", \"%s\")", arg1, arg2, arg3);
    return (jobject)strdup("dummy");
}

jobject SendPushToMyself(jmethodID id, va_list args) {
    jobject arg1 = va_arg(args, jobject);
    jstring arg2 = va_arg(args, jstring);
    jstring arg3 = va_arg(args, jstring);
    fjni_logv_info("SendPushToMyself(%p, \"%s\", \"%s\")", arg1, arg2, arg3);
    return (jobject)strdup("dummy");
}

void SetOfflineUserCredential(jmethodID id, va_list args) {
    jstring arg1 = va_arg(args, jstring);
    jstring arg2 = va_arg(args, jstring);
    fjni_logv_info("SetOfflineUserCredential(\"%s\", \"%s\")", arg1, arg2);
}

void SetOnlineUserCredential(jmethodID id, va_list args) {
    jstring arg1 = va_arg(args, jstring);
    jstring arg2 = va_arg(args, jstring);
    fjni_logv_info("SetOnlineUserCredential(\"%s\", \"%s\")", arg1, arg2);
}

jint SetOfflineDeviceCredential(jmethodID id, va_list args) {
    fjni_log_info("SetOfflineDeviceCredential(void)");
    return (jint)0;
}

jint DeletePush(jmethodID id, va_list args) {
    jstring arg1 = va_arg(args, jstring);
    fjni_logv_info("DeletePush(\"%s\")", arg1);
    return (jint)0;
}

jobject GetBundleData(jmethodID id, va_list args) {
    fjni_log_info("GetBundleData(void)");
    return (jobject)0x42424242;
}

jobject GetJanusToken(jmethodID id, va_list args) {
    jstring arg1 = va_arg(args, jstring);
    jstring arg2 = va_arg(args, jstring);
    jstring arg3 = va_arg(args, jstring);
    fjni_logv_info("GetJanusToken(\"%s\", \"%s\", \"%s\")", arg1, arg2, arg3);
    return (jobject)strdup("dummy");
}

jobject getString(jmethodID id, va_list args) {
    jstring arg1 = va_arg(args, jstring);
    fjni_logv_info("getString(\"%s\")", arg1);
    return (jobject)strdup("dummy");
}

jobject getIMEI(jmethodID id, va_list args) {
    fjni_log_info("getString()");
    return (jobject)strdup("351066496380730");
}

jobject getHDIDFVd(jmethodID id, va_list args) {
    fjni_log_info("getHDIDFVd()");
    return (jobject)strdup("351066496380730");
}

jobject getCountryCode(jmethodID id, va_list args) {
    fjni_log_info("getCountryCode()");
    return (jobject)strdup("us");
}

jobject getDeviceName(jmethodID id, va_list args) {
    fjni_log_info("getDeviceName()");
    return (jobject)strdup("Xperia Play");
}

jobject getDeviceFW(jmethodID id, va_list args) {
    fjni_log_info("getDeviceFW()");
    return (jobject)strdup("4.1.2");
}

jint IsWifiEnabled(jmethodID id, va_list args) {
    fjni_log_info("IsWifiEnabled()");
    return (jint)1;
}

NameToMethodID nameToMethodId[] = {
    { 102, "getPreferenceString", METHOD_TYPE_OBJECT },
    { 103, "getPackage", METHOD_TYPE_OBJECT },
    { 104, "getSaveFolder", METHOD_TYPE_OBJECT },
    { 105, "getContext", METHOD_TYPE_OBJECT },
    { 106, "getUserAgent", METHOD_TYPE_OBJECT },
    { 107, "getResource", METHOD_TYPE_UNKNOWN },
    { 108, "setupPaths", METHOD_TYPE_VOID },
    { 109, "createView", METHOD_TYPE_VOID },
    { 110, "setViewSettings", METHOD_TYPE_VOID },
    { 111, "setCurrentContext", METHOD_TYPE_BOOLEAN },
    { 112, "enableAccelerometer", METHOD_TYPE_VOID },
    { 113, "enableGyroscope", METHOD_TYPE_VOID },
    { 114, "openBrowser", METHOD_TYPE_VOID },
    { 115, "openGLLive", METHOD_TYPE_VOID },
    { 116, "notifyTrophy", METHOD_TYPE_VOID },
    { 117, "openIGP", METHOD_TYPE_VOID },
    { 118, "getDeviceLanguage", METHOD_TYPE_OBJECT },
    { 119, "setCalibratingGyro", METHOD_TYPE_VOID },
    { 120, "resetGyroCalibration", METHOD_TYPE_VOID },
    { 121, "setVisibility", METHOD_TYPE_VOID },
    { 122, "getText", METHOD_TYPE_OBJECT },
    { 123, "setText", METHOD_TYPE_VOID },
    { 124, "getSystemService", METHOD_TYPE_OBJECT },
    { 125, "hasGyroscope", METHOD_TYPE_BOOLEAN },
    { 126, "isXperiaPlay", METHOD_TYPE_BOOLEAN },
    { 127, "isDeviceOpened", METHOD_TYPE_BOOLEAN },
    { 128, "mustSwapButtons", METHOD_TYPE_BOOLEAN },
    { 129, "isHTCDevice", METHOD_TYPE_BOOLEAN },
    { 130, "isGyroSensibilityHigh", METHOD_TYPE_BOOLEAN },
    { 131, "isGyroSensibilityLow", METHOD_TYPE_BOOLEAN },
    { 132, "isGyroPendingToFix", METHOD_TYPE_BOOLEAN },
    { 133, "ignoreGyroLowInc", METHOD_TYPE_BOOLEAN },
    { 134, "isGyroInvertedX", METHOD_TYPE_BOOLEAN },
    { 135, "isInvertedPitchYaw", METHOD_TYPE_BOOLEAN },
    { 136, "isSwitchRollToYaw", METHOD_TYPE_BOOLEAN },
    { 137, "useGeneration0", METHOD_TYPE_BOOLEAN },
    { 138, "useGeneration1", METHOD_TYPE_BOOLEAN },
    { 139, "useGeneration2", METHOD_TYPE_BOOLEAN },
    { 140, "getProcessorNum", METHOD_TYPE_INT },
    { 141, "isFirstRunOfTheDay", METHOD_TYPE_BOOLEAN },
    { 142, "isHoneycombDevice", METHOD_TYPE_BOOLEAN },
    { 143, "isIceCreamSandwich", METHOD_TYPE_BOOLEAN },
    { 144, "isSamsung_I9250", METHOD_TYPE_BOOLEAN },
    { 145, "isSamsung_I510", METHOD_TYPE_BOOLEAN },
    { 146, "isSamsung_P7100", METHOD_TYPE_BOOLEAN },
    { 147, "isAsus_TF201", METHOD_TYPE_BOOLEAN },
    { 148, "isHTC_Nexus9", METHOD_TYPE_BOOLEAN },
    { 149, "getZipName", METHOD_TYPE_OBJECT },
    { 150, "getPatchFileLocation", METHOD_TYPE_OBJECT },
    { 151, "deleteOldData", METHOD_TYPE_BOOLEAN },
    { 152, "sendURLRequest", METHOD_TYPE_VOID },
    { 153, "getServerResponse", METHOD_TYPE_OBJECT },
    { 154, "hasServerResponse", METHOD_TYPE_BOOLEAN },
    { 155, "showDisclaimer", METHOD_TYPE_VOID },
    { 156, "showCantGoBackPopup", METHOD_TYPE_VOID },
    { 157, "showParentButton", METHOD_TYPE_VOID },
    { 158, "hideParentButton", METHOD_TYPE_VOID },
    { 159, "getTotalItems", METHOD_TYPE_INT },
    { 160, "getData", METHOD_TYPE_OBJECT },
    { 161, "GetState", METHOD_TYPE_INT },
    { 162, "android/content/Intent/<init>", METHOD_TYPE_OBJECT },
    { 163, "setClassName", METHOD_TYPE_OBJECT },
    { 164, "putExtra", METHOD_TYPE_OBJECT },
    { 165, "android/os/Bundle/<init>", METHOD_TYPE_OBJECT },
    { 166, "putString", METHOD_TYPE_VOID },
    { 167, "getByteArray", METHOD_TYPE_UNKNOWN },
    { 168, "putByteArray", METHOD_TYPE_VOID },
    { 169, "getInt", METHOD_TYPE_INT },
    { 170, "putInt", METHOD_TYPE_VOID },
    { 171, "getLong", METHOD_TYPE_LONG },
    { 172, "putLong", METHOD_TYPE_VOID },
    { 173, "containsKey", METHOD_TYPE_BOOLEAN },
    { 174, "clear", METHOD_TYPE_VOID },
    { 175, "getSDFolder", METHOD_TYPE_OBJECT },
    { 176, "showloading", METHOD_TYPE_VOID },

    { 177, "android/media/AudioTrack/<init>", METHOD_TYPE_OBJECT },
    { 178, "getMinBufferSize", METHOD_TYPE_INT },
    { 179, "play", METHOD_TYPE_VOID },
    { 180, "pause", METHOD_TYPE_VOID },
    { 181, "stop", METHOD_TYPE_VOID },
    { 182, "release", METHOD_TYPE_VOID },
    { 183, "write", METHOD_TYPE_INT },
    { 184, "getPlayState", METHOD_TYPE_INT },
    { 185, "getNativeOutputSampleRate", METHOD_TYPE_INT },
    { 186, "setThreadPriority", METHOD_TYPE_VOID },
    { 187, "setFlags", METHOD_TYPE_OBJECT },
    { 188, "getIntExtra", METHOD_TYPE_INT },
    { 189, "removeExtra", METHOD_TYPE_VOID },
    { 190, "addCategory", METHOD_TYPE_OBJECT },
    { 191, "startActivity", METHOD_TYPE_VOID },
    { 192, "startActivityForResult", METHOD_TYPE_VOID },
    { 193, "hasWindowFocus", METHOD_TYPE_BOOLEAN },
    { 194, "getWindow", METHOD_TYPE_OBJECT },
    { 195, "setVolumeControlStream", METHOD_TYPE_VOID },
    { 196, "requestWindowFeature", METHOD_TYPE_BOOLEAN },
    { 197, "setContentView", METHOD_TYPE_VOID },
    { 198, "getIntent", METHOD_TYPE_OBJECT },
    { 199, "finish", METHOD_TYPE_VOID },
    { 200, "getWindowManager", METHOD_TYPE_OBJECT },
    { 201, "hideloading", METHOD_TYPE_VOID },
    { 202, "hidelogoscreen", METHOD_TYPE_VOID },
    { 203, "HasPushNotification", METHOD_TYPE_INT },
    { 204, "LaunchGamebyNotification", METHOD_TYPE_INT },
    { 205, "ResetNotificationStatus", METHOD_TYPE_VOID },
    { 206, "ResetLaunchStatus", METHOD_TYPE_VOID },
    { 207, "EnableDisablePushNotifications", METHOD_TYPE_VOID },
    { 208, "ShowAlert", METHOD_TYPE_VOID },
    { 209, "GetTokenID", METHOD_TYPE_OBJECT },
    { 210, "GetUserID", METHOD_TYPE_OBJECT },
    { 211, "SendPush", METHOD_TYPE_OBJECT },
    { 212, "SendPushToMyself", METHOD_TYPE_OBJECT },
    { 213, "SetOfflineUserCredential", METHOD_TYPE_VOID },
    { 214, "SetOnlineUserCredential", METHOD_TYPE_VOID },
    { 215, "SetOfflineDeviceCredential", METHOD_TYPE_INT },
    { 216, "DeletePush", METHOD_TYPE_INT },
    { 217, "GetBundleData", METHOD_TYPE_OBJECT },
    { 218, "GetJanusToken", METHOD_TYPE_OBJECT },
    { 219, "getString", METHOD_TYPE_OBJECT },
    { 220, "getIMEI", METHOD_TYPE_OBJECT },
    { 221, "getHDIDFVd", METHOD_TYPE_OBJECT },
    { 222, "getCountryCode", METHOD_TYPE_OBJECT },
    { 223, "getDeviceName", METHOD_TYPE_OBJECT },
    { 224, "getDeviceFW", METHOD_TYPE_OBJECT },
    { 225, "IsWifiEnabled", METHOD_TYPE_INT },
};
MethodsBoolean methodsBoolean[] = {
    { 111, setCurrentContext },
    { 125, hasGyroscope },
    { 126, isXperiaPlay },
    { 127, isDeviceOpened },
    { 128, mustSwapButtons },
    { 129, isHTCDevice },
    { 130, isGyroSensibilityHigh },
    { 131, isGyroSensibilityLow },
    { 132, isGyroPendingToFix },
    { 133, ignoreGyroLowInc },
    { 134, isGyroInvertedX },
    { 135, isInvertedPitchYaw },
    { 136, isSwitchRollToYaw },
    { 137, useGeneration0 },
    { 138, useGeneration1 },
    { 139, useGeneration2 },
    { 141, isFirstRunOfTheDay },
    { 142, isHoneycombDevice },
    { 143, isIceCreamSandwich },
    { 144, isSamsung_I9250 },
    { 145, isSamsung_I510 },
    { 146, isSamsung_P7100 },
    { 147, isAsus_TF201 },
    { 148, isHTC_Nexus9 },
    { 151, deleteOldData },
    { 154, hasServerResponse },
    { 173, containsKey },
    { 193, hasWindowFocus },
    { 196, requestWindowFeature },
};

MethodsByte methodsByte[] = {
};

MethodsChar methodsChar[] = {
};

MethodsDouble methodsDouble[] = {
};

MethodsFloat methodsFloat[] = {
};

MethodsInt methodsInt[] = {
    { 140, getProcessorNum },
    { 159, getTotalItems },
    { 161, GetState },
    { 169, getInt },
    { 178, getMinBufferSize },
    { 183, audio_write },
    { 184, getPlayState },
    { 185, getNativeOutputSampleRate },
    { 188, getIntExtra },
    { 203, HasPushNotification },
    { 204, LaunchGamebyNotification },
    { 215, SetOfflineDeviceCredential },
    { 216, DeletePush },
    { 225, IsWifiEnabled },
};

MethodsLong methodsLong[] = {
    { 171, getLong },
};

MethodsObject methodsObject[] = {
    { 102, getPreferenceString },
    { 103, getPackage },
    { 104, getSaveFolder },
    { 105, getContext },
    { 106, getUserAgent },
    { 118, getDeviceLanguage },
    { 122, getText },
    { 124, getSystemService },
    { 149, getZipName },
    { 150, getPatchFileLocation },
    { 153, getServerResponse },
    { 160, getData },
    { 163, setClassName },
    { 164, putExtra },
    { 165, androidOsBundleInit },
    { 162, androidContentIntentInit },
    { 175, getSDFolder },
    { 177, AudioTrack_init },
    { 187, setFlags },
    { 190, addCategory },
    { 194, getWindow },
    { 198, getIntent },
    { 200, getWindowManager },
    { 209, GetTokenID },
    { 210, GetUserID },
    { 211, SendPush },
    { 212, SendPushToMyself },
    { 217, GetBundleData },
    { 218, GetJanusToken },
    { 219, getString },
    { 220, getIMEI },
    { 221, getHDIDFVd },
    { 222, getCountryCode },
    { 223, getDeviceName },
    { 224, getDeviceFW },
};

MethodsShort methodsShort[] = {
};

MethodsVoid methodsVoid[] = {
    { 108, setupPaths },
    { 109, createView },
    { 110, setViewSettings },
    { 112, enableAccelerometer },
    { 113, enableGyroscope },
    { 114, openBrowser },
    { 115, openGLLive },
    { 116, notifyTrophy },
    { 117, openIGP },
    { 119, setCalibratingGyro },
    { 120, resetGyroCalibration },
    { 121, setVisibility },
    { 123, setText },
    { 152, sendURLRequest },
    { 155, showDisclaimer },
    { 156, showCantGoBackPopup },
    { 157, showParentButton },
    { 158, hideParentButton },
    { 166, putString },
    { 168, putByteArray },
    { 170, putInt },
    { 172, putLong },
    { 174, clear },
    { 176, showloading },
    { 179, play },
    { 180, pause },
    { 181, stop },
    { 182, release },
    { 186, setThreadPriority },
    { 189, removeExtra },
    { 191, startActivity },
    { 192, startActivityForResult },
    { 195, setVolumeControlStream },
    { 197, setContentView },
    { 199, finish },
    { 201, hideloading },
    { 202, hidelogoscreen },
    { 205, ResetNotificationStatus },
    { 206, ResetLaunchStatus },
    { 207, EnableDisablePushNotifications },
    { 208, ShowAlert },
    { 213, SetOfflineUserCredential },
    { 214, SetOnlineUserCredential },
};

/*
 * JNI Fields
*/

// System-wide constant that applications sometimes request
// https://developer.android.com/reference/android/content/Context.html#WINDOW_SERVICE
char WINDOW_SERVICE[] = "window";

// System-wide constant that's often used to determine Android version
// https://developer.android.com/reference/android/os/Build.VERSION.html#SDK_INT
// Possible values: https://developer.android.com/reference/android/os/Build.VERSION_CODES
const int SDK_INT = 19; // Android 4.4 / KitKat

NameToFieldID nameToFieldId[] = {
		{ 0, "WINDOW_SERVICE", FIELD_TYPE_OBJECT }, 
		{ 1, "SDK_INT", FIELD_TYPE_INT },
};

FieldsBoolean fieldsBoolean[] = {};
FieldsByte fieldsByte[] = {};
FieldsChar fieldsChar[] = {};
FieldsDouble fieldsDouble[] = {};
FieldsFloat fieldsFloat[] = {};
FieldsInt fieldsInt[] = {
		{ 1, SDK_INT },
};
FieldsObject fieldsObject[] = {
		{ 0, WINDOW_SERVICE },
};
FieldsLong fieldsLong[] = {};
FieldsShort fieldsShort[] = {};

__FALSOJNI_IMPL_CONTAINER_SIZES
