#include <dlfcn.h>
#include <wctype.h>
#include <string.h>
#include <signal.h>
#include <sys/eventfd.h>
#include <sys/epoll.h>
#include <jnivm.h>
#include "native_activity.h"
#include "../../bionic/linker/linker_soinfo.h"

extern "C" void* __loader_dlopen(const char* filename, int flags, const void* caller_addr);
extern "C" void* __loader_dlsym(void* handle, const char* symbol, const void* caller_addr);
extern "C" int __loader_dl_iterate_phdr(int (*cb)(struct dl_phdr_info* info, size_t size, void* data),
                             void* data);
void solist_init();

extern "C" {
struct _hook {
    const char *name;
    void *func;
};
extern struct _hook main_hooks[];
extern struct _hook dirent_hooks[];
extern struct _hook io_hooks[];
extern struct _hook net_hooks[];
extern struct _hook pthread_hooks[];
#ifdef __APPLE__
extern struct _hook net_darwin_hooks[];
#endif
}

const char* libm_symbols[] = {
        "__sF",
        "acos",
        "acosf",
        "acosh",
        "acoshf",
        "acoshl",
        "acosl",
        "asin",
        "asinf",
        "asinh",
        "asinhf",
        "asinhl",
        "asinl",
        "atan",
        "atan2",
        "atan2f",
        "atan2l",
        "atanf",
        "atanh",
        "atanhf",
        "atanhl",
        "atanl",
        "cabsl",
        "cbrt",
        "cbrtf",
        "cbrtl",
        "ceil",
        "ceilf",
        "ceill",
        "copysign",
        "copysignf",
        "copysignl",
        "cos",
        "cosf",
        "cosh",
        "coshf",
        "coshl",
        "cosl",
        "cprojl",
        "csqrtl",
        "drem",
        "dremf",
        "erf",
        "erfc",
        "erfcf",
        "erfcl",
        "erff",
        "erfl",
        "exp",
        "exp2",
        "exp2f",
        "exp2l",
        "expf",
        "expl",
        "expm1",
        "expm1f",
        "expm1l",
        "fabs",
        "fabsf",
        "fabsl",
        "fdim",
        "fdimf",
        "fdiml",
        "feclearexcept",
        "fedisableexcept",
        "feenableexcept",
        "fegetenv",
        "fegetexcept",
        "fegetexceptflag",
        "fegetround",
        "feholdexcept",
        "feraiseexcept",
        "fesetenv",
        "fesetexceptflag",
        "fesetround",
        "fetestexcept",
        "feupdateenv",
        "finite",
        "finitef",
        "floor",
        "floorf",
        "floorl",
        "fma",
        "fmaf",
        "fmal",
        "fmax",
        "fmaxf",
        "fmaxl",
        "fmin",
        "fminf",
        "fminl",
        "fmod",
        "fmodf",
        "fmodl",
        "frexp",
        "frexpf",
        "frexpl",
        "gamma",
        "gammaf",
        "gammaf_r",
        "gamma_r",
        "hypot",
        "hypotf",
        "hypotl",
        "ilogb",
        "ilogbf",
        "ilogbl",
        "j0",
        "j0f",
        "j1",
        "j1f",
        "jn",
        "jnf",
        "ldexpf",
        "ldexpl",
        "lgamma",
        "lgammaf",
        "lgammaf_r",
        "lgammal",
        "lgammal_r",
        "lgamma_r",
        "llrint",
        "llrintf",
        "llrintl",
        "llround",
        "llroundf",
        "llroundl",
        "log",
        "log10",
        "log10f",
        "log10l",
        "log1p",
        "log1pf",
        "log1pl",
        "log2",
        "log2f",
        "log2l",
        "logb",
        "logbf",
        "logbl",
        "logf",
        "logl",
        "lrint",
        "lrintf",
        "lrintl",
        "lround",
        "lroundf",
        "lroundl",
        "modf",
        "modff",
        "modfl",
        "nan",
        "nanf",
        "nanl",
        "nearbyint",
        "nearbyintf",
        "nearbyintl",
        "nextafter",
        "nextafterf",
        "nextafterl",
        "nexttoward",
        "nexttowardf",
        "nexttowardl",
        "pow",
        "powf",
        "powl",
        "remainder",
        "remainderf",
        "remainderl",
        "remquo",
        "remquof",
        "remquol",
        "rint",
        "rintf",
        "rintl",
        "round",
        "roundf",
        "roundl",
        "scalb",
        "scalbf",
        "scalbln",
        "scalblnf",
        "scalblnl",
        "scalbn",
        "scalbnf",
        "scalbnl",
        "__signbit",
        "__signbitf",
        "__signbitl",
        "signgam",
        "significand",
        "significandf",
        "significandl",
        "sin",
        "sincos",
        "sincosf",
        "sincosl",
        "sinf",
        "sinh",
        "sinhf",
        "sinhl",
        "sinl",
        "sqrt",
        "sqrtf",
        "sqrtl",
        "tan",
        "tanf",
        "tanh",
        "tanhf",
        "tanhl",
        "tanl",
        "tgamma",
        "tgammaf",
        "tgammal",
        "trunc",
        "truncf",
        "truncl",
        "y0",
        "y0f",
        "y1",
        "y1f",
        "yn",
        "ynf",
        "isnan",
        nullptr
};

const char* egl_symbols[] = {
        "eglGetCurrentDisplay",
        "eglChooseConfig",
        "eglGetError",
        "eglCreateWindowSurface",
        "eglGetConfigAttrib",
        "eglCreateContext",
        "eglDestroySurface",
        "eglSwapBuffers",
        "eglMakeCurrent",
        "eglDestroyContext",
        "eglTerminate",
        "eglGetDisplay",
        "eglInitialize",
        "eglQuerySurface",
        "eglSwapInterval",
        "eglQueryString",
        "eglGetCurrentContext",
        nullptr
};

const char* android_symbols[] = {
        "ANativeWindow_setBuffersGeometry",
        "AAssetManager_open",
        "AAsset_getLength",
        "AAsset_getBuffer",
        "AAsset_close",
        "AAsset_read",
        "AAsset_seek64",
        "AAsset_getLength64",
        "AAsset_getRemainingLength64",
        "ALooper_pollAll",
        "ANativeActivity_finish",
        "AInputQueue_getEvent",
        "AKeyEvent_getKeyCode",
        "AInputQueue_preDispatchEvent",
        "AInputQueue_finishEvent",
        "AKeyEvent_getAction",
        "AMotionEvent_getAxisValue",
        "AKeyEvent_getRepeatCount",
        "AKeyEvent_getMetaState",
        "AInputEvent_getDeviceId",
        "AInputEvent_getType",
        "AInputEvent_getSource",
        "AMotionEvent_getAction",
        "AMotionEvent_getPointerId",
        "AMotionEvent_getX",
        "AMotionEvent_getRawX",
        "AMotionEvent_getY",
        "AMotionEvent_getRawY",
        "AMotionEvent_getPointerCount",
        "AConfiguration_new",
        "AConfiguration_fromAssetManager",
        "AConfiguration_getLanguage",
        "AConfiguration_getCountry",
        "ALooper_prepare",
        "ALooper_addFd",
        "AInputQueue_detachLooper",
        "AConfiguration_delete",
        "AInputQueue_attachLooper",
        "AAssetManager_openDir",
        "AAssetDir_getNextFileName",
        "AAssetDir_close",
        "AAssetManager_fromJava",
        nullptr
};

const char* fmod_symbols[] = {
    "_ZN4FMOD6System12mixerSuspendEv",
    "_ZN4FMOD6System11mixerResumeEv",
    "_ZN4FMOD14ChannelControl7setMuteEb",
    "_ZN4FMOD14ChannelControl9setVolumeEf",
    "_ZN4FMOD14ChannelControl9isPlayingEPb",
    "_ZN4FMOD14ChannelControl4stopEv",
    "_ZN4FMOD6System9playSoundEPNS_5SoundEPNS_12ChannelGroupEbPPNS_7ChannelE",
    "_ZN4FMOD5Sound15getNumSubSoundsEPi",
    "_ZN4FMOD5Sound11getSubSoundEiPPS0_",
    "_ZN4FMOD14ChannelControl15set3DAttributesEPK11FMOD_VECTORS3_S3_",
    "_ZN4FMOD14ChannelControl15set3DAttributesEPK11FMOD_VECTORS3_",/*New x64*/
    "_ZN4FMOD14ChannelControl8setPitchEf",
    "_ZN4FMOD14ChannelControl9setPausedEb",
    "_ZN4FMOD5Sound7releaseEv",
    "_ZN4FMOD6System5closeEv",
    "_ZN4FMOD6System7releaseEv",
    "FMOD_System_Create",
    "_ZN4FMOD6System10getVersionEPj",
    "_ZN4FMOD6System9setOutputE15FMOD_OUTPUTTYPE",
    "_ZN4FMOD6System4initEijPv",
    "_ZN4FMOD6System13set3DSettingsEfff",
    "_ZN4FMOD6System18createChannelGroupEPKcPPNS_12ChannelGroupE",
    "_ZN4FMOD6System21getMasterChannelGroupEPPNS_12ChannelGroupE",
    "_ZN4FMOD12ChannelGroup8addGroupEPS0_bPPNS_13DSPConnectionE",
    "_ZN4FMOD6System23set3DListenerAttributesEiPK11FMOD_VECTORS3_S3_S3_",
    "_ZN4FMOD6System6updateEv",
    "_ZN4FMOD6System12createStreamEPKcjP22FMOD_CREATESOUNDEXINFOPPNS_5SoundE",
    "_ZN4FMOD6System11createSoundEPKcjP22FMOD_CREATESOUNDEXINFOPPNS_5SoundE",
    "_ZN4FMOD5Sound19set3DMinMaxDistanceEff",
    "_ZN4FMOD6System13getNumDriversEPi",
    "_ZN4FMOD6System13getDriverInfoEiPciP9FMOD_GUIDPiP16FMOD_SPEAKERMODES4_",
    "_ZN4FMOD6System9setDriverEi",
    "_ZN4FMOD5Sound7setModeEj",
    "_ZN4FMOD5Sound9getFormatEP15FMOD_SOUND_TYPEP17FMOD_SOUND_FORMATPiS5_",
    "_ZN4FMOD6System17getSoftwareFormatEPiP16FMOD_SPEAKERMODES1_",
    "_ZN4FMOD14ChannelControl11getDSPClockEPyS1_",
    "_ZN4FMOD14ChannelControl12addFadePointEyf",
    "_ZN4FMOD14ChannelControl8setDelayEyyb",
    "_ZN4FMOD6System17set3DNumListenersEi",
    "_ZN4FMOD6System13setFileSystemEPF11FMOD_RESULTPKcPjPPvS5_EPFS1_S5_S5_EPFS1_S5_S5_jS4_S5_EPFS1_S5_jS5_EPFS1_P18FMOD_ASYNCREADINFOS5_ESI_i",
    "FMOD_Memory_GetStats",
    "_ZN4FMOD6System11getCPUUsageEPfS1_S1_S1_S1_",
    "_ZN4FMOD6System18getChannelsPlayingEPiS1_",
    "_ZN4FMOD6System12getFileUsageEPxS1_S1_",
    nullptr
};

static void __android_log_vprint(int prio, const char *tag, const char *fmt, va_list args) {
    // Log::vlog(convertAndroidLogLevel(prio), tag, fmt, args);
}
static void __android_log_print(int prio, const char *tag, const char *fmt, ...) {
    // va_list args;
    // va_start(args, fmt);
    // Log::vlog(convertAndroidLogLevel(prio), tag, fmt, args);
    // va_end(args);
}
static void __android_log_write(int prio, const char *tag, const char *text) {
    // Log::log(convertAndroidLogLevel(prio), tag, "%s", text);
}

int main(int argc, char** argv) {
    solist_init();
    std::unordered_map<std::string, void *> symbols;
    for (size_t i = 0; main_hooks[i].name; i++) {
        symbols[main_hooks[i].name] = main_hooks[i].func;
    }
    for (size_t i = 0; dirent_hooks[i].name; i++) {
        symbols[dirent_hooks[i].name] = dirent_hooks[i].func;
    }
    for (size_t i = 0; io_hooks[i].name; i++) {
        symbols[io_hooks[i].name] = io_hooks[i].func;
    }
    for (size_t i = 0; net_hooks[i].name; i++) {
        symbols[net_hooks[i].name] = net_hooks[i].func;
    }
    for (size_t i = 0; pthread_hooks[i].name; i++) {
        symbols[pthread_hooks[i].name] = pthread_hooks[i].func;
    }
    auto h = dlopen("libm.so.6", RTLD_LAZY);
    for (size_t i = 0; libm_symbols[i]; i++) {
        symbols[libm_symbols[i]] = dlsym(h, libm_symbols[i]);
    }
    symbols["newlocale"] = (void*)newlocale;
    symbols["uselocale"] = (void*)uselocale;
    symbols["mbsrtowcs"] = (void*)mbsrtowcs;
    symbols["freelocale"] = (void*)freelocale;
    symbols["iswlower"] = (void*)iswlower;
    symbols["iswprint"] = (void*)iswprint;
    symbols["iswblank"] = (void*)iswblank;
    symbols["iswcntrl"] = (void*)iswcntrl;
    symbols["iswupper"] = (void*)iswupper;
    symbols["iswalpha"] = (void*)iswalpha;
    symbols["iswdigit"] = (void*)iswdigit;
    symbols["iswpunct"] = (void*)iswpunct;
    symbols["iswxdigit"] = (void*)iswxdigit;
    symbols["wcsnrtombs"] = (void*)wcsnrtombs;
    symbols["mbsnrtowcs"] = (void*)mbsnrtowcs;
    symbols["__ctype_get_mb_cur_max"] = (void*)__ctype_get_mb_cur_max;
    symbols["mbrlen"] = (void*)mbrlen;
    symbols["vasprintf"] = (void*)+ []() {

    };
    symbols["wcstol"] = (void*)wcstol;
    symbols["wcstoul"] = (void*)wcstoul;
    symbols["wcstoll"] = (void*)wcstoll;
    symbols["wcstoull"] = (void*)wcstoull;
    symbols["wcstof"] = (void*)wcstof;
    symbols["wcstod"] = (void*)wcstod;
    symbols["wcstold"] = (void*)wcstold;
    symbols["swprintf"] = (void*)swprintf;
    symbols["android_set_abort_message"] = (void*)+[](const char msg) {
        
    };
    symbols["sigemptyset"] = (void*)sigemptyset;
    symbols["sigaddset"] = (void*)sigaddset;
    symbols["arc4random"] = (void*)+[]() -> uint32_t{
        return 0;
    };
    symbols["strptime"] = (void*)+[]() {

    };
    symbols["strptime_l"] = (void*)+[]() {
        
    };
    symbols["__FD_SET_chk"] = (void*)+[]() {
        
    };
    symbols["__FD_ISSET_chk"] = (void*)+[]() {
        
    };
    
    symbols["epoll_create1"] = (void*)epoll_create1;/* (void*)+[]() {
        
    }; */
    
    symbols["eventfd"] = (void*)eventfd;

    symbols["__memcpy_chk"] = (void*) + [](void* dst, const void* src, size_t count, size_t dst_len) -> void*{
        return memcpy(dst, src, count);
    };
    symbols["__vsnprintf_chk"] = (void*) + [](char* dst, size_t supplied_size, int /*flags*/,
                               size_t dst_len_from_compiler, const char* format, va_list va) -> int {
        return vsnprintf(dst, supplied_size, format, va);
    };

    symbols["__fgets_chk"] = (void*) + [](char* dst, int supplied_size, FILE* stream, size_t dst_len_from_compiler) {
        return fgets(dst, supplied_size, stream);
    };
    
    
    // soinfo::load_library("libhybris.so", symbols);
    soinfo::load_library("libdl.so", { { std::string("dl_iterate_phdr"), (void*)&__loader_dl_iterate_phdr },
                                       { std::string("dlsym"), (void*)+ [](void* dl, const char * name)-> void* {
                                           return __loader_dlsym(dl, name, nullptr);
                                       }} });
    soinfo::load_library("libc.so", symbols);
    // symbols.clear();
    // auto h = dlopen("libm.so.6", RTLD_LAZY);
    // for (size_t i = 0; libm_symbols[i]; i++) {
    //     symbols[libm_symbols[i]] = dlsym(h, libm_symbols[i]);
    // }
    soinfo::load_library("libm.so", /* symbols */ {});
    symbols.clear();
    for (size_t i = 0; egl_symbols[i]; i++) {
        symbols[egl_symbols[i]] = (void*)+[]() {
            
        };
    }
    symbols["eglGetProcAddress"] = (void*) + [](const char* name) -> void* {
        return nullptr;
    };
    soinfo::load_library("libEGL.so", symbols);
    symbols.clear();
    symbols["__android_log_print"] = (void*) __android_log_print;
    symbols["__android_log_vprint"] = (void*) __android_log_vprint;
    symbols["__android_log_write"] = (void*) __android_log_write;

    soinfo::load_library("liblog.so", symbols);
    symbols.clear();
    for (size_t i = 0; android_symbols[i]; i++) {
        symbols[android_symbols[i]] = (void*)+[]() {
            
        };
    }
    soinfo::load_library("libandroid.so", symbols);
    soinfo::load_library("libOpenSLES.so", { });
    // soinfo::load_library("libc++_shared.so", { });
    // void * libc = __loader_dlopen("/home/christopher/Downloads/android-ndk-r20/platforms/android-29/arch-x86_64/usr/lib64/libc.so", 0, 0);
    void * libcpp = __loader_dlopen("/home/christopher/Downloads/android-ndk-r20/sources/cxx-stl/llvm-libc++/libs/x86_64/libc++_shared.so", 0, 0);
    // soinfo::load_empty_library("libc.so");
    symbols.clear();
    for (size_t i = 0; fmod_symbols[i]; i++) {
        symbols[fmod_symbols[i]] = (void*)+[]() {
            
        };
    }
    soinfo::load_library("libfmod.so", symbols);
    // void * libfmod = __loader_dlopen("/home/christopher/Downloads/android-ndk-r20/platforms/android-29/arch-x86_64/usr/lib64/libfmod.so", 0, 0);
    void * libmcpe = __loader_dlopen("/home/christopher/.local/share/mcpelauncher/versions/1.16.0.55/libs/libminecraftpe.so", 0, 0);
    // void * libdl = __loader_dlopen("/home/christopher/libdl.so", 0, 0);
    // void * libcpp = __loader_dlopen("/home/christopher/Downloads/android-ndk-r20/sources/cxx-stl/llvm-libc++/libs/x86_64/libc++_shared.so", 0, 0);
    // auto sym = (void*(*)(void*, int, size_t))__loader_dlsym(lib, "memset", nullptr);
    // auto dlopen = (void*(*)(void *handle, const char *symbol))__loader_dlsym(0, "__loader_dlsym", nullptr);
    // auto memset = dlopen(libc, "/home/christopher/Downloads/android-ndk-r20/platforms/android-29/arch-x86_64/usr/lib64/libc.so");
    // char buf[10];
    // auto res = sym(buf, 0, 10);
    // if(buf[0]) {
    //     throw 34;
    // }
    auto JNI_OnLoad = (jint (*)(JavaVM* vm, void* reserved))__loader_dlsym(libmcpe, "JNI_OnLoad", nullptr);

    auto vm = std::make_shared<jnivm::VM>();
    auto ver = JNI_OnLoad(vm->GetJavaVM(), nullptr);
    ANativeActivity activity;
    memset(&activity, 0, sizeof(ANativeActivity));
    activity.internalDataPath = "./idata/";
    activity.externalDataPath = "./edata/";
    activity.obbPath = "./oob/";
    activity.sdkVersion = 28;
    activity.vm = vm->GetJavaVM();
    // activity.assetManager = (struct AAssetManager*)23;
    ANativeActivityCallbacks callbacks;
    memset(&callbacks, 0, sizeof(ANativeActivityCallbacks));
    activity.callbacks = &callbacks;
    activity.vm->GetEnv(&(void*&)activity.env, 0);
    auto ANativeActivity_onCreate = (ANativeActivity_createFunc*)__loader_dlsym(libmcpe, "ANativeActivity_onCreate", 0);
    ANativeActivity_onCreate(&activity, nullptr, 0);
    auto nativeRegisterThis = (void(*)(JNIEnv * env, void*))__loader_dlsym(libmcpe, "Java_com_mojang_minecraftpe_MainActivity_nativeRegisterThis", 0);
    auto fakeact = std::make_shared<jnivm::Object>();
    fakeact->clazz = std::make_shared<jnivm::Class>();
    fakeact->clazz->name = "Stolen";
    fakeact->clazz->nativeprefix = "Help/I/was/Stolen";
    nativeRegisterThis(activity.env, &fakeact);
    activity.callbacks->onNativeWindowCreated(&activity, (ANativeWindow *)1);
    return 0;
}