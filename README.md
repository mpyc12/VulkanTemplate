# Vulkan Template
Template for Vulkan setup. Includes required code for vulkan setup. Has blank .spv files. Using slang shader language.
## What is Vulkan?
Vulkan is a graphics programming language developed by the Khronos group. You can download by going to [this](https://vulkan.lunarg.com/sdk/home) website.  
Vulkan is considered a verbose engine due to the huge amount of setup code required when developing. This is why I wrote the setup code for it.  
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
* No shader code was provided. Use slang.
