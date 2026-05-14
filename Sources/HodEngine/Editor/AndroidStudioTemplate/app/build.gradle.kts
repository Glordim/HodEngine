plugins {
    alias(libs.plugins.android.application)
}

android {
    namespace = "com.glordim.hodandroidstudiotemplate"
    compileSdk {
        version = release(36) {
            minorApiLevel = 1
        }
    }

    defaultConfig {
        applicationId = "com.glordim.hodandroidstudiotemplate"
        minSdk = 29
        targetSdk = 36
        versionCode = 1
        versionName = "1.0"
        externalNativeBuild {
            cmake {
                arguments([[PROJECT_CMAKE_ARGS]])
                abiFilters("arm64-v8a", "x86_64")
            }
        }
    }

    sourceSets {
        getByName("main") {
            assets.srcDirs("[[PROJECT_GAME_DATAS]]")
            jniLibs.srcDirs("[[JNI_LIBS_DIR]]")
        }
    }

    buildTypes {
        release {
            isMinifyEnabled = false
            proguardFiles(
                getDefaultProguardFile("proguard-android-optimize.txt"),
                "proguard-rules.pro"
            )
        }
    }
    compileOptions {
        sourceCompatibility = JavaVersion.VERSION_11
        targetCompatibility = JavaVersion.VERSION_11
    }
    buildFeatures {
        prefab = true
    }
    externalNativeBuild {
        cmake {
            path = file("[[PROJECT_GAME_CMAKELIST]]")
            version = "3.22.1"
        }
    }
    ndkVersion = "29.0.14206865"
}

dependencies {
    implementation(libs.androidx.appcompat)
    implementation(libs.androidx.core.ktx)
    implementation(files("[[EXTERNAL_JAR_DIR]]/games-activity.jar"))
    implementation(libs.material)
}