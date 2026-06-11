#pragma once

#include "pch.h"

class SphereMesh {

public:
  int _resolution;
  Mesh _mesh{0};
  std::vector<Vector3> verts;
  std::vector<int> tris;

  SphereMesh(int resolution);
  void genIcoSphere();
  void buildMesh();
  int getMidpoint(int a, int b, std::vector<Vector3> &verts,
                  std::map<std::pair<int, int>, int> &cache);
};