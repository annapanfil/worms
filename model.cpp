#include "model.hpp"


Model::Model(const std::string& obj_filename, bool _whole=true){
  load(obj_filename);
  this->whole = _whole;
}

void Model::load(const std::string& filename){
  std::cout<<"\nReading model "<<filename;
  Assimp::Importer importer;
  const aiScene* scene = importer.ReadFile(filename,
    aiProcess_Triangulate |	//model będzie składał się tylko z trójkątów
    aiProcess_FlipUVs			|	//odwrócenie współrzędnych - (0,0) będzie w lewym dolnym rogu
    aiProcess_GenSmoothNormals //Generuj uśrednione wektory normalne, jeśli ich nie ma
    // aiProcess_JoinIdenticalVertices
    | aiProcess_CalcTangentSpace //wylicz przestrzeń styczną
    // | aiProcess_SortByPType
  );
  std::cout<<importer.GetErrorString()<<std::endl;

  if (scene->HasMeshes()){
    for (int i=0; i<scene->mNumMeshes;i++){
      meshes.emplace_back(scene, i);
    }
  }
}

void Model::draw(GLFWwindow* window,float angle_x,float angle_y, glm::vec3 position, glm::mat4 V, glm::vec3 scale=glm::vec3(1,1,1)){
  glm::mat4 P=glm::perspective(glm::radians(50.0f), 1.0f, 1.0f, 120.0f); //compute projection matrix

  glm::mat4 M=glm::mat4(1.0f);
  M=glm::translate(M, position);
  M=glm::scale(M, scale);
  M=glm::rotate(M,angle_y,glm::vec3(1.0f,0.0f,0.0f)); //Compute model matrix
  M=glm::rotate(M,angle_x,glm::vec3(0.0f,1.0f,0.0f));

  for (int i=0; i<meshes.size(); i++){
    if (this->whole)
      meshes[i].draw(window, V, P, M, this->textures);
    else
      meshes[i].draw_part(window, V, P, M, this->textures[i]);
  }
}


void Model::readTexture(const char* filename){
  GLuint tex;

  glActiveTexture(GL_TEXTURE0); //choose a texture handler to edit

  //Read the file into computers memory
  std::vector<unsigned char> image;   //Allocate a vector for storing the image
  unsigned width, height;   //Variables which will contain the image size

  //Read the image
  unsigned error = lodepng::decode(image, width, height, filename);

  //Import the image into graphics cards memory
  glGenTextures(1,&tex); //Initialize one handle
  glBindTexture(GL_TEXTURE_2D, tex); //Activate handle (bind it to the active texturing unit)
  //Import the image into the GC memory associated with the handle
  glTexImage2D(GL_TEXTURE_2D, 0, 4, width, height, 0,
    GL_RGBA, GL_UNSIGNED_BYTE, (unsigned char*) image.data());

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  this->textures.push_back(tex);
}


void Model::readTextures(std::vector<const char*> filenames){
  for (int i = 0; i < filenames.size(); i++){
    this->readTexture(filenames[i]);
    std::cout<<"Texture "<<filenames[i]<<" read\n";
  }

}


//////////////////////////////////////////////////////////////////////

Mesh::Mesh(const aiScene* scene, int nr){
  aiMesh* mesh = scene->mMeshes[nr];

	//WIERZCHOŁKI
	for (int i=0; i<mesh->mNumVertices; i++){
    // WSPÓŁRZĘDNE WIERZCHOŁKÓW
	  aiVector3D vertex = mesh->mVertices[i];		//aiVector3D podobny do glm::vec3
		verts.push_back(glm::vec4(vertex.x, vertex.y, vertex.z, 1));

    // WEKTORY NORMALNE
		aiVector3D normal = mesh->mNormals[i];	//wertory znormalizowane
		norms.push_back(glm::vec4(normal.x, normal.y, normal.z, 0)); //kierunek a nie pozycja, więc w=0

    // WSPÓŁRZĘDNE TEKSTUROWANIA
		aiVector3D texCoord = mesh->mTextureCoords[0][i]; //numer zestawu, numer wierzchołka
		texCoords.push_back(glm::vec2(texCoord.x, texCoord.y)); //jeżeli tekstura ma tylko 2 wymiary

    // // PRZESTRZEŃ STYCZNA
    aiVector3D tangent = mesh->mTangents[i];
    tangents.push_back(glm::vec4(tangent.x, tangent.y, tangent.z, 0));
	}

	//WIELOKĄTY SKŁADOWE
	for (int i=0; i<mesh->mNumFaces; i++){
		aiFace& face = mesh->mFaces[i];	// face to 1 wielokąt siatki

		for (int j=0; j<face.mNumIndices; j++){ //zawsze 3, bo przerobiliśmy na trójkąty
			indices.push_back(face.mIndices[j]);
		}

	}

  //TEKSTURY
  aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
}

void Mesh::draw(GLFWwindow* window, glm::mat4 V, glm::mat4 P, glm::mat4 M, std::vector<GLuint> textures){
  sp->use();  //activate shading program

	//Send parameters to graphics card
  glUniformMatrix4fv(sp->u("P"),1,false,glm::value_ptr(P));
  glUniformMatrix4fv(sp->u("V"),1,false,glm::value_ptr(V));
  glUniformMatrix4fv(sp->u("M"),1,false,glm::value_ptr(M));
  // glUniformMatrix4fv(sp->u("light_position"), )

	glEnableVertexAttribArray(sp->a("vertex")); //Enable sending data to the attribute vertex
  glVertexAttribPointer(sp->a("vertex"),4,GL_FLOAT,false,0, verts.data()); //Specify source of the data for the attribute vertex

	glEnableVertexAttribArray(sp->a("texCoord"));
  glVertexAttribPointer(sp->a("texCoord"),2,GL_FLOAT,false,0, texCoords.data());

	glEnableVertexAttribArray(sp->a("normal"));
  glVertexAttribPointer(sp->a("normal"),4,GL_FLOAT,false,0, norms.data());

  glEnableVertexAttribArray(sp->a("tangent"));
  glVertexAttribPointer(sp->a("tangent"),4,GL_FLOAT,false,0, tangents.data());

  glUniform1i(sp->u("texMapColor"), 0); // powiązanie zmiennej z jednostką teksturującą
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textures[0]);

  glUniform1i(sp->u("texMapReflect"), 1);
  glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, textures[1]);

  glUniform1i(sp->u("texMapNormal"), 2);
  glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, textures[2]);

  glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, indices.data()); //Draw the object

  glDisableVertexAttribArray(sp->a("vertex")); //Disable sending data to the attribute vertex
	glDisableVertexAttribArray(sp->a("texCoord"));
	glDisableVertexAttribArray(sp->a("normal"));
  glDisableVertexAttribArray(sp->a("tangent"));
  glDisableVertexAttribArray(sp->a("texMapColor"));
  glDisableVertexAttribArray(sp->a("texMapReflect"));
  glDisableVertexAttribArray(sp->a("texMapNormal"));
}

void Mesh::draw_part(GLFWwindow* window, glm::mat4 V, glm::mat4 P, glm::mat4 M, GLuint texture){
  sp_simpler->use();  //activate shading program

	//Send parameters to graphics card
  glUniformMatrix4fv(sp_simpler->u("P"),1,false,glm::value_ptr(P));
  glUniformMatrix4fv(sp_simpler->u("V"),1,false,glm::value_ptr(V));
  glUniformMatrix4fv(sp_simpler->u("M"),1,false,glm::value_ptr(M));
  // glUniformMatrix4fv(sp_simpler->u("light_position"), )

	glEnableVertexAttribArray(sp_simpler->a("vertex")); //Enable sending data to the attribute vertex
  glVertexAttribPointer(sp_simpler->a("vertex"),4,GL_FLOAT,false,0, verts.data()); //Specify source of the data for the attribute vertex

	glEnableVertexAttribArray(sp_simpler->a("texCoord"));
  glVertexAttribPointer(sp_simpler->a("texCoord"),2,GL_FLOAT,false,0, texCoords.data());

	glEnableVertexAttribArray(sp_simpler->a("normal"));
  glVertexAttribPointer(sp_simpler->a("normal"),4,GL_FLOAT,false,0, norms.data());

  glUniform1i(sp->u("tex"), 0); // powiązanie zmiennej z jednostką teksturującą
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

  glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, indices.data()); //Draw the object

  glDisableVertexAttribArray(sp->a("vertex")); //Disable sending data to the attribute vertex
	glDisableVertexAttribArray(sp->a("texCoord"));
	glDisableVertexAttribArray(sp->a("normal"));
  glDisableVertexAttribArray(sp->a("tex"));

}

/////////////////////////////////////////////////////////////////////
