#include "Cloth.h"
#include <iostream>

Cloth::Cloth(const float& mass, const int& samplerAmount, const float& clothSize)
	: m_SamplerAmount(samplerAmount), m_ClothSize(clothSize), m_Mass(mass)
{
	m_EachMass = m_Mass / (m_SamplerAmount * m_SamplerAmount);
	m_EachMassInvert = 1.0f / m_EachMass;
	m_EachLength = m_ClothSize / (m_SamplerAmount - 1);

	m_PointMass = new PointMass* [samplerAmount * samplerAmount];
	for (int row = 0; row < samplerAmount; row++)
	{
		for (int col = 0; col < samplerAmount; col++)
		{
			int index = (row * samplerAmount) + col;

			float PosX = ((float)row / samplerAmount) * clothSize;
			float PosY = ((float)col / samplerAmount) * clothSize;

			m_PointMass[index] = new PointMass({ PosX, 0.0f, PosY });
		}
	}

	for (int row = 0; row < samplerAmount; row++)
	{
		for (int col = 0; col < samplerAmount; col++)
		{
			int index = (row * samplerAmount) + col;
			size_t& size = m_PointMass[index]->neighborSize;
			PointMass* currentPointMass = m_PointMass[index];

			// Left
			if (col - 1 >= 0)
				currentPointMass->neighborList[size++] = { GetPointMass(row, col - 1), STRUCTURAL_X };
			// Right
			if (col + 1 < m_SamplerAmount)
				currentPointMass->neighborList[size++] = { GetPointMass(row, col + 1), STRUCTURAL_X };
			
			// Up
			if (row - 1 >= 0)
				currentPointMass->neighborList[size++] = { GetPointMass(row - 1, col), STRUCTURAL_Y };
			// Down
			if (row + 1 < m_SamplerAmount)
				currentPointMass->neighborList[size++] = { GetPointMass(row + 1, col), STRUCTURAL_Y };

			// Up_Left
			if (row - 1 >= 0 && col - 1 >= 0)
				currentPointMass->neighborList[size++] = { GetPointMass(row - 1, col - 1), SHEAR };
			// Down_Left
			if (row + 1 < m_SamplerAmount && col - 1 >= 0)
				currentPointMass->neighborList[size++] = { GetPointMass(row + 1, col - 1), SHEAR };
			// Up_Right
			if (row - 1 >= 0 && col + 1 < m_SamplerAmount)
				currentPointMass->neighborList[size++] = { GetPointMass(row - 1, col + 1), SHEAR };
			// Down_Right
			if (row + 1 < m_SamplerAmount && col + 1 < m_SamplerAmount)
				currentPointMass->neighborList[size++] = { GetPointMass(row + 1, col + 1), SHEAR };
			
			// Left-Left
			if (col - 2 >= 0)
				currentPointMass->neighborList[size++] = { GetPointMass(row, col - 2), FLEXION_X };
			// Right-Right
			if (col + 2 < m_SamplerAmount)
				currentPointMass->neighborList[size++] = { GetPointMass(row, col + 2), FLEXION_X };

			// Up-Up
			if (row - 2 >= 0)
				currentPointMass->neighborList[size++] = { GetPointMass(row - 2, col), FLEXION_Y };
			// Down-Down
			if (row + 2 < m_SamplerAmount)
				currentPointMass->neighborList[size++] = { GetPointMass(row + 2, col), FLEXION_Y };
		}
	}

	GenerateCubeBuffer(m_PointMassMesh.vao, m_PointMassMesh.vbo, m_PointMassMesh.ebo, &m_PointMassMesh.indices);
	m_ClothTexture.SetTexture("Texture/RedCloth.jpg");
}
Cloth::~Cloth()
{
	for (int index = 0; index < m_SamplerAmount; index++)
	{
		delete m_PointMass[index];
	}

	delete m_PointMass;
}

void Cloth::Reset()
{
	ResetComponent();
	ResetPosition();
}
void Cloth::ResetPosition()
{
	for (int row = 0; row < m_SamplerAmount; row++)
	{
		for (int col = 0; col < m_SamplerAmount; col++)
		{
			int index = (row * m_SamplerAmount) + col;

			float PosX = ((float)row / m_SamplerAmount) * m_ClothSize;
			float PosY = ((float)col / m_SamplerAmount) * m_ClothSize;

			m_PointMass[index]->position = { PosX, 0.0f, PosY };
			m_PointMass[index]->Reset();
		}
	}
}
void Cloth::ResetComponent()
{
	m_WindForceFlag = glm::bvec3(false);
	m_WindForceCoeff = glm::vec3(0.5f);
	m_Color = glm::vec3(1.0f);
	memset(m_PinPoint, false, sizeof(m_PinPoint));
}

void Cloth::DrawWireframe(const Shader& shader)
{
	std::vector<glm::vec3> vertices;

	shader.Activate();
	shader.SetBool("u_IsTexture", false);

	// Bind VAO, EBO & Draw PointMassMesh
	m_PointMassMesh.vao.Bind();
	for (int row = 0; row < m_SamplerAmount; row++)
	{
		for (int col = 0; col < m_SamplerAmount; col++)
		{
			PointMass* currentPointMass = GetPointMass(row, col);

			if (row + 1 < m_SamplerAmount)
			{
				vertices.push_back(currentPointMass->position);
				vertices.push_back(m_PointMass[GetIndex(row + 1, col)]->position);
			}
			if (col + 1 < m_SamplerAmount)
			{
				vertices.push_back(currentPointMass->position);
				vertices.push_back(m_PointMass[GetIndex(row, col + 1)]->position);
			}
			if (row + 1 < m_SamplerAmount && col + 1 < m_SamplerAmount)
			{
				vertices.push_back(currentPointMass->position);
				vertices.push_back(m_PointMass[GetIndex(row + 1, col + 1)]->position);
			}
			if (row + 1 < m_SamplerAmount && col - 1 >= 0)
			{
				vertices.push_back(currentPointMass->position);
				vertices.push_back(m_PointMass[GetIndex(row + 1, col - 1)]->position);
			}
		}
	}
	m_PointMassMesh.ebo.UnBind();
	m_PointMassMesh.vao.UnBind();

	// Assign vertices of WireFrame into Buffer
	m_LineMesh.vbo.BufferData(sizeof(glm::vec3) * vertices.size(), vertices.data(), false);
	m_LineMesh.vao.Attribute(m_LineMesh.vbo, 0, 3, GL_FLOAT, sizeof(glm::vec3), 0);

	shader.SetMat4("u_Model", glm::mat4(1.0f));
	shader.SetVec3("u_Color", glm::vec3(1.0f, 1.0f, 1.0f));

	m_LineMesh.vao.Bind();
	glDrawArrays(GL_LINES, 0, vertices.size());
	m_LineMesh.vao.UnBind();
}
void Cloth::DrawTexture(const Camera& camera, const Shader& shader)
{
	glm::vec3 lightPos[5] = { 
		glm::vec3(length / 2.0f, 3.0f, length / 2.0f),
		glm::vec3(0.0f, 0.0f, length / 2.0f),
		glm::vec3(length, 0.0f, length / 2.0f),
	};
	
	m_ClothTexture.Activate(GL_TEXTURE0);

	shader.Activate();
	shader.SetVec3("u_Color", m_Color);
	shader.SetBool("u_DoLight", true);
	shader.SetBool("u_IsTexture", true);
	shader.SetInt("u_Texture", 0);
	shader.SetMat4("u_Model", glm::mat4(1.0f));
	shader.SetVec3("u_LightPos[0]", lightPos[0]);
	shader.SetVec3("u_LightPos[1]", lightPos[1]);
	shader.SetVec3("u_LightPos[2]", lightPos[2]);
	shader.SetVec3("u_LightPos[3]", lightPos[3]);
	shader.SetVec3("u_LightPos[4]", lightPos[4]);
	shader.SetVec3("u_CameraPos", camera.GetPosition());

	// Compute Normal
	for (int row = 0; row < m_SamplerAmount - 1; row++)
	{
		for (int col = 0; col < m_SamplerAmount - 1; col++)
		{
			PointMass* v1 = GetPointMass(row, col);
			PointMass* v2 = GetPointMass(row + 1, col);
			PointMass* v3 = GetPointMass(row, col + 1);
			PointMass* v4 = GetPointMass(row + 1, col + 1);

			glm::vec3 normal1 = glm::cross(v3->position - v1->position, v2->position - v1->position);
			glm::vec3 normal2 = glm::cross(v1->position - v2->position, v4->position - v2->position);
			glm::vec3 normal3 = glm::cross(v4->position - v3->position, v1->position - v3->position);
			glm::vec3 normal4 = glm::cross(v2->position - v4->position, v3->position - v4->position);
		
			v1->AddNormal(normal1);
			v2->AddNormal(normal2);
			v3->AddNormal(normal3);
			v4->AddNormal(normal4);
		}
	}

	std::vector<glm::vec3> vertices;
	std::vector<GLuint> indices;
	GLuint idx = 0;
	
	m_EachClothResolution = 1.0f / samplerAmount * m_ClothResolution;

	// Assign to Buffer
	for (int row = 0; row < m_SamplerAmount - 1; row++)
	{
		float curRow = row;
		float nextRow = row + 1;
		for (int col = 0; col < m_SamplerAmount - 1; col++)
		{
			float curCol = col;
			float nextCol = col + 1;

			PointMass* v1 = GetPointMass(row		, col);
			PointMass* v2 = GetPointMass(row + 1	, col);
			PointMass* v3 = GetPointMass(row		, col + 1);
			PointMass* v4 = GetPointMass(row + 1	, col + 1);

			vertices.push_back(v1->position);
			vertices.push_back(v1->normal);
			vertices.push_back({ curRow * m_EachClothResolution, curCol * m_EachClothResolution, 0.0f });
			vertices.push_back(v2->position);
			vertices.push_back(v2->normal);
			vertices.push_back({ nextRow * m_EachClothResolution, curCol * m_EachClothResolution, 0.0f });
			vertices.push_back(v3->position);
			vertices.push_back(v3->normal);
			vertices.push_back({ curRow * m_EachClothResolution, nextCol * m_EachClothResolution, 0.0f });
			vertices.push_back(v4->position);
			vertices.push_back(v4->normal);
			vertices.push_back({ nextRow * m_EachClothResolution, nextCol * m_EachClothResolution, 0.0f });

			indices.push_back(idx * 4);
			indices.push_back(idx * 4 + 3);
			indices.push_back(idx * 4 + 1);
			indices.push_back(idx * 4);
			indices.push_back(idx * 4 + 2);
			indices.push_back(idx * 4 + 3);

			/*
			m_SquareMesh.SetMesh(
				// Vertices
				{	v1->position, v1->normal,
					v2->position, v2->normal,
					v3->position, v3->normal,
					v4->position, v4->normal},
				// Indices
				{ 0, 3, 1,
				  0, 2, 3 }
				, true
			);

			m_SquareMesh.Draw(GL_TRIANGLES);
			*/

			idx++;
		}
	}

	m_ClothMesh.vbo.BufferData(sizeof(glm::vec3) * vertices.size(), vertices.data(), false);
	m_ClothMesh.ebo.BufferData(sizeof(GLuint) * indices.size(), indices.data(), false);

	m_ClothMesh.vao.Attribute(m_ClothMesh.vbo, 0, 3, GL_FLOAT, 3 * sizeof(glm::vec3), 0);
	m_ClothMesh.vao.Attribute(m_ClothMesh.vbo, 1, 3, GL_FLOAT, 3 * sizeof(glm::vec3), sizeof(glm::vec3));
	m_ClothMesh.vao.Attribute(m_ClothMesh.vbo, 2, 3, GL_FLOAT, 3 * sizeof(glm::vec3), 2 * sizeof(glm::vec3));

	// Bind VAO & Draw
	m_ClothMesh.vao.Bind();
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, indices.data());
	m_ClothMesh.ebo.UnBind();
	m_ClothMesh.vao.UnBind();
}


void Cloth::UpdateForce(const float& dt, const Sphere& sphere, const Floor& floor)
{
	float k = sqrt(kx * kx + ky * ky);
	m_Time += dt;

	for (int row = 0; row < m_SamplerAmount; row++)
	{
		for (int col = 0; col < m_SamplerAmount; col++)
		{
			PointMass* currentPointMass = GetPointMass(row, col);

			// Apply Gravitational Force
			currentPointMass->force += m_EachMass * glm::vec3(0.0f, -g, 0.0f);

			// Apply Spring Force
			for (int pm_index = 0; pm_index < currentPointMass->neighborSize; pm_index++)
			{
				glm::vec3 v = currentPointMass->neighborList[pm_index].first->position - currentPointMass->position;
				if(currentPointMass->neighborList[pm_index].second == STRUCTURAL_X)
					currentPointMass->force += glm::normalize(v) * (glm::length(v) - m_EachLength) * kx;
				else if (currentPointMass->neighborList[pm_index].second == STRUCTURAL_Y)
					currentPointMass->force += glm::normalize(v) * (glm::length(v) - m_EachLength) * ky;
				else if (currentPointMass->neighborList[pm_index].second == SHEAR)
					currentPointMass->force += glm::normalize(v) * (glm::length(v) - m_EachLength * SQRT_2) * k;
				else if (currentPointMass->neighborList[pm_index].second == FLEXION_X)
					currentPointMass->force += glm::normalize(v) * (glm::length(v) - m_EachLength * 2.0f) * kx;
				else if (currentPointMass->neighborList[pm_index].second == FLEXION_Y)
					currentPointMass->force += glm::normalize(v) * (glm::length(v) - m_EachLength * 2.0f) * ky;
			}

			// Apply Wind Force
			float windX = std::sin(currentPointMass->position.x * currentPointMass->position.y * m_Time);
			float windY = std::fabs(std::sin(0.1f * m_Time)) - 0.2f;
			float windZ = std::sin(std::cos(currentPointMass->position.x * m_Time)) - 0.8f;
			//currentPointMass->force += glm::vec3(0.0f, 0.0005f * windY, 0.002f * windZ);
			//currentPointMass->force += glm::vec3(0.0f, 0.005f * windY, 0.0f);

			if (m_WindForceFlag.x) { currentPointMass->force.x += (m_WindForceCoeff.x / 100.0f) * windX; }
			if (m_WindForceFlag.y) { currentPointMass->force.y += (m_WindForceCoeff.y / 100.0f) * windY; }
			if (m_WindForceFlag.z) { currentPointMass->force.z += (m_WindForceCoeff.z / 100.0f) * windZ; }

			// Apply Dampling Force
			currentPointMass->force += -0.005f * currentPointMass->velocity;

			// F = ma
			// a = F / m
			//currentPointMass->acceleration = m_EachMassInvert * currentPointMass->force;
			currentPointMass->acceleration = m_EachMassInvert * currentPointMass->force * dt;
			//std::cout << currentPointMass->acceleration.x << " " << currentPointMass->acceleration.y << " " << currentPointMass->acceleration.z << "\n";

			currentPointMass->force = glm::vec3(0.0f);

			// Set normal Vector to (0,0,0) -> will be compute when drawing
			currentPointMass->normal = glm::vec3(0.0f);
		}
	}

	if (m_PinPoint[0]) { GetPointMass(m_SamplerAmount - 1, m_SamplerAmount - 1)->acceleration = glm::vec3(0.0f); }
	if (m_PinPoint[1]) { GetPointMass(0, m_SamplerAmount - 1)->acceleration = glm::vec3(0.0f); }
	if (m_PinPoint[2]) { GetPointMass(m_SamplerAmount - 1, 0)->acceleration = glm::vec3(0.0f); }
	if (m_PinPoint[3]) { GetPointMass(0, 0)->acceleration = glm::vec3(0.0f); }
	
	for (int row = 0; row < m_SamplerAmount; row++)
	{
		for (int col = 0; col < m_SamplerAmount; col++)
		{
			PointMass* currentPointMass = GetPointMass(row, col);
			currentPointMass->UpdatePosition(dt);

			if (sphere.IsActive())
			{
				// Check Collision with Sphere
				glm::vec3 offsetSphere = currentPointMass->position - sphere.GetPosition();
				if (glm::length(offsetSphere) - 0.02f <= sphere.GetRadius())
				{
					currentPointMass->acceleration = glm::vec3(0.0f);
					currentPointMass->velocity = glm::vec3(0.0f);
					currentPointMass->position += glm::normalize(offsetSphere) * (sphere.GetRadius() - glm::length(offsetSphere) + 0.02f);
				}
			}

			if (floor.IsActive())
			{
				// Check Collision with Floor
				if (currentPointMass->position.y - 0.01f <= floor.GetPosition().y + floor.scale.y / 2.0f)
				{
					currentPointMass->acceleration = glm::vec3(0.0f);
					currentPointMass->velocity = glm::vec3(0.0f);
					currentPointMass->position.y += floor.scale.y / 2.0f - (currentPointMass->position.y - floor.GetPosition().y) + 0.01f;
				}
			}

			//maxVelo = (glm::length(currentPointMass->velocity) > glm::length(maxVelo) ? currentPointMass->velocity : maxVelo);

		}
	}

	// Cloth - Cloth Collision
	/*
	// Check Before Hit the Floor
	BuildSpatialMap();
	const float thickness = 0.005f;
	for (int i = 0; i < m_SamplerAmount * m_SamplerAmount; i++) 
	{
		PointMass* pm1 = m_PointMass[i];
		std::vector<PointMass*>& nearby_pms = m_SpatialMap[HashPosition(pm1->position)];
		for (int j = 0; j < nearby_pms.size(); j++) 
		{	
			PointMass* pm2 = nearby_pms[j];
			if (pm1 != pm2) 
			{
				float dist = glm::length(pm1->position - pm2->position);
				if (dist < 2 * thickness)
				{
					glm::vec3 correction = glm::normalize(pm1->position - pm2->position) * (2 * thickness - dist) * 5.0f;
					std::cout << "COLLIDE\n";
					//std::cout << correction.x << " " << correction.y << " " << correction.z << "\n";
					
					pm1->acceleration = glm::vec3(0.0f);
					pm1->velocity = glm::vec3(0.0f);
					pm1->position += correction;
				}
			}
		}
	}
	*/
}

int Cloth::GetSamplerAmount() const
{
	return m_SamplerAmount;
}
float Cloth::GetClothSize() const
{
	return m_ClothSize;
}
float Cloth::GetMass() const
{
	return m_Mass;
}
glm::bvec3& Cloth::GetWindForceFlag() const
{
	return m_WindForceFlag;
}
glm::vec3& Cloth::GetWindForceCoeff() const 
{
	return m_WindForceCoeff;
}
bool& Cloth::GetPinPoint(const int& index) const
{
	return m_PinPoint[index];
}
int& Cloth::GetClothResolution() const
{
	return m_ClothResolution;
}
PointMass* Cloth::GetPointMass(const int& index) const
{
	return m_PointMass[index];
}
PointMass* Cloth::GetPointMass(const int& row, const int& column) const
{
	return m_PointMass[(row * m_SamplerAmount) + column];
}
std::vector<PointMass*> Cloth::GetStructuralXAxisNeighborPointMass(const int& row, const int& column) const
{
	std::vector<PointMass*> ret;

	// Left
	if (column - 1 >= 0)
	{
		ret.push_back(GetPointMass(row, column - 1));
	}
	// Right
	if (column + 1 < m_SamplerAmount)
	{
		ret.push_back(GetPointMass(row, column + 1));
	}

	return ret;
}
std::vector<PointMass*> Cloth::GetStructuralYAxisNeighborPointMass(const int& row, const int& column) const
{
	std::vector<PointMass*> ret;

	// Up
	if (row - 1 >= 0)
	{
		ret.push_back(GetPointMass(row - 1, column));
	}
	// Down
	if (row + 1 < m_SamplerAmount)
	{
		ret.push_back(GetPointMass(row + 1, column));
	}

	return ret;
}
std::vector<PointMass*> Cloth::GetShearNeighborPointMass(const int& row, const int& column) const
{
	std::vector<PointMass*> ret;

	// Up_Left
	if (row - 1 >= 0 && column - 1 >= 0)
	{
		ret.push_back(GetPointMass(row - 1, column - 1));
	}
	// Down_Left
	if (row + 1 < m_SamplerAmount && column - 1 >= 0)
	{
		ret.push_back(GetPointMass(row + 1, column - 1));
	}
	// Up_Right
	if (row - 1 >= 0 && column + 1 < m_SamplerAmount)
	{
		ret.push_back(GetPointMass(row - 1, column + 1));
	}
	// Down_Right
	if (row + 1 < m_SamplerAmount && column + 1 < m_SamplerAmount)
	{
		ret.push_back(GetPointMass(row + 1, column + 1));
	}

	return ret;
}
std::vector<PointMass*> Cloth::GetFlexionXAxisNeighborPointMass(const int& row, const int& column) const
{
	std::vector<PointMass*> ret;

	// Left-Left
	if (column - 2 >= 0)
	{
		ret.push_back(GetPointMass(row, column - 2));
	}
	// Right-Right
	if (column + 2 < m_SamplerAmount)
	{
		ret.push_back(GetPointMass(row, column + 2));
	}

	return ret;
}
std::vector<PointMass*> Cloth::GetFlexionYAxisNeighborPointMass(const int& row, const int& column) const
{
	std::vector<PointMass*> ret;

	// Up-Up
	if (row - 2 >= 0)
	{
		ret.push_back(GetPointMass(row - 2, column));
	}
	// Down-Down
	if (row + 2 < m_SamplerAmount)
	{
		ret.push_back(GetPointMass(row + 2, column));
	}

	return ret;
}

void Cloth::GetStructuralXAxisNeighborPointMass(const int& row, const int& column, PointMass* pointMassList[], size_t& newSize) const
{
	newSize = 0;

	// Left
	if (column - 1 >= 0)
	{
		pointMassList[newSize] = GetPointMass(row, column - 1);
		newSize++;
	}
	// Right
	if (column + 1 < m_SamplerAmount)
	{
		pointMassList[newSize] = GetPointMass(row, column + 1);
		newSize++;
	}
}
void Cloth::GetStructuralYAxisNeighborPointMass(const int& row, const int& column, PointMass* pointMassList[], size_t& newSize) const
{
	newSize = 0;
	
	// Up
	if (row - 1 >= 0)
	{
		pointMassList[newSize] = GetPointMass(row - 1, column);
		newSize++;
	}
	// Down
	if (row + 1 < m_SamplerAmount)
	{
		pointMassList[newSize] = GetPointMass(row + 1, column);
		newSize++;
	}
}
void Cloth::GetShearNeighborPointMass(const int& row, const int& column, PointMass* pointMassList[], size_t& newSize) const
{
	newSize = 0;

	// Up_Left
	if (row - 1 >= 0 && column - 1 >= 0)
	{
		pointMassList[newSize] = GetPointMass(row - 1, column - 1);
		newSize++;
	}
	// Down_Left
	if (row + 1 < m_SamplerAmount && column - 1 >= 0)
	{
		pointMassList[newSize] = GetPointMass(row + 1, column - 1);
		newSize++;
	}
	// Up_Right
	if (row - 1 >= 0 && column + 1 < m_SamplerAmount)
	{
		pointMassList[newSize] = GetPointMass(row - 1, column + 1);
		newSize++;
	}
	// Down_Right
	if (row + 1 < m_SamplerAmount && column + 1 < m_SamplerAmount)
	{
		pointMassList[newSize] = GetPointMass(row + 1, column + 1);
		newSize++;
	}
}
void Cloth::GetFlexionXAxisNeighborPointMass(const int& row, const int& column, PointMass* pointMassList[], size_t& newSize) const
{
	newSize = 0;

	// Left-Left
	if (column - 2 >= 0)
	{
		pointMassList[newSize] = GetPointMass(row, column - 2);
		newSize++;
	}
	// Right-Right
	if (column + 2 < m_SamplerAmount)
	{
		pointMassList[newSize] = GetPointMass(row, column + 2);
		newSize++;
	}
}
void Cloth::GetFlexionYAxisNeighborPointMass(const int& row, const int& column, PointMass* pointMassList[], size_t& newSize) const
{
	newSize = 0;

	// Up-Up
	if (row - 2 >= 0)
	{
		pointMassList[newSize] = GetPointMass(row - 2, column);
		newSize++;
	}
	// Down-Down
	if (row + 2 < m_SamplerAmount)
	{
		pointMassList[newSize] = GetPointMass(row + 2, column);
		newSize++;
	}
}
// ---------------------- Private Function ----------------------
int Cloth::GetIndex(const int& row, const int& column)
{
	int index = (row * m_SamplerAmount) + column;
	return index;
}
int Cloth::HashPosition(const glm::vec3& position) 
{
	int h = (int(position.x * 10.0f) * 92837111) ^ (int(position.y * 10.0f) * 689287499) ^ (int(position.z * 10.0f) * 283923481);	// fantasy function
	return std::abs(h);

	/*
	float w = 3.0f * m_ClothSize / m_SamplerAmount;
	float h = 3.0f * m_ClothSize / m_SamplerAmount;
	float t = std::max(w, h);
	float x = std::fmod(position.x, w);
	float y = std::fmod(position.y, h);
	float z = std::fmod(position.z, t);
	return (x + y * w + z * w * h) * 100000.0f;
	*/
}
void Cloth::BuildSpatialMap() 
{
	m_SpatialMap.clear();
	for (int i = 0; i < m_SamplerAmount * m_SamplerAmount; i++) 
	{
		int hash_key = HashPosition(m_PointMass[i]->position);
		m_SpatialMap[hash_key].push_back(m_PointMass[i]);
	}
}