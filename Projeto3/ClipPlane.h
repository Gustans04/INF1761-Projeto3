#include <memory>
class ClipPlane;
using ClipPlanePtr = std::shared_ptr<ClipPlane>;

#ifndef CLIPPLANE_H
#define CLIPPLANE_H

#include "appearance.h"
#include <glm/glm.hpp>
#include <string>

class ClipPlane : public Appearance {
	std::string m_name;
	std::vector<glm::vec4> m_planes;
protected:
	ClipPlane(const std::string& name);
	ClipPlane(const std::string& name, float a, float b, float c, float d);
public:
	static ClipPlanePtr Make(const std::string& name);
	static ClipPlanePtr Make(const std::string& name, float a, float b, float c, float d);
	virtual ~ClipPlane();
	void AddPlane(float a, float b, float c, float d);
	virtual void Load(StatePtr st);
	virtual void Unload(StatePtr st);
};

#endif