#include <windows.h>
#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include <TlHelp32.h>

#include <jni.h>

#include <jdistorm.h>

HANDLE WINAPI OpenThread(DWORD dwDesiredAccess, BOOL bInheritHandle, DWORD dwThreadId);
DWORD WINAPI GetThreadId(HANDLE Thread);

HINSTANCE instanceHandle;
JavaVM *jvm;

DWORD GetProcessThreadId(DWORD dwProcessID) {
    THREADENTRY32 te = { sizeof(THREADENTRY32) };
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);

    if(Thread32First(hSnapshot, &te)) {
        while(Thread32Next(hSnapshot, &te)) {
            if(te.th32OwnerProcessID == dwProcessID) {
                CloseHandle(hSnapshot);
                return te.th32ThreadID;
            }
        }
    }

    CloseHandle(hSnapshot);
    return 0;
}

void showConsole() {
    AllocConsole();

    HANDLE handle_out = GetStdHandle(STD_OUTPUT_HANDLE);
    int hCrt = _open_osfhandle((long) handle_out, _O_TEXT);
    FILE* hf_out = _fdopen(hCrt, "w");
    setvbuf(hf_out, NULL, _IONBF, 1);
    *stdout = *hf_out;

    HANDLE handle_in = GetStdHandle(STD_INPUT_HANDLE);
    hCrt = _open_osfhandle((long) handle_in, _O_TEXT);
    FILE* hf_in = _fdopen(hCrt, "r");
    setvbuf(hf_in, NULL, _IONBF, 128);
    *stdin = *hf_in;
}

jobject JNICALL Java_boundless_Native_getByteBuffer(JNIEnv *env, jclass clazz, jlong address, jlong capacity) {
    DWORD old;
    VirtualProtect((void *)address, capacity, PAGE_EXECUTE_READWRITE, &old);
    return (*env)->NewDirectByteBuffer(env, (void *)address, capacity);
}

jobject JNICALL Java_boundless_Native_allocMemory(JNIEnv *env, jclass clazz, jlong capacity) {
    return (*env)->NewDirectByteBuffer(env, VirtualAlloc(NULL, capacity, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE), capacity);
}

void JNICALL Java_boundless_Native_resumeLaunch(JNIEnv *env, jclass clazz) {
    HANDLE hThread = OpenThread(THREAD_SUSPEND_RESUME, FALSE, GetProcessThreadId(GetCurrentProcessId()));
    ResumeThread(hThread);
    CloseHandle(hThread);
}

jlong JNICALL Java_boundless_Native_addressOf(JNIEnv *env, jclass clazz, jobject buffer) {
    return (jlong) (*env)->GetDirectBufferAddress(env, buffer);
}

typedef void (*scall)();

void JNICALL Java_boundless_Native_call(JNIEnv *env, jclass clazz, jlong address) {
    ((scall)address)();
}

jlong JNICALL Java_boundless_Native_getJVMPointer(JNIEnv *env, jclass clazz) {
    return (jlong) jvm;
}

jlong JNICALL Java_boundless_JNIEnv_getEnvPointer(JNIEnv *env, jclass clazz) {
    return (jlong) env;
}

jlong JNICALL Java_boundless_JNIEnv_getMethodID(JNIEnv *env, jclass _clazz, jclass clazz, jstring name, jstring signature) {
    const char *utf_name, *utf_signature;
    jboolean isCopy;
    utf_name = (*env)->GetStringUTFChars(env, name, &isCopy);
    utf_signature = (*env)->GetStringUTFChars(env, signature, &isCopy);

    jmethodID methodID = (*env)->GetMethodID(env, clazz, utf_name, utf_signature);

    (*env)->ReleaseStringUTFChars(env, name, utf_name);
    (*env)->ReleaseStringUTFChars(env, signature, utf_signature);
    return (jlong) methodID;
}

jlong JNICALL Java_boundless_JNIEnv_getStaticMethodID(JNIEnv *env, jclass _clazz, jclass clazz, jstring name, jstring signature) {
    const char *utf_name, *utf_signature;
    jboolean isCopy;
    utf_name = (*env)->GetStringUTFChars(env, name, &isCopy);
    utf_signature = (*env)->GetStringUTFChars(env, signature, &isCopy);

    jmethodID methodID = (*env)->GetStaticMethodID(env, clazz, utf_name, utf_signature);

    (*env)->ReleaseStringUTFChars(env, name, utf_name);
    (*env)->ReleaseStringUTFChars(env, signature, utf_signature);
    return (jlong) methodID;
}

jlong JNICALL Java_boundless_JNIEnv_getClassPtr(JNIEnv *env, jclass _clazz, jclass clazz) {
    return (jlong) clazz;
}

typedef jboolean (*booleanCall)(void *argsAddress);
jboolean JNICALL Java_boundless_natives_call_CallInterface_callBoolean(JNIEnv *env, jclass clazz, jlong address, jobject argsBuffer) {
    return ((booleanCall) address)((*env)->GetDirectBufferAddress(env, argsBuffer));
}

typedef jbyte (*byteCall)(void *argsAddress);
jbyte JNICALL Java_boundless_natives_call_CallInterface_callByte(JNIEnv *env, jclass clazz, jlong address, jobject argsBuffer) {
    return ((byteCall) address)((*env)->GetDirectBufferAddress(env, argsBuffer));
}

typedef jchar (*charCall)(void *argsAddress);
jchar JNICALL Java_boundless_natives_call_CallInterface_callChar(JNIEnv *env, jclass clazz, jlong address, jobject argsBuffer) {
    return ((charCall) address)((*env)->GetDirectBufferAddress(env, argsBuffer));
}

typedef jshort (*shortCall)(void *argsAddress);
jshort JNICALL Java_boundless_natives_call_CallInterface_callShort(JNIEnv *env, jclass clazz, jlong address, jobject argsBuffer) {
    return ((shortCall) address)((*env)->GetDirectBufferAddress(env, argsBuffer));
}

typedef jint (*intCall)(void *argsAddress);
jint JNICALL Java_boundless_natives_call_CallInterface_callInt(JNIEnv *env, jclass clazz, jlong address, jobject argsBuffer) {
    return ((intCall) address)((*env)->GetDirectBufferAddress(env, argsBuffer));
}

typedef jlong (*longCall)(void *argsAddress);
jlong JNICALL Java_boundless_natives_call_CallInterface_callLong(JNIEnv *env, jclass clazz, jlong address, jobject argsBuffer) {
    return ((longCall) address)((*env)->GetDirectBufferAddress(env, argsBuffer));
}

typedef jfloat (*floatCall)(void *argsAddress);
jfloat JNICALL Java_boundless_natives_call_CallInterface_callFloat(JNIEnv *env, jclass clazz, jlong address, jobject argsBuffer) {
    return ((floatCall) address)((*env)->GetDirectBufferAddress(env, argsBuffer));
}

typedef jdouble (*doubleCall)(void *argsAddress);
jdouble JNICALL Java_boundless_natives_call_CallInterface_callDouble(JNIEnv *env, jclass clazz, jlong address, jobject argsBuffer) {
    return ((doubleCall) address)((*env)->GetDirectBufferAddress(env, argsBuffer));
}

typedef void (*voidCall)(void *argsAddress);
void JNICALL Java_boundless_natives_call_CallInterface_callVoid(JNIEnv *env, jclass clazz, jlong address, jobject argsBuffer) {
    ((voidCall) address)((*env)->GetDirectBufferAddress(env, argsBuffer));
}

DWORD WINAPI loadjre(LPVOID lpParam) {
    char moduleFileName[MAX_PATH];
    GetModuleFileNameA(instanceHandle, moduleFileName, MAX_PATH);

    char *jarName = "boundless.jar";
    char jarPath[MAX_PATH];

    int pathLen = (strrchr(moduleFileName, '\\') - moduleFileName) + 1;
    memcpy(jarPath, moduleFileName, pathLen);
    memcpy(jarPath + pathLen, jarName, strlen(jarName) + 1);

    HKEY jreKey;
    RegOpenKeyExA(HKEY_LOCAL_MACHINE, "SOFTWARE\\JavaSoft\\Java Runtime Environment", 0, KEY_READ, &jreKey);
    
    char jreVersion[8] = {0};   
    DWORD jreVersionSize = sizeof(jreVersion);
    RegQueryValueExA(jreKey, "CurrentVersion", 0, NULL, (LPBYTE) jreVersion, &jreVersionSize);

    HKEY jreSubKey;
    RegOpenKeyExA(jreKey, jreVersion, 0, KEY_READ, &jreSubKey);

    char jreRuntimeLib[MAX_PATH] = {0};
    DWORD jreRuntimeLibSize = sizeof(jreRuntimeLib);
    RegQueryValueExA(jreSubKey, "RuntimeLib", 0, NULL, (LPBYTE) jreRuntimeLib, &jreRuntimeLibSize);

    RegCloseKey(jreSubKey);
    RegCloseKey(jreKey);

    typedef jint (JNICALL CreateJavaVM_t)(JavaVM **pvm, JNIEnv **env, void *args);
    HMODULE jreHandle = LoadLibraryA(jreRuntimeLib);

    CreateJavaVM_t *CreateJavaVM = (CreateJavaVM_t *) GetProcAddress(jreHandle, "JNI_CreateJavaVM");

    JNIEnv *env;
    JavaVMOption vm_options[1] = { 0 };
    
    char *defineClassPath = "-Djava.class.path=";

    char classPath[MAX_PATH];
    memcpy(classPath, defineClassPath, strlen(defineClassPath));
    memcpy(classPath + strlen(defineClassPath), jarPath, strlen(jarPath) + 1);

    vm_options[0].optionString = classPath;
    
    JavaVMInitArgs vm_args;
    vm_args.version = JNI_VERSION_1_6;
    vm_args.options = vm_options;
    vm_args.nOptions = 1;
    vm_args.ignoreUnrecognized = JNI_FALSE;

    if(CreateJavaVM(&jvm, &env, &vm_args) != JNI_OK) {
        MessageBoxA(NULL, "Couldn't create java vm", "Boundless", MB_OK);
        return 1;
    }

    JNINativeMethod nativeMethods[0xff] = { 0 };
    ZeroMemory(&nativeMethods, sizeof(nativeMethods));

    nativeMethods[0].name = "getByteBuffer";
    nativeMethods[0].signature = "(JJ)Ljava/nio/ByteBuffer;";
    nativeMethods[0].fnPtr = &Java_boundless_Native_getByteBuffer;

    nativeMethods[1].name = "allocMemory";
    nativeMethods[1].signature = "(J)Ljava/nio/ByteBuffer;";
    nativeMethods[1].fnPtr = &Java_boundless_Native_allocMemory;

    nativeMethods[2].name = "resumeLaunch";
    nativeMethods[2].signature = "()V";
    nativeMethods[2].fnPtr = &Java_boundless_Native_resumeLaunch;

    nativeMethods[3].name = "addressOf";
    nativeMethods[3].signature = "(Ljava/nio/ByteBuffer;)J";
    nativeMethods[3].fnPtr = &Java_boundless_Native_addressOf;

    nativeMethods[4].name = "call";
    nativeMethods[4].signature = "(J)V";
    nativeMethods[4].fnPtr = &Java_boundless_Native_call;

    nativeMethods[5].name = "getJVMPointer";
    nativeMethods[5].signature = "()J";
    nativeMethods[5].fnPtr = &Java_boundless_Native_getJVMPointer;

    jclass boundless_Native = (*env)->FindClass(env, "boundless/Native");
    (*env)->RegisterNatives(env, boundless_Native, nativeMethods, 6);

    nativeMethods[0].name = "getEnvPointer";
    nativeMethods[0].signature = "()J";
    nativeMethods[0].fnPtr = &Java_boundless_JNIEnv_getEnvPointer;

    nativeMethods[1].name = "getMethodID";
    nativeMethods[1].signature = "(Ljava/lang/Class;Ljava/lang/String;Ljava/lang/String;)J";
    nativeMethods[1].fnPtr = &Java_boundless_JNIEnv_getMethodID;

    nativeMethods[2].name = "getStaticMethodID";
    nativeMethods[2].signature = "(Ljava/lang/Class;Ljava/lang/String;Ljava/lang/String;)J";
    nativeMethods[2].fnPtr = &Java_boundless_JNIEnv_getStaticMethodID;

    nativeMethods[3].name = "getClassPtr";
    nativeMethods[3].signature = "(Ljava/lang/Class;)J";
    nativeMethods[3].fnPtr = &Java_boundless_JNIEnv_getClassPtr;

    jclass boundless_JNIEnv = (*env)->FindClass(env, "boundless/JNIEnv");
    (*env)->RegisterNatives(env, boundless_JNIEnv, nativeMethods, 4);

    nativeMethods[0].name = "decompose";
    nativeMethods[0].signature = "(Ldistorm/CodeInfo;Ldistorm/DecomposedResult;)V";
    nativeMethods[0].fnPtr = &Java_distorm_Distorm_decompose;

    nativeMethods[1].name = "decode";
    nativeMethods[1].signature = "(Ldistorm/CodeInfo;Ldistorm/DecodedResult;)V";
    nativeMethods[1].fnPtr = &Java_distorm_Distorm_decode;

    nativeMethods[2].name = "format";
    nativeMethods[2].signature = "(Ldistorm/CodeInfo;Ldistorm/DecomposedInst;)Ldistorm/DecodedInst;";
    nativeMethods[2].fnPtr = &Java_distorm_Distorm_format;

    jclass distorm_Distorm = (*env)->FindClass(env, "distorm/Distorm");
    (*env)->RegisterNatives(env, distorm_Distorm, nativeMethods, 3);

    nativeMethods[0].name = "callBoolean";
    nativeMethods[0].signature = "(JLjava/nio/ByteBuffer;)Z";
    nativeMethods[0].fnPtr = &Java_boundless_natives_call_CallInterface_callBoolean;

    nativeMethods[1].name = "callByte";
    nativeMethods[1].signature = "(JLjava/nio/ByteBuffer;)B";
    nativeMethods[1].fnPtr = &Java_boundless_natives_call_CallInterface_callByte;

    nativeMethods[2].name = "callChar";
    nativeMethods[2].signature = "(JLjava/nio/ByteBuffer;)C";
    nativeMethods[2].fnPtr = &Java_boundless_natives_call_CallInterface_callChar;

    nativeMethods[3].name = "callShort";
    nativeMethods[3].signature = "(JLjava/nio/ByteBuffer;)S";
    nativeMethods[3].fnPtr = &Java_boundless_natives_call_CallInterface_callShort;

    nativeMethods[4].name = "callInt";
    nativeMethods[4].signature = "(JLjava/nio/ByteBuffer;)I";
    nativeMethods[4].fnPtr = &Java_boundless_natives_call_CallInterface_callInt;

    nativeMethods[5].name = "callLong";
    nativeMethods[5].signature = "(JLjava/nio/ByteBuffer;)J";
    nativeMethods[5].fnPtr = &Java_boundless_natives_call_CallInterface_callLong;

    nativeMethods[6].name = "callFloat";
    nativeMethods[6].signature = "(JLjava/nio/ByteBuffer;)F";
    nativeMethods[6].fnPtr = &Java_boundless_natives_call_CallInterface_callFloat;

    nativeMethods[7].name = "callDouble";
    nativeMethods[7].signature = "(JLjava/nio/ByteBuffer;)D";
    nativeMethods[7].fnPtr = &Java_boundless_natives_call_CallInterface_callDouble;

    nativeMethods[8].name = "callVoid";
    nativeMethods[8].signature = "(JLjava/nio/ByteBuffer;)V";
    nativeMethods[8].fnPtr = &Java_boundless_natives_call_CallInterface_callVoid;

    jclass boundless_natives_call_CallInterface = (*env)->FindClass(env, "boundless/natives/call/CallInterface");
    (*env)->RegisterNatives(env, boundless_natives_call_CallInterface, nativeMethods, 9);

    jdistorm_init(env);


    jclass boundless_Loader = (*env)->FindClass(env, "boundless/Loader");
    jmethodID boundless_Loader_main = (*env)->GetStaticMethodID(env, boundless_Loader, "main", "()V");
    (*env)->CallStaticVoidMethod(env, boundless_Loader, boundless_Loader_main);

    if((*env)->ExceptionOccurred(env)) {
        (*env)->ExceptionDescribe(env);
        (*env)->ExceptionClear(env);
    }
    // jmethodID boundless_Loader_callback = (*env)->GetStaticMethodID(env, boundless_Loader, "callback", "(II)V");
    // asm("int  $0x13");
    // (*env)->CallStaticVoidMethod(env, boundless_Loader, boundless_Loader_callback, 0xdeadbeef, 0x123456);    
    return 0;
}

BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD reason, LPVOID reserved) {
    switch(reason) {
        case DLL_PROCESS_ATTACH:
            {
                showConsole();
                instanceHandle = hInstance;
                DisableThreadLibraryCalls(hInstance);
                CreateThread(NULL, 0, loadjre, NULL, 0, NULL);
            }
            break;
        case DLL_THREAD_ATTACH:
        case DLL_THREAD_DETACH:
            // Should never happen
        case DLL_PROCESS_DETACH:
            // Ignore
            break;
    }

    return TRUE;
}