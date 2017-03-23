#include "TMeshGroup.h"
#include "ResourceManager.h"
#include <glm/gtc/type_ptr.hpp>
#include "SceneManager.h"


TMeshGroup::TMeshGroup(GLchar * path, Shader* shaderPath) : sm(SceneManager::i()) {
	/*Shader* shader;
	if (*shaderPath) {
		shader = ResourceManager::i().getShader(shaderPath);
	} else {
		shader = ResourceManager::i().getShader("assets/model_loading.vs", "assets/model_loading.frag");
	}*/


	// Carmamos el modelo
	this->loadModel(path);
}

TMeshGroup::~TMeshGroup() {
	for (auto it = meshes.begin(); it != meshes.end(); ++it) {
		delete *it;
	}
	meshes.clear();
}

void TMeshGroup::beginDraw() {

	//Dibujamos los hijos (Si los hay)
	for (GLuint i = 0; i < this->meshes.size(); i++) {
		this->meshes[i]->beginDraw();
	}

}

void TMeshGroup::loadModel(const std::string& path) {
	// Leemos con ASSIMP
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
	// Miramos si hay alg�n error
	if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // Si no es 0
	{
		std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
		return;
	}
	// Cogemos el directorio del recurso
	this->directory = path.substr(0, path.find_last_of('/'));

	// Recorremos el nodo padre de ASSIMP recursivamente
	this->processNode(scene->mRootNode, scene);
}

void TMeshGroup::processNode(aiNode * node, const aiScene * scene) {
	// Procesamos cada malla
	for (GLuint i = 0; i < node->mNumMeshes; i++) {
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		//Guardamos la malla en nuestro array de mallas
		this->meshes.push_back(this->processMesh(mesh, scene));
	}

	// Procesamos los hijos de este nodo (recursivamente)
	for (GLuint i = 0; i < node->mNumChildren; i++) {
		this->processNode(node->mChildren[i], scene);
	}
}

TMesh* TMeshGroup::processMesh(aiMesh * mesh, const aiScene * scene) {
	// Datos b�sicos de las mallas (v�rtices, �ndices y texturas)
	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;
	std::vector<Texture*> textures;

	// Recorremos todos los v�rtices de la malla
	for (GLuint i = 0; i < mesh->mNumVertices; i++) {
		Vertex vertex;
		glm::vec3 vector;
		// Posiciones
		vector.x = mesh->mVertices[i].x;
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;
		vertex.Position = vector;
		// Normales
		vector.x = mesh->mNormals[i].x;
		vector.y = mesh->mNormals[i].y;
		vector.z = mesh->mNormals[i].z;
		vertex.Normal = vector;
		if (mesh->mTangents != NULL) {
			//tangentes
			vector.x = mesh->mTangents[i].x;
			vector.y = mesh->mTangents[i].y;
			vector.z = mesh->mTangents[i].z;
			vertex.Tangent = vector;
		}
		if (mesh->mBitangents != NULL) {
			//bitangentes
			vector.x = mesh->mBitangents[i].x;
			vector.y = mesh->mBitangents[i].y;
			vector.z = mesh->mBitangents[i].z;
			vertex.Bitangent = vector;
		}
		// Coordenadas de textura
		if (mesh->mTextureCoords[0]) // Tiene coordenadas de texturas?
		{
			glm::vec2 vec;
			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = mesh->mTextureCoords[0][i].y;
			vertex.TexCoords = vec;
		} else {
			vertex.TexCoords = glm::vec2(0.0f, 0.0f);
		}
		//Si no las tiene le ponemos unas por defecto
		vertices.push_back(vertex);
	}
	// Recorremos todas las caras de la malla (tri�ngulos) y nos guardamos sus vertex index
	for (GLuint i = 0; i < mesh->mNumFaces; i++) {
		aiFace face = mesh->mFaces[i];
		// Nos guardamos todos los �ndices
		for (GLuint j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}
	// Materiales!
	if (mesh->mMaterialIndex >= 0) {
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

		// Diffuse: texture_diffuseN
		//vector<Texture*> diffuseMaps;
		// Specular: texture_specularN
		//vector<Texture*> specularMaps;
		// Normal: texture_normalN
		//vector<Texture*> normalMaps;

		// 1. Diffuse maps
		this->loadMaterialTextures(textures, material, aiTextureType_DIFFUSE, "texture_diffuse");
		//textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
		// 2. Specular maps
		this->loadMaterialTextures(textures, material, aiTextureType_SPECULAR, "texture_specular");
		//textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
		//3. Normal maps
		this->loadMaterialTextures(textures, material, aiTextureType_NORMALS, "texture_normal");
		//textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
		//4.Tangent
		this->loadMaterialTextures(textures, material, aiTextureType_NORMALS, "texture_tangent");
		//5.Bitangent
		this->loadMaterialTextures(textures, material, aiTextureType_NORMALS, "texture_bitangent");
	}

	// Return del mesh preparado
	return new TMesh(vertices, indices, textures, SceneManager::i().shaderGeometria);
}

void TMeshGroup::loadMaterialTextures(std::vector<Texture*>& textVec, aiMaterial * mat, aiTextureType type, const std::string& typeName) {
	for (GLuint i = 0; i < mat->GetTextureCount(type); i++) {
		aiString str;
		mat->GetTexture(type, i, &str);
		// Miramos si se ha cargado la textura con anterioridad en el ResourceManager
		// Si no se ha cargado, el RM la carga. Si ya se hab�a cargado, el RM nos da un puntero a ella :)
		ResourceManager& rm = ResourceManager::i();
		Texture* text = rm.getTexture(str.C_Str(), typeName, this->directory);
		textVec.push_back(text);
	}
}


void TMeshGroup::endDraw() {
	//std::cout << u8"Adi�s" << std::endl;
	//TEntity::endDraw();
}