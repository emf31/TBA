#include "ResourceManager.h"



ResourceManager::ResourceManager() {
}


TModel* ResourceManager::getMesh(const std::string& path, Shader* shader) {
	if (path != "") {
		if (models.find(path) != models.end()) {
			//Si ya esta en el mapa lo devolvemos al instante. No lo cargamos dos veces!
			return models[path];
		} else {
			//Creamos un modelo nuevo y le pasamos el path
			TModel *model = new TModel((GLchar*)path.c_str(), shader);
			//Guardamos el puntero en el mapa de models
			models[path] = model;
			//Devolvemos el puntero del modelo en el mapa
			return models[path];
		}
		std::cout << "ERROR CARGANDO MESH: " << path << std::endl;
		return NULL;
	}
	std::cout << "ERROR CARGANDO MESH: NO SE HA ESPECIFICADO UN MESH..." << std::endl;
	return NULL;
}



/* Si se pasan los dos paths, primero se busca por el vertexShader y lo devuelve si est� en el mapa. Si no est�, se crea
Si solo se pasa el vertex simplemente se busca en el mapa y se devuelve si est� */
Shader* ResourceManager::getShader(const std::string& vertexShader, std::string fragmentShader) {
	if (vertexShader != "" && fragmentShader != "") {
		if (shaders.find(vertexShader) != shaders.end()) {
			return shaders[vertexShader];
		} else {
			Shader *shader = new Shader((GLchar*)vertexShader.c_str(), (GLchar*)fragmentShader.c_str());
			shaders[vertexShader] = shader;
			return shaders[vertexShader];
		}
		std::cout << "Error al cargar shaders: " << vertexShader << " - " << fragmentShader << std::endl;
	} else if (fragmentShader == "") {
		if (shaders.find(vertexShader) != shaders.end()) {
			return shaders[vertexShader];
		}
	}
	return NULL;
}

// Carga una textura de un fichero
Texture* ResourceManager::getTexture(const std::string& path, const std::string& type, const std::string& directory) {
	if (path != "") {
		if (textures.find(path) != textures.end()) {
			// Si la textura ya est� en nuestro mapa la devolvemos.
			return textures[path];
		} else {
			// Si no lop est�, primero la cargamos :))
			Texture* texture = new Texture();
			texture->id = TextureFromFile(path.c_str(), directory.substr(0, directory.find_last_of('/')));
			texture->type = type;
			textures[path] = texture;
			return textures[path];
		}
	}
}

void ResourceManager::shutdown()
{
	//models
	for (auto it = models.begin(); it != models.end(); ++it) {
		delete it->second;
	}
	//textures
	for (auto it = textures.begin(); it != textures.end(); ++it) {
		delete it->second;
	}
	//shaders
	for (auto it = shaders.begin(); it != shaders.end(); ++it) {
		delete it->second;
	}
	models.clear();
	textures.clear();
	shaders.clear();
}

GLint ResourceManager::TextureFromFile(const char * path, string directory) {
	// Generamos un ID de textura y cargamos la imagen con SOIL 
	string filename = string(path);
	filename = directory + '/' + filename;
	cout << filename << endl;
	GLuint textureID;
	glGenTextures(1, &textureID);
	int width, height;
	unsigned char* image = SOIL_load_image(filename.c_str(), &width, &height, 0, SOIL_LOAD_RGB); // Podrian cambiarse a RGBA para imagenes con Alpha channel (es mejor?)
																								 // Assignamos la textura al nuevo ID
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);

	// Parametros de filtrado
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	SOIL_free_image_data(image); // Liberamos la imagen de SOIL (Ya no hace falta)
	return textureID;
}
