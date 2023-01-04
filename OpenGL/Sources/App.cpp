#include <App.hpp>

namespace Core
{
	App::~App()
	{
		mThreadManager.Destroy();
		mRscManager->Clear();

		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();

		// glfw: terminate, clearing all previously allocated GLFW resources.
		// ------------------------------------------------------------------
		glfwTerminate();
	}

	bool App::Init(AppInitializer& init)
	{

		mWidth = init.width; mHeight = init.height;
		// glfw: initialize and configure
		// ------------------------------
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, init.major);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, init.minor);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

		// glfw Window creation
		// --------------------
		Window = glfwCreateWindow(init.width, init.height, init.name, NULL, NULL);
		if (Window == NULL)
		{
			std::cout << "Failed to create GLFW Window" << std::endl;
			glfwTerminate();
			return false;
		}
		glfwMakeContextCurrent(Window);
		glfwSetFramebufferSizeCallback(Window, init.framebuffer_size_callback);


		// glad: load all OpenGL function pointers
		// ---------------------------------------
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			std::cout << "Failed to initialize GLAD" << std::endl;
			return false;
		}


		GLint flags = 0;
		glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
		if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
		{
			glEnable(GL_DEBUG_OUTPUT);
			glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
			glDebugMessageCallback(init.glDebugOutput, nullptr);
			glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
		}

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);

		glfwGetWindowSize(Window, &mWidth, &mHeight);
		mCam.Aspect = mWidth / mHeight;

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();

		ImGui::StyleColorsDark();
		ImGuiStyle& style = ImGui::GetStyle();

		ImGui_ImplGlfw_InitForOpenGL(Window, true);
		ImGui_ImplOpenGL3_Init("#version 130");

		return true;
	}

	// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
	// ---------------------------------------------------------------------------------------------------------
	void App::processInput(GLFWwindow* Window)
	{
		mPlayer.PlayerInput.moveForward = glfwGetKey(Window, mPlayer.keyBoardInput.moveForward) == GLFW_PRESS;
		mPlayer.PlayerInput.moveBackward = glfwGetKey(Window, mPlayer.keyBoardInput.moveBackward) == GLFW_PRESS;
		mPlayer.PlayerInput.moveLeft = glfwGetKey(Window, mPlayer.keyBoardInput.moveLeft) == GLFW_PRESS;
		mPlayer.PlayerInput.moveRight = glfwGetKey(Window, mPlayer.keyBoardInput.moveRight) == GLFW_PRESS;
		mPlayer.PlayerInput.moveJump = glfwGetKey(Window, mPlayer.keyBoardInput.moveJump) == GLFW_PRESS;

		if (glfwGetMouseButton(Window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
		{
			mInWindow = true;
			glfwSetInputMode(Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
		else
		{
			mInWindow = false;
			glfwSetInputMode(Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}


		double newMouseX, newMouseY;
		float deltaX, deltaY;
		glfwGetCursorPos(Window, &newMouseX, &newMouseY);
		if (mFirstMouse)
		{
			mMouseX = newMouseX;
			mMouseY = newMouseY;
			mFirstMouse = false;
		}
		deltaX = (float)(newMouseX - mMouseX);
		deltaY = (float)(mMouseY - newMouseY);
		mMouseX = newMouseX;
		mMouseY = newMouseY;

		if (!mInWindow)
			return;

		mCam.CamInputs.deltaX = deltaX;
		mCam.CamInputs.deltaY = deltaY;
		//mCam.CamInputs.moveForward = glfwGetKey(Window, GLFW_KEY_W) == GLFW_PRESS;
		//mCam.CamInputs.moveBackward = glfwGetKey(Window, GLFW_KEY_S) == GLFW_PRESS;
		//mCam.CamInputs.moveDown = glfwGetKey(Window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS;
		//mCam.CamInputs.moveUp = glfwGetKey(Window, GLFW_KEY_SPACE) == GLFW_PRESS;
		//mCam.CamInputs.moveLeft = glfwGetKey(Window, GLFW_KEY_A) == GLFW_PRESS;
		//mCam.CamInputs.moveRight = glfwGetKey(Window, GLFW_KEY_D) == GLFW_PRESS;

	}

	void App::InitResources()
	{
		mRscManager = Resources::ResourceManager::GetInstance();

		start = std::chrono::steady_clock::now();

		//mRscManager->Create<Resources::Shader>("shader", "Resources/Shaders/shader.vert:Resources/Shaders/shader.frag");
		mRscManager->Create<Resources::Shader>("shader", "Resources/Shaders/shader.vert:Resources/Shaders/cel-shading.frag");
		mShaderProgram = mRscManager->Get<Resources::Shader>("shader")->GetShaderProgram();

		Resources::Shader* colliderShader = Resources::ResourceManager::GetInstance()->Create<Resources::Shader>("shaderCollider", "Resources/Shaders/collider.vert:Resources/Shaders/collider.frag");

		mRscManager->Create<Resources::Texture>("cat", "Resources/Textures/cat.png");
		mRscManager->Create<Resources::Texture>("dog", "Resources/Textures/dog.png");
		mRscManager->Create<Resources::Texture>("wall", "Resources/Textures/wall.jpg");
		mRscManager->Create<Resources::Texture>("potato", "Resources/Textures/potatOS.png");
		mRscManager->Create<Resources::Texture>("chocoboTex", "Resources/Textures/chocobo.png");
		mRscManager->Create<Resources::Texture>("gordoTex", "Resources/Textures/Gordo.png");
		mRscManager->Create<Resources::Texture>("ferdiTex", "Resources/Textures/ferdinand.png");


		mRscManager->Create<Resources::Model>("cubeMesh", "Resources/Obj/cube.obj");
		mRscManager->Create<Resources::Model>("potatos", "Resources/Obj/potatOS.obj");
		mRscManager->Create<Resources::Model>("chocobo", "Resources/Obj/chocobo.obj");
		mRscManager->Create<Resources::Model>("gordo", "Resources/Obj/Gordo.obj");
		mRscManager->Create<Resources::Model>("ferdinand", "Resources/Obj/Ferdinand.obj");

		//Just here to see the difference between Mono and Multi thread
		mRscManager->Create<Resources::Model>("sponza0", "Resources/Obj/sponza.obj");
		mRscManager->Create<Resources::Model>("sponza1", "Resources/Obj/sponza.obj");
		mRscManager->Create<Resources::Model>("sponza2", "Resources/Obj/sponza.obj");
		mRscManager->Create<Resources::Model>("sponza3", "Resources/Obj/sponza.obj");
		mRscManager->Create<Resources::Model>("sponza4", "Resources/Obj/sponza.obj");

		mRscManager->Create<Resources::Model>("capsule", Maths::Primitive::Capsule(20));
		mRscManager->Create<Resources::Model>("sphere", Maths::Primitive::Sphere(20));
		mRscManager->Create<Resources::Model>("cylinder", Maths::Primitive::Cylinder(20));
		mRscManager->Create<Resources::Model>("cube", Maths::Primitive::Cube());

		mRscManager->LoadResource();

		std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
		std::chrono::duration<double> elapsed_seconds = end - start;
		std::cout << "elapsed time: " << elapsed_seconds.count() * 1000 << "ms\n";
		

		mGraph.Objects.push_back(new DataStructure::GameObject(Physics::Transform(Vec3(0.f, 12.0f, 0.f)), LowRenderer::Mesh(mRscManager->Get<Resources::Model>("sphere"), mShaderProgram, mRscManager->Get<Resources::Texture>("dog")), new Physics::SphereCollider(colliderShader->GetShaderProgram())));
		mGraph.RootChildren.push_back({ mGraph.Objects[mGraph.Objects.size() - 1] });

		mGraph.Objects.push_back(new DataStructure::GameObject(Physics::Transform(Vec3(0.f, 0.f, 0.f), Vec3(20.f, 0.5f, 20.f)), LowRenderer::Mesh(mRscManager->Get<Resources::Model>("cube"), mShaderProgram, mRscManager->Get<Resources::Texture>("wall")), new Physics::BoxCollider(colliderShader->GetShaderProgram())));
		mGraph.RootChildren.push_back({ mGraph.Objects[mGraph.Objects.size() - 1] });

		mGraph.Objects.push_back(new DataStructure::GameObject(Physics::Transform(Vec3(0.f, 1.f, -6.f), Vec3(2.5f, 0.1f, 7.5f), Vec3(15.f, 0.f, 0.f)), LowRenderer::Mesh(mRscManager->Get<Resources::Model>("cube"), mShaderProgram, mRscManager->Get<Resources::Texture>("cat")), new Physics::BoxCollider(colliderShader->GetShaderProgram())));
		mGraph.RootChildren.push_back({ mGraph.Objects[mGraph.Objects.size() - 1] });

		mGraph.Objects.push_back(new DataStructure::GameObject(Physics::Transform(Vec3(1.5f, 1.f, 2.f), Vec3(2.f, 2.f, 2.f)), LowRenderer::Mesh(mRscManager->Get<Resources::Model>("cube"), mShaderProgram, mRscManager->Get<Resources::Texture>("potato")), new Physics::BoxCollider(colliderShader->GetShaderProgram())));
		mGraph.RootChildren.push_back({ mGraph.Objects[mGraph.Objects.size() - 1] });

		mGraph.Objects.push_back(new DataStructure::GameObject(Physics::Transform(Vec3(3.5f, 1.f, 4.f), Vec3(2.f, 2.f, 2.f)), LowRenderer::Mesh(mRscManager->Get<Resources::Model>("cube"), mShaderProgram, mRscManager->Get<Resources::Texture>("gordoTex")), new Physics::BoxCollider(colliderShader->GetShaderProgram())));
		mGraph.RootChildren.push_back({ mGraph.Objects[mGraph.Objects.size() - 1] });

		mGraph.Objects.push_back(new DataStructure::GameObject(Physics::Transform(Vec3(2.5f, 1.f, -1.f)), LowRenderer::Mesh(mRscManager->Get<Resources::Model>("cube"), mShaderProgram, mRscManager->Get<Resources::Texture>("chocoboTex")), new Physics::BoxCollider(colliderShader->GetShaderProgram())));
		mGraph.RootChildren.push_back({ mGraph.Objects[mGraph.Objects.size() - 1] });

		mGraph.Objects.push_back(new DataStructure::GameObject(Physics::Transform(Vec3(15.f, 1.f, 15.f), Vec3(10.f, 10.f, 10.f), Vec3(45.f, 45.f, 45.f)), LowRenderer::Mesh(mRscManager->Get<Resources::Model>("cube"), mShaderProgram, mRscManager->Get<Resources::Texture>("wall")), new Physics::BoxCollider(colliderShader->GetShaderProgram())));
		mGraph.RootChildren.push_back({ mGraph.Objects[mGraph.Objects.size() - 1] });

		mGraph.Objects.push_back(new DataStructure::GameObject(Physics::Transform(Vec3(-5.f, 2.f, -5.f)), LowRenderer::Mesh(mRscManager->Get<Resources::Model>("capsule"), mShaderProgram, mRscManager->Get<Resources::Texture>("wall"))));
		mGraph.RootChildren.push_back({ mGraph.Objects[mGraph.Objects.size() - 1] });

		mGraph.Objects.push_back(new DataStructure::GameObject(Physics::Transform(Vec3(35.f * cosf(2.f * M_PI / 5.f), 1.f, sinf(2.f * M_PI / 5.f) * 35.f), Vec3(0.005f, 0.005f, 0.005f), Vec3()), LowRenderer::Mesh(mRscManager->Get<Resources::Model>("sponza0"), mShaderProgram, mRscManager->Get<Resources::Texture>("cat"))));
		mGraph.RootChildren.push_back({ mGraph.Objects[mGraph.Objects.size() - 1] });

		mGraph.Objects.push_back(new DataStructure::GameObject(Physics::Transform(Vec3(35.f * cosf(2.f * 2.f * M_PI / 5.f), 1.f, sinf(2.f * 2.f * M_PI / 5.f) * 35.f), Vec3(0.005f, 0.005f, 0.005f), Vec3()), LowRenderer::Mesh(mRscManager->Get<Resources::Model>("sponza1"), mShaderProgram, mRscManager->Get<Resources::Texture>("wall"))));
		mGraph.RootChildren.push_back({ mGraph.Objects[mGraph.Objects.size() - 1] });

		mGraph.Objects.push_back(new DataStructure::GameObject(Physics::Transform(Vec3(35.f * cosf(3.f * 2.f * M_PI / 5.f), 1.f, sinf(3.f * 2.f * M_PI / 5.f) * 35.f), Vec3(0.005f, 0.005f, 0.005f), Vec3()), LowRenderer::Mesh(mRscManager->Get<Resources::Model>("sponza2"), mShaderProgram, mRscManager->Get<Resources::Texture>("dog"))));
		mGraph.RootChildren.push_back({ mGraph.Objects[mGraph.Objects.size() - 1] });

		mGraph.Objects.push_back(new DataStructure::GameObject(Physics::Transform(Vec3(35.f * cosf(4.f * 2.f * M_PI / 5.f), 1.f, sinf(4.f * 2.f * M_PI / 5.f) * 35.f), Vec3(0.005f, 0.005f, 0.005f), Vec3()), LowRenderer::Mesh(mRscManager->Get<Resources::Model>("sponza3"), mShaderProgram, mRscManager->Get<Resources::Texture>("ferdiTex"))));
		mGraph.RootChildren.push_back({ mGraph.Objects[mGraph.Objects.size() - 1] });

		mGraph.Objects.push_back(new DataStructure::GameObject(Physics::Transform(Vec3(35.f, 1.f, 0.f), Vec3(0.005f, 0.005f, 0.005f), Vec3()), LowRenderer::Mesh(mRscManager->Get<Resources::Model>("sponza4"), mShaderProgram, mRscManager->Get<Resources::Texture>("chocoboTex"))));
		mGraph.RootChildren.push_back({ mGraph.Objects[mGraph.Objects.size() - 1] });

		mPlayer.Object = mGraph.Objects[0];

		mDirectionalLights.push_back(LowRenderer::DirectionalLight(Vec3(-1.f, -1.f, -1.f)));

		for (DataStructure::GameObject* obj : mGraph.Objects)
		{
			if (obj->Collider)
				obj->Collider->ComputeBoundaries(obj->Transform);
		}

		mSpotLights.push_back(LowRenderer::SpotLight(Vec3(), Vec3(), cosf(20.f * M_PI / 180.f), cosf(40.f * M_PI / 180.f), Vec4(1.f, 1.f, 1.f), Vec4(1.f, 1.f, 1.f), Vec4(1.f, 1.f, 1.f), Vec3(1.f, 0.045f, 0.0075f)));
	}

	void App::InitResourcesMultithread()
	{
		mThreadManager.Init();

		mRscManager = Resources::ResourceManager::GetInstance();

		start = std::chrono::steady_clock::now();

		//mThreadManager.RsrcToLoadQueue.push(mRscManager->Create<Resources::Shader>("shader", "Resources/Shaders/shader.vert:Resources/Shaders/shader.frag"));
		mThreadManager.RsrcToLoadQueue.push(mRscManager->Create<Resources::Shader>("shader", "Resources/Shaders/shader.vert:Resources/Shaders/cel-shading.frag"));
		mShaderProgram = mRscManager->Get<Resources::Shader>("shader")->GetShaderProgram();

		Resources::Shader* colliderShader = Resources::ResourceManager::GetInstance()->Create<Resources::Shader>("shaderCollider", "Resources/Shaders/collider.vert:Resources/Shaders/collider.frag");
		mThreadManager.RsrcToLoadQueue.push(colliderShader);

		mThreadManager.RsrcToLoadQueue.push(mRscManager->Create<Resources::Texture>("cat", "Resources/Textures/cat.png"));
		mThreadManager.RsrcToLoadQueue.push(mRscManager->Create<Resources::Texture>("dog", "Resources/Textures/dog.png"));
		mThreadManager.RsrcToLoadQueue.push(mRscManager->Create<Resources::Texture>("wall", "Resources/Textures/wall.jpg"));
		mThreadManager.RsrcToLoadQueue.push(mRscManager->Create<Resources::Texture>("potato", "Resources/Textures/potatOS.png"));
		mThreadManager.RsrcToLoadQueue.push(mRscManager->Create<Resources::Texture>("chocoboTex", "Resources/Textures/chocobo.png"));
		mThreadManager.RsrcToLoadQueue.push(mRscManager->Create<Resources::Texture>("gordoTex", "Resources/Textures/Gordo.png"));
		mThreadManager.RsrcToLoadQueue.push(mRscManager->Create<Resources::Texture>("ferdiTex", "Resources/Textures/ferdinand.png"));


		mThreadManager.RsrcToLoadQueue.push(mRscManager->Create<Resources::Model>("cubeObj", "Resources/Obj/cube.obj"));
		mThreadManager.RsrcToLoadQueue.push(mRscManager->Create<Resources::Model>("potatos", "Resources/Obj/potatOS.obj"));
		mThreadManager.RsrcToLoadQueue.push(mRscManager->Create<Resources::Model>("chocobo", "Resources/Obj/chocobo.obj"));
		mThreadManager.RsrcToLoadQueue.push(mRscManager->Create<Resources::Model>("gordo", "Resources/Obj/Gordo.obj"));
		mThreadManager.RsrcToLoadQueue.push(mRscManager->Create<Resources::Model>("ferdinand", "Resources/Obj/Ferdinand.obj"));

		//Just here to see the difference between Mono and Multi thread
		mThreadManager.RsrcToLoadQueue.push(mRscManager->Create<Resources::Model>("sponza0", "Resources/Obj/sponza.obj"));
		mThreadManager.RsrcToLoadQueue.push(mRscManager->Create<Resources::Model>("sponza1", "Resources/Obj/sponza.obj"));
		mThreadManager.RsrcToLoadQueue.push(mRscManager->Create<Resources::Model>("sponza2", "Resources/Obj/sponza.obj"));
		mThreadManager.RsrcToLoadQueue.push(mRscManager->Create<Resources::Model>("sponza3", "Resources/Obj/sponza.obj"));
		mThreadManager.RsrcToLoadQueue.push(mRscManager->Create<Resources::Model>("sponza4", "Resources/Obj/sponza.obj"));

		mThreadManager.RsrcToLoadQueue.push(mRscManager->Create<Resources::Model>("capsule", Maths::Primitive::Capsule(20)));
		mThreadManager.RsrcToLoadQueue.push(mRscManager->Create<Resources::Model>("sphere", Maths::Primitive::Sphere(20)));
		mThreadManager.RsrcToLoadQueue.push(mRscManager->Create<Resources::Model>("cylinder", Maths::Primitive::Cylinder(20)));
		mThreadManager.RsrcToLoadQueue.push(mRscManager->Create<Resources::Model>("cube", Maths::Primitive::Cube()));

		mResourceLoadedCount = mRscManager->Size();

		mGraph.Objects.push_back(new DataStructure::GameObject(Physics::Transform(Vec3(0.f, 12.0f, 0.f)), LowRenderer::Mesh(mRscManager->Get<Resources::Model>("sphere"), mShaderProgram, mRscManager->Get<Resources::Texture>("dog")), new Physics::SphereCollider(colliderShader->GetShaderProgram())));
		mGraph.RootChildren.push_back({ mGraph.Objects[mGraph.Objects.size() - 1] });

		mGraph.Objects.push_back(new DataStructure::GameObject(Physics::Transform(Vec3(0.f, 0.f, 0.f), Vec3(20.f, 0.5f, 20.f)), LowRenderer::Mesh(mRscManager->Get<Resources::Model>("cube"), mShaderProgram, mRscManager->Get<Resources::Texture>("wall")), new Physics::BoxCollider(colliderShader->GetShaderProgram())));
		mGraph.RootChildren.push_back({ mGraph.Objects[mGraph.Objects.size() - 1] });

		mGraph.Objects.push_back(new DataStructure::GameObject(Physics::Transform(Vec3(0.f, 1.f, -6.f), Vec3(2.5f, 0.1f, 7.5f), Vec3(15.f, 0.f, 0.f)), LowRenderer::Mesh(mRscManager->Get<Resources::Model>("cube"), mShaderProgram, mRscManager->Get<Resources::Texture>("cat")), new Physics::BoxCollider(colliderShader->GetShaderProgram())));
		mGraph.RootChildren.push_back({ mGraph.Objects[mGraph.Objects.size() - 1] });

		mGraph.Objects.push_back(new DataStructure::GameObject(Physics::Transform(Vec3(1.5f, 1.f, 2.f), Vec3(2.f, 2.f, 2.f)), LowRenderer::Mesh(mRscManager->Get<Resources::Model>("cube"), mShaderProgram, mRscManager->Get<Resources::Texture>("potato")), new Physics::BoxCollider(colliderShader->GetShaderProgram())));
		mGraph.RootChildren.push_back({ mGraph.Objects[mGraph.Objects.size() - 1] });

		mGraph.Objects.push_back(new DataStructure::GameObject(Physics::Transform(Vec3(3.5f, 1.f, 4.f), Vec3(2.f, 2.f, 2.f)), LowRenderer::Mesh(mRscManager->Get<Resources::Model>("cube"), mShaderProgram, mRscManager->Get<Resources::Texture>("gordoTex")), new Physics::BoxCollider(colliderShader->GetShaderProgram())));
		mGraph.RootChildren.push_back({ mGraph.Objects[mGraph.Objects.size() - 1] });

		mGraph.Objects.push_back(new DataStructure::GameObject(Physics::Transform(Vec3(2.5f, 1.f, -1.f)), LowRenderer::Mesh(mRscManager->Get<Resources::Model>("cube"), mShaderProgram, mRscManager->Get<Resources::Texture>("chocoboTex")), new Physics::BoxCollider(colliderShader->GetShaderProgram())));
		mGraph.RootChildren.push_back({ mGraph.Objects[mGraph.Objects.size() - 1] });

		mGraph.Objects.push_back(new DataStructure::GameObject(Physics::Transform(Vec3(15.f, 1.f, 15.f), Vec3(10.f, 10.f, 10.f), Vec3(45.f, 45.f, 45.f)), LowRenderer::Mesh(mRscManager->Get<Resources::Model>("cube"), mShaderProgram, mRscManager->Get<Resources::Texture>("wall")), new Physics::BoxCollider(colliderShader->GetShaderProgram())));
		mGraph.RootChildren.push_back({ mGraph.Objects[mGraph.Objects.size() - 1] });

		mGraph.Objects.push_back(new DataStructure::GameObject(Physics::Transform(Vec3(-5.f, 2.f, -5.f)), LowRenderer::Mesh(mRscManager->Get<Resources::Model>("capsule"), mShaderProgram, mRscManager->Get<Resources::Texture>("wall"))));
		mGraph.RootChildren.push_back({ mGraph.Objects[mGraph.Objects.size() - 1] });

		mGraph.Objects.push_back(new DataStructure::GameObject(Physics::Transform(Vec3(35.f * cosf(2.f*M_PI/5.f), 1.f, sinf(2.f * M_PI / 5.f) * 35.f), Vec3(0.005f, 0.005f, 0.005f), Vec3()), LowRenderer::Mesh(mRscManager->Get<Resources::Model>("sponza0"), mShaderProgram, mRscManager->Get<Resources::Texture>("cat"))));
		mGraph.RootChildren.push_back({ mGraph.Objects[mGraph.Objects.size() - 1] });

		mGraph.Objects.push_back(new DataStructure::GameObject(Physics::Transform(Vec3(35.f * cosf(2.f * 2.f * M_PI / 5.f), 1.f, sinf(2.f * 2.f * M_PI / 5.f) * 35.f), Vec3(0.005f, 0.005f, 0.005f), Vec3()), LowRenderer::Mesh(mRscManager->Get<Resources::Model>("sponza1"), mShaderProgram, mRscManager->Get<Resources::Texture>("wall"))));
		mGraph.RootChildren.push_back({ mGraph.Objects[mGraph.Objects.size() - 1] });

		mGraph.Objects.push_back(new DataStructure::GameObject(Physics::Transform(Vec3(35.f * cosf(3.f * 2.f * M_PI / 5.f), 1.f, sinf(3.f* 2.f * M_PI / 5.f) * 35.f), Vec3(0.005f, 0.005f, 0.005f), Vec3()), LowRenderer::Mesh(mRscManager->Get<Resources::Model>("sponza2"), mShaderProgram, mRscManager->Get<Resources::Texture>("dog"))));
		mGraph.RootChildren.push_back({ mGraph.Objects[mGraph.Objects.size() - 1] });

		mGraph.Objects.push_back(new DataStructure::GameObject(Physics::Transform(Vec3(35.f * cosf(4.f * 2.f * M_PI / 5.f), 1.f, sinf(4.f * 2.f * M_PI / 5.f) * 35.f), Vec3(0.005f, 0.005f, 0.005f), Vec3()), LowRenderer::Mesh(mRscManager->Get<Resources::Model>("sponza3"), mShaderProgram, mRscManager->Get<Resources::Texture>("ferdiTex"))));
		mGraph.RootChildren.push_back({ mGraph.Objects[mGraph.Objects.size() - 1] });

		mGraph.Objects.push_back(new DataStructure::GameObject(Physics::Transform(Vec3(35.f, 1.f, 0.f), Vec3(0.005f, 0.005f, 0.005f), Vec3()), LowRenderer::Mesh(mRscManager->Get<Resources::Model>("sponza4"), mShaderProgram, mRscManager->Get<Resources::Texture>("chocoboTex"))));
		mGraph.RootChildren.push_back({ mGraph.Objects[mGraph.Objects.size() - 1] });



		mPlayer.Object = mGraph.Objects[0];

		mDirectionalLights.push_back(LowRenderer::DirectionalLight(Vec3(-1.f, -1.f, -1.f)));

		for (DataStructure::GameObject* obj : mGraph.Objects)
		{
			if (obj->Collider)
				obj->Collider->ComputeBoundaries(obj->Transform);
		}

		mSpotLights.push_back(LowRenderer::SpotLight(Vec3(), Vec3(), cosf(20.f * M_PI / 180.f), cosf(40.f * M_PI / 180.f), Vec4(1.f, 1.f, 1.f), Vec4(1.f, 1.f, 1.f), Vec4(1.f, 1.f, 1.f), Vec3(1.f, 0.045f, 0.0075f)));
	}

	void App::DisplaySliderFloatPosObject(std::vector<DataStructure::SceneNode> &Children, std::string index)
	{
		if (Children.size() > 0)
		{
			std::string name = "object";
			for (int j = 0; j < Children.size(); j++)
			{
				ImGui::DragFloat3((name + index + std::to_string(j)).c_str(), &Children[j].Object->Transform.Position.x, 0.5f, -10.f, 10.f, "%.3f", 0);
				DisplaySliderFloatPosObject(Children[j].Children, index + std::to_string(j));
			}
		}
	}

	void App::ThreadManagement()
	{
		if (!mThreadManager.RsrcToInitList.empty() && mThreadManager.IsOpenGLListAvailable.load() == 0)
		{
			int tmpAccessValue = ++mThreadManager.IsOpenGLListAvailable;

			if (tmpAccessValue == 1)
			{
				for (Resources::IResource* rsrc : mThreadManager.RsrcToInitList)
				{
					if (rsrc->State == Resources::ResourceState::RS_LOADED)
					{
						rsrc->OpenGLInit();
						rsrc->State = Resources::ResourceState::RS_INIT;
					}
				}
				while (!mThreadManager.RsrcToInitList.empty() && mThreadManager.RsrcToInitList.front()->State == Resources::ResourceState::RS_INIT)
				{
					mThreadManager.RsrcToInitList.pop_front();
					--mResourceLoadedCount;
				}

				mThreadManager.IsOpenGLListAvailable.store(0);
			}
		}
		else if (mResourceLoadedCount == 0 && mThreadManager.RsrcToInitList.empty() && mThreadManager.RsrcToLoadQueue.empty())
		{
			std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
			std::chrono::duration<double> elapsed_seconds = end - start;
			std::cout << "elapsed time: " << elapsed_seconds.count() * 1000 << "ms\n";
			mResourceLoadedCount = -1;
		}
	}

	void App::Update()
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		float currentFrame = glfwGetTime();
		mDeltaTime = currentFrame - mLastFrame;
		mLastFrame = currentFrame;
		glfwPollEvents();

		glfwGetWindowSize(Window, &mWidth, &mHeight);
		mCam.Aspect = (float)mWidth / (float)mHeight;
		if (mMenu.gameState == GS_PLAY || mMenu.gameState == GS_MULTITHREAD)
		{
			if (mMenu.gameState == GS_PLAY && !isInit)
			{
				InitResources();
				isInit = true;

			}
			else if(!isInit)
			{
				InitResourcesMultithread();
				isInit = true;
			}

			processInput(Window);
			if (ImGui::Begin("Config"))
			{
				ImGui::Checkbox("Gravity", &mGravity);
				if (ImGui::CollapsingHeader("Player", ImGuiTreeNodeFlags_DefaultOpen))
				{
					ImGui::InputFloat3("Velocity", &mPlayer.Object->Velocity.x, "%.3f", 0);
					ImGui::Checkbox("Slide ramp", &mSlide);
				}
				if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen))
				{
					ImGui::InputFloat3("Pos Camera", &mCam.Position.x, "%.3f", 0);
				}

				if (ImGui::CollapsingHeader("Light", ImGuiTreeNodeFlags_DefaultOpen))
				{
					ImGui::ColorEdit3("ColorLight", &mDirectionalLights[0].DiffuseColor.x);
				}

				std::string name = "object";
				if (ImGui::CollapsingHeader("PosObj", ImGuiTreeNodeFlags_DefaultOpen))
				{

					for (int i = 0; i < mGraph.RootChildren.size(); i++)
					{
						ImGui::DragFloat3((name + std::to_string(i)).c_str(), &mGraph.RootChildren[i].Object->Transform.Position.x, 0.5f, -10.f, 10.f, "%.3f", 0);
						DisplaySliderFloatPosObject(mGraph.RootChildren[i].Children, std::to_string(i));
					}

				}
			}
			ImGui::End();

			// input
			// -----
			if (mResourceLoadedCount == -1 && glfwGetKey(Window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			{
				if (mMenu.gameState == GS_MULTITHREAD)
					mThreadManager.Destroy();
				mRscManager->Clear();
				mGraph.Destroy();
				mDirectionalLights.clear();
				mSpotLights.clear();
				mPointLights.clear();
				isInit = false;
				mMenu.gameState = GS_MAIN_MENU;
			}
			else if(isInit)
			{
				if (mMenu.gameState == GS_MULTITHREAD)
					ThreadManagement();

				mCam.Update(mDeltaTime, mPlayer.Object->Transform.Position);

				mPlayer.Update(mDeltaTime, mCam);

				// render
				// ------
				glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

				static float rotationIncrement = 0.f;
				rotationIncrement += 0.1f;
				mGraph.RootChildren[mGraph.RootChildren.size() - 1].Object->Transform.Rotation = Vec3(rotationIncrement, rotationIncrement, rotationIncrement);
				mGraph.RootChildren[mGraph.RootChildren.size() - 2].Object->Transform.Rotation = Vec3(rotationIncrement, rotationIncrement, rotationIncrement);
				mGraph.RootChildren[mGraph.RootChildren.size() - 3].Object->Transform.Rotation = Vec3(rotationIncrement, rotationIncrement, rotationIncrement);
				mGraph.RootChildren[mGraph.RootChildren.size() - 4].Object->Transform.Rotation = Vec3(rotationIncrement, rotationIncrement, rotationIncrement);
				mGraph.RootChildren[mGraph.RootChildren.size() - 5].Object->Transform.Rotation = Vec3(rotationIncrement, rotationIncrement, rotationIncrement);

				mGraph.RootChildren[2].Object->Transform.Rotation = Vec3(15.f * cosf(rotationIncrement * 0.1f), 0.f, 0.f);

				mGraph.Update(mCam, mDirectionalLights, mPointLights, mSpotLights);



				for (DataStructure::GameObject* obj : mGraph.Objects)
				{
					if (obj->Collider)
						obj->Collider->ComputeBoundaries(obj->Transform);
				}

				for (DataStructure::GameObject* obj : mGraph.Objects)
				{
					if(obj->Collider && obj->Collider->Type == Physics::ColliderType::COLT_BOX)
						reinterpret_cast<Physics::BoxCollider*>(obj->Collider)->UpdateCollisionData(obj->Mesh.ModelMatrix);

					mPlayer.Object->Collider->ComputeCollision(obj->Collider, obj->Transform.Scale, &mPlayer.Object->Transform.Position);

					if (mPlayer.Object->Collider->IsColliding())
						mPlayer.CanJump = true;
				}

					if (mGravity && !mPlayer.Object->Collider->IsColliding())
						mPlayer.Object->Velocity.y += -9.81f * mDeltaTime;

					if (!mSlide && (!mGravity || mPlayer.Object->Collider->IsColliding()) && mPlayer.CanJump)
						mPlayer.Object->Velocity.y = 0.f;
			}
		}
		else
		{
			ImGui::Begin("Menu", 0, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove);
			ImGui::SetWindowPos(ImVec2(0, 0), 0);
			ImGui::SetWindowSize(ImVec2(mWidth, mHeight), 0);
			mMenu.Render(Window, mWidth, mHeight);
			
			ImGui::End();
		}

		if (mMenu.gameState == GS_OPTION)
		{
			ImGui::Begin("Option", 0, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove);
			ImGui::SetWindowSize(ImVec2(mWidth, mHeight), 0);
			ImGui::SetWindowPos(ImVec2(0, 0), 0);
			mMenu.RenderOption(mPlayer, Window,mWidth, mHeight);
			if (glfwGetKey(Window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
				mMenu.gameState = GS_MAIN_MENU;
			ImGui::End();
		}

		ImGui::Render();
		ImGui::EndFrame();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(Window);
	}
}