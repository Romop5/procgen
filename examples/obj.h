#include <glm/glm.hpp>
#include <iostream>
#include <vector>
#include <stdio.h>

glm::vec3 memberTimes(const glm::vec3& a, const glm::vec3& b)
{
	return glm::vec3(a.x*b.x,a.y*b.y,a.z*b.z);
}

class Object
{
	std::vector<glm::vec3> vertices;
	public:
	void addPrimitive(glm::vec3 center, glm::vec3 size)
	{
		// front 
		vertices.push_back(center+memberTimes(glm::vec3(1.0f,1.0f,1.0f),size));
		vertices.push_back(center+memberTimes(glm::vec3(-1.0f,1.0f,1.0f),size));
		vertices.push_back(center+memberTimes(glm::vec3(1.0f,-1.0f,1.0f),size));
		vertices.push_back(center+memberTimes(glm::vec3(-1.0f,-1.0f,1.0f),size));

		// back 
		vertices.push_back(center+memberTimes(glm::vec3(-1.0f,-1.0f,-1.0f),size));
		vertices.push_back(center+memberTimes(glm::vec3(1.0f,-1.0f,-1.0f),size));
		vertices.push_back(center+memberTimes(glm::vec3(-1.0f,1.0f,-1.0f),size));
		vertices.push_back(center+memberTimes(glm::vec3(1.0f,1.0f,-1.0f),size));

		// right
		vertices.push_back(center+memberTimes(glm::vec3(1.0f,1.0f,1.0f),size));
		vertices.push_back(center+memberTimes(glm::vec3(1.0f,1.0f,-1.0f),size));
		vertices.push_back(center+memberTimes(glm::vec3(1.0f,-1.0f,1.0f),size));
		vertices.push_back(center+memberTimes(glm::vec3(1.0f,-1.0f,-1.0f),size));

		// left 
		vertices.push_back(center+memberTimes(glm::vec3(-1.0f,-1.0f,-1.0f),size));
		vertices.push_back(center+memberTimes(glm::vec3(-1.0f,-1.0f,1.0f),size));
		vertices.push_back(center+memberTimes(glm::vec3(-1.0f,1.0f,-1.0f),size));
		vertices.push_back(center+memberTimes(glm::vec3(-1.0f,1.0f,1.0f),size));

		// top 
		vertices.push_back(center+memberTimes(glm::vec3(1.0f,1.0f,1.0f),size));
		vertices.push_back(center+memberTimes(glm::vec3(-1.0f,1.0f,1.0f),size));
		vertices.push_back(center+memberTimes(glm::vec3(1.0f, 1.0f,-1.0f),size));
		vertices.push_back(center+memberTimes(glm::vec3(-1.0f,1.0f,-1.0f),size));

		// top 
		vertices.push_back(center+memberTimes(glm::vec3(-1.0f,-1.0f,-1.0f),size));
		vertices.push_back(center+memberTimes(glm::vec3(1.0f,-1.0f,-1.0f),size));
		vertices.push_back(center+memberTimes(glm::vec3(-1.0f, -1.0f,1.0f),size));
		vertices.push_back(center+memberTimes(glm::vec3(1.0f,-1.0f,1.0f),size));
	}
	void dumpVertices()
	{
		for(size_t i = 0; i < vertices.size(); i++)
		{
		//	std::cout << "v " << vertices[i].x << " " << vertices[i].y << " " << vertices[i].z << std::endl;
			printf("v %.1f %.1f %.1f\n", vertices[i].x, vertices[i].y, vertices[i].z);
		}
		for(size_t i = 1; i < vertices.size()+1; i+=4)
		{
			std::cout << "f " << i << " " << (i+1) << " " << (i+2) << std::endl;
			std::cout << "f " << (i+3) << " " << (i+1) << " " << (i+2) << std::endl;
		}

	}
	void dumpVertices(std::string fileName)
	{
		FILE* output = fopen(fileName.c_str(), "w");
		if(!output)
			return;
		for(size_t i = 0; i < vertices.size(); i++)
		{
		//	std::cout << "v " << vertices[i].x << " " << vertices[i].y << " " << vertices[i].z << std::endl;
			fprintf(output,"v %.1f %.1f %.1f\n", vertices[i].x, vertices[i].y, vertices[i].z);
		}
		for(size_t i = 1; i < vertices.size()+1; i+=4)
		{
			fprintf(output,"f %d %d %d\n",i,i+1,i+2);
			fprintf(output,"f %d %d %d\n",i+3,i+1,i+2);
		}

	}

};


/*
int main()
{
	Object o;
	o.addPrimitive(glm::vec3(0.f,0.f,0.f),glm::vec3(3.f,1.f,3.f));
	o.addPrimitive(glm::vec3(0.f,1.f,0.f),glm::vec3(2.f,1.f,2.f));
	o.addPrimitive(glm::vec3(0.f,2.f,0.f),glm::vec3(1.f,1.f,1.f));
	o.dumpVertices();
}; */
