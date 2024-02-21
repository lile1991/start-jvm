#include <jni.h>       // JNI header provided by JDK
#include <stdio.h>     // C Standard IO Header
#include <windows.h>
#include <string>


const wchar_t* toWchar(const char* orig) {
    int origLen = strlen(orig) + 1;
    int len = MultiByteToWideChar(CP_ACP, 0, orig, origLen, NULL, 0);
    wchar_t* wszPath = new wchar_t[len];
    MultiByteToWideChar(CP_ACP, 0, orig, origLen, wszPath, len);
    return wszPath;
}

int main() {
    JavaVM *jvm;       // Pointer to the JVM (Java Virtual Machine)
    JNIEnv *env;       // Pointer to native interface

    // JVM initialization arguments
    JavaVMInitArgs vm_args;                         
    
    JavaVMOption* options = new JavaVMOption[4];

    options[0].optionString = (char*) "-Djava.class.path=jdk-17.0.9/lib;."; // 设置你的类路径 (classpath)，这里只是一个例子
    options[1].optionString = (char*) "-XX:MaxPermSize=512m";
    options[2].optionString = (char*) "-Xms512m";
    options[3].optionString = (char*) "-Xmx1G";
    // options[3].optionString = (char*) "-Djava.library.path=D:/SDK/jdk-17.0.9/bin";
    vm_args.version = JNI_VERSION_10;             // JVM version. This indicates version 1.6
    vm_args.nOptions = 4;
    vm_args.options = options;
    vm_args.ignoreUnrecognized = JNI_TRUE;
    
    printf("JNI_CreateJavaVM %s\n", options[0].optionString);

    
    // LPCWSTR libPath = L"jdk-17.0.9/bin/server/jvm.dll";
    // std::string libPath = "jdk-17.0.9/bin/server/jvm.dll";
    // 将D:\SDK\jdk-17.0.9复制到当前路径, 否则执行时会提示找不到dll
    LPCSTR libPath = "jdk-17.0.9/bin/server/jvm.dll";
    HINSTANCE hVM = LoadLibrary(libPath);

    if (hVM == NULL) {
        printf("Load failed..");
    }

    typedef jint (CALLBACK *fpCJV)(JavaVM**, void**, JavaVMInitArgs*);
	fpCJV CreateJavaVM = (fpCJV)GetProcAddress(hVM, "JNI_CreateJavaVM");
	jint rc = CreateJavaVM(&jvm, (void**)&env, &vm_args);

    // 加载并初始化一个JVM，"jvm"是返回的JVM接口的指针，"env"是返回的JNI接口的指针
   
    //jint rc = JNI_CreateJavaVM(&jvm, (void**)&env, &vm_args);  
    printf("JNI_CreateJavaVM Finished!\n");
    
    if (rc != JNI_OK) {
        if (rc == JNI_EVERSION) {
            fprintf(stderr, "FATAL ERROR: JVM is oudated and doesn't meet requirements");
        }
        else if (rc == JNI_ENOMEM) {
            fprintf(stderr, "FATAL ERROR: Not enough memory for JVM");
        }
        else if (rc == JNI_EINVAL) {
            fprintf(stderr, "FATAL ERROR: invalid ragument for launching JVM");
        }
        else if (rc == JNI_EEXIST) {
            fprintf(stderr, "FATAL ERROR: the process can only launch one JVM an not multiple");
        }
        else {
            fprintf(stderr, "FATAL ERROR:  unknown error");
        }
        return rc;
    }
    else {
        jint jversion = env->GetVersion();
        printf("JVM load succeeded. Version %ld\n", jversion);
    }

    // 在这里你可以开始调用Java代码 ...
    /* invoke the Main.test method using the JNI */
    jclass cls = env->FindClass("JavaMain");
    printf("FindClass JavaMain\n");
    jmethodID mid = env->GetStaticMethodID(cls, "main", "([Ljava/lang/String;)V");
    printf("GetStaticMethodID main\n");
    env->CallStaticVoidMethod(cls, mid, 100);
    printf("CallStaticVoidMethod main(String[])\n");

    // 关闭JVM
     printf("DestroyJavaVM\n");
    jvm->DestroyJavaVM();
    
    return 0;
}
