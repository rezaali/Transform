Transform
--

### Instructions:


#### 1. Clone Cinder
```
git clone --recursive https://github.com/cinder/Cinder.git Cinder
```
#### 2. Clone Transform:
```
cd Cinder/samples
mkdir _reza
cd _reza
git clone git@github.com:rezaali/Transform.git
```

#### 3. Clone Dependencies
```
cd ../../blocks/
git clone git@github.com:rezaali/Cinder-MovieSaver.git MovieSaver
git clone git@github.com:rezaali/Cinder-Tiler.git Tiler
git clone git@github.com:rezaali/Cinder-SequenceSaver.git SequenceSaver
git clone git@github.com:rezaali/Cinder-AppPaths.git AppPaths
git clone git@github.com:rezaali/Cinder-AppUI.git AppUI
git clone git@github.com:rezaali/Cinder-LiveCode.git LiveCode
git clone git@github.com:rezaali/Cinder-ParticleSystem.git ParticleSystem
git clone git@github.com:rezaali/Cinder-GlslParams.git GlslParams
git clone git@github.com:rezaali/Cinder-EasyCamera.git EasyCamera
git clone git@github.com:rezaali/Cinder-ImageSaver.git ImageSaver
git clone git@github.com:rezaali/Cinder-SaveLoadWindow.git SaveLoadWindow
git clone git@github.com:rezaali/Cinder-SaveLoadCamera.git SaveLoadCamera
git clone git@github.com:rezaali/Watchdog.git Watchdog
git clone git@github.com:rezaali/Cinder-UI.git UI
```

#### 4. Build Cinder
```
cd ../proj/xcode/ && ./fullbuild.sh
```

#### 5. Open Transform Xcode project
```
cd ../samples/_reza/Transform/xcode
open Transform.xcodeproj/
```

#### 6. Run
In Xcode, go to the Product Menu and then Run or press Apple + R



