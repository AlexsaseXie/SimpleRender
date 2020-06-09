# SimpleRender
A simple rasterization renderer and a path tracer written with VS2015 and Qt5.7

---

## Environment

Windows 10

VS 2015

Qt 5.7.1

## Run exe

copy windeployqt.exe to ./exe/

run the follow scripts in cmd

```bash
windeployqt.exe SimpleRender.exe
windeployqt.exe PathTracer.exe
```
then you can run
```bash
SimpleRender.exe
PathTracer.exe 40[optional, the sample numbers]
```

## Build the project

VS2015 + Qt 5.7

## Results

Simple Render:

![R](/img/pipeline.gif)



Path Tracer

samp:40

![v40](/img/image_gen()~267s.png)

samp:100

![v100](/img/image_gen()~646s.png)