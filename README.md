# Vulkan Template
Template for setting up Vulkan. This project includes the required code for every Vulkan project. This however does not contain any shader files.
## What is Vulkan?
Vulkan is a graphics API developed by the Khronos group. You can download by going to [this](https://vulkan.lunarg.com/sdk/home) website.  
Vulkan is considered a verbose API due to the huge amount of setup code required when developing. This is why I wrote the setup code for it.  
### How to set up
In Vulkan you have to do a list of things to get the code ready for rendering. Here is a list in order:  
* Create an Instance
* Pick Physical Device (GPU / VRAM)
* Create a logical device
* Create a window surface
* Create a swap chain
* Create Image Views
* Create Render pass
* Create Graphics Pipeline
* Create a command pool
* Allocate a command buffer
## Notes
* This was made in Eclipse IDE
* No shader code was provided. Use slang shaders.
