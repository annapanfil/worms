#include "model.hpp"

using std::endl;
using std::cout;

Model::Model(const std::string& obj_filename, bool _whole=true){
  load(obj_filename);
  this->whole = _whole;
}

void Model::load(const std::string& filename){
  cout<<"wczytywanie modelu "<<filename<<endl;
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
  // for (auto i = meshes.begin(); i!=meshes.end(); ++i){
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
  cout<<"Mesh "<<nr<<endl;

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

  //niepotrzebne
  //może być do 8 zestawów współrzędnych teksturowania.
  // unsigned int uv_num = mesh->GetNumUVChannels();
  // std::cout<<"Liczba zestawów współrzędnych teksturowania: "<<uv_num<<std::endl;
  // unsigned int uv_dim = mesh->mNumUVComponents[0]; //Ilość składowych wsp. teksturowania dla 0. zestawu
  // std::cout<<"Liczba współrzędnych: "<<uv_dim<<std::endl;


	//WIELOKĄTY SKŁADOWE
	for (int i=0; i<mesh->mNumFaces; i++){
		aiFace& face = mesh->mFaces[i];	// face to 1 wielokąt siatki

		for (int j=0; j<face.mNumIndices; j++){ //zawsze 3, bo przerobiliśmy na trójkąty
			indices.push_back(face.mIndices[j]);
		}


    // this->calc_TBN_vectors(face);


	}

  //TEKSTURY
  aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

  //sprawdzenie jakie tekstury ma obiekt i gdzie one są
  //aiTextureType - rodzaj tekstury (enum)
  // cout<<"Rodzaje tekstur"<<endl;
  // for (int i=0; i<19; i++){
  //   cout<<i<<" "<<material->GetTextureCount(aiTextureType(i))<<endl;
  // }
  //
  // for (int i=0; i<material->GetTextureCount(aiTextureType(1));
  // i++){
  //   aiString filename;
  //   material->GetTexture(aiTextureType(1), i, &filename);
  //   cout<<filename.C_Str()<<endl;
  //
  // }
}

 /*void Mesh::calc_TBN_vectors(aiFace& face){
  glm::vec2 uv1 = this->texCoords[face.mIndices[0]];
  glm::vec2 uv2 = this->texCoords[face.mIndices[1]];
  glm::vec2 uv3 = this->texCoords[face.mIndices[2]];

  glm::vec4 pos1 = this->verts[face.mIndices[0]];
  glm::vec4 pos2 = this->verts[face.mIndices[1]];
  glm::vec4 pos3 = this->verts[face.mIndices[2]];

  glm::vec2 deltaUV1 = uv2-uv1;
  glm::vec2 deltaUV2 = uv3-uv1;
  glm::vec4 edge1 = pos2-pos1;
  glm::vec4 edge2 = pos3-pos1;

  // glm::mat2 c = glm::mat2(glm::vec2(c21[0], c31[0]), glm::vec2(c21[1],c31[1]));
  // glm::vec2 v = glm::vec2(edge1,edge2);
  // glm::vec2 tb = glm::transpose(c)*v;
  // auto t = glm::normalize(tb[0]);
  // auto b = glm::normalize(tb[1]);

  // glm::vec4 t = glm::normalize(c21[0] * edge1 + c31[0] * edge2);
  // glm::vec4 b = glm::normalize(c21[1] * edge1 + c31[1] * edge2);
  // auto n = glm::normalize(t * b);

  float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

  glm::vec4 t;
  glm::vec4 b;

  t.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
  t.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
  t.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);

  b.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
  b.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
  b.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);

  glm::vec4 n = glm::normalize(t * b);
  t = glm::normalize(t);
  b = glm::normalize(b);


  this->invTBNx.push_back(glm::vec4(t.x, b.x, n.x, 0));
  this->invTBNy.push_back(glm::vec4(t.y, b.y, n.y, 0));
  this->invTBNz.push_back(glm::vec4(t.z, b.z, n.z, 0));
}*/

/* ze strony assimpa, do wczytania większej ilości tekstur
vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName)
{
    vector<Texture> textures;
    for(unsigned int i = 0; i < mat->GetTextureCount(type); i++)
    {
        aiString str;
        mat->GetTexture(type, i, &str);
        Texture texture;
        texture.id = TextureFromFile(str.C_Str(), directory);
        texture.type = typeName;
        texture.path = str;
        textures.push_back(texture);
    }
    return textures;
}  */

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
