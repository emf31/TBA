#include "SceneManager.h"
#include "TTransform.h"



void SceneManager::inicializar() {


	quadVAO = 0;

	//gui.createWidget("AlfiskoSkin/Button", perc, perx, "Test");
	scene = new TNode(-1);
	m_matrizActual = glm::mat4();
	//camaraActiva = crearNodoCamara();
	sunlight = nullptr;
	shaderGeometria = ResourceManager::i().getShader("assets/geometria.vs", "assets/geometria.frag");
	shaderLuces = ResourceManager::i().getShader("assets/luces.vs", "assets/luces.frag");
	shaderBombillas = ResourceManager::i().getShader("assets/luz_loading.vs", "assets/luz_loading.frag");
	shaderLineas = ResourceManager::i().getShader("assets/lines.vs", "assets/lines.frag");
	shaderBlur = ResourceManager::i().getShader("assets/blur.vs", "assets/blur.frag");
	shaderSkybox = ResourceManager::i().getShader("assets/skybox.vs", "assets/skybox.frag");
	shaderBildboard = ResourceManager::i().getShader("assets/bildboard.vs", "assets/bildboard.frag");

	inicializarBuffers();
	inicializarBuffersBlur();
	inicializarBuffersLineas();
	inicializarBufferSkybox();
	inicializarBufferBildboard();
	numLines = 0;
	drawTarget = false;
}




TMeshGroup* SceneManager::getMesh(const std::string& path,Shader* shader) {
	if (path != "") {
		return ResourceManager::i().getMesh(path,shader);
	}
	return NULL;
}

void SceneManager::draw() {
	//enlazamos el buffer sobre el que queremos escribir
	//activamos el g buffer cuando vamos a dibujar modelos
	
	//std::cout << "activo gBuffer" << std::endl;
	glPolygonMode(GL_FRONT_AND_BACK, 0 ? GL_LINE : GL_FILL);
	
	

	//****************RENDER DE ESCENA COMPLETA CON DEFERRED SHADING**************
		//-------GUARDAMOS INFORMACION DE GEOMETRIA Y TEXTURA---------
			//activamos el gBuffer donde nos vamos a guardar toda la informacion de geometria y texturas
			glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
			// Update matrices
			projection = camaraActiva->getProjectionMatrix();
			view = camaraActiva->GetViewMatrix();
			//activeCameraPos = Vec3<float>(camaraActiva->getPosition().x, camaraActiva->getPosition().y, camaraActiva->getPosition().z) ;
			activeCameraPos = camaraActiva->getPosition();
	
			scene->draw();
			//gui.draw();
			//std::cout << "desactivo gBuffer" << std::endl;
		//-------APLICAMOS LAS LUCES (TODAS A LA VEZ) A LA INFORMACION GUARDADA EN GBUFFER --------
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			renderBlur();
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			
			renderLuces();
			
	//**************** FIN RENDER DE ESCENA COMPLETA CON DEFERRED SHADING**************
			//glBindFramebuffer(GL_FRAMEBUFFER, 0);
			//renderBlur();
			//glBindFramebuffer(GL_FRAMEBUFFER, 0);
			//renderBloom();


	//****************RENDER DE LINEAS PARA DEBUG DE FISICAS**************
		
		//hacemos un clear de la profundidad para que las lineas salgas por delante del resto de la escena
		//glClear(GL_DEPTH_BUFFER_BIT);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		glBlitFramebuffer(0, 0, (GLint)screenWidth, (GLint)screenHeight, 0, 0, (GLint)screenWidth, (GLint)screenHeight, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);
		if (vertices3.size() > 0) {
			drawAllLines();
		}
	//**************** FIN RENDER DE LINEAS PARA DEBUG DE FISICAS**************
		
		renderSkybox();

		renderBildboard();

		//glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
	
}

void SceneManager::renderFrame(GLFWwindow* window) {
	glfwSwapBuffers(window);
}

void SceneManager::inicializarBuffers()
{
	GLuint screenWidth = 1280, screenHeight = 720;

	shaderLuces->Use();
	glUniform1i(glGetUniformLocation(shaderLuces->Program, "gPosition"), 0);
	glUniform1i(glGetUniformLocation(shaderLuces->Program, "gNormal"), 1);
	glUniform1i(glGetUniformLocation(shaderLuces->Program, "gTextura"), 2);
	glUniform1i(glGetUniformLocation(shaderLuces->Program, "gTangent"), 3);
	glUniform1i(glGetUniformLocation(shaderLuces->Program, "gBitangent"), 4);
	glUniform1i(glGetUniformLocation(shaderLuces->Program, "gEmisivo"), 5); 
	glUniform1i(glGetUniformLocation(shaderLuces->Program, "gObjectColor"), 6);
	glUniform1i(glGetUniformLocation(shaderLuces->Program, "gBloom"), 7);

	
	glGenFramebuffers(1, &gBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
	
	// Posicion
	glGenTextures(1, &gPosition);
	glBindTexture(GL_TEXTURE_2D, gPosition);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, screenWidth, screenHeight, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);
	//Normales
	glGenTextures(1, &gNormal);
	glBindTexture(GL_TEXTURE_2D, gNormal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, screenWidth, screenHeight, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);
	// Difuso + especular
	glGenTextures(1, &gTextura);
	glBindTexture(GL_TEXTURE_2D, gTextura);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, screenWidth, screenHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gTextura, 0);
	//tangente
	glGenTextures(1, &gTangent);
	glBindTexture(GL_TEXTURE_2D, gTangent);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, screenWidth, screenHeight, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, gTangent, 0);
	//bitangente
	glGenTextures(1, &gBitangent);
	glBindTexture(GL_TEXTURE_2D, gBitangent);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, screenWidth, screenHeight, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D, gBitangent, 0);
	//emisivo
	glGenTextures(1, &gEmisivo);
	glBindTexture(GL_TEXTURE_2D, gEmisivo);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, screenWidth, screenHeight, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT5, GL_TEXTURE_2D, gEmisivo, 0);
	//objectColor
	glGenTextures(1, &gObjectColor);
	glBindTexture(GL_TEXTURE_2D, gObjectColor);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, screenWidth, screenHeight, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT6, GL_TEXTURE_2D, gObjectColor, 0);


	// tenemos que juntar los 3 color buffers en el framebuffer que tenemos activo para ello los juntamos en el array
	// pero como ahora tenemos 3 render targets en el fragment shader vamos a tener que definir 3 capas (layout)
	GLuint attachments[7] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4, GL_COLOR_ATTACHMENT5, GL_COLOR_ATTACHMENT6 };
	glDrawBuffers(7, attachments);
	
		//Depthbuffer
	glGenRenderbuffers(1, &rboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, screenWidth, screenHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	//glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
}

void SceneManager::inicializarBuffersBlur()
{
	GLuint screenWidth = 1280, screenHeight = 720;

	glGenFramebuffers(2, bloomFBO);
	glGenTextures(2, bloomBuffers);
	for (GLuint i = 0; i < 2; i++)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, bloomFBO[i]);
		glBindTexture(GL_TEXTURE_2D, bloomBuffers[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, screenWidth, screenHeight, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bloomBuffers[i], 0);
	}
}

void SceneManager::inicializarBufferSkybox()
{
	GLfloat skyboxVertices[] = {
		// Positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f,  1.0f
	};

	
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glBindVertexArray(0);

	
	faces.push_back("assets/skybox/right.jpg");
	faces.push_back("assets/skybox/left.jpg");
	faces.push_back("assets/skybox/top.jpg");
	faces.push_back("assets/skybox/bottom.jpg");
	faces.push_back("assets/skybox/back.jpg");
	faces.push_back("assets/skybox/front.jpg");

	
	glGenTextures(1, &skyboxTexture);

	int width, height;
	unsigned char* image;

	glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
	for (GLuint i = 0; i < faces.size(); i++)
	{
		image = SOIL_load_image(faces[i], &width, &height, 0, SOIL_LOAD_RGB);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		SOIL_free_image_data(image);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void SceneManager::inicializarBufferBildboard()
{
	billboardFrameName.push_back("assets/explosion/explosion0.png");
	billboardFrameName.push_back("assets/explosion/explosion1.png");
	billboardFrameName.push_back("assets/explosion/explosion2.png");
	billboardFrameName.push_back("assets/explosion/explosion3.png");
	billboardFrameName.push_back("assets/explosion/explosion4.png");
	billboardFrameName.push_back("assets/explosion/explosion5.png");
	billboardFrameName.push_back("assets/explosion/explosion6.png");
	billboardFrameName.push_back("assets/explosion/explosion7.png");
	billboardFrameName.push_back("assets/explosion/explosion8.png");
	billboardFrameName.push_back("assets/explosion/explosion9.png");
	billboardFrameName.push_back("assets/explosion/explosion10.png");
	billboardFrameName.push_back("assets/explosion/explosion11.png");
	billboardFrameName.push_back("assets/explosion/explosion12.png");
	billboardFrameName.push_back("assets/explosion/explosion13.png");
	//billboardFrameName.push_back("assets/muzzle1.png");
	//billboardFrameName.push_back("assets/muzzle2.png");
	//billboardFrameName.push_back("assets/muzzle3.png");
	//billboardFrameName.push_back("assets/muzzle4.png");
	//billboardFrameName.push_back("assets/muzzle5.png");
	//billboardFrameName.push_back("assets/muzzle6.png");
	//billboardFrameName.push_back("assets/muzzle7.png");
	//billboardFrameName.push_back("assets/muzzle8.png");
	//billboardFrameName.push_back("assets/muzzle9.png");
	//billboardFrameName.push_back("assets/m1.png");
	//billboardFrameName.push_back("assets/m2.png");
	//billboardFrameName.push_back("assets/m3.png");
	//billboardFrameName.push_back("assets/m4.png");
	//billboardFrameName.push_back("assets/m5.png");
	//billboardFrameName.push_back("assets/m6.png");
	//billboardFrameName.push_back("assets/m7.png");
	//billboardFrameName.push_back("assets/m8.png");
	//billboardFrameName.push_back("assets/m9.png");
	//billboardFrameName.push_back("assets/m10.png");
	//billboardFrameName.push_back("assets/m11.png");
	//billboardFrameName.push_back("assets/m12.png");


	glGenTextures(billboardFrameName.size(), billboardFrameAnimation);

	int width, height;
	unsigned char* image;
	for (int i = 0; i < billboardFrameName.size(); i++) {
		glBindTexture(GL_TEXTURE_2D, billboardFrameAnimation[i]);

		image = SOIL_load_image(billboardFrameName[i], &width, &height, 0, SOIL_LOAD_RGBA);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
		SOIL_free_image_data(image);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	

}



void SceneManager::inicializarBuffersLineas() {
	glGenVertexArrays(1, &LVAO);
	glGenBuffers(1, &LVBO);
	// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
	glLineWidth(1.0f);
}



void SceneManager::renderLuces()
{
	//GLuint screenWidth = 1280, screenHeight = 720;
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	shaderLuces->Use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gPosition);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, gNormal);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, gTextura);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, gTangent);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, gBitangent);
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, gEmisivo);
	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_2D, gObjectColor);
	glActiveTexture(GL_TEXTURE7);
	glBindTexture(GL_TEXTURE_2D, bloomBuffers[0]);
	

	//LUZ SOLAR
	if (sunlight != nullptr) {
		sunlight->pasarDatosAlShader(shaderLuces);
	}
	//POINTLIGHT
	for (size_t i = 0; i < vecPointLight.size(); i++) {
		vecPointLight[i]->pasarDatosAlShader(shaderLuces, i);
	}
	//LUZ LINTERNA
	for (size_t i = 0; i < vecFlashLight.size(); i++) {
		vecFlashLight[i]->pasarDatosAlShader(shaderLuces, i);
	}

	//numero luces
	glUniform1i(glGetUniformLocation(shaderLuces->Program, "num_pointlight"), vecPointLight.size());
	glUniform1i(glGetUniformLocation(shaderLuces->Program, "num_flashlight"), vecFlashLight.size());

	//camaras
	glUniform3f(glGetUniformLocation(shaderLuces->Program, "viewPos"), activeCameraPos.getX(), activeCameraPos.getY(), activeCameraPos.getZ());
	glUniform1i(glGetUniformLocation(shaderLuces->Program, "draw_mode"), draw_mode);
	// renderizamos el plano pegado a la pantalla donde se visualiza nuestra imagen
	
	
	//copiamos el frame burffer leido
	glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); 
	glBlitFramebuffer(0, 0, (GLint)screenWidth, (GLint)screenHeight, 0, 0, (GLint)screenWidth, (GLint)screenHeight, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	RenderQuad();

}

void SceneManager::renderBlur()
{
	GLboolean horizontal = true, first_iteration = true;
	GLuint amount = 10;
	shaderBlur->Use();
	//glBindFramebuffer(GL_READ_FRAMEBUFFER, gDeferred);
	//glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	//glBlitFramebuffer(0, 0, (GLint)screenWidth, (GLint)screenHeight, 0, 0, (GLint)screenWidth, (GLint)screenHeight, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
	glActiveTexture(GL_TEXTURE0);
	for (GLuint i = 0; i < amount; i++)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, bloomFBO[horizontal]);
		glUniform1i(glGetUniformLocation(shaderBlur->Program, "horizontal"), horizontal);
		//si es la priemera iteracion activamos la textura de gEmisivo que es sobre la que se tiene que hacer blur
		//si no es la primera iteracion activamos la textura de bloomBuffers que es la de emisivo pero sobre la que ya se ha hecho alguna iteracion de blur
		glBindTexture(GL_TEXTURE_2D, first_iteration ? gEmisivo : bloomBuffers[!horizontal]); 
		RenderQuad();
		horizontal = !horizontal;
		first_iteration = false;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void SceneManager::renderBloom()
{
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//shaderBloom->Use();

	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, bloomBuffers[0]);
	//glActiveTexture(GL_TEXTURE1);
	//glBindTexture(GL_TEXTURE_2D, gEscena);
	////glUniform1f(glGetUniformLocation(shaderBloom->Program, "exposure"), exposure);
	//RenderQuad();
	//glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void SceneManager::renderSkybox()
{
	// Draw skybox first
	//glDepthMask(GL_FALSE);// Remember to turn depth writing off
	glDepthFunc(GL_LEQUAL);
	shaderSkybox->Use();
	glm::mat4 viewWithOutTras = glm::mat4(glm::mat3(camaraActiva->GetViewMatrix()));	
	glm::mat4 projec = camaraActiva->getProjectionMatrix();
	glUniformMatrix4fv(glGetUniformLocation(shaderSkybox->Program, "view"), 1, GL_FALSE, glm::value_ptr(viewWithOutTras));
	glUniformMatrix4fv(glGetUniformLocation(shaderSkybox->Program, "projection"), 1, GL_FALSE, glm::value_ptr(projec));
	// skybox cube
	glBindVertexArray(skyboxVAO);
	glActiveTexture(GL_TEXTURE0);
	glUniform1i(glGetUniformLocation(shaderSkybox->Program, "skybox"), 0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	//glDepthMask(GL_TRUE);
	glDepthFunc(GL_LESS);
}

void SceneManager::renderBildboard()
{

	glEnable(GL_BLEND);
	//shaderBildboard->Use();
	//glm::mat4 viewWithOutTras = camaraActiva->GetViewMatrix();	
	//glm::mat4 projec = camaraActiva->getProjectionMatrix();
	//glUniformMatrix4fv(glGetUniformLocation(shaderBildboard->Program, "view"), 1, GL_FALSE, glm::value_ptr(viewWithOutTras));
	//glUniformMatrix4fv(glGetUniformLocation(shaderBildboard->Program, "projection"), 1, GL_FALSE, glm::value_ptr(projec));
	//

	//glBindVertexArray(bildboardVAO);
	//glActiveTexture(GL_TEXTURE0);
	//glUniform1i(glGetUniformLocation(shaderBildboard->Program, "muzzle"), 0);
	//glBindTexture(GL_TEXTURE_2D, bildboardTexture);
	//glDrawArrays(GL_TRIANGLES, 0, 36);
	//glBindVertexArray(0);


	shaderBildboard->Use();
	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, SceneManager::i().bildboardTexture);
	billboardrendering = true;

	for (auto i = vectorBillboards.cbegin(); i != vectorBillboards.cend();) {
		if ((*i)->isAlive()) {
			(*i)->draw();
			i++;
		}
		else {
			delete (*i);
			i = vectorBillboards.erase(i);
		}	
	}

	glDisable(GL_BLEND);
	billboardrendering = false;
}

bool SceneManager::removeNode(TNode * node) {
	if (node != nullptr) {
		node->removeNode();
		return true;
	}
	return false;
}

TModel * SceneManager::crearNodoMalla(TMeshGroup * mesh)
{
	TNode * nuevoNodoMalla;
	TNode * nuevoNodoRotacion;
	TNode * nuevoNodoEscalado;
	TNode * nuevoNodoTraslacion;

	TModel* model = new TModel(mesh);

	//creamos los nodos malla y los nodos transformaciones necesaria para esta
	int id = model->getID();
	
	//rotacion antes de traslacion
	nuevoNodoRotacion = crearNodoRotacion(scene, id);
	nuevoNodoEscalado = crearNodoEscalado(nuevoNodoRotacion, id);
	nuevoNodoTraslacion = crearNodoTraslacion(nuevoNodoEscalado, id);
	nuevoNodoMalla = new TNode(id, nuevoNodoTraslacion);
	
	//asignamos los hijos
	scene->addChild(nuevoNodoRotacion);
	nuevoNodoRotacion->addChild(nuevoNodoEscalado);
	nuevoNodoEscalado->addChild(nuevoNodoTraslacion);
	nuevoNodoTraslacion->addChild(nuevoNodoMalla);
	
	
	

	//asignamos matrices de transformacion
	model->setTransformacionRotacion(static_cast<TTransform*> (nuevoNodoRotacion->getEntity()));
	model->setTransformacionEscalado(static_cast<TTransform*> (nuevoNodoEscalado->getEntity()));
	model->setTransformacionTraslacion(static_cast<TTransform*> (nuevoNodoTraslacion->getEntity()));
	model->setMiNodo(nuevoNodoMalla);

	//TODOOO aqui no tendriamos qeu setearle el modelo al TModel?
	nuevoNodoMalla->setEntity(model);
	//nuevoNodoMalla->setModel(model);
	//nuevoNodoMalla->getEntity()->setModel();

	return model;
}

TBillboard * SceneManager::crearBillBoard(Vec3<float> posicion)
{
	TBillboard* billboard = new TBillboard(getMesh("assets/billboard.obj"),posicion);

	vectorBillboards.push_back(billboard);

	return billboard;
}

TAnimation * SceneManager::crearNodoAnimacion(TAnimationGroupMesh * animGroup)
{
	TNode * nuevoNodoAnimacion;
	TNode * nuevoNodoRotacion;
	TNode * nuevoNodoEscalado;
	TNode * nuevoNodoTraslacion;

	TAnimation* animation = new TAnimation(animGroup);

	//creamos los nodos malla y los nodos transformaciones necesaria para esta
	int id = animation->getID();

	//rotacion antes de traslacion
	nuevoNodoRotacion = crearNodoRotacion(scene, id);
	nuevoNodoEscalado = crearNodoEscalado(nuevoNodoRotacion, id);
	nuevoNodoTraslacion = crearNodoTraslacion(nuevoNodoEscalado, id);
	nuevoNodoAnimacion = new TNode(id, nuevoNodoTraslacion);

	//asignamos los hijos
	scene->addChild(nuevoNodoRotacion);
	nuevoNodoRotacion->addChild(nuevoNodoEscalado);
	nuevoNodoEscalado->addChild(nuevoNodoTraslacion);
	nuevoNodoTraslacion->addChild(nuevoNodoAnimacion);




	//asignamos matrices de transformacion
	animation->setTransformacionRotacion(static_cast<TTransform*> (nuevoNodoRotacion->getEntity()));
	animation->setTransformacionEscalado(static_cast<TTransform*> (nuevoNodoEscalado->getEntity()));
	animation->setTransformacionTraslacion(static_cast<TTransform*> (nuevoNodoTraslacion->getEntity()));
	animation->setMiNodo(nuevoNodoAnimacion);

	//TODOOO aqui no tendriamos qeu setearle el modelo al TModel?
	nuevoNodoAnimacion->setEntity(animation);
	//nuevoNodoMalla->setModel(model);
	//nuevoNodoMalla->getEntity()->setModel();

	return animation;

	return nullptr;
}

TNode * SceneManager::crearNodoTransformacion(int entityID)
{
	//DUDA aqui tendriamos que tener tambien algun identificador en el nodo no? porque yo estoy creando 3 nodos de transformacion pero no se cual es cual.
	TNode* transNode = new TNode(entityID, scene);
	TTransform* trans = new TTransform();
	transNode->setEntity(trans);

	return transNode;
}

TNode * SceneManager::crearNodoTraslacion(TNode * nodoPadre, int entityID)
{
	TNode* transNode = new TNode(entityID, nodoPadre);
	TTransform* trans = new TTransform();
	trans->setPosition(Vec3<float>(0.0f, 0.0f, 0.0f));
	transNode->setEntity(trans);
	return transNode;
}

TNode * SceneManager::crearNodoRotacion(TNode * nodoPadre, int entityID)
{
	TNode* transNode = new TNode(entityID, nodoPadre);
	TTransform* trans = new TTransform();
	transNode->setEntity(trans);
	return transNode;
}

TNode * SceneManager::crearNodoEscalado(TNode * nodoPadre, int entityID)
{
	TNode* transNode = new TNode(entityID, nodoPadre);
	TTransform* trans = new TTransform();
	transNode->setEntity(trans);
	
	return transNode;
}

TSunLight * SceneManager::crearNodoSunLight(Vec3<float> direccion)
{
	TNode* luzNode;
	TNode * nuevoNodoRotacion;
	TNode * nuevoNodoTraslacion;

	//importante crear primero la entity y luego su nodo ya que tenemos que pasarle el id de la entity
	TSunLight* luz = new TSunLight(direccion);
	int id = luz->getID();

	//rotacion antes de traslacion
	nuevoNodoRotacion = crearNodoRotacion(scene, id);
	nuevoNodoTraslacion = crearNodoTraslacion(nuevoNodoRotacion, id);
	luzNode = new TNode(id, nuevoNodoTraslacion);

	//asignamos los hijos
	scene->addChild(nuevoNodoRotacion);
	nuevoNodoRotacion->addChild(nuevoNodoTraslacion);
	nuevoNodoTraslacion->addChild(luzNode);




	//asignamos matrices de transformacion
	luz->setTransformacionRotacion(static_cast<TTransform*> (nuevoNodoRotacion->getEntity()));
	luz->setTransformacionTraslacion(static_cast<TTransform*> (nuevoNodoTraslacion->getEntity()));
	luz->setMiNodo(luzNode);

	//seteamos la entity
	luzNode->setEntity(luz);

	//la a�adimos al sceneManager
	setSunLight(luz);


	return luz;
}

TPointLight * SceneManager::crearNodoPointLight(Vec3<float> posicion, float radioIn, float radioEx )
{
	TNode* luzNode;
	TNode * nuevoNodoRotacion;
	TNode * nuevoNodoTraslacion;

	//importante crear primero la entity y luego su nodo ya que tenemos que pasarle el id de la entity
	TPointLight* luz = new TPointLight(posicion, radioIn, radioEx);
	int id = luz->getID();

	//rotacion antes de traslacion
	nuevoNodoRotacion = crearNodoRotacion(scene, id);
	nuevoNodoTraslacion = crearNodoTraslacion(nuevoNodoRotacion, id);
	luzNode = new TNode(id, nuevoNodoTraslacion);

	//asignamos los hijos
	scene->addChild(nuevoNodoRotacion);
	nuevoNodoRotacion->addChild(nuevoNodoTraslacion);
	nuevoNodoTraslacion->addChild(luzNode);

	//asignamos matrices de transformacion
	luz->setTransformacionRotacion(static_cast<TTransform*> (nuevoNodoRotacion->getEntity()));
	luz->setTransformacionTraslacion(static_cast<TTransform*> (nuevoNodoTraslacion->getEntity()));
	luz->setMiNodo(luzNode);

	//seteamos la entity
	luzNode->setEntity(luz);

	//la a�adimos al sceneManager
	vecPointLight.push_back(luz);


	return luz;
}

TFlashLight * SceneManager::crearNodoFlashLight(Vec3<float> posicion, Vec3<float> direccion, float radioIn, float radioEx)
{

	TNode* luzNode;
	TNode * nuevoNodoRotacion;
	TNode * nuevoNodoTraslacion;

	//importante crear primero la entity y luego su nodo ya que tenemos que pasarle el id de la entity
	TFlashLight* luz = new TFlashLight(posicion,direccion, radioIn, radioEx);
	int id = luz->getID();

	//rotacion antes de traslacion
	nuevoNodoRotacion = crearNodoRotacion(scene, id);
	nuevoNodoTraslacion = crearNodoTraslacion(nuevoNodoRotacion, id);
	luzNode = new TNode(id, nuevoNodoTraslacion);

	//asignamos los hijos
	scene->addChild(nuevoNodoRotacion);
	nuevoNodoRotacion->addChild(nuevoNodoTraslacion);
	nuevoNodoTraslacion->addChild(luzNode);




	//asignamos matrices de transformacion
	luz->setTransformacionRotacion(static_cast<TTransform*> (nuevoNodoRotacion->getEntity()));
	luz->setTransformacionTraslacion(static_cast<TTransform*> (nuevoNodoTraslacion->getEntity()));
	luz->setMiNodo(luzNode);
	

	//seteamos la entity
	luzNode->setEntity(luz);


	//luz->setDirection(direccion);
	//la a�adimos al sceneManager
	vecFlashLight.push_back(luz);

	return luz;

}


TCamera * SceneManager::crearNodoCamara()
{
	TNode* cameraNode;
	TNode * nuevoNodoRotacion;
	TNode * nuevoNodoTraslacion;

	//importante crear primero la entity y luego su nodo ya que tenemos que pasarle el id de la entity
	TCamera* camara = new TCamera();
	int id = camara->getID();

	//rotacion antes de traslacion
	nuevoNodoRotacion = crearNodoRotacion(scene, id);
	nuevoNodoTraslacion = crearNodoTraslacion(nuevoNodoRotacion, id);
	cameraNode = new TNode(id, nuevoNodoTraslacion);

	//asignamos los hijos
	scene->addChild(nuevoNodoRotacion);
	nuevoNodoRotacion->addChild(nuevoNodoTraslacion);
	nuevoNodoTraslacion->addChild(cameraNode);




	//asignamos matrices de transformacion
	camara->setTransformacionRotacion(static_cast<TTransform*> (nuevoNodoRotacion->getEntity()));
	camara->setTransformacionTraslacion(static_cast<TTransform*> (nuevoNodoTraslacion->getEntity()));
	camara->setMiNodo(cameraNode);

	//seteamos la entity
	cameraNode->setEntity(camara);

	//la a�adimos al vector de camaras
	vectorCamaras.push_back(camara);


	return camara;
}






void SceneManager::RenderQuad()
{
	if (quadVAO == 0)
	{
		GLfloat quadVertices[] = {
			// Positions        // Texture Coords
			-1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
			1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};
		// Setup plane VAO
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	}
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}
void SceneManager::ziZoom(float z)
{
	camaraActiva->aumentarMira(z);
}

void SceneManager::zoomZout()
{
	camaraActiva->resetMira();
}
void SceneManager::shutdown()
{
	delete scene;

	ResourceManager::i().shutdown();
}

