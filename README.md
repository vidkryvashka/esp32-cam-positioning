# Thesis "Application of image processing algorithms for automated device positioning"

<!-- I have esp32-cam board and a sun for now to target. Don't feel pain thinking about that camera. -->


## How it works for now

### mode detecting edges
![alt text fast9](https://github.com/vidkryvashka/esp32-cam-positioning/blob/main/demo/working.gif)

### mode looking for sun: finds brightest points, filters it, detects middle point of them and x y angle to turn camera towards it.
![alt text look4sun](https://github.com/vidkryvashka/esp32-cam-positioning/blob/main/demo/look4sun.png)