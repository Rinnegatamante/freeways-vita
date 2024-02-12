#include <falso_jni/FalsoJNI_Impl.h>
#include <falso_jni/FalsoJNI_Logger.h>
#include <string.h>
#include <vitasdk.h>
#include <AFakeNative/keycodes.h>

// System-wide constant that's often used to determine Android version
// https://developer.android.com/reference/android/os/Build.VERSION.html#SDK_INT
// Possible values: https://developer.android.com/reference/android/os/Build.VERSION_CODES
const int SDK_INT = 19; // Android 4.4 / KitKat

/*
 * JNI Methods
*/

NameToMethodID nameToMethodId[] = {
	{ 1, "Inches", METHOD_TYPE_FLOAT },
	{ 2, "GetSystemTime", METHOD_TYPE_LONG },
	{ 3, "LoadSound", METHOD_TYPE_INT },
	{ 4, "PlaySound", METHOD_TYPE_INT },
};

MethodsBoolean methodsBoolean[] = {
};

jfloat Inches(jmethodID id, va_list args) {
    return 5.0f;
}

jlong GetSystemTime(jmethodID id, va_list args) {
    return sceKernelGetProcessTimeWide();
}

extern int load_sound(const char *name);
jint LoadSound(jmethodID id, va_list args) {
	jstring name = va_arg(args, jstring);
    return load_sound(name);
}

extern void play_sound(int id, float vol, int loop, float rate);
jint PlaySound(jmethodID id, va_list args) {
	jint sid = va_arg(args, jint);
	jfloat left_vol = va_arg(args, double);
	jfloat right_vol = va_arg(args, double);
	jint priority = va_arg(args, jint);
	jint loop = va_arg(args, jint);
	jfloat rate = va_arg(args, double);
	play_sound(sid, left_vol, loop, rate);
	return 0;
}

MethodsByte methodsByte[] = {};
MethodsChar methodsChar[] = {};
MethodsDouble methodsDouble[] = {};
MethodsFloat methodsFloat[] = {
	{ 1, Inches },
};
MethodsLong methodsLong[] = {
	{ 2, GetSystemTime },
};
MethodsShort methodsShort[] = {};

MethodsInt methodsInt[] = {
	{ 3, LoadSound },
	{ 4, PlaySound },
};

MethodsObject methodsObject[] = {
};

MethodsVoid methodsVoid[] = {
};

/*
 * JNI Fields
*/

// System-wide constant that applications sometimes request
// https://developer.android.com/reference/android/content/Context.html#WINDOW_SERVICE
char WINDOW_SERVICE[] = "window";

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
