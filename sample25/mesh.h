#ifndef MESH_H
#define MESH_H

#include "shader.h"
#include "texture.h"
#include "vertex.h"
#include <glad/glad.h>
#include <vector>

class Mesh
{
private:
   std::vector<Vertex> mVertecis;
   std::vector<unsigned int> mIndeices;
   std::vector<Texture> mTextures;
   GLuint mVao, mVbo, mEbo;
   mutable int mUseCount;
   void incRef() const
   {
      mUseCount++;
   }
   void release()
   {
      if (--mUseCount == 0)
      {
         if (mVbo)
         {
            glDeleteBuffers(1, &mVbo);
            mVbo = 0;
         }

         if (mEbo)
         {
            glDeleteBuffers(1, &mEbo);
            mEbo = 0;
         }

         if (mVao)
         {
            glDeleteVertexArrays(1, &mVao);
            mVao = 0;
         }
         mUseCount = 0;
      }
   }

public:
   Mesh(std::vector<Vertex> verteices, std::vector<unsigned int> indeices, std::vector<Texture> textures)
       : mVertecis(verteices), mIndeices(indeices), mTextures(textures), mUseCount(1)
   {
      setupMesh();
   }

   Mesh(const Mesh &src)
   {
      if (&src == this)
      {
         return;
      }

      release();
      src.incRef();
      this->mVertecis = src.mVertecis;
      this->mIndeices = src.mIndeices;
      this->mTextures = src.mTextures;
      this->mVbo = src.mVbo;
      this->mEbo = src.mEbo;
      this->mVao = src.mVao;
      this->mUseCount = src.mUseCount;
   }

   Mesh(const Mesh &&src)
   {
      if (&src == this)
      {
         return;
      }
      release();
      src.incRef();
      this->mVertecis = src.mVertecis;
      this->mIndeices = src.mIndeices;
      this->mTextures = src.mTextures;
      this->mVbo = src.mVbo;
      this->mEbo = src.mEbo;
      this->mVao = src.mVao;
      this->mUseCount = src.mUseCount;
   }

   Mesh &operator=(const Mesh &src)
   {
      if (&src == this)
      {
         return *this;
      }

      release();
      src.incRef();
      this->mVertecis = src.mVertecis;
      this->mIndeices = src.mIndeices;
      this->mTextures = src.mTextures;
      this->mVbo = src.mVbo;
      this->mEbo = src.mEbo;
      this->mVao = src.mVao;
      this->mUseCount = src.mUseCount;
      return *this;
   }
   Mesh &operator=(const Mesh &&src)
   {
      if (&src == this)
      {
         return *this;
      }
      release();
      src.incRef();
      this->mVertecis = src.mVertecis;
      this->mIndeices = src.mIndeices;
      this->mTextures = src.mTextures;
      this->mVbo = src.mVbo;
      this->mEbo = src.mEbo;
      this->mVao = src.mVao;
      this->mUseCount = src.mUseCount;
      return *this;
   }

   void setupMesh()
   {
      glGenBuffers(1, &mVbo);
      glBindBuffer(GL_ARRAY_BUFFER, mVbo);
      glBufferData(GL_ARRAY_BUFFER, mVertecis.size() * sizeof(Vertex), &mVertecis[0], GL_STATIC_DRAW);
      glBindBuffer(GL_ARRAY_BUFFER, 0);

      glGenBuffers(1, &mEbo);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEbo);
      glBufferData(GL_ELEMENT_ARRAY_BUFFER, mIndeices.size() * sizeof(unsigned int), &mIndeices[0], GL_STATIC_DRAW);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

      glGenVertexArrays(1, &mVao);
      glBindVertexArray(mVao);
      glBindBuffer(GL_ARRAY_BUFFER, mVbo);
      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
      glEnableVertexAttribArray(0);
      glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, normal));
      glEnableVertexAttribArray(1);
      glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, texCoords));
      glEnableVertexAttribArray(2);
      glBindVertexArray(0);
   }
   void draw(Shader &shader)
   {
      unsigned int diffuseNr = 1;
      unsigned int specularNr = 1;

      for (unsigned int i = 0; i < mTextures.size(); i++)
      {
         glActiveTexture(GL_TEXTURE0 + i);
         std::string number;
         std::string type = mTextures[i].getType();
         if (type == "texture_diffuse")
         {
            number = std::to_string(diffuseNr++);
         }
         else if (type == "texture_specular")
         {
            number = std::to_string(specularNr++);
         }
         shader.setInt(("material." + type + number).c_str(), i);
         glBindTexture(GL_TEXTURE_2D, mTextures[i].getTextureId());
      }
      glActiveTexture(GL_TEXTURE0);
      glBindVertexArray(mVao);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEbo);
      glDrawElements(GL_TRIANGLES, mIndeices.size(), GL_UNSIGNED_INT, 0);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
      glBindVertexArray(0);
   }
   ~Mesh();

   GLuint getVao(){
      return mVao;
   }

   GLuint getEbo(){
      return mEbo;
   }

   std::vector<unsigned int>& getIndices(){
      return mIndeices;
   }

   std::vector<Texture>& getTextures(){
      return mTextures;
   }
};

Mesh::~Mesh()
{
   release();
}

#endif