# OpenPlayer
iOS音视频交流学习demo，包含C++播放器，人脸识别，滤镜链
掘金地址：https://juejin.cn/column/7446299097538396210

###编译
一、项目中人脸识别使用到dlib（19.24.4），dlib文件夹解压到当前文件夹即可，dlib已经编译过了，这里只需要用到*.h路径
dlib级动漫化模型源码地址：
链接: https://pan.baidu.com/s/1MMAftkxrYHbcB2-XVu_UDw 提取码: wf74

二、项目中为了探索SIFT，podfile中删除了OpenCV，使用自己编译的版本（4.9.0），由于Framework太大无法上传，可以自己根据一下编译一下。也可以恢复podfile，删除opencv相关相关代码。

git clone https://github.com/opencv/opencv.git # 切换到合适版本
git clone https://github.com/opencv/opencv_contrib.git # 切换到合适版本

cd opencv/platforms/ios
python build_framework.py ios --contrib ../../opencv_contrib --disable-swift



###说明
1. 基于FFmpeg的解码演示，音频帧由AudioToolBox处理，视频帧直接转成图片，在主线程中显示出来，没有同步；
2. 基于FFmpeg+SDL播放器，按照个人思路以C++风格实现的简单播放器；仅写了音频同步，也只处理了YUV数据，后续优化
3. 简单的GPUImage测试
4. OpenCV+dlib，68人脸点，测试Demo
5. C++实现了滤镜链，参考GPUImage，并加入了AI能力（人脸点），可以实现Mask等效果；AI以两种模式运行，同步检测到人脸点再去渲染；异步检测人脸点石渲染上一帧。。。
6. 相机基本展示，接入AI滤镜链
7. PyTorch模型接入测试，官网的segment.pt模型，自己使用PyTorch训练了动漫化gan（paddlepaddle的数据源和模型）文件位置放在百度网盘

###效果图

![效果图](demo.gif)




