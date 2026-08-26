#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

#include <iostream>
#include <vector>
#include <fstream>

using namespace std;

struct swapChainSupportDetails {
	VkSurfaceCapabilitiesKHR capabilities;
	vector<VkSurfaceFormatKHR> formats;
	vector<VkPresentModeKHR> presentModes;
};

class vulkanSetup {
public:
	void setup() {
		initVulkan_();
	}

	void createWindow(const int width, const int height, const char* name) {
		glfwInit();
		// change if you want to allow client to use own API. [NOT RECOMMENDED]
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		// change if you want to allow resize of window [ONLY ALLOW IF CODE WRITTEN TO ALLOW RESIZE]
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
		window_ = glfwCreateWindow(width, height, name, nullptr, nullptr);
		// can add initVulkan_() to end to initialise in creation of window [RECOMMENDED]
	}
private:
	GLFWwindow* window_;
	VkInstance instance_;
	VkPhysicalDevice physicalDevice_ = VK_NULL_HANDLE;
	VkDevice device_;
	VkQueue graphicsQueue_;
	VkSurfaceKHR surface_;
	VkSwapchainKHR swapChain_;
	vector<VkImageView> swapChainImageViews_;
	VkRenderPass renderPass_;
	VkPipelineLayout pipelineLayout_;
	VkPipeline graphicsPipeline_;
	VkCommandPool commandPool_;
	VkCommandBuffer commandBuffer_;

	void initVulkan_() {
		createInstance_();
		pickPhysicalDevice_();
		createLogicalDevice_();
		createWindowSurface_();
		createSwapChain_();
		createImageViews_();
		createRenderPass_();
		createGraphicsPipeline_();
		createCommandPool_();
		createCommandBuffer_();
	}

	void createInstance_() {
		VkApplicationInfo appInfo {};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = "Application Name";
		appInfo.apiVersion = VK_API_VERSION_1_4;
		// add engine name if wanted.
		// add application version if wanted.

		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		VkInstanceCreateInfo createInfo {};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appInfo;
		createInfo.enabledExtensionCount = glfwExtensionCount;
		createInfo.ppEnabledExtensionNames = glfwExtensions;

		if(vkCreateInstance(&createInfo, nullptr, &instance_) != VK_SUCCESS) {
			cerr << "Failed to create Vulkan Instatnce!";
		}
	}

	void pickPhysicalDevice_() {
		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices(instance_, &deviceCount, nullptr);

		if (deviceCount == 0) {
			cerr << "Failed to find GPU with Vulkan support!";
			return;
		}

		vector<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(instance_, &deviceCount, devices.data());

		physicalDevice_ = devices[0];
	}

	void createLogicalDevice_() {
		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice_, &queueFamilyCount, nullptr);
		vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice_, &queueFamilyCount, queueFamilies.data());
		int graphicsFamily = -1;

		for (uint32_t i = 0; i < queueFamilyCount; i++) {
			if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
				graphicsFamily = i;
				break;
			}
		}

		float queuePriority = 1.0f;

		VkDeviceQueueCreateInfo queueCreateInfo {};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = graphicsFamily;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;

		VkDeviceCreateInfo createInfo {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		createInfo.queueCreateInfoCount = 1;
		createInfo.pQueueCreateInfos = &queueCreateInfo;

		if (vkCreateDevice(physicalDevice_, &createInfo, nullptr, &device_) != VK_SUCCESS) {
			cerr << "Failed to create logical device!";
		}

		vkGetDeviceQueue(device_, graphicsFamily, 0, &graphicsQueue_);
	}

	void createWindowSurface_() {
		if (glfwCreateWindowSurface(instance_, window_, nullptr, &surface_) != VK_SUCCESS) {
			cerr << "Failed to create window surface!";
		}
	}

	 swapChainSupportDetails querySwapChainSupport_(VkPhysicalDevice device) {
		 swapChainSupportDetails details;
		 vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface_, &details.capabilities);
		 uint32_t formatCount;
		 vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface_, &formatCount, nullptr);

		 if (formatCount != 0) {
			 details.formats.resize(formatCount);
			 vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface_, &formatCount, details.formats.data());
		 }

		 uint32_t presentModeCount;
		 vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface_, &presentModeCount, nullptr);

		 if (presentModeCount != 0) {
			 details.presentModes.resize(presentModeCount);
			 vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface_, &presentModeCount, details.presentModes.data());
		 }

		 return details;
	 }

	 VkSurfaceFormatKHR chooseSwapSurfaceFormat_(const vector<VkSurfaceFormatKHR>& availableFormats) {
		 for (const auto& availableFormat : availableFormats) {
			 if(availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) { // [RECOMMENDED] (But depending on OS support this can change)
				 return availableFormat;
			 }
		 }
		 return availableFormats[0];
	 }

	 VkPresentModeKHR chooseSwapPresentMode_(const vector<VkPresentModeKHR>& availablePresentModes) {
		 for (const auto& availablePresentMode : availablePresentModes) {
			 if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) { // [RECOMMENDED] (FIFO can be used though in most cases)
				 return availablePresentMode;
			 }
		 }
		 return availablePresentModes[0];
	 }

	 // Add 3D for 3D models support
	 VkExtent2D chooseSwapExtent_(const VkSurfaceCapabilitiesKHR& capabilities) {
		 if (capabilities.currentExtent.width != UINT32_MAX) {
			 return capabilities.currentExtent;
		 } else {
			 int width, height;
			 glfwGetFramebufferSize(window_, &width, &height);

			 VkExtent2D actualExtent = {
					 static_cast<uint32_t>(width),
					 static_cast<uint32_t>(height)
			 };

			 actualExtent.width = max(capabilities.minImageExtent.width, min(capabilities.maxImageExtent.width, actualExtent.width));
			 actualExtent.height = max(capabilities.minImageExtent.height, min(capabilities.maxImageExtent.height, actualExtent.height));

			 return actualExtent;
		 }
	 }

	 void createSwapChain_() {
		 swapChainSupportDetails swapChainSupport = querySwapChainSupport_(physicalDevice_);
		 VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat_(swapChainSupport.formats);
		 VkPresentModeKHR presentMode = chooseSwapPresentMode_(swapChainSupport.presentModes);
		 VkExtent2D extent = chooseSwapExtent_(swapChainSupport.capabilities);

		 uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
		 if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
			 imageCount = swapChainSupport.capabilities.maxImageCount;
		 }

		 VkSwapchainCreateInfoKHR createInfo {};
		 createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		 createInfo.surface = surface_;
		 createInfo.minImageCount = imageCount;
		 createInfo.imageFormat = surfaceFormat.format;
		 createInfo.imageColorSpace = surfaceFormat.colorSpace;
		 createInfo.imageExtent = extent;
		 createInfo.imageArrayLayers = 1;
		 createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT; // [RECOMMENDED]
		 createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE; // [RECOMMENDED]
		 createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
		 createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR; // [RECOMMENDED]
		 createInfo.presentMode = presentMode;
		 createInfo.clipped = VK_TRUE; // [RECOMMENDED]
		 createInfo.oldSwapchain = VK_NULL_HANDLE; // [HIGHLY RECOMMENDED]

		 if (vkCreateSwapchainKHR(device_, &createInfo, nullptr, &swapChain_) != VK_SUCCESS) {
			 cerr << "Failed to create swap chain!";
		 }
	 }

	 void createImageViews_() {
		 uint32_t imageCount = 0;
		 vkGetSwapchainImagesKHR(device_, swapChain_, &imageCount, nullptr);
		 vector<VkImage> swapChainImages(imageCount);
		 vkGetSwapchainImagesKHR(device_, swapChain_, &imageCount, swapChainImages.data());
		 swapChainImageViews_.resize(swapChainImages.size());

		 for (size_t i = 0; i < swapChainImages.size(); i++) {
			 VkImageViewCreateInfo createInfo {};
			 createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			 createInfo.image = swapChainImages[i];
			 createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			 createInfo.format = chooseSwapSurfaceFormat_(querySwapChainSupport_(physicalDevice_).formats).format;

			 createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			 createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			 createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			 createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

			 if (vkCreateImageView(device_, &createInfo, nullptr, &swapChainImageViews_[i]) != VK_SUCCESS) {
				 cerr << "Failed to create Image View!";
			 }
		 }
	 }

	 void createRenderPass_() {
		 VkAttachmentDescription colorAttachment {};
		 colorAttachment.format = chooseSwapSurfaceFormat_(querySwapChainSupport_(physicalDevice_).formats).format;
		 colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		 colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		 colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		 colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE; // should work with most cases
		 colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE; // should work with most cases
		 colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		 colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		 VkAttachmentReference colorAttachmentRef {};
		 colorAttachmentRef.attachment = 0;
		 colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		 VkSubpassDescription subpass {};
		 subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		 subpass.colorAttachmentCount = 1;
		 subpass.pColorAttachments = &colorAttachmentRef;

		 VkSubpassDependency dependency {};
		 dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		 dependency.dstSubpass = 0;
		 dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		 dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		 dependency.srcAccessMask = 0;
		 dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		 VkRenderPassCreateInfo renderPassInfo {};
		 renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		 renderPassInfo.attachmentCount = 1;
		 renderPassInfo.pAttachments = &colorAttachment;
		 renderPassInfo.subpassCount = 1;
		 renderPassInfo.pSubpasses = &subpass;
		 renderPassInfo.dependencyCount = 1;
		 renderPassInfo.pDependencies = &dependency;

		 if (vkCreateRenderPass(device_, &renderPassInfo, nullptr, &renderPass_) != VK_SUCCESS) {
			 cerr << "Failed to create render pass!";
		 }
	 }

	 vector<char> readFile_(const string& filename) {
		 ifstream file(filename, ios::ate | ios::binary);
		 if (!file.is_open()) {
			 cerr << "Failed to open file!";
			 return {};
		 }
		 size_t fileSize = (size_t)file.tellg();
		 vector<char> buffer(fileSize);
		 file.seekg(0);
		 file.read(buffer.data(), fileSize);
		 file.close();
		 return buffer;
	 }

	 VkShaderModule createShaderModule_(const vector<char>& code) {
		 VkShaderModuleCreateInfo createInfo {};
		 createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		 createInfo.codeSize = code.size();
		 createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

		 VkShaderModule shaderModule;
		 if (vkCreateShaderModule(device_, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
			 cerr << "Failed to create shader module!";
		 }
		 return shaderModule;
	 }

	 void createGraphicsPipeline_() {
		 auto vertShaderCode = readFile_("vert.spv"); // change vert.spv to what your .spv file directory is
		 auto fragShaderCode = readFile_("frag.spv"); // change frag.spv to what your .spv file directory is
		 VkShaderModule vertShaderModule = createShaderModule_(vertShaderCode);
		 VkShaderModule fragShaderModule = createShaderModule_(fragShaderCode);

		 VkPipelineShaderStageCreateInfo vertShaderStageInfo {};
		 vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		 vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
		 vertShaderStageInfo.module = vertShaderModule;
		 vertShaderStageInfo.pName = "main";

		 VkPipelineShaderStageCreateInfo fragShaderStageInfo {};
		 fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		 fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		 fragShaderStageInfo.module = fragShaderModule;
		 fragShaderStageInfo.pName = "main";

		 VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

		 VkPipelineVertexInputStateCreateInfo vertexInputInfo {};
		 vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		 vertexInputInfo.vertexBindingDescriptionCount = 0;
		 vertexInputInfo.vertexAttributeDescriptionCount = 0;

		 VkPipelineInputAssemblyStateCreateInfo inputAssembly {};
		 inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		 inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		 inputAssembly.primitiveRestartEnable = VK_FALSE;

		 VkExtent2D swapChainExtent = chooseSwapExtent_(querySwapChainSupport_(physicalDevice_).capabilities);

		 VkViewport viewport {};
		 viewport.x = 0.0f;
		 viewport.y = 0.0f;
		 viewport.width = (float)swapChainExtent.width;
		 viewport.height = (float)swapChainExtent.height;
		 viewport.minDepth = 0.0f;
		 viewport.maxDepth = 1.0f;

		 VkRect2D scissor {};
		 scissor.offset = {0, 0};
		 scissor.extent = swapChainExtent;

		 VkPipelineViewportStateCreateInfo viewportState {};
		 viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		 viewportState.viewportCount = 1;
		 viewportState.pViewports = &viewport;
		 viewportState.scissorCount = 1;
		 viewportState.pScissors = &scissor;

		 VkPipelineRasterizationStateCreateInfo rasterizer {};
		 rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		 rasterizer.depthClampEnable = VK_FALSE;
		 rasterizer.rasterizerDiscardEnable = VK_FALSE;
		 rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
		 rasterizer.lineWidth = 1.0f;
		 rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
		 rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE; // change this to set which direction it renders (clockwise or anti-clockwise)
		 rasterizer.depthBiasEnable = VK_FALSE;

		 VkPipelineMultisampleStateCreateInfo multisampling {};
		 multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		 multisampling.sampleShadingEnable = VK_FALSE;
		 multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

		 VkPipelineColorBlendAttachmentState colorBlendAttachment {};
		 colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		 colorBlendAttachment.blendEnable = VK_FALSE;

		 VkPipelineColorBlendStateCreateInfo colorBlending {};
		 colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		 colorBlending.logicOpEnable = VK_FALSE;
		 colorBlending.attachmentCount = 1;
		 colorBlending.pAttachments = &colorBlendAttachment;

		 VkPipelineLayoutCreateInfo pipelineLayoutInfo {};
		 pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		 pipelineLayoutInfo.setLayoutCount = 0;
		 pipelineLayoutInfo.pushConstantRangeCount = 0;

		 if (vkCreatePipelineLayout(device_, &pipelineLayoutInfo, nullptr, &pipelineLayout_) != VK_SUCCESS) {
			 cerr << "Failed to create pipeline layout!";
		 }

		 VkGraphicsPipelineCreateInfo pipelineInfo {};
		 pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		 pipelineInfo.stageCount = 2;
		 pipelineInfo.pStages = shaderStages;
		 pipelineInfo.pVertexInputState = &vertexInputInfo;
		 pipelineInfo.pInputAssemblyState = &inputAssembly;
		 pipelineInfo.pViewportState = &viewportState;
		 pipelineInfo.pRasterizationState = &rasterizer;
		 pipelineInfo.pMultisampleState = &multisampling;
		 pipelineInfo.pColorBlendState = &colorBlending;
		 pipelineInfo.layout = pipelineLayout_;
		 pipelineInfo.renderPass = renderPass_;
		 pipelineInfo.subpass = 0;
		 pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

		 if (vkCreateGraphicsPipelines(device_, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline_) != VK_SUCCESS) {
			 cerr << "Failed to create graphics pipeline!";
		 }

		 vkDestroyShaderModule(device_, vertShaderModule, nullptr);
		 vkDestroyShaderModule(device_, fragShaderModule, nullptr);
	 }

	void createCommandPool_() {
		 uint32_t queueFamilyCount = 0;
		 vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice_, &queueFamilyCount, nullptr);
		 vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		 vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice_, &queueFamilyCount, queueFamilies.data());
		
		 int graphicsFamily = -1;
		 for (uint32_t i = 0; i < queueFamilyCount; i++) {
		 	if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
				graphicsFamily = i;
				break;
			}
		 }

		 VkCommandPoolCreateInfo poolInfo {};
		 poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		 poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		 poolInfo.queueFamilyIndex = graphicsFamily;

		 if (vkCreateCommandPool(device_, &poolInfo, nullptr, &commandPool_) != VK_SUCCESS) {
			 cerr << "Failed to create command pool!";
		 }
	 }

	 void createCommandBuffer_() {
		 VkCommandBufferAllocateInfo allocInfo {};
		 allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		 allocInfo.commandPool = commandPool_;
		 allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		 allocInfo.commandBufferCount = 1;

		 if (vkAllocateCommandBuffers(device_, &allocInfo, &commandBuffer_) != VK_SUCCESS) {
			 cerr << "Failed to allocate command buffers!";
		 }
	 }
};
