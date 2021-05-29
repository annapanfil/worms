#include "model.hpp"

//TODO: wywołać readTexture

Model::Model(const std::string& obj_filename){
  load(obj_filename);
}

void Model::load(const std::string& filename){
  Assimp::Importer importer;
  const aiScene* scene = importer.ReadFile(filename,
    aiProcess_Triangulate |	//model będzie składał się tylko z trójkątów
    aiProcess_FlipUVs			|	//odwrócenie współrzędnych - (0,0) będzie w lewym dolnym rogu
    aiProcess_GenSmoothNormals //Generuj uśrednione wektory normalne, jeśli ich nie ma
    // aiProcess_JoinIdenticalVertices
    //   aiProcess_CalcTangentSpace       | //wylicz przestrzeń styczną, które tworzą przestrzeń ściany; przydatne dla bump shadingu
    // aiProcess_SortByPType
  );
  std::cout<<importer.GetErrorString()<<std::endl;

  if (scene->HasMeshes()){
    for (int i=0; i<scene->mNumMeshes;i++){
      meshes.emplace_back(scene, i);
    }
  }
}

void Model::draw(GLFWwindow* window,float angle_x,float angle_y){
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glm::mat4 V=glm::lookAt(glm::vec3(0.0f, 7.0f,-10.0f), glm::vec3(0.0f,0.0f,0.0f), glm::vec3(0.0f,1.0f,0.0f)); //compute view matrix
  glm::mat4 P=glm::perspective(glm::radians(50.0f), 1.0f, 1.0f, 50.0f); //compute projection matrix

  glm::mat4 M=glm::mat4(1.0f);
  M=glm::rotate(M,angle_y,glm::vec3(1.0f,0.0f,0.0f)); //Compute model matrix
  M=glm::rotate(M,angle_x,glm::vec3(0.0f,1.0f,0.0f));

  for (auto i = meshes.begin(); i!=meshes.end(); ++i){
    i->draw(window, V, P, M);
  }
  glfwSwapBuffers(window); //Copy back buffer to front buffer
}

void Model::readTextures(std::vector<const char*> filenames){
  for (int i = 0; i < filenames.size(); i++){
    meshes[i].readTexture(filenames[i]);
    std::cout<<"Texture "<<filenames[i]<<" read\n";
  }

}

Mesh::Mesh(const aiScene* scene, int nr){
  aiMesh* mesh = scene->mMeshes[nr];

	//WIERZCHOŁKI
	for (int i=0; i<mesh->mNumVertices; i++){
		aiVector3D vertex = mesh->mVertices[i];		//aiVector3D podobny do glm::vec3
		verts.push_back(glm::vec4(vertex.x, vertex.y, vertex.z, 1));

		aiVector3D normal = mesh->mNormals[i];	//wertory znormalizowane
		norms.push_back(glm::vec4(normal.x, normal.y, normal.z, 0)); //kierunek a nie pozycja, więc w=0

		//może być do 8 zestawów współrzędnych teksturowania.
		unsigned int uv_num = mesh->GetNumUVChannels();
		// std::cout<<"Liczba zestawów współrzędnych teksturowania: "<<uv_num<<std::endl;
		unsigned int uv_dim = mesh->mNumUVComponents[0]; //Ilość składowych wsp. teksturowania dla 0. zestawu
		// std::cout<<"Liczba współrzędnych: "<<uv_dim<<std::endl;

		aiVector3D texCoord = mesh->mTextureCoords[0][i];
		texCoords.push_back(glm::vec2(texCoord.x, texCoord.y)); //jeżeli tekstura ma tylko 2 wymiary
	}

	//WIELOKĄTY SKŁADOWE
	for (int i=0; i<mesh->mNumFaces; i++){
		aiFace& face = mesh->mFaces[i];	// face to 1 wielokąt siatki

		for (int j=0; j<face.mNumIndices; j++){ //zawsze 3, bo przerobiliśmy na trójkąty
			indices.push_back(face.mIndices[j]);
		}
	}

  //TEKSTURY
  //TODO again ^%&^*&^%#$@
}

void Mesh::readTexture(const char* filename){
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

  texture = tex;
}

void Mesh::draw(GLFWwindow* window, glm::mat4 V, glm::mat4 P, glm::mat4 M){
  spLambertTextured->use();  //activate shading program

	//Send parameters to graphics card
  glUniformMatrix4fv(spLambertTextured->u("P"),1,false,glm::value_ptr(P));
  glUniformMatrix4fv(spLambertTextured->u("V"),1,false,glm::value_ptr(V));
  glUniformMatrix4fv(spLambertTextured->u("M"),1,false,glm::value_ptr(M));

	glEnableVertexAttribArray(spLambertTextured->a("vertex")); //Enable sending data to the attribute vertex
  glVertexAttribPointer(spLambertTextured->a("vertex"),4,GL_FLOAT,false,0, verts.data()); //Specify source of the data for the attribute vertex

	glEnableVertexAttribArray(spLambertTextured->a("texCoord"));
  glVertexAttribPointer(spLambertTextured->a("texCoord"),4,GL_FLOAT,false,0, texCoords.data());

	glEnableVertexAttribArray(spLambertTextured->a("normal"));
  glVertexAttribPointer(spLambertTextured->a("normal"),4,GL_FLOAT,false,0, norms.data());

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glUniform1i(spLambertTextured->u("tex"), 0);


  glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, indices.data()); //Draw the object

  glDisableVertexAttribArray(spLambertTextured->a("vertex")); //Disable sending data to the attribute vertex
	glDisableVertexAttribArray(spLambertTextured->a("texCoord"));
	glDisableVertexAttribArray(spLambertTextured->a("normal"));
}
