/*
Project 1 Submission for CMPSC458
Name: Joe Smith
psu id: xyz123
*/

#include <Project2.hpp>


//  Modify this preamble based on your code.  It should contain all the functionality of your project.  
std::string preamble =
"Project 2 code \n\n"
"Press the U,I,O to increase transformations \n"
"Press the J,K,L to decrease transformations \n"
"\tShift+Key will alter scale\n "
"\tControl+Key will alter translation\n "
"\tKey along will alter rotation rate\n "
"Pressing Comma will increase transformation Step\n "
"Pressing Period will decrease transformation Step\n "
"Pressing G will reset transformations\n "
"Pressing Q will toggle quaternion rotation\n "
"Pressing B will toggle reflections for the box textures\n "
"Pressing H will toggle heightmap\n "
"Pressing N will toggle Normals\n "
"Pressing P will print information\n\n";

int main()
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 4);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

	std:printf(preamble.c_str());
	// glfw window creation
	// --------------------
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Project 2", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// configure global opengl state
	// -----------------------------
	glEnable(GL_MULTISAMPLE); // Enabled by default on some drivers, but not all so always enable to make sure
	glEnable(GL_DEPTH_TEST);

	// build and compile shaders
	// -------------------------
	Shader lightingShader_basic("../Project_2/Shaders/lightingShader_basic.vert", "../Project_2/Shaders/lightingShader_basic.frag");
	Shader reflectionShader("../Project_2/Shaders/reflectionShader.vert", "../Project_2/Shaders/reflectionShader.frag");
	Shader skyboxShader("../Project_2/Shaders/skyboxShader.vert", "../Project_2/Shaders/skyboxShader.frag");
	Shader lightingShader_specular("../Project_2/Shaders/lightingShader_specular.vert", "../Project_2/Shaders/lightingShader_specular.frag");
	Shader normalShader("../Project_2/Shaders/normal.vert", "../Project_2/Shaders/normal.frag", "../Project_2/Shaders/normal.geom");
	Shader lightingShader_nMap("../Project_2/Shaders/lightingShader_nMap.vert", "../Project_2/Shaders/lightingShader_nMap.frag");

	// set up vertex data (and buffer(s)) and configure vertex attributes
	// These are vertices for cubes
	// ------------------------------------------------------------------
	float vertices[] = {
		// positions          // normals           // texture coords
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
		0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
		0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

		0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
		0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
		0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
		0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
		0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
	};


	// first, configure the cube's VAO (and VBO)
	unsigned int VBO, cubeVAO;
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Send buffer data using layout of the vertices
	glBindVertexArray(cubeVAO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0); // Positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float))); // Normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float))); // Texture coords
	glEnableVertexAttribArray(2);

	// second, configure the light's VAO (VBO stays the same; the vertices are the same for the light object which is also a 3D cube)
	unsigned int lightVAO;
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// note that we update the lamp's position attribute's stride to reflect the updated buffer data
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// Skybox uses the same vertices so no need to make a new one.  Just making a new name for sanity
	unsigned int skyboxVAO = lightVAO;

	// load textures
	// -------------
	std::vector<std::string> faces =
	{
		"../Project_2/Media/skybox/right.jpg",
		"../Project_2/Media/skybox/left.jpg",
		"../Project_2/Media/skybox/top.jpg",
		"../Project_2/Media/skybox/bottom.jpg",
		"../Project_2/Media/skybox/back.jpg",
		"../Project_2/Media/skybox/front.jpg"
	};
	unsigned int cubemapTexture = loadCubemap(faces);

	// initialize heatmap
	Heightmap heightmap("../Project_2/Media/heightmaps/hflab4.jpg");
	unsigned int heightmap_texture = loadTexture("../Project_2/Media/skybox_old/bottom.jpg");
	unsigned int diffuseMap = loadTexture("../Project_2/Media/textures/container2.png");
	unsigned int specularMap = loadTexture("../Project_2/Media/textures/container2_specular.png");
	unsigned int rail = loadTexture("../Project_2/Media/textures/rail.png");

	// initialize track object
	Track track("spline/track.sp");


	// positions of the point lights
	glm::vec3 pointLightPositions[] = {
		glm::vec3(0.7f,  0.2f,  2.0f),
		glm::vec3(2.3f, -3.3f, -4.0f),
		glm::vec3(-4.0f,  2.0f, -12.0f),
		glm::vec3(0.0f,  0.0f, -3.0f)
	};

	// load models
	// -----------
	//Model ourModel("../Project_2/Media/nanosuit/nanosuit.obj");
	//car model link: 
	//https://sketchfab.com/3d-models/wdw-space-mountain-ride-vehicle-e387630a8c2d4c0887c0e408b21a6faa
	Model ourModel("../Project_2/Media/car/model.obj");

	// shader configuration
	// --------------------
	reflectionShader.use();
	reflectionShader.setInt("skybox", 0);

	skyboxShader.use();
	skyboxShader.setInt("skybox", 0);

	lightingShader_basic.use();
	lightingShader_basic.setInt("material.diffuse", 0);

	lightingShader_specular.use();
	lightingShader_specular.setInt("material.diffuse", 0);
	lightingShader_specular.setInt("material.specular", 1);

	lightingShader_nMap.use();
	lightingShader_nMap.setInt("material.diffuse", 0);
	lightingShader_nMap.setInt("material.specular", 1);
	lightingShader_nMap.setInt("material.normal", 2);

	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		// per-frame time logic
		// --------------------
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		// weighted avg for framerate
		framerate = (0.4f / (deltaTime)+1.6f * framerate) / 2.0f;

		// input
		// -----
		processInput(window);


		// get camera position
		if (camera.onTrack) {
			camera.ProcessTrackMovement(deltaTime, track, window);
		}


		// render
		// ------
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// draw scene as normal, get camera parameters
		glm::mat4 model;
		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		model = glm::rotate(model, glm::radians(10.0f * currentFrame), glm::vec3(1.0f, 0.3f, 0.5f));


		// Setup shader info
		reflectionShader.use();
		reflectionShader.setMat4("model", model);
		reflectionShader.setMat4("view", view);
		reflectionShader.setMat4("projection", projection);
		reflectionShader.setVec3("cameraPos", camera.Position);

		lightingShader_basic.use();
		lightingShader_basic.setMat4("model", model);
		lightingShader_basic.setMat4("view", view);
		lightingShader_basic.setMat4("projection", projection);

		lightingShader_specular.use();
		lightingShader_specular.setMat4("model", model);
		lightingShader_specular.setMat4("view", view);
		lightingShader_specular.setMat4("projection", projection);

		lightingShader_nMap.use();
		lightingShader_nMap.setMat4("model", model);
		lightingShader_nMap.setMat4("view", view);
		lightingShader_nMap.setMat4("projection", projection);

		set_lighting(lightingShader_basic, pointLightPositions);
		set_lighting(lightingShader_specular, pointLightPositions);
		set_lighting(lightingShader_nMap, pointLightPositions);


		// Turn rotation rate into quaternion and cumulate the rotations
		rotation *= glm::quat(rotation_rate * deltaTime);
		// add rotation rate to euler rotation
		rotation_euler += rotation_rate * deltaTime;

		// Draw the track
		if (drawTrack) {
			track.Draw(lightingShader_basic, rail, diffuseMap);
		}


		if (drawBoxes){
			if (drawSpecular)
			{  // if you want reflective boxes
				reflectionShader.use();
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
			}
			
			else
			{ // if you want normal looking boxes, use ourShader
				lightingShader_specular.use();
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, diffuseMap);
				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, specularMap);
				lightingShader_specular.setFloat("material.shininess", 16.0f);
			}

			/*************************************************
			You can get rid of the boxes once you have a track
			Or you can find some other place to render them
			**************************************************/
			// Draw a box at every control point on the track

			glBindVertexArray(cubeVAO);
			for (unsigned int i = 0; i < track.controlPoints.size(); i++)
			{
				// calculate the model matrix for each object and pass it to shader before drawing
				glm::mat4 box_model;

				// Translate box for final offset
				box_model = glm::translate(box_model, track.controlPoints[i]);
				box_model = glm::translate(box_model, translation);

				// initial rotation of boxes
				float angle = 20.0f * i;
				box_model = glm::rotate(box_model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));


				// apply continuous rotation and update based on rate
				if (use_quats)
				{  // if we are using quaternion (better way)
				   // Add the rotations to the box matrix
					box_model = box_model * glm::mat4_cast(rotation);
				}
				else
				{  // if we are using Euler angles (not as good, creates unnatural rotation)
				   // Apply for each axis at once
					box_model = glm::rotate(box_model, rotation_euler.x, glm::vec3(1.0f, 0.0f, 0.0f));
					box_model = glm::rotate(box_model, rotation_euler.y, glm::vec3(0.0f, 1.0f, 0.0f));
					box_model = glm::rotate(box_model, rotation_euler.z, glm::vec3(0.0f, 0.0f, 1.0f));
				}

				// Scale the boxes 
				box_model = glm::scale(box_model, scale);

				// Send the model matrix to the shader (whichever one we are using).  
				if (drawBoxes)
				{
					lightingShader_specular.use();
					lightingShader_specular.setMat4("model", box_model);
				}
				else
				{
					reflectionShader.use();
					reflectionShader.setMat4("model", box_model);
				}

				//Finally draw the boxes
				glDrawArrays(GL_TRIANGLES, 0, 36);

				// Draw the normals if desired
				if (drawNormals)
				{
					normalShader.use();
					normalShader.setMat4("model", box_model);
					normalShader.setMat4("projection", projection);
					normalShader.setMat4("view", view);
					glDrawArrays(GL_TRIANGLES, 0, 36);
				}
			}
			// Unbind the vertex array
			glBindVertexArray(0);
		}

		// Draw the heightmap
		if (drawHeightmap)
		{
			heightmap.Draw(lightingShader_basic, heightmap_texture);
		}


		//car model render
		lightingShader_nMap.use();
		lightingShader_nMap.setFloat("material.shininess", 16.0f);
		model = glm::mat4();  // Set to idenity matrix
		model = glm::translate(model, camera.carPosition);  //move with camera
		model *= camera.carRotationMat;						//rotate with camera
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.02f, 0.02f, 0.02f));
		lightingShader_nMap.setMat4("model", model);
		// Draw the car
		ourModel.Draw(lightingShader_nMap);


		// Draw the normals if desired for heightmap and nano suit
		if (drawNormals)
		{
			normalShader.use();
			normalShader.setMat4("projection", projection);
			normalShader.setMat4("view", view);
			heightmap.Draw(normalShader, heightmap_texture);

			normalShader.use();
			normalShader.setMat4("model", model);
			ourModel.Draw(normalShader);
		}

		// draw skybox 
		glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
		skyboxShader.use();
		view = glm::mat4(glm::mat3(camera.GetViewMatrix())); // remove translation from the view matrix
		skyboxShader.setMat4("view", view);
		skyboxShader.setMat4("projection", projection);
		// skybox cube
		glBindVertexArray(skyboxVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glDepthFunc(GL_LESS); // set depth function back to default

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// optional: de-allocate all resources once they've outlived their purpose:
	// ------------------------------------------------------------------------
	glDeleteVertexArrays(1, &cubeVAO);
	glDeleteVertexArrays(1, &skyboxVAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &skyboxVAO);
	heightmap.delete_buffers();

	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// The movement of the boxes is still here.  Feel free to use it or take it out
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
	// Escape Key quits
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	//toggle free look mode
	camera.tCurPressed = glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS;
	if (!camera.tPressed && camera.tCurPressed) {
		if (camera.onTrack == false) {
			camera.onTrack = true;
		}
		else
			camera.onTrack = false;
	}
	camera.tPressed = camera.tCurPressed;

	// Movement Keys  -  Need to disable this while you are moving along your track
	if (!camera.onTrack) {
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			camera.ProcessKeyboard(FORWARD, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			camera.ProcessKeyboard(BACKWARD, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			camera.ProcessKeyboard(LEFT, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			camera.ProcessKeyboard(RIGHT, deltaTime);
		if (!(glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) && glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
			camera.ProcessKeyboard(UP, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
			camera.ProcessKeyboard(DOWN, deltaTime);
	}

	// change stepsize multiplier (to make it less if necessary
	if (glfwGetKey(window, GLFW_KEY_COMMA))
		step_multiplier *= 1.01f;
	if (glfwGetKey(window, GLFW_KEY_PERIOD))
		step_multiplier /= 1.01f;

	// update step based on framerate (prevents excessive changes)
	float step = deltaTime * step_multiplier;


	// Changing overall behavior (only want these to trigger once so add a delay of half a second)
	float currentFrame = glfwGetTime();
	bool somethingPressed = glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS ||
		glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS ||
		glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS ||
		glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS ||
		glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS ||
		glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS ||
		glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS;
	if (somethingPressed && last_pressed < currentFrame - 0.5f || last_pressed == 0.0f)
	{
		// Toggle heightmap visibility
		if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS)
			drawHeightmap ? drawHeightmap = false : drawHeightmap = true;
		// Toggle box rendering mode
		if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS)
			drawBoxes ? drawBoxes = false : drawBoxes = true;
		// Toggle drawing normals
		if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS)
			drawNormals ? drawNormals = false : drawNormals = true;
		// Toggle using quaternions
		if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
			if (use_quats)
			{
				use_quats = false;
				std::cout << "Not using Quaternions" << std::endl;
				rotation_euler = glm::eulerAngles(rotation);
			}
			else
			{
				use_quats = true;
				std::cout << "Using Quaternions" << std::endl;
				rotation = glm::quat(rotation_euler);
			}

		// reset all changes to 0
		if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
		{
			rotation_rate = glm::vec3(0.0f, 0.0f, 0.0f);
			scale = glm::vec3(1.0f, 1.0f, 1.0f);
			translation = glm::vec3(0.0f, 0.0f, 0.0f);
			rotation = glm::quat(glm::vec3(0.0f, 0.0f, 0.0f));
			rotation_euler = glm::vec3(0.0f, 0.0f, 0.0f);
			step_multiplier = 1.0f;
		}
		// Use a preset set of transformations
		if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		{
			rotation_rate = 50.0f*glm::vec3(M_PI / 64.0f, M_PI / 64.0f, M_PI / 64.0f);
			scale = glm::vec3(2.0f, 0.5f, 0.2f);
			translation = glm::vec3(0.0f, 0.0f, 0.0f);
			rotation = glm::quat(glm::vec3(0.0f, 0.0f, 0.0f));
			rotation_euler = glm::vec3(0.0f, 0.0f, 0.0f);
		}

		// Print all info
		if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
		{
			std::printf("Frame Rate: %.05f\nCurrent Frame: %.05f\tLast Pressed: %.05f\n", framerate, currentFrame, last_pressed);
			std::printf("Step: %.05f\tStep Multiplier: %.04f\n", step, step_multiplier);
			std::printf("Rotation Rate (%.05f,%.05f,%.05f)\n", rotation_rate.x, rotation_rate.y, rotation_rate.z);
			std::printf("Rotation Euler (%.05f,%.05f,%.05f)\n", rotation_euler.x, rotation_euler.y, rotation_euler.z);
			std::printf("Rotation quaternions (%.05f,%.05f,%.05f,%.05f)\n", rotation.x, rotation.y, rotation.z, rotation.w);
			std::printf("Translation (%.05f,%.05f,%.05f)\n", rotation_rate.x, rotation_rate.y, rotation_rate.z);
			std::printf("Scale (%.05f,%.05f,%.05f)\n", scale.x, scale.y, scale.z);
			std::printf("Front (%.05f,%.05f,%.05f)\n", camera.Front.x, camera.Front.y, camera.Front.z);
			use_quats ? std::printf("Using quaternions\n") : std::printf("Not Using quaternions\n");
			std::printf("\n");
		}


		last_pressed = currentFrame;
	}

	// Print adjusted step size for transformationss
	if (glfwGetKey(window, GLFW_KEY_COMMA) || glfwGetKey(window, GLFW_KEY_PERIOD))
		std::printf("Step: %.05f\tStep Multiplier: %.04f\\tFrame Rate: %.05f\n", step, step_multiplier, framerate);

	// Make changes to transformations depending on the key
	glm::vec3 change;
	if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS)
		change.x += step;
	if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
		change.y += step;
	if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
		change.z += step;
	if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
		change.x -= step;
	if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
		change.y -= step;
	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
		change.z -= step;



	// figure out what to change
	bool shift = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT);
	bool ctrl = glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) || glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL);

	// update rotation rate
	if (!shift && !ctrl)
		rotation_rate += change;

	// update scale
	if (shift && !ctrl)
		scale += scale * change * deltaTime * 1e2f;

	// update translation
	if (!shift && ctrl)
		translation += change * deltaTime * 1e2f;



}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
	SCR_WIDTH = width;
	SCR_HEIGHT = height;

}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;
	/*************************************************
	Make sure you disable this if the camera is on the track
	**************************************************/
	if (!camera.onTrack) {
		camera.ProcessMouseMovement(xoffset, yoffset);
	}
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

// utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int loadTexture(char const * path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}

// loads a cubemap texture from 6 individual texture faces
// order:
// +X (right)
// -X (left)
// +Y (top)
// -Y (bottom)
// +Z (front) 
// -Z (back)
// -------------------------------------------------------
unsigned int loadCubemap(std::vector<std::string> faces)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrComponents;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrComponents, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}

void set_lighting(Shader shader, glm::vec3 * pointLightPositions)
{
	shader.use();
	shader.setVec3("viewPos", camera.Position);

	/*
	Here we set all the uniforms for the 5/6 types of lights we have. We have to set them manually and index
	the proper PointLight struct in the array to set each uniform variable. This can be done more code-friendly
	by defining light types as classes and set their values in there, or by using a more efficient uniform approach
	by using 'Uniform buffer objects', but that is something we'll discuss in the 'Advanced GLSL' tutorial.
	*/
	// directional light
	//shader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
	shader.setVec3("dirLight.direction", 0.24f, -.3f, 0.91f); // Tried to target the sun
	shader.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
	shader.setVec3("dirLight.diffuse", 0.5f, 0.5f, 0.5f);
	shader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);
	// point light 1
	shader.setVec3("pointLights[0].position", pointLightPositions[0]);
	shader.setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
	shader.setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
	shader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
	shader.setFloat("pointLights[0].constant", 1.0f);
	shader.setFloat("pointLights[0].linear", 0.09);
	shader.setFloat("pointLights[0].quadratic", 0.032);
	// point light 2
	shader.setVec3("pointLights[1].position", pointLightPositions[1]);
	shader.setVec3("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
	shader.setVec3("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
	shader.setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
	shader.setFloat("pointLights[1].constant", 1.0f);
	shader.setFloat("pointLights[1].linear", 0.09);
	shader.setFloat("pointLights[1].quadratic", 0.032);
	// point light 3
	shader.setVec3("pointLights[2].position", pointLightPositions[2]);
	shader.setVec3("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
	shader.setVec3("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
	shader.setVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
	shader.setFloat("pointLights[2].constant", 1.0f);
	shader.setFloat("pointLights[2].linear", 0.09);
	shader.setFloat("pointLights[2].quadratic", 0.032);
	// point light 4
	shader.setVec3("pointLights[3].position", pointLightPositions[3]);
	shader.setVec3("pointLights[3].ambient", 0.05f, 0.05f, 0.05f);
	shader.setVec3("pointLights[3].diffuse", 0.8f, 0.8f, 0.8f);
	shader.setVec3("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
	shader.setFloat("pointLights[3].constant", 1.0f);
	shader.setFloat("pointLights[3].linear", 0.09);
	shader.setFloat("pointLights[3].quadratic", 0.032);
	// spotLight
	shader.setVec3("spotLight.position", camera.Position);
	shader.setVec3("spotLight.direction", camera.Front);
	shader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
	shader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
	shader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
	shader.setFloat("spotLight.constant", 1.0f);
	shader.setFloat("spotLight.linear", 0.09);
	shader.setFloat("spotLight.quadratic", 0.032);
	shader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
	shader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));

}
