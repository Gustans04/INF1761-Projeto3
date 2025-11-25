#include <memory>
class Quad;
using QuadPtr = std::shared_ptr<Quad>; 

#ifndef QUAD_H
#define QUAD_H

#include "shape.h"

class Quad : public Shape {
  unsigned int m_vao;
protected:
  Quad();
public:
  static QuadPtr Make ();
  virtual ~Quad ();
  virtual void Draw (StatePtr st);
};
#endif