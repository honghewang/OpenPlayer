# OpenPlayer
iOS音视频交流学习demo，包含C++播放器，人脸识别，滤镜链

###编译
项目中人脸识别使用到dlib（19.24.4），解压到当前文件夹即可，dlib已经编译过了，这里只需要用到*.h路径

###说明
1. 基于FFmpeg的解码演示，音频帧由AudioToolBox处理，视频帧直接转成图片，在主线程中显示出来，没有同步；
2. 基于FFmpeg+SDL播放器，按照个人思路以C++风格实现的简单播放器；仅写了音频同步，也只处理了YUV数据，后续优化
3. 简单的GPUImage测试
4. OpenCV+dlib，68人脸点，dlib::get_frontal_face_detector模拟器运行很慢，使用了haarcascade_frontalface_alt检测人脸
5. C++实现了滤镜链，参考GPUImage，并加入了AI能力（人脸点），可以实现Mask等效果；AI以两种模式运行，同步检测到人脸点再去渲染；异步检测人脸点石渲染上一帧。。。
6. 相机基本展示，接入AI滤镜链
7. PyTorch模型接入测试，官网的segment.pt模型（超过100M无法上传，可以到官网下载，放到文件夹 https://github.com/pytorch/ios-demo-app）

###效果图
![效果图](demo.gif)




